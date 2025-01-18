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
#include <dirent.h>

#define MODEL_SCALE 0.0039216
#define MODEL_MEAN 0.0
#define MODEL_CLASS_CNT 80
#define MODEL_THRESH 0.8
#define MODEL_NMS_THRESH 0.8

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
    int vpssgrp_width = 1920;
    int vpssgrp_height = 1080;
    CVI_S32 ret = MMF_INIT_HELPER2(vpssgrp_width, vpssgrp_height, PIXEL_FORMAT_RGB_888, 1,
                                   vpssgrp_width, vpssgrp_height, PIXEL_FORMAT_RGB_888, 1);
    if (ret != CVI_TDL_SUCCESS)
    {
        printf("Init sys failed with %#x!\n", ret);
        return ret;
    }

    cvitdl_handle_t tdl_handle = NULL;
    ret = CVI_TDL_CreateHandle(&tdl_handle);
    if (ret != CVI_SUCCESS)
    {
        printf("Create tdl handle failed with %#x!\n", ret);
        return ret;
    }

    // change param of yolov8_detection
    ret = init_param(tdl_handle);

    printf("---------------------openmodel-----------------------");
    ret = CVI_TDL_OpenModel(tdl_handle, CVI_TDL_SUPPORTED_MODEL_YOLOV8_DETECTION, argv[1]);

    if (ret != CVI_SUCCESS)
    {
        printf("open model failed with %#x!\n", ret);
        return ret;
    }
    printf("---------------------to do detection-----------------------\n");



    imgprocess_t img_handle;
    CVI_TDL_Create_ImageProcessor(&img_handle);


    DIR* dirp = opendir(argv[2]);
    struct dirent * dp;
    double fps = 0.0;
    double fps_sum = 0.0;
    double fps_min = 1000.0;
    double fps_max = 0.0;
    int counter = 0;

    while ((dp = readdir(dirp)) != NULL) {
	if (!strcmp(dp->d_name, "..") || !strcmp(dp->d_name, ".")) continue; // Skip ../ path
        VIDEO_FRAME_INFO_S bg;
        char absFilePath[512];
        sprintf(absFilePath, "%s/%s", argv[2], dp->d_name);
        ret = CVI_TDL_ReadImage(img_handle, absFilePath, &bg, PIXEL_FORMAT_RGB_888_PLANAR);
        cvtdl_object_t obj_meta = {0};
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        CVI_TDL_YOLOV8_Detection(tdl_handle, &bg, &obj_meta);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        fps = 1 / std::chrono::duration<double>(end - begin).count();
        fps_sum += fps;
        fps_max = std::max(fps_max, fps);
        fps_min = std::min(fps_min, fps);

        printf("IMG %s, %d: %lf\n", dp->d_name, counter, fps);
        counter++;
        CVI_TDL_ReleaseImage(img_handle, &bg);
    }
    fps_sum = fps_sum / counter;
    printf("\n\n-------\nProcessed images: %d\nAVG FPS: %lf\nMin FPS: %lf\nMax FPS: %lf\n-------\n", counter, fps_sum, fps_min, fps_max);

    CVI_TDL_Destroy_ImageProcessor(img_handle);
    CVI_TDL_DestroyHandle(tdl_handle);
    
    
    return ret;
}
