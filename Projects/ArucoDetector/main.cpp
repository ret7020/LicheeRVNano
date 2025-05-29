#include "MJPEGWriter.h"
#include <chrono>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/aruco_detector.hpp>
#include <signal.h>
#include <stdio.h>

#define DEVICE_PATH 0
#define VIDEO_RECORD_FRAME_WIDTH 100
#define VIDEO_RECORD_FRAME_HEIGHT 100

volatile uint8_t interrupted = 0;
void interrupt_handler(int signum) {
    printf("Signal: %d\n", signum);
    interrupted = 1;
}

double avgFps = 0.0;
int framesRead = 0;
int main() {
    signal(SIGINT, interrupt_handler);
    // Camera init
    cv::VideoCapture cap;
    MJPEGWriter test(7777);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, VIDEO_RECORD_FRAME_WIDTH);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, VIDEO_RECORD_FRAME_HEIGHT);
    cap.open(0);

    cv::Mat bgr;
    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_5X5_50);
    cv::aruco::ArucoDetector detector(dictionary, detectorParams);

    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;

    // "Warmup" camera
    for (int i = 0; i < 5; i++) {
        cap >> bgr;
    }

    test.write(bgr);
    test.start();

    printf("Warmup finished\n");

    while (!interrupted){
        cap >> bgr;
        if (bgr.empty()){ printf("Frame empty\n"); }
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        detector.detectMarkers(bgr, markerCorners, markerIds, rejectedCandidates);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        double fps = 1 / std::chrono::duration<double>(end - begin).count();
        printf("Detection FPS: %lf\n", fps);
        printf("Count: %d\n", markerIds.size());
        

        // std::pair<void *, void *> imagePtrs = cap.capture(bgr);
        // void *image_ptr = imagePtrs.first;

        // if (image_ptr != nullptr) {
        //     detector.detectMarkers(bgr, markerCorners, markerIds, rejectedCandidates);
        // } else {
        //     printf("Frame ignored\n");
        // }

        test.write(bgr);
        bgr.release();
    }

    cap.release();

    return 0;
}