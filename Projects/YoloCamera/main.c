#include "core/cvi_tdl_types_mem_internal.h"
#include "core/utils/vpss_helper.h"
#include "cvi_tdl.h"
#include "cvi_tdl_media.h"
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <time.h>
#include <unistd.h>
#include <algorithm>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "config.h"
#include "MJPEGWriter.h"

// Globals
cv::VideoCapture cap;
cvitdl_handle_t tdl_handle = nullptr;
volatile sig_atomic_t interrupted = 0;

void clean_up() {
    cap.release();
    if (tdl_handle != nullptr) {
        CVI_TDL_DestroyHandle(tdl_handle);
        tdl_handle = nullptr;
    }
}

void interrupt_handler(int signum) {
    printf("[MAIN] Signal: %d\n", signum);
    interrupted = 1;
}

CVI_S32 SET_YOLO_Params(const cvitdl_handle_t tdl_handle, int class_cnt)
{
    // setup preprocess
    YoloPreParam preprocess_cfg =
        CVI_TDL_Get_YOLO_Preparam(tdl_handle, CVI_TDL_SUPPORTED_MODEL_YOLOV8_DETECTION);

    for (int i = 0; i < 3; i++)
    {
        printf("asign val %d \n", i);
        preprocess_cfg.factor[i] = MODEL_SCALE;
        preprocess_cfg.mean[i] = MODEL_MEAN;
    }
    preprocess_cfg.format = PIXEL_FORMAT_RGB_888_PLANAR;

    printf("setup yolov8 param \n");
    CVI_S32 ret = CVI_TDL_Set_YOLO_Preparam(tdl_handle, CVI_TDL_SUPPORTED_MODEL_YOLOV8_DETECTION,
                                            preprocess_cfg);
    if (ret != CVI_SUCCESS)
    {
        printf("Can not set yolov8 preprocess parameters %#x\n", ret);
        return ret;
    }

    YoloAlgParam yolov8_param =
        CVI_TDL_Get_YOLO_Algparam(tdl_handle, CVI_TDL_SUPPORTED_MODEL_YOLOV8_DETECTION);
    yolov8_param.cls = class_cnt;

    printf("setup yolov8 algorithm param \n");
    ret =
        CVI_TDL_Set_YOLO_Algparam(tdl_handle, CVI_TDL_SUPPORTED_MODEL_YOLOV8_DETECTION, yolov8_param);
    if (ret != CVI_SUCCESS)
    {
        printf("Can not set yolov8 algorithm parameters %#x\n", ret);
        return ret;
    }

    // set theshold
    CVI_TDL_SetModelThreshold(tdl_handle, CVI_TDL_SUPPORTED_MODEL_YOLOV8_DETECTION, MODEL_THRESH);
    CVI_TDL_SetModelNmsThreshold(tdl_handle, CVI_TDL_SUPPORTED_MODEL_YOLOV8_DETECTION, MODEL_NMS_THRESH);

    printf("yolov8 algorithm parameters setup success!\n");
    return ret;
}


int main(int argc, char *argv[]) {

    signal(SIGINT, interrupt_handler); // Correct program stop

    if (argc < 4) {
        printf("Usage: ./stream_yolo <model_path> <frame_size> <class_cnt>\n");
        return 0;
    }

    CVI_S32 ret;

    printf("%s[MAIN] CVI init logs:\n", KYEL);

    cap.set(cv::CAP_PROP_FRAME_HEIGHT, atoi(argv[2]));
    cap.set(cv::CAP_PROP_FRAME_WIDTH, atoi(argv[2]));

    cap.open(0);

    cv::Mat bgr;

    MJPEGWriter test(7777);
    cap >> bgr;
    test.write(bgr);
    test.start();

    ret = CVI_TDL_CreateHandle(&tdl_handle);
    if (ret != CVI_SUCCESS) {
        printf("%s[MAIN] Create tdl handle failed with %#x!%s\n", ret, KRED, KNRM);
        return ret;
    }

    ret = SET_YOLO_Params(tdl_handle, atoi(argv[3]));
    ret = CVI_TDL_OpenModel(tdl_handle, CVI_TDL_SUPPORTED_MODEL_YOLOV8_DETECTION, argv[1]);

    if (ret != CVI_SUCCESS) {
        printf("%s[MAIN] Open model failed with %#x!%s\n", ret, KRED, KNRM);
        return ret;
    }
    printf("%s[MAIN] Detector ready%s\n", KGRN, KNRM);

    while (!interrupted) {
        cv::Mat frame;
        std::pair<void *, void *> imagePtrs = cap.capture(frame);
        void *image_ptr = imagePtrs.first;

        // Check frame read ok and inference
        if (image_ptr != nullptr) {
            VIDEO_FRAME_INFO_S *frameInfo = reinterpret_cast<VIDEO_FRAME_INFO_S *>(image_ptr);
            cvtdl_object_t obj_meta = {0};
            CVI_TDL_YOLOV8_Detection(tdl_handle, frameInfo, &obj_meta);
            cap.releaseImagePtr();
            image_ptr = nullptr;
            for (uint32_t i = 0; i < obj_meta.size; i++)
            {
                // cv::Mat res;
                cv::Rect r = cv::Rect(obj_meta.info[i].bbox.x1, obj_meta.info[i].bbox.y1, obj_meta.info[i].bbox.x2 - obj_meta.info[i].bbox.x1, obj_meta.info[i].bbox.y2 - obj_meta.info[i].bbox.y1);
                // crop object from image
                // res = bgr(r);
                // printf("x1 = %lf, y1 = %lf, x2 = %lf, y2 = %lf, cls: %d, score: %lf\n", obj_meta.info[i].bbox.x1, obj_meta.info[i].bbox.y1, obj_meta.info[i].bbox.x2, obj_meta.info[i].bbox.y2, obj_meta.info[i].classes, obj_meta.info[i].bbox.score);
                cv::rectangle(frame, r, cv::Scalar(50, 255, 100), 1, 8, 0);
            }
            test.write(frame);
            
            
        } else { // prevent seg fault
            interrupted = 1;
        }
    }    

    printf("Exit...\n");
    clean_up();

    return 0;
}
