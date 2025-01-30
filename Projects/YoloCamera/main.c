#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "core/cvi_tdl_types_mem_internal.h"
#include "core/utils/vpss_helper.h"
#include "cvi_tdl.h"
#include "cvi_tdl_media.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define MODEL_SCALE 0.0039216
#define MODEL_MEAN 0.0
#define MODEL_CLASS_CNT 2
#define MODEL_THRESH 0.2
#define MODEL_NMS_THRESH 0.2

// set preprocess and algorithm param for yolov8 detection
// if use official model, no need to change param (call this function)
CVI_S32 init_param(const cvitdl_handle_t tdl_handle)
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

    // setup yolo algorithm preprocess
    YoloAlgParam yolov8_param =
        CVI_TDL_Get_YOLO_Algparam(tdl_handle, CVI_TDL_SUPPORTED_MODEL_YOLOV8_DETECTION);
    yolov8_param.cls = MODEL_CLASS_CNT;

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

int main(int argc, char *argv[])
{

    cv::VideoCapture cap;
    cap.open(0);
    cv::Mat bgr;
    
    
    

    CVI_S32 ret;
    // VSSGRP already inited by VideoCapture from OpenCV Mobile, second init will do some strange and cause memory problems
    // int vpssgrp_width = 1920;
    // int vpssgrp_height = 1080;
    // CVI_S32 ret = MMF_INIT_HELPER2(vpssgrp_width, vpssgrp_height, PIXEL_FORMAT_RGB_888, 1,
    //                                vpssgrp_width, vpssgrp_height, PIXEL_FORMAT_RGB_888, 1);
    // if (ret != CVI_TDL_SUCCESS)
    // {
    //     printf("Init sys failed with %#x!\n", ret);
    //     return ret;
    // }

    cvitdl_handle_t tdl_handle = NULL;
    ret = CVI_TDL_CreateHandle(&tdl_handle);
    if (ret != CVI_SUCCESS)
    {
        printf("Create tdl handle failed with %#x!\n", ret);
        return ret;
    }

    std::string strf1(argv[2]);

    // change param of yolov8_detection
    ret = init_param(tdl_handle);

    ret = CVI_TDL_OpenModel(tdl_handle, CVI_TDL_SUPPORTED_MODEL_YOLOV8_DETECTION, argv[1]);

    if (ret != CVI_SUCCESS)
    {
        printf("open model failed with %#x!\n", ret);
        return ret;
    }
    imgprocess_t img_handle;
    CVI_TDL_Create_ImageProcessor(&img_handle);

    VIDEO_FRAME_INFO_S bg;
    ret = CVI_TDL_ReadImage(img_handle, strf1.c_str(), &bg, PIXEL_FORMAT_RGB_888_PLANAR);
    if (ret != CVI_SUCCESS)
    {
        printf("open img failed with %#x!\n", ret);
        return ret;
    }
    else
    {
        printf("image read,width:%d\n", bg.stVFrame.u32Width);
        printf("image read,hidth:%d\n", bg.stVFrame.u32Height);
    }

    cvtdl_object_t obj_meta = {0};
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    CVI_TDL_YOLOV8_Detection(tdl_handle, &bg, &obj_meta);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    double fps = 1 / std::chrono::duration<double>(end - begin).count();
    printf("\n\n----------\nDetection FPS: %lf\nDetected objects cnt: %d\n\nDetected objects:\n", fps, obj_meta.size);
    for (uint32_t i = 0; i < obj_meta.size; i++)
    {
        printf("x1 = %lf, y1 = %lf, x2 = %lf, y2 = %lf, cls: %d, score: %lf\n", obj_meta.info[i].bbox.x1, obj_meta.info[i].bbox.y1, obj_meta.info[i].bbox.x2, obj_meta.info[i].bbox.y2, obj_meta.info[i].classes, obj_meta.info[i].bbox.score);
    }

    cap >> bgr;
    cv::imwrite("captured.jpg", bgr);
    cap.release();

    CVI_TDL_ReleaseImage(img_handle, &bg);
    CVI_TDL_DestroyHandle(tdl_handle);
    CVI_TDL_Destroy_ImageProcessor(img_handle);


    

    
    return ret;
}
