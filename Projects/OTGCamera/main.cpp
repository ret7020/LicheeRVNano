#include <linux/videodev2.h>
#include <opencv2/opencv.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <cstring>
#include <chrono>

#define DEBUG
#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480
#define DEVICE "/dev/video0"

struct Buffer
{
	void *start;
	size_t length;
};
#define CLEAR(x) memset(&(x), 0, sizeof(x))

int main()
{
	const char *device = DEVICE;
	int fd = open(device, O_RDWR);
	if (fd == -1)
	{
		perror("Opening video device");
		return -1;
	}
	// Query device capabilities
	struct v4l2_capability cap;
	if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1)
	{
		perror("Querying Capabilities");
		close(fd);
		return -1;
	}
#ifdef DEBUG
	printf("Driver: %s\nCard: %s\nVersion: %d.%d.%d", cap.driver, cap.card,
		   ((cap.version >> 16) & 0xFF), ((cap.version >> 8) & 0xFF), (cap.version & 0xFF));
#endif

	struct v4l2_format fmt;
	CLEAR(fmt);
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = IMAGE_WIDTH;   // Image width
	fmt.fmt.pix.height = IMAGE_HEIGHT; // Image height
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

	if (ioctl(fd, VIDIOC_S_FMT, &fmt) == -1)
	{
		perror("Setting Pixel Format");
		close(fd);
		return -1;
	}

	struct v4l2_requestbuffers req;
	CLEAR(req);
	req.count = 1;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1)
	{
		perror("Requesting Buffer");
		close(fd);
		return -1;
	}

	// Query buffer to map memory
	struct v4l2_buffer buf;
	CLEAR(buf);
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = 0;

	if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1)
	{
		perror("Querying Buffer");
		close(fd);
		return -1;
	}

	Buffer buffer;
	buffer.length = buf.length;
	buffer.start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);

	if (buffer.start == MAP_FAILED)
	{
		perror("Mapping Buffer");
		close(fd);
		return -1;
	}

	// Start streaming
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_STREAMON, &type) == -1)
	{
		perror("Starting Stream");
		close(fd);
		return -1;
	}

	// Capture loop
	cv::Mat yuyv(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC2, buffer.start);
	cv::Mat bgr;
	auto last = std::chrono::steady_clock::now();
	auto curr = std::chrono::steady_clock::now();
	while (true)
	{
		CLEAR(buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;

		if (ioctl(fd, VIDIOC_QBUF, &buf) == -1)
		{
			perror("Queue Buffer");
			break;
		}

		if (ioctl(fd, VIDIOC_DQBUF, &buf) == -1)
		{
			perror("Dequeue Buffer");
			break;
		}
		
		cv::cvtColor(yuyv, bgr, cv::COLOR_YUV2BGR_YUYV);
		curr = std::chrono::steady_clock::now();
		printf("Frame latency: %lf\n", std::chrono::duration<double>(curr - last).count());
		last = curr;
		// cv::imwrite("res.jpg", bgr);
	}

	return 0;
}
