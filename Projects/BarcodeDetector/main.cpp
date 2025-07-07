#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/objdetect/barcode.hpp>
#include "MJPEGWriter.h"
#include <opencv2/opencv.hpp>

volatile uint8_t interrupted = 0;

void interrupt_handler(int signum)
{
    printf("Signal: %d\n", signum);
    interrupted = 1;
}

int main() {

    signal(SIGINT, interrupt_handler);

    cv::VideoCapture cap;
    cv::barcode::BarcodeDetector detector;
    MJPEGWriter test(7777);
    cv::Mat bgr;

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 640);


    cap.open(0);

    cap >> bgr;
    test.write(bgr);
    test.start();

    while (!interrupted) {
        cap >> bgr;
        if (bgr.empty()) {
            interrupted = 1;
            break;
        }

        std::vector<std::string> decodedInfo;
        std::vector<std::string> decodedTypes;
        cv::Mat points;  // You can also use std::vector<std::vector<cv::Point2f>> if you prefer
        bool success = detector.detectAndDecodeWithType(bgr, decodedInfo, decodedTypes, points);
        printf("Success: %d\n", success);
        if (success && !decodedInfo.empty()) {
            for (size_t i = 0; i < decodedInfo.size(); ++i) {
                printf("Decoded Barcode: %s, Type: %s\n", decodedInfo[i].c_str(), decodedTypes[i].c_str());
            }
        } else {
            std::cout << "No barcode detected or failed to decode." << std::endl;
        }

        test.write(bgr);
        bgr.release();
    }


    

    return 0;
}
