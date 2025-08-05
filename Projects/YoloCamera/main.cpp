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

    YoloModelDetector detector;

    cv::Mat bgr;
    cap >> bgr;
    mjpeg.write(bgr);
    mjpeg.start();

    detector.setup_model(argv[1], atoi(argv[2]), 0.7, 0.7);

    while (!interrupted) {
        cv::Mat frame;
        std::pair<void *, void *> imagePtrs = cap.capture(frame);
        void *image_ptr = imagePtrs.first;

        if (image_ptr != nullptr) {
            VIDEO_FRAME_INFO_S *frame_info = reinterpret_cast<VIDEO_FRAME_INFO_S *>(image_ptr);
            cvtdl_object_t objects = detector.detect(frame_info);
            cap.releaseImagePtr();
            image_ptr = nullptr;

            // printf("Size: %d\n", objects.size);

            // Flip the original frame vertically
            //cv::Mat flipped;
            //cv::flip(frame, flipped, 1);  // 0 = vertical flip

            //int height = flipped.rows;

            // Draw vertically flipped bounding boxes
            for (uint32_t i = 0; i < objects.size; i++) {
                int x1 = objects.info[i].bbox.x1;
                int x2 = objects.info[i].bbox.x2;
                int y1 = objects.info[i].bbox.y1;
                int y2 = objects.info[i].bbox.y2;

                // Flip y-coordinates
                int flipped_y1 = y1;
                int flipped_y2 = y2;
		char label[128];
		sprintf(label, "%s: %s", std::to_string(objects.info[i].classes).c_str(), std::to_string(objects.info[i].bbox.score).c_str());
		printf("%s\n", label);

                cv::Rect r = cv::Rect(x1, flipped_y1, x2 - x1, flipped_y2 - flipped_y1);
                cv::rectangle(frame, r, cv::Scalar(0, 255, 0), 5, 8, 0);
                cv::putText(frame, label,
                            cv::Point(x1, flipped_y1 - 5),
                            cv::FONT_HERSHEY_DUPLEX, 1.0, cv::Scalar(0, 255, 0), 1);
            }

            // Send vertically flipped frame to MJPEG stream
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
