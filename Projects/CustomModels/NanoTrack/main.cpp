#include <cviruntime.h>
#include <cvitpu_debug.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "assert.h"
#include "cnpy.h"

void print_tensor_data(const CVI_TENSOR *tensor) {
	int n = tensor->shape.dim[0];
	int c = tensor->shape.dim[1];
	int h = tensor->shape.dim[2];
	int w = tensor->shape.dim[3];

	printf("%d %d %d %d\n", n, c, h, w);

	float *data = (float *)tensor->sys_mem;

	for (int ni = 0; ni < n; ++ni) {
		for (int ci = 0; ci < c; ++ci) {
			printf("Tensor[%d][%d]:\n", ni, ci);
			for (int hi = 0; hi < h; ++hi) {
				for (int wi = 0; wi < w; ++wi) {
					int index = ni * c * h * w + ci * h * w + hi * w + wi;
					printf("%8.4f ", data[index]);
				}
				printf("\n");
			}
			printf("\n");
		}
	}
}

#define EXIT_IF_ERROR(cond, statement)                                         \
	if ((cond)) {                                                              \
		TPU_LOG_ERROR("%s\n", statement);                                      \
		exit(1);                                                               \
	}

static void ConvertFp32ToBf16(float *src, uint16_t *dst, int count) {
	for (int i = 0; i < count; ++i) {
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
	for (auto &npy : input_npz) {
		auto &arr = npy.second;
		auto &tensor = tensors[idx++];
		ConvertFp32ToBf16(arr.data<float>(),
		                  (uint16_t *)CVI_NN_TensorPtr(&tensor),
		                  CVI_NN_TensorCount(&tensor));
	}
}

static inline uint16_t float_to_bf16(float f) {
	uint32_t as_int;
	memcpy(&as_int, &f, sizeof(float));
	return (uint16_t)(as_int >> 16); // Drop lower 16 bits
}

void MMMConvertFp32ToBf16(const float *src, uint16_t *dst, size_t count) {
	for (int i = 0; i < count; ++i) {
		auto fval = src[i];
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
		dst[i] = ((uint16_t *)&fval)[0];
#else
		dst[i] = ((uint16_t *)&fval)[1];
#endif
	}
}

static void saveResultToNpz(const std::string &name, CVI_TENSOR *tensors,
                            int32_t num) {
	assert(isNpzFile(name) && "output should be a npz file");

	cnpy::npz_t npz;
	for (int i = 0; i < num; i++) {
		auto &tensor = tensors[i];
		std::vector<size_t> shape = {
		    (size_t)tensor.shape.dim[0], (size_t)tensor.shape.dim[1],
		    (size_t)tensor.shape.dim[2], (size_t)tensor.shape.dim[3]};
		switch (tensor.fmt) {
		case CVI_FMT_FP32:
			cnpy::npz_add_array<float>(
			    npz, tensor.name, (float *)CVI_NN_TensorPtr(&tensor), shape);
			break;
		case CVI_FMT_BF16: // we use uint16_t to represent BF16
			cnpy::npz_add_array<uint16_t>(
			    npz, tensor.name, (uint16_t *)CVI_NN_TensorPtr(&tensor), shape);
			break;
		case CVI_FMT_INT8:
			cnpy::npz_add_array<int8_t>(
			    npz, tensor.name, (int8_t *)CVI_NN_TensorPtr(&tensor), shape);
			break;
		case CVI_FMT_UINT8:
			cnpy::npz_add_array<uint8_t>(
			    npz, tensor.name, (uint8_t *)CVI_NN_TensorPtr(&tensor), shape);
			break;
		default:
			TPU_LOG_ERROR("Error, Current unsupported type:%d\n", tensor.fmt);
			assert(0);
		}
	}
	cnpy::npz_save_all(name, npz);
}

class NanoTrack {
  private:
	// Weights
	std::string backbone_127_path =
	    "/root/nano/nanotrack_backbone_template_input_quant.cvimodel";
	std::string backbone_255_path =
	    "/root/nano/nanotrack_backbone_exampler_input_quant.cvimodel";
	std::string head_path = "/root/nano/nanotrack_head.cvimodel";

	// Data
	std::string backbone_127_data_path = "/root/data_in/127_input.npz";
	std::string backbone_255_data_path = "/root/data_in/255_input.npz";
	std::string tmp_head_data_path =
	    "/root/data_in/head_input_from_board_data.npz";

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

	NanoTrack() { printf("NanoTrack\n"); }

	~NanoTrack() {
		CVI_NN_CleanupModel(backbone_255_model);
		CVI_NN_CleanupModel(head_model);
		// CVI_NN_CleanupModel(backbone_127_model);
	}

	CVI_RC infer_template_backbone() {
		CVI_TENSOR *b127_input_tensors = nullptr;
		int32_t input_num, output_num;

		ret = CVI_NN_RegisterModel(backbone_127_path.c_str(),
		                           &backbone_127_model);
		EXIT_IF_ERROR(ret != CVI_RC_SUCCESS,
		              "failed to register backbone 127 cvimodel");
		CVI_NN_SetConfig(backbone_127_model, OPTION_PROGRAM_INDEX, 0);
		CVI_NN_SetConfig(backbone_127_model, OPTION_OUTPUT_ALL_TENSORS, 0);
		ret = CVI_NN_GetInputOutputTensors(backbone_127_model,
		                                   &b127_input_tensors, &input_num,
		                                   &b127_output_tensors, &output_num);
		// Load from npz
		load_input(backbone_127_data_path, b127_input_tensors, input_num);

		// Inference
		ret = CVI_NN_Forward(backbone_127_model, b127_input_tensors, input_num,
		                     b127_output_tensors, output_num);
		return ret;
	}

	CVI_RC setup_exampler_backbone() {
		ret = CVI_NN_RegisterModel(backbone_255_path.c_str(),
		                           &backbone_255_model);
		EXIT_IF_ERROR(ret != CVI_RC_SUCCESS,
		              "failed to register backbone 255 cvimodel");

		CVI_NN_SetConfig(backbone_255_model, OPTION_PROGRAM_INDEX, 0);
		CVI_NN_SetConfig(backbone_255_model, OPTION_OUTPUT_ALL_TENSORS, 0);

		return CVI_NN_GetInputOutputTensors(
		    backbone_255_model, &b255_input_tensors, &b255_input_num,
		    &b255_output_tensors, &b255_output_num);
	}

	CVI_RC infer_exampler_backbone() {
		// Data loaded from main code (Load from npz)
		// Inference
		return CVI_NN_Forward(backbone_255_model, b255_input_tensors,
		                      b255_input_num, b255_output_tensors,
		                      b255_output_num);
		// print_tensor_data(b255_output_tensors);
	}

	CVI_RC setup_head() {
		ret = CVI_NN_RegisterModel(head_path.c_str(), &head_model);
		EXIT_IF_ERROR(ret != CVI_RC_SUCCESS,
		              "failed to register backbone 255 cvimodel");

		CVI_NN_SetConfig(head_model, OPTION_PROGRAM_INDEX, 0);
		CVI_NN_SetConfig(head_model, OPTION_OUTPUT_ALL_TENSORS, 0);

		return CVI_NN_GetInputOutputTensors(
		    head_model, &head_input_tensors, &head_input_num,
		    &head_output_tensors, &head_output_num);
	}

	CVI_RC infer_head() {
		// load_input(tmp_head_data_path, head_input_tensors, head_input_num);
		// print_tensor_data(&head_input_tensors[0]);

		const float *src_fp32 = (const float *)head_input_tensors[0].sys_mem;
		uint16_t *dst_bf16 = (uint16_t *)b127_output_tensors->sys_mem;
		size_t count = b127_output_tensors->count;

		MMMConvertFp32ToBf16(src_fp32, dst_bf16, count);

		const float *src_fp32_ = (const float *)head_input_tensors[1].sys_mem;
		uint16_t *dst_bf16_ = (uint16_t *)b255_output_tensors->sys_mem;
		size_t count_ = b255_output_tensors->count;

		MMMConvertFp32ToBf16(src_fp32_, dst_bf16_, count_);

		ret = CVI_NN_Forward(head_model, head_input_tensors, head_input_num,
		                     head_output_tensors, head_output_num);
		// print_tensor_data(&head_output_tensors[1]);
		// saveResultToNpz("aaaaa.npz", head_output_tensors, head_output_num);
		return ret;
	}
};

int main() {
	CVI_RC ret;
	NanoTrack model = NanoTrack();
	model.setup_exampler_backbone();
	model.setup_head();

	// Template inference
	model.infer_template_backbone();
	std::string curr_data_path = "/root/data_in/255_input.npz";

	load_input(curr_data_path, model.b255_input_tensors, model.b255_input_num);

	// print_tensor_data(model.b127_output_tensors);
	// print_tensor_data(model.b255_output_tensors);
	double avg_latency = 0;
	int iter = 0;
	for (int i = 0; i < 1000; i++) {
		auto start = std::chrono::steady_clock::now();
		model.infer_exampler_backbone();
		model.infer_head();
		auto end = std::chrono::steady_clock::now();
		double latency = std::chrono::duration<double>(end - start).count();
		avg_latency += latency;
		iter++;
	}

	printf("latency: %lf; fps: %lf", avg_latency / iter,
	       1 / (avg_latency / iter));
	// print_tensor_data(&model.head_output_tensors[1]);

	// printf("head_input = %d\n", model.head_input_num);
	// Checks
	// for (int i = 0; i < 100; i++){
	// 	model.infer_exampler_backbone();
	// }

	return 0;
}