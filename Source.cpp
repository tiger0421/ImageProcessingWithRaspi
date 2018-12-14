#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
using namespace cv;


class Coordnt {
private:
	std::vector<int> Xcoordinate_;
	std::vector<int> Ycoordinate_;
public:
	Coordnt(int X, int Y) {
		Xcoordinate_.push_back(X);
		Ycoordinate_.push_back(Y);
	}
	void add(int X, int Y) {
		Xcoordinate_.push_back(X);
		Ycoordinate_.push_back(Y);
	}
	void resize(int size) {
		Xcoordinate_.resize(size);
		Ycoordinate_.resize(size);
	}
	void print(int num) {
		std::cout << "Xcoordinate=" << Xcoordinate_[num] << std::endl;
		std::cout << "Ycoordinate=" << Ycoordinate_[num] << std::endl;
	}
};

class ImgFilter {
private:
	Mat kernel;
public:
	void UnsharpMasking(Mat src, Mat dst, float k) {
		kernel = (cv::Mat_<float>(3, 3) <<
			-k / 9.0f, -k / 9.0f, -k / 9.0f,
			-k / 9.0f, 1 + (8 * k) / 9.0f, -k / 9.0f,
			-k / 9.0f, -k / 9.0f, -k / 9.0f);
		filter2D(src, dst, -1, kernel, Point(-1, -1), 0.0, BORDER_DEFAULT);
	}
};

class leastSquare {
private:
	int i;
	double a0, a1;
	double A00, A01, A02, A11, A12;
public:
	leastSquare(std::vector<int> listX, std::vector<int> listY) {
		A00 = A01 = A02 = A11 = A12 = 0.0;
		for (i = 0; i < listX.size(); i++) {
			A00 += 1.0;
			A01 += listX[i];
			A02 += listY[i];
			A11 += listX[i] * listX[i];
			A12 += listX[i] * listY[i];
		}
		a0 = (A02 * A11 - A01 * A12) / (A00 * A11 - A01 * A01);
		a1 = (A00 * A12 - A01 * A02) / (A00 * A11 - A01 * A01);
		std::cout << a0 << a1 << std::endl;
	}
};

class Search {
private:
	int pxNum_=0;
	int widch_, high_;
	int LEdge_, REdge_;
	int th_ = 100;
	int i;
	int y_=0;
	int count_;
	int LOOPTIME = 3;
	int MAXHIGH = 150;
	int RANGE = MAXHIGH/LOOPTIME;
	float k = 4.0;
	std::vector<int> listX_, listY_, midPoint_;
public:
	Mat hsvRoi, maskRoi, filteredImg;
	Mat cpRoi, dstImg, imgRoi, filterRoi;

	Search(Mat Img) {
		widch_ = Img.cols;
		high_ = Img.rows;
		dstImg = Img.clone();

		for (y_ = 0; y_ < LOOPTIME; y_++) {
			Rect roi(0, RANGE*y_, widch_, 3);
			imgRoi = Img(roi);
			cpRoi = dstImg(roi);
/*
			Mat kernel = (cv::Mat_<float>(3, 3) <<
				-k / 9.0f, -k / 9.0f, -k / 9.0f,
				-k / 9.0f, 1 + (8 * k) / 9.0f, -k / 9.0f,
				-k / 9.0f, -k / 9.0f, -k / 9.0f);
			filter2D(imgRoi, cpRoi, -1, kernel, Point(-1, -1), 0.0, BORDER_DEFAULT);
			convertScaleAbs(cpRoi, cpRoi, 1, 0);
*/
			cvtColor(cpRoi, hsvRoi, CV_BGR2HSV, 3);
			inRange(hsvRoi, Scalar(0, 0, 0), Scalar(45, 255, 255), maskRoi);
			medianBlur(maskRoi, filterRoi, 5);

			for (i = 1, count_ = 1; i <= widch_; i++) {
				if ((int)filterRoi.at<uchar>(1, i-1) == 255) {
					pxNum_ += i;
					count_++;
				}
			}
			pxNum_ /= count_;
			pxNum_ -= widch_ / 2;
			listX_.push_back(pxNum_);
			listY_.push_back(RANGE*y_);
			circle(Img, Point(pxNum_+(widch_/2), RANGE*y_), 10, Scalar(0, 100, 0), -1, CV_AA);
		}
		leastSquare(listX_, listY_);
		listX_.clear();
		listY_.clear();
		namedWindow("filtered", cv::WINDOW_NORMAL);
		imshow("filtered", Img);
		int key = waitKey(1);
		if (key == 113) {
//			imwrite("result.png", Img);
			exit(0);
		}

	}
};

int main(){
	Mat Img;
	Mat filteredImg, sharpImg;
//	Mat img = imread("33.png");
//	if (!img.data) return - 1;

	VideoCapture cap(2);
	if (!cap.isOpened())	return -1;

	while (1) {
		cap >> Img;
		//	ImgFilter filter;
		//	filter.UnsharpMasking(img, out, 4.0);

		Search search(Img);
	}
	destroyAllWindows();


	/*
	namedWindow("WINDOW_NAME", cv::WINDOW_NORMAL);
	imshow("WINDOW_NAME", img);
	waitKey(0);
	destroyAllWindows();
	*/
	return 0;
}



/*==========================================================
1. getting img
2. masking(Unsharp -> HSV_threshold -> median)
3. calculate the middle point(area of a line to weight)
4. linearized by least squares method
5. communication with arduino
6. run motor
*/