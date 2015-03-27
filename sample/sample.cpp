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
    int contour_index = -1;
    double max_area = 0.0;
    for (int i = 0; i < (int)contours.size(); i++) {
        double area = fabs(contourArea(contours[i]));
        if (area > max_area) {
            contour_index = i;
            max_area = area;
        }
    }

    Rect rect;

    // マーカが見つかった
    if (contour_index >= 0 && max_area > 150) {
        // 重心
        Moments moments = cv::moments(contours[contour_index], true);
        y = (int)(moments.m01 / moments.m00);
        x = (int)(moments.m10 / moments.m00);

        // 表示
        rect = boundingRect(contours[contour_index]);

        //drawContours(image, contours, contour_index, Scalar(0,255,0));

        visible = true;
    } else {
        visible = false;
    }

    Mat output(image.size(),image.type(), Scalar::all(0));
    image.copyTo(output ,binalized);
    rectangle(output, rect, Scalar(0, 255, 0));

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

    VideoCapture cap(1); // デフォルトカメラをオープン
    if(!cap.isOpened())  // 成功したかどうかをチェック
        return -1;

    int lowerH = 80;
    int upperH = 160;
    int lowerS = 100;
    int upperS = 200;
    int lowerV = 100;
    int upperV = 200;
    obj.init(lowerH, upperH, lowerS, upperS, lowerV, upperV);
    namedWindow("Captured Image", WINDOW_NORMAL);

    createTrackbar("Lower Hue/2", "Captured Image", &lowerH, 180, callbackForLowerH);
    createTrackbar("Upper Hue/2", "Captured Image", &upperH, 180, callbackForUpperH);
    createTrackbar("Lower Sat", "Captured Image", &lowerS, 255, callbackForLowerS);
    createTrackbar("Upper Sat", "Captured Image", &upperS, 255, callbackForUpperS);
    createTrackbar("Lower Val", "Captured Image", &lowerV, 255, callbackForLowerV);
    createTrackbar("Upper Val", "Captured Image", &upperV, 255, callbackForUpperV);

    for (;;) {
        cap >> image; // カメラから新しいフレームを取得

        obj.detect(image);

        int k = waitKey(30);
        if (k == 'q' || k == 'Q') break;
    }

    return 0;
}

