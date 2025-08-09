#include "MJPEGWriter.h"
#include "yolo.hpp"
#include <signal.h>
#include <stdio.h>
#include <opencv2/opencv.hpp>

volatile uint8_t interrupted = 0;

void interrupt_handler(int signum) {
    printf("Signal: %d\n", signum);
    interrupted = 1;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, interrupt_handler);


    MJPEGWriter mjpeg(7777);
    cv::VideoCapture cap;

    if (argc < 4) {
        printf("Usage: ./stream_yolo <model_file_path> <class_cnt> <frame_size>\n");
        return 0;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, atoi(argv[3]));
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, atoi(argv[3]));
    cap.open(0);

    if (!cap.isOpened()) {
        printf("Failed to open video capture device.\n");
        return -1;
    }

    cvitdl_handle_t tdl_handle = nullptr;

    // Deepsort
    cvtdl_deepsort_config_t ds_conf;
    cvtdl_tracker_t tracker;

    CVI_S32 ret = CVI_TDL_CreateHandle(&tdl_handle);

    CVI_TDL_DeepSORT_GetDefaultConfig(&ds_conf);

    // ds_conf.max_unmatched_times_for_bbox_matching = 600;
    // ds_conf.ktracker_conf.max_unmatched_num = 600;

    CVI_TDL_DeepSORT_SetConfig(tdl_handle, &ds_conf, -1, true); 
    CVI_TDL_DeepSORT_Init(tdl_handle, false);
    memset(&tracker, 0, sizeof(tracker));

    // Detector
    YoloModelDetector detector(&tdl_handle);

    cv::Mat bgr;
    cap >> bgr;
    mjpeg.write(bgr);
    mjpeg.start();

    detector.setup_model(argv[1], atoi(argv[2]), 0.5, 0.4);

    while (!interrupted) {
        cv::Mat frame;
        std::pair<void *, void *> imagePtrs = cap.capture(frame);
        void *image_ptr = imagePtrs.first;

        if (image_ptr != nullptr) {
            VIDEO_FRAME_INFO_S *frame_info = reinterpret_cast<VIDEO_FRAME_INFO_S *>(image_ptr);
            cvtdl_object_t objects = detector.detect(frame_info);
            cap.releaseImagePtr();
            image_ptr = nullptr;

            printf("Size: %d\n", objects.size);
            CVI_TDL_DeepSORT_Obj(tdl_handle, &objects, &tracker, false);

            for (uint32_t i = 0; i < objects.size; i++) {
                int x1 = objects.info[i].bbox.x1;
                int x2 = objects.info[i].bbox.x2;
                int y1 = objects.info[i].bbox.y1;
                int y2 = objects.info[i].bbox.y2;

                cv::Rect r = cv::Rect(x1, y1, x2 - x1, y2 - y1);
                cv::rectangle(frame, r, cv::Scalar(0, 255, 0), 2);

                // Tracker tested on yolo with 2 classes: person and face
                // Display unique ID (only for persons, ignore faces)
                if (objects.info[i].classes){
                    char id_str[32];
                    sprintf(id_str, "ID: %d", objects.info[i].unique_id);
                    cv::putText(frame, id_str, cv::Point(x1, y1 + 50),
                                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 0, 0), 2);
                }
            }

            mjpeg.write(frame);
            
        } else {
            interrupted = 1;
        }
    }

    printf("Stopping...\n");
    CVI_TDL_DeepSORT_CleanCounter(tdl_handle);
    CVI_TDL_DestroyHandle(tdl_handle);

    cap.release();
    detector.release();

    return 0;
}

