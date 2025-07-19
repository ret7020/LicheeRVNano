#include "MJPEGWriter.h"
#include <opencv2/opencv.hpp>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>

volatile uint8_t interrupted = 0;

#define SHM_NAME "/frame_shm"
#define FRAME_WIDTH 320
#define FRAME_HEIGHT 240
#define CHANNELS 3
#define FRAME_SIZE (FRAME_WIDTH * FRAME_HEIGHT * CHANNELS)

void interrupt_handler(int signum)
{
    printf("Signal: %d\n", signum);
    interrupted = 1;
}

int main(int argc, char* argv[])
{
    signal(SIGINT, interrupt_handler);

    cv::Mat bgr;

    MJPEGWriter test(7777);

    int shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (shm_fd == -1) {
        printf("Can't open shm\n");
        return 1;
    }
    void* ptr = mmap(0, FRAME_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        std::cerr << "Failed to mmap" << std::endl;
        return 1;
    }

    test.start();

    while (!interrupted) {
        cv::Mat frame(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC3, ptr);
        test.write(frame);
        usleep(1000);
    }

    munmap(ptr, FRAME_SIZE);
    close(shm_fd);
    test.stop();


    // 
    // test.start();

    // while (!interrupted)
    // {
    //     cap >> bgr;
    //     //cv::cvtColor(bgr, bgr, cv::COLOR_BGR2GRAY);
    //     test.write(bgr);
    //     bgr.release();
    // }

    // printf("Stopping stream:\n");
    // test.stop();


    return 0;
}
