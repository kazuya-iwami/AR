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

    IplImage *detect(IplImage *image);
};

IplImage *Object::detect(IplImage *image) {

    // HSVに変換
    IplImage *hsv = cvCloneImage(image);
    cvCvtColor(image, hsv, CV_BGR2HSV);

    // 2値化画像
    IplImage *binalized = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);

    // 2値化
    CvScalar lower = cvScalar(minH, minS, minV);
    CvScalar upper = cvScalar(maxH, maxS, maxV);
    cvInRangeS(hsv, lower, upper, binalized);

    // ノイズの除去
    cvMorphologyEx(binalized, binalized, NULL, NULL, CV_MOP_CLOSE);

    // 輪郭検出
    CvSeq *contour = NULL, *maxContour = NULL;
    CvMemStorage *contourStorage = cvCreateMemStorage();
    cvFindContours(binalized, contourStorage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    // 一番大きな輪郭を抽出
    double max_area = 0.0;
    while (contour) {
        double area = fabs(cvContourArea(contour));
        if (area > max_area) {
            maxContour = contour;
            max_area = area;
        }
        contour = contour->h_next;
    }

    // 検出できた
    if (maxContour && max_area > 150) {
        // 輪郭の描画
        cvZero(binalized);
        cvDrawContours(binalized, maxContour, cvScalarAll(255), cvScalarAll(255), -1, CV_FILLED, 8);

        // 重心を求める
        CvMoments moments;
        cvMoments(binalized, &moments, 1);
        int my = (int) (moments.m01 / moments.m00);
        int mx = (int) (moments.m10 / moments.m00);
        cvCircle(image, cvPoint(mx, my), 10, CV_RGB(255, 0, 0));

        x = mx;
        y = my;
        visible = true;
    } else {
        visible = false;
    }

    return binalized;
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

    CvCapture *capture;
    IplImage *image;

    capture = cvCaptureFromCAM(1);

    int lowerH = 100;
    int upperH = 200;
    int lowerS = 100;
    int upperS = 200;
    int lowerV = 100;
    int upperV = 200;
    obj.init(lowerH, upperH, lowerS, upperS, lowerV, upperV);
    namedWindow("Captured Image", WINDOW_AUTOSIZE);
    createTrackbar("Lower Hue", "Captured Image", &lowerH, 255, callbackForLowerH);
    createTrackbar("Upper Hue", "Captured Image", &upperH, 255, callbackForUpperH);
    createTrackbar("Lower Sat", "Captured Image", &lowerS, 255, callbackForLowerS);
    createTrackbar("Upper Sat", "Captured Image", &upperS, 255, callbackForUpperS);
    createTrackbar("Lower Val", "Captured Image", &lowerV, 255, callbackForLowerV);
    createTrackbar("Upper Val", "Captured Image", &upperV, 255, callbackForUpperV);

    for (;;) {
        image = cvQueryFrame(capture);
        IplImage *binalized = obj.detect(image);

        cvShowImage("Captured Image", binalized);

        int k = cvWaitKey(1);
        if (k == 'q' || k == 'Q') break;
    }

    cvDestroyWindow("Captured Image");
    cvReleaseImage(&image);

    return 0;
}

