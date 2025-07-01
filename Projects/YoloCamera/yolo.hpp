#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "core/cvi_tdl_types_mem_internal.h"
#include "core/utils/vpss_helper.h"
#include "cvi_tdl.h"
#include "cvi_tdl_media.h"

class YoloModelDetector {
    public:
        YoloModelDetector();
        int setup_model(char *model_path, int model_class_cnt, float model_thresh, float model_nms_thresh, float model_scale=0.0039216, float model_mean=0.0);
        cvtdl_object_t detect(VIDEO_FRAME_INFO_S *frame_info);
        void release();
        

    private:
        cvitdl_handle_t tdl_handle = nullptr;

};