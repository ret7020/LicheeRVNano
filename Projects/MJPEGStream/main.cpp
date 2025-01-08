#include "MJPEGWriter.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

volatile uint8_t interrupted = 0;

void interrupt_handler(int signum)
{
    printf("Signal: %d\n", signum);
    interrupted = 1;
}

int main()
{
    signal(SIGINT, interrupt_handler);

    cv::VideoCapture cap;
    cv::Mat bgr;

    cap.open(0);
    MJPEGWriter test(7777);

    cap >> bgr;
    cv::cvtColor(bgr, bgr, cv::COLOR_BGR2GRAY);

    test.write(bgr);
    test.start();

    while (!interrupted)
    {
        cap >> bgr;
        cv::cvtColor(bgr, bgr, cv::COLOR_BGR2GRAY);
        test.write(bgr);
        bgr.release();
    }

    printf("Stopping stream:\n");
    test.stop();
    cap.release();

    return 0;
}
