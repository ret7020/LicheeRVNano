#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/time.h>
#include <cviruntime.h>
#include <cvitpu_debug.h>
#include "cnpy.h"
#include "assert.h"
#include <stdint.h>
#include <string>

void print_tensor_data(const CVI_TENSOR *tensor) {
	int n = tensor->shape.dim[0];
	int c = tensor->shape.dim[1];
	int h = tensor->shape.dim[2];
	int w = tensor->shape.dim[3];

	printf("%d %d %d %d\n", n, c, h, w);

	float *data = (float *)tensor->sys_mem;

	for (int ni = 0; ni < n; ++ni)
	{
		for (int ci = 0; ci < c; ++ci)
		{
			printf("Tensor[%d][%d]:\n", ni, ci);
			for (int hi = 0; hi < h; ++hi)
			{
				for (int wi = 0; wi < w; ++wi)
				{
					int index = ni * c * h * w + ci * h * w + hi * w + wi;
					printf("%8.4f ", data[index]);
				}
				printf("\n");
			}
			printf("\n");
		}
	}
}

#define EXIT_IF_ERROR(cond, statement)    \
	if ((cond))                           \
	{                                     \
		TPU_LOG_ERROR("%s\n", statement); \
		exit(1);                          \
	}

static void ConvertFp32ToBf16(float *src, uint16_t *dst, int count) {
	for (int i = 0; i < count; ++i)
	{
		auto fval = src[i];
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
		dst[i] = ((uint16_t *)&fval)[0];
#else
		dst[i] = ((uint16_t *)&fval)[1];
#endif
	}
}

static void load_input(std::string &input_file, CVI_TENSOR *tensors, int num) {
	assert(isNpzFile(input_file) && "input should be npz file");

	cnpy::npz_t input_npz = cnpy::npz_load(input_file);
	EXIT_IF_ERROR(!input_npz.size(), "cannot open input npz file");
	assert(num == (int)input_npz.size());

	printf("Size: %d\n", (int)input_npz.size());

	int idx = 0;
	for (auto &npy : input_npz)
	{
		auto &arr = npy.second;
		auto &tensor = tensors[idx++];
		ConvertFp32ToBf16(
			arr.data<float>(),
			(uint16_t *)CVI_NN_TensorPtr(&tensor),
			CVI_NN_TensorCount(&tensor));
	}
}

class NanoTrack {
private:
	// Weights
	std::string backbone_127_path = "/root/nano/nanotrack_backbone_template_input_quant.cvimodel";
	std::string backbone_255_path = "/root/nano/nanotrack_backbone_exampler_input_quant.cvimodel";
	std::string head_path = "/root/nano/nanotrack_head.cvimodel";

	// Data
	std::string backbone_127_data_path = "/root/data_in/127_input.npz";
	std::string backbone_255_data_path = "/root/data_in/255_input.npz";

	CVI_RC ret;

public:
	CVI_MODEL_HANDLE backbone_127_model = NULL;
	CVI_MODEL_HANDLE backbone_255_model = NULL;
	CVI_MODEL_HANDLE head_model = NULL;
	CVI_TENSOR *b127_output_tensors = nullptr; // Use later for head model
	CVI_TENSOR *b255_input_tensors = nullptr, *b255_output_tensors = nullptr;
	CVI_TENSOR *head_input_tensors = nullptr, *head_output_tensors = nullptr;

	int32_t b255_input_num, b255_output_num;
	int32_t head_input_num, head_output_num;

	NanoTrack() {
		printf("NanoTrack\n");
	}

	~NanoTrack() {
		CVI_NN_CleanupModel(backbone_255_model);
		CVI_NN_CleanupModel(head_model);
		// CVI_NN_CleanupModel(backbone_127_model);
	}

	CVI_RC infer_template_backbone() {
		CVI_TENSOR *b127_input_tensors = nullptr;
		int32_t input_num, output_num;

		ret = CVI_NN_RegisterModel(backbone_127_path.c_str(), &backbone_127_model);
		EXIT_IF_ERROR(ret != CVI_RC_SUCCESS, "failed to register backbone 127 cvimodel");
		CVI_NN_SetConfig(backbone_127_model, OPTION_PROGRAM_INDEX, 0);
		CVI_NN_SetConfig(backbone_127_model, OPTION_OUTPUT_ALL_TENSORS, 0);
		ret = CVI_NN_GetInputOutputTensors(backbone_127_model, &b127_input_tensors, &input_num, &b127_output_tensors, &output_num);
		// Load from npz
		load_input(backbone_127_data_path, b127_input_tensors, input_num);

		// Inference
		ret = CVI_NN_Forward(backbone_127_model, b127_input_tensors, input_num, b127_output_tensors, output_num);
		return ret;
	}

	CVI_RC setup_exampler_backbone() {
		ret = CVI_NN_RegisterModel(backbone_255_path.c_str(), &backbone_255_model);
		EXIT_IF_ERROR(ret != CVI_RC_SUCCESS, "failed to register backbone 255 cvimodel");

		CVI_NN_SetConfig(backbone_255_model, OPTION_PROGRAM_INDEX, 0);
		CVI_NN_SetConfig(backbone_255_model, OPTION_OUTPUT_ALL_TENSORS, 0);

		return CVI_NN_GetInputOutputTensors(backbone_255_model, &b255_input_tensors, &b255_input_num, &b255_output_tensors, &b255_output_num);
	}

	CVI_RC infer_exampler_backbone() {
		// Load from npz
		load_input(backbone_255_data_path, b255_input_tensors, b255_input_num);

		// Inference
		return CVI_NN_Forward(backbone_255_model, b255_input_tensors, b255_input_num, b255_output_tensors, b255_output_num);
		// print_tensor_data(b255_output_tensors);
	}

	CVI_RC setup_head() {
		ret = CVI_NN_RegisterModel(head_path.c_str(), &head_model);
		EXIT_IF_ERROR(ret != CVI_RC_SUCCESS, "failed to register backbone 255 cvimodel");

		CVI_NN_SetConfig(head_model, OPTION_PROGRAM_INDEX, 0);
		CVI_NN_SetConfig(head_model, OPTION_OUTPUT_ALL_TENSORS, 0);

		return CVI_NN_GetInputOutputTensors(head_model, &head_input_tensors, &head_input_num, &head_output_tensors, &head_output_num);
	}

	CVI_RC infer_head() {
		return 0;
	}
};


int main()
{
	CVI_RC ret;
	NanoTrack model = NanoTrack();
	model.setup_exampler_backbone();
	model.setup_head();

	// Template inference
	model.infer_template_backbone();
	model.infer_exampler_backbone();
	model.setup_head();

	print_tensor_data(model.b127_output_tensors);
	print_tensor_data(model.b255_output_tensors);
	model.infer_head();

	// printf("head_input = %d\n", model.head_input_num);
	// Checks
	// for (int i = 0; i < 100; i++){
	// 	model.infer_exampler_backbone();
	// }

	return 0;
}