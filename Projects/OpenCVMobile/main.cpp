#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>

// This code will generate red/green/blue images and save to red.jpg green.jpg blue.jpg

int main()
{
	printf("Generating images test");
	cv::Mat redImg(480, 640, CV_8UC3, cv::Scalar(0, 0, 255));
	cv::imwrite("red.jpg", redImg);

	cv::Mat greenImg(480, 640, CV_8UC3, cv::Scalar(0, 255, 0));
	cv::imwrite("green.jpg", greenImg);

	cv::Mat blueImg(480, 640, CV_8UC3, cv::Scalar(255, 0, 0));
	cv::imwrite("blue.jpg", blueImg);
	return 0;
}
