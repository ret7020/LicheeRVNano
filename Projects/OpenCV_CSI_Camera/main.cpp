#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <chrono>


#define VIDEO_RECORD_FRAME_WIDTH 640
#define VIDEO_RECORD_FRAME_HEIGHT 640


int main()
{

	cv::VideoCapture cap;
	// cap.set(cv::CAP_PROP_FRAME_WIDTH, VIDEO_RECORD_FRAME_WIDTH);
	// cap.set(cv::CAP_PROP_FRAME_HEIGHT, VIDEO_RECORD_FRAME_HEIGHT);
	cap.open(0);

	cv::Mat bgr;

	// "Warmup" camera
	for (int i = 0; i < 5; i++) cap >> bgr;

	for (int i = 0; i < 25; i++)
	{
		std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
		cap >> bgr;
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		printf("Get frame - OK\n");
		double fps = 1 / std::chrono::duration<double>(end - begin).count();
		printf("%lf", fps);
		if (bgr.empty())
			break;
		cv::imwrite("captured.jpg", bgr);
	}

	cap.release();

	return 0;
}
