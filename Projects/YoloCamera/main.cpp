#include "MJPEGWriter.h"
#include "yolo.hpp"
#include <signal.h>
#include <stdio.h>

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

    YoloModelDetector detector;

    cv::Mat bgr;
    cap >> bgr;
    mjpeg.write(bgr);
    mjpeg.start();

    detector.setup_model(argv[1], atoi(argv[2]), 0.4, 0.4);

    while (!interrupted) {
        cv::Mat frame;
        std::pair<void *, void *> imagePtrs = cap.capture(frame);
        void *image_ptr = imagePtrs.first;
        if (image_ptr != nullptr) {
            VIDEO_FRAME_INFO_S *frame_info = reinterpret_cast<VIDEO_FRAME_INFO_S *>(image_ptr);
            cvtdl_object_t objects = detector.detect(frame_info);
            cap.releaseImagePtr();
            image_ptr = nullptr;
            for (uint32_t i = 0; i < objects.size; i++) {
                cv::Rect r = cv::Rect(objects.info[i].bbox.x1, objects.info[i].bbox.y1,
                                      objects.info[i].bbox.x2 - objects.info[i].bbox.x1,
                                      objects.info[i].bbox.y2 - objects.info[i].bbox.y1);
                cv::rectangle(frame, r, cv::Scalar(0, 255, 0), 1, 8, 0);
                cv::putText(frame, std::to_string(objects.info[i].classes), cv::Point(objects.info[i].bbox.x1, objects.info[i].bbox.y1 - 5),
                            cv::FONT_HERSHEY_DUPLEX, 1.0, cv::Scalar(0, 255, 0), 1);
            }
            mjpeg.write(frame);
        } else {
            interrupted = 1;
        }
    }
    printf("Stopping...\n");

    cap.release();
    detector.release();

    return 0;
}
