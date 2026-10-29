#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

Mat  preprocessing(Mat image)
{
	Mat gray, th_img, morph;
	Mat kernel(3, 17, CV_8UC1, Scalar(1));		// 닫힘 연산 마스크 커널 필터 크기 조정 3, 17 (best)
	cvtColor(image, gray, COLOR_BGR2GRAY);		// 명암도 영상 변환
	blur(gray, gray, Size(5, 5));				// 블러링
	Sobel(gray, gray, CV_8U, 1, 0, 3);			// 소벨 에지 검출 Sobel(gray, gray, CV_8U, 1, 0, 3);

	// threshold(gray, th_img, 120, 255, THRESH_BINARY); // 원본 이진화 수행
	//threshold(gray, th_img, 65, 255, THRESH_BINARY);	// 개선
	threshold(gray, th_img, 0, 255, THRESH_BINARY | THRESH_OTSU); // 참고 https://bkshin.tistory.com/entry/OpenCV-8-%EC%8A%A4%EB%A0%88%EC%8B%9C%ED%99%80%EB%94%A9Thresholding
	morphologyEx(th_img, morph, MORPH_CLOSE, kernel, Point(-1, -1), 1);	// 열림 연산 수행 , Point(-1, -1), 2
	//imshow("th_img", th_img), imshow("morph", morph);
	imshow("sobel", gray);
	imshow("th_img", th_img);
	return morph;
}

bool vertify_plate(RotatedRect mr)
{
	float size = mr.size.area();	
	float aspect = (float)mr.size.height / mr.size.width;	// 종횡비 계산
	if (aspect < 1)  aspect = 1 / aspect;

	bool  ch1 = size > 750 && size < 65000;		// 번호판 넓이 조건 2000, 30000, 1500, 35000
	bool  ch2 = aspect > 1.0 && aspect < 9.0;		// 번호판 종횡비 조건 1.3, 6.4

	//bool ch1 = size > 700 && size < 160000;
	//bool ch2 = aspect > 0.9 && aspect < 12.0;

	return ch1 && ch2;
}

void find_candidates(Mat img, vector<RotatedRect>& candidates)
{
	vector< vector< Point> > contours;				// 외곽선
	// 외곽선 검출
	findContours(img.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);  // findContours(img.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < (int)contours.size(); i++)	// 검출 외곽선 조회
	{
		//if (contours[i].size() < 3) continue; // 번호판이 아닌 다른 노이즈 제거용 5 -> 3 차이 없어서 제외
		RotatedRect  rot_rect = minAreaRect(contours[i]);	// 외곽선 최소영역 회전사각형
		if (vertify_plate(rot_rect))						// 번호판 검증
			candidates.push_back(rot_rect);
	}			// 회전사각형 저장
}


void draw_rotatedRect(Mat& img, RotatedRect mr, Scalar color, int thickness = 2)
{
	Point2f  pts[4];
	mr.points(pts);

	for (int i = 0; i < 4; ++i) {
		line(img, pts[i], pts[(i + 1) % 4], color, thickness);
	}
}
