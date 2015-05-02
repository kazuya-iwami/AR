#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
// #include "opencv2/legacy/legacy.hpp"

#define 
using namespace std;
using namespace cv;

void detect(Mat image) {
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
int main() {
  Mat image;

  VideoCapture cap(1);
  if (!cap.isOpened()) return -1;
  
  namedWindow("Captured Imaged", WINDOW_NORMAL);

  for (;;) {
    cap >> image;

  }


  return 0;
}