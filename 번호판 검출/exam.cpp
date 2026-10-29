#include "preprocess.hpp"					// 전처리 및 후보영역 검출 함수
#include "candiate.hpp"						// 후보영역 개선 및 후보영상 생성 함수
#include "SVM.hpp"						// 학습데이터 로드 및 SVM 수행
#include "kNN.hpp"						// kNN 학습 함수 – 10장 3절
#include "classify_objects.hpp"			// kNN 분류 함수

int main()
{
	int K1 = 15, K2 = 15;
	Ptr<ml::KNearest> knn[2];
	knn[0] = kNN_train("./image/trainimage/number.png", K1, 10, 20); // 숫자
	knn[1] = kNN_train("./image/trainimage/real.png", K2, 25, 20); // 텍스트
	// 	학습된 데이터 로드
	Ptr<ml::SVM> svm = ml::StatModel::load<ml::SVM>("./SVMtrain.xml");

	int car_no;
	while (1) {
		cout << "차량 영상 번호 (0-19) : ";
		cin >> car_no;
		if (cin.fail() || car_no == 8 || car_no == 18 || car_no >= 20) { 
			cout << "정수로 0~20 사이의 숫자를 다시 입력해보세요. ( 8, 18 제외 )\n";
		}
		else break;
	}
	string fn = format("./image/test_car/%02d.jpg", car_no);
	Mat image = imread(fn, 1);
	CV_Assert(image.data);

	Mat morph = preprocessing(image);								// 전처리
	vector<RotatedRect> candidates;
	find_candidates(morph, candidates);									// 후보 영역 검출
	vector<Mat> candidate_img = make_candidates(image, candidates);// 후보 영상 생성

	int plate_no = classify_plates(svm, candidate_img);			// SVM 분류

	if (plate_no >= 0)
	{
		vector <Rect> obejct_rects, sorted_rects;
		vector <Mat> numbers;							// 숫자 객체 
		Mat  plate_img, color_plate;								// 컬러 번호판 영상 

		preprocessing_plate(candidate_img[plate_no], plate_img);	// 번호판 영상 전처리
		cvtColor(plate_img, color_plate, COLOR_GRAY2BGR);

		find_objects(~plate_img, obejct_rects);		// 숫자 및 문자 검출  

		sort_rects(obejct_rects, sorted_rects);			// 검출객체 정렬(x 좌표기준)

		for (size_t i = 0; i < sorted_rects.size(); i++)
		{
			numbers.push_back(plate_img(sorted_rects[i]));	// 정렬된 숫자 영상
			rectangle(color_plate, sorted_rects[i], Scalar(0, 0, 255), 1); // 사각형 그리기
		}

		if (numbers.size() == 7) { // numbers.size() >= 5 && numbers.size() <= 9 객체가 많은 경우 대비
			classify_numbers(numbers, knn, K1, K2);		// kNN 분류 수행
		}
		else  cout << "숫자(문자) 객체를 정확히 검출하지 못했습니다." << endl;

		imshow("번호판 영상", color_plate);					// 번호판 영상 표시
		draw_rotatedRect(image, candidates[plate_no], Scalar(0, 0, 255), 1);
	}
	else  cout << "번호판을 검출하지 못하였습니다. " << endl;

	imshow("image", image);
	waitKey(0);
	return 0;
}

/*
TODO:
테스트 result ( 최종 프로그램 )
00 o
01 x
02 x
03 o
04 o
05 x
06 o 
07 x
09 o ( 번호판 뒤집어짐 )
10 x
11 o
12 o
13 o ( 저를 서로 인식 11번 이미지의 앞)
14 x
15 x
16 o ( 마를 라로 인식 )
17 o
19 o
서, 어, 오, 저 없음
*/