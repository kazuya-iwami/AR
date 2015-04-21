#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/legacy/legacy.hpp"

using namespace std;
using namespace cv;

class Object {

public:
    int x, y;

    int minH, maxH;
    int minS, maxS;
    int minV, maxV;

    bool visible; //視界に入っているかのフラグ

    void init(int minH_, int maxH_, int minS_, int maxS_, int minV_, int maxV_) {
        minH = minH_;
        minS = minS_;
        minV = minV_;
        maxH = maxH_;
        maxS = maxS_;
        maxV = maxV_;

        x = 0;
        y = 0;

        visible = false;
    }

   void detect(Mat image);
};

void Object::detect(Mat image) {

	Object obj;

    // HSVに変換
    Mat hsv;
    cvtColor(image, hsv, COLOR_BGR2HSV);

    // 2値化
    Mat binalized;
    Scalar lower(minH, minS, minV);
    Scalar upper(maxH, maxS, maxV);
    inRange(hsv, lower, upper, binalized);

    // ノイズ除去
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    morphologyEx(binalized, binalized, MORPH_CLOSE, kernel);
    //imshow("morphologyEx", binalized);

    // 輪郭を検出
    std::vector<std::vector<Point>> contours;
    findContours(binalized.clone(), contours, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

    // 一番大きい輪郭を抽出

	Mat output(image.size(),image.type(), Scalar::all(0));

    int contour_index = -1;
    double max_area = 0.0;
	std::vector<double> area;
    for (int i = 0; i < (int)contours.size(); i++) {
        area[i] = fabs(contourArea(contours[i]));
	}
	for(int s = 0; s < i; s++){
		for(int j = 0; j < i; j++){
			int smallarea;
			smallarea = area[i];
			if(area[j] < area[j+1]){
				area[j+1] = area[j];
				area[j] = smallarea;
			}
		}
	}
	i = 0;
		// マーカが見つかった
		while (i < 4) {
        // 重心
			Rect rect;
			Moments moments = cv::moments(contours[i], true);
			y = (int)(moments.m01 / moments.m00);
			x = (int)(moments.m10 / moments.m00);

        // 表示
			rect = boundingRect(contours[i]);

        //drawContours(image, contours, contour_index, Scalar(0,255,0));
			image.copyTo(output ,binalized);
			rectangle(output, rect, Scalar(0, 255, 0));
			visible = true;
		} else {
			visible = false;
		}
        //if (area > max_area) {
        //    contour_index = i;
        //    max_area = area;
        //}
    }

    
    // 表示
    imshow("Captured Image", output);
}

Object obj;

void callbackForLowerH(int lowerH, void *data) {
    obj.minH = lowerH;
}

void callbackForUpperH(int upperH, void *data) {
    obj.maxH = upperH;
}

void callbackForLowerS(int lowerS, void *data) {
    obj.minS = lowerS;
}

void callbackForUpperS(int upperS, void *data) {
    obj.maxS = upperS;
}

void callbackForLowerV(int lowerV, void *data) {
    obj.minV = lowerV;
}

void callbackForUpperV(int upperV, void *data) {
    obj.maxV = upperV;
}

int main(int argc, char *argv[]) {

    Mat image;

    //VideoCapture cap(0); // デフォルトカメラをオープン
    //if(!cap.isOpened())  // 成功したかどうかをチェック
    //   return -1;

	image = imread("1428928245212.jpg", 1);

    int lowerH = 0;
    int upperH = 180;
    int lowerS = 0;
    int upperS = 255;
    int lowerV = 0;
    int upperV = 255;
    obj.init(lowerH, upperH, lowerS, upperS, lowerV, upperV);
    namedWindow("Captured Image", WINDOW_NORMAL);

    createTrackbar("Lower Hue/2", "Captured Image", &lowerH, 180, callbackForLowerH);
    createTrackbar("Upper Hue/2", "Captured Image", &upperH, 180, callbackForUpperH);
    createTrackbar("Lower Sat", "Captured Image", &lowerS, 255, callbackForLowerS);
    createTrackbar("Upper Sat", "Captured Image", &upperS, 255, callbackForUpperS);
    createTrackbar("Lower Val", "Captured Image", &lowerV, 255, callbackForLowerV);
    createTrackbar("Upper Val", "Captured Image", &upperV, 255, callbackForUpperV);

	for(;;){
		obj.detect(image);

		if( waitKey(1) == 'q') break;

	}
    return 0;
}

