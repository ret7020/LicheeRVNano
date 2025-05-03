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
#include <stdio.h>
#include <stdint.h>

void print_tensor_data(const CVI_TENSOR *tensor) {
    int n = tensor->shape.dim[0];
    int c = tensor->shape.dim[1];
    int h = tensor->shape.dim[2];
    int w = tensor->shape.dim[3];

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

#define EXIT_IF_ERROR(cond, statement)                                                   \
  if ((cond)) {                                                                          \
    TPU_LOG_ERROR("%s\n", statement);                                                         \
    exit(1);                                                                             \
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


static void loadInput(std::string &input_file, CVI_TENSOR *tensors, int num) {
  assert(isNpzFile(input_file) && "input should be npz file");

  cnpy::npz_t input_npz = cnpy::npz_load(input_file);
  EXIT_IF_ERROR(!input_npz.size(), "cannot open input npz file");
  assert(num == (int)input_npz.size());

  int idx = 0;
  for (auto &npy : input_npz) {
    auto &arr = npy.second;
    auto &tensor = tensors[idx++];
    ConvertFp32ToBf16(
          arr.data<float>(),
          (uint16_t *)CVI_NN_TensorPtr(&tensor),
          CVI_NN_TensorCount(&tensor));
  }
}

// Weights
static std::string backbone_127_path = "/root/nano/nanotrack_backbone_template_input_quant.cvimodel";
static std::string backbone_255_path = "/root/nano/nanotrack_backbone_exampler_input_quant.cvimodel";

// Data
static std::string backbone_127_data_path = "/root/data_in/127_input.npz";
static std::string backbone_255_data_path = "/root/data_in/255_input.npz";

int main(){
  // Template Backbone 127 extraction
  printf("Backbone 127 init\n");
  CVI_MODEL_HANDLE backbone_127 = NULL;
  CVI_RC ret = CVI_NN_RegisterModel(backbone_127_path.c_str(), &backbone_127);
  EXIT_IF_ERROR(ret != CVI_RC_SUCCESS, "failed to register backbone 127 cvimodel");

  CVI_NN_SetConfig(backbone_127, OPTION_PROGRAM_INDEX, 0);
  CVI_NN_SetConfig(backbone_127, OPTION_OUTPUT_ALL_TENSORS, 0);

  CVI_TENSOR *input_tensors, *output_tensors;
  int32_t input_num, output_num;
  ret = CVI_NN_GetInputOutputTensors(backbone_127, &input_tensors, &input_num, &output_tensors, &output_num);

  // Load from npz
  loadInput(backbone_127_data_path, input_tensors, input_num);

  // Inference
  ret = CVI_NN_Forward(backbone_127, input_tensors, input_num, output_tensors, output_num);
  print_tensor_data(output_tensors);

  CVI_NN_CleanupModel(backbone_127);
  printf("Exit code: %d\n", ret);



  return 0;
}