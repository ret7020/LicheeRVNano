#include "yolo.hpp"

YoloModelDetector::YoloModelDetector() {
    CVI_S32 ret = CVI_TDL_CreateHandle(&tdl_handle);
    if (ret != CVI_SUCCESS) {
        throw std::runtime_error("Create TDL handle failed with error code: " + std::to_string(ret));
    }
}

int YoloModelDetector::setup_model(char *model_path, int model_class_cnt, float model_thresh, float model_nms_thresh, float model_scale,
                                   float model_mean) {

    InputPreParam preprocess_cfg = CVI_TDL_GetPreParam(tdl_handle, CVI_TDL_SUPPORTED_MODEL_YOLOV8_DETECTION);
    for (int i = 0; i < 3; i++) {
        preprocess_cfg.factor[i] = model_scale;
        preprocess_cfg.mean[i] = model_mean;
    }
    preprocess_cfg.format = PIXEL_FORMAT_RGB_888_PLANAR;
    CVI_S32 ret = CVI_TDL_SetPreParam(tdl_handle, CVI_TDL_SUPPORTED_MODEL_YOLOV8_DETECTION, preprocess_cfg);
    if (ret != CVI_SUCCESS) {
        return -1;
    }

    cvtdl_det_algo_param_t yolov8_param =
        CVI_TDL_GetDetectionAlgoParam(tdl_handle, CVI_TDL_SUPPORTED_MODEL_YOLOV8_DETECTION);
    yolov8_param.cls = model_class_cnt;
    ret = CVI_TDL_SetDetectionAlgoParam(tdl_handle, CVI_TDL_SUPPORTED_MODEL_YOLOV8_DETECTION, yolov8_param);
    if (ret != CVI_SUCCESS) {
        return -2;
    }

    CVI_TDL_SetModelThreshold(tdl_handle, CVI_TDL_SUPPORTED_MODEL_YOLOV8_DETECTION, model_thresh);
    CVI_TDL_SetModelNmsThreshold(tdl_handle, CVI_TDL_SUPPORTED_MODEL_YOLOV8_DETECTION, model_nms_thresh);
    ret = CVI_TDL_OpenModel(tdl_handle, CVI_TDL_SUPPORTED_MODEL_YOLOV8_DETECTION, model_path);
    if (ret != CVI_SUCCESS) {
        return -3;
    }

    return 1;
}

cvtdl_object_t YoloModelDetector::detect(VIDEO_FRAME_INFO_S *frame_info) {
    cvtdl_object_t obj_meta = {0};
    CVI_TDL_Detection(tdl_handle, frame_info, CVI_TDL_SUPPORTED_MODEL_YOLOV8_DETECTION, &obj_meta);
    return obj_meta;
}

void YoloModelDetector::release() {
    if (tdl_handle != nullptr) {
        CVI_TDL_DestroyHandle(tdl_handle);
        tdl_handle = nullptr;
    }
}