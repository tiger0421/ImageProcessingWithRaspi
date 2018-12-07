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
	int LOOPTIME = 3;
	std::vector<int> listX_, listY_, midPoint_;
public:
	Mat hsvImg, mask, filteredImg;

	Search(Mat sharpImg) {
		widch_ = sharpImg.cols;
		high_ = sharpImg.rows;

		cvtColor(sharpImg, hsvImg, CV_BGR2HSV, 3);

		inRange(hsvImg, Scalar(0, 0, 0), Scalar(40, 255, 255), mask);
		medianBlur(mask, filteredImg, 5);
		for (y_ = 0; y_ < high_; y_++) {
			for (i = 0; i < widch_; i++) {
				std::cout << (int)mask.at<uchar>(y_, i)<<std::endl;
			}
		}
		std::cout <<"next" << std::endl;

		for (y_ = 0; y_ < high_; y_++) {
			for (i = 0; i < widch_; i++) {
				std::cout << (int)filteredImg.at<uchar>(y_, i) << std::endl;
				if ((int)filteredImg.at<uchar>(y_, i) == 0) {
					pxNum_ += i;
					std::cout << "ok" << i << "	" << y_ << std::endl;
				}
				pxNum_ /= i;
				pxNum_ -= widch_ / 2;
				listX_.push_back(pxNum_);
				listY_.push_back(y_);
			}
		}
		listX_.clear();
		listY_.clear();
	}
};

int main()
{
	Mat filteredImg, sharpImg;
	Mat img = imread("33.png");
	if (!img.data) return - 1;
	
//	ImgFilter filter;
//	filter.UnsharpMasking(img, out, 4.0);
	float k = 4.0;
	Mat kernel = (cv::Mat_<float>(3, 3) <<
		-k / 9.0f, -k / 9.0f, -k / 9.0f,
		-k / 9.0f, 1 + (8 * k) / 9.0f, -k / 9.0f,
		-k / 9.0f, -k / 9.0f, -k / 9.0f);
	filter2D(img, sharpImg, -1, kernel, Point(-1, -1), 0.0, BORDER_DEFAULT);
	convertScaleAbs(sharpImg, sharpImg, 1, 0);

	Search search(sharpImg);

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