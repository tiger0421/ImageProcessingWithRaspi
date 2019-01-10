#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
//#include <wiringSerial.h>
//#include <wiringPi.h>
#include <chrono>
using namespace cv;

//int fd = serialOpen("/dev/ttyACM0", 9600);
int width_, high_;
int LOOPTIME = 3;
int MAXHIGH = 200;
int RANGE = MAXHIGH / LOOPTIME;
double Xu, Xd;
double Yu, Yd = 0;
auto avetime = 0;
int count = 0;

/*
class Communicate {
private:
	char var;
public:
	Communicate(double a0, double a1) {
		if (a1 < 0) {
			if (a0 < 0) {
				if (a0 > -1.5) {
					var = 'R';
				}
				else {
					var = 'r';
				}
			}
			else {
				if (a0 < 3) {
					var = 'L';
				}
				else {
					var = 's';
				}
			}
		}
		else if (a1 > 0) {
			if (a0 < 0) {
				if (a0 > -1.5) {
					var = 'R';
				}
				else {
					var = 's';
				}
			}
			else {
				if (a0 < 3) {
					var = 'L';
				}
				else {
					var = 'l';
				}
			}
		}
		else {
			var = 'q';
		}
		std::cout << var << std::endl;
		serialPutchar(fd, var);
	}
};
*/
class leastSquare {
private:
	int i;
	double a0, a1, b0, b1;
	double A00, A01, A02, A11, A12;
public:
	leastSquare(std::vector<int> listX, std::vector<int> listY) {

		A00 = A01 = A02 = A11 = A12 = 0.0;


		b0 = (double)(listY[LOOPTIME - 1] - listY[0]) / (listX[LOOPTIME - 1] - listX[0]);
		for (i = 0; i < LOOPTIME; i++) {
			A00 += listX[i];
		}
		b1 = (double)A00 / LOOPTIME;
		A00 = 0.0;
//std::cout << b0 << "	" << b1 << "	" ;

						for (i = 0; i < LOOPTIME; i++) {
								A00 += 1.0;
								A01 += listX[i];
								A02 += listY[i];
								A11 += listX[i] * listX[i];
								A12 += listX[i] * listY[i];

						}

						a0 = (A02 * A11 - A01 * A12) / (A00 * A11 - A01 * A01);
						a1 = (A00 * A12 - A01 * A02) / (A00 * A11 - A01 * A01);
						Xu = (high_ - a1) / a0 + (width_ / 2);
						Xd = - a1 / a0 + (width_ / 2);
//		std::cout << a0 << "	" << Xu << "	" << Xd <<std::endl;

		std::cout<<a0<<"      "<<b0 <<std::endl;
						
//		Communicate communicate(a0, a1);

	}
};

class Search {
private:
	int aveXCoordinate_ = 0;
	int i;
	int y_ = 0;
	int count_;
	std::vector<int> listX_, listY_, midPoint_;
	Mat hsvRoi, maskRoi;
	Mat cpRoi, cpImg, imgRoi, filteredRoi;
public:
	Search(Mat Img) {
		cpImg = Img.clone();
		auto start = std::chrono::system_clock::now();

		for (y_ = 0; y_ < LOOPTIME; y_++) {
			Rect roi(0, high_ - RANGE*y_ -3, width_, 3);
			imgRoi = Img(roi);
			cpRoi = cpImg(roi);
			cvtColor(cpRoi, hsvRoi, CV_BGR2HSV, 3);

			inRange(hsvRoi, Scalar(0, 0, 0), Scalar(180, 255, 170), maskRoi);
			medianBlur(maskRoi, filteredRoi, 3);

			for (i = 1, count_ = 1; i <= width_; i++) {
				if ((int)filteredRoi.at<uchar>(1, i - 1) == 255) {
					aveXCoordinate_ += i;
					count_++;
				}
			}
			aveXCoordinate_ /= count_;
			aveXCoordinate_ -= width_ / 2;
			listX_.push_back(aveXCoordinate_);
			listY_.push_back(RANGE*y_);
//			circle(Img, Point(aveXCoordinate_ + (width_ / 2), high_ - RANGE*y_), 10, Scalar(142, 255, 142), -1, CV_AA);
		}
		leastSquare(listX_, listY_);
//		line(Img, Point((int)Xd, (int)Yd), Point((int)Xu, (int)Yu), Scalar(0, 255, 255), 10, CV_AA);

/*
		auto end = std::chrono::system_clock::now();
		auto dur = end - start;
		auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
		avetime += msec;
		std::cout<<(double)avetime/count<<std::endl;
*/
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

int main() {
	Mat Img;

//	if (fd < 0) exit(1);

	VideoCapture cap(2);
	if (!cap.isOpened())    return -1;
	cap >> Img;
	width_ = Img.cols;
	high_ = Img.rows;
	Yu = high_ - 1;

	while (count<100) {
		cap >> Img;
		count++;

		Search search(Img);
	}
			/*
			namedWindow("WINDOW_NAME", cv::WINDOW_NORMAL);
			imshow("WINDOW_NAME", img);
			waitKey(0);
			destroyAllWindows();
			*/
	destroyAllWindows();
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