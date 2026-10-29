#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

void Mouse(int, int, int, int, void*);

Scalar blue(255, 0, 0), red(0, 0, 255), green(0, 255, 0), magenta(255, 0, 255), black(0, 0, 0), white(255, 255, 255);
Mat image(800, 1600, CV_8UC3, white);
Point pt1(200, 200), pt2(200, 400), pt3(400, 400), pt4(400, 200), center=(Point)image.size()/2;

int main()
{
	namedWindow("창");
	setMouseCallback("창", Mouse, 0);
	while (1) {
		int key = waitKey(1);
		if (key == 27) break;
		switch (key)
		{
		case 'a': putText(image, "Text", pt1, FONT_HERSHEY_DUPLEX, 2, red); break;
		case 'b': putText(image, "Text", pt4, FONT_HERSHEY_DUPLEX, 2, green); break;
		case 'c': putText(image, "Text", pt2, FONT_HERSHEY_DUPLEX, 2, blue); break;
		case 'd': putText(image, "Text", pt3, FONT_HERSHEY_DUPLEX, 2, black); break;
		case'0': ellipse(image, center, Size(200, 100), 90, 0, 360, magenta, 4); break;
		case 'o': circle(image, center, 300, red); break;
		case 0x4F: circle(image, center, 600, blue); break; // 대문자 o
		}
		imshow("창", image);
	}
	return 0;
}

void Mouse(int event, int x, int y, int flags, void* param)
{
	switch (event)			
	{
	case EVENT_LBUTTONDOWN:	line(image, pt1, pt2, green, 2, LINE_AA); break;
	case EVENT_RBUTTONDOWN:	line(image, pt3, pt4, black, 2, LINE_AA);	break;
	case EVENT_MOUSEWHEEL: ellipse(image, center, Size(600, 400), 0, 0, 360, magenta, 4); break;
	}
}