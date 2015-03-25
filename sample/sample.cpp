#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/legacy/legacy.hpp"

using namespace std;
using namespace cv;

enum EObject{
    E_ENEMY1=0,
    E_ENEMY2,
    E_ENEMY3,
    E_ITEM1,
    E_NUM_OBJECTS //EObjectの個数を示す
};

void  detectAllObject(IplImage *image);


class Object{

    int minH, maxH;
    int minS, maxS;
    int minV, maxV;

public:

    int x,y;

    bool visible;//視界に入っているかのフラグ

    void init(int minH_, int maxH_, int minS_, int maxS_, int minV_, int maxV_){
        minH = minH_;
        minS = minS_;
        minV = minV_;
        maxH = maxH_;
        maxS = maxS_;
        maxV = maxV_;

        x=0;
        y=0;

        visible=false;

    }
    void detect(IplImage *image);
    
};

void Object::detect(IplImage *image) {

    // HSVに変換
    IplImage *hsv = cvCloneImage(image);
    cvCvtColor(image, hsv, CV_RGB2HSV_FULL);

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

        cvShowImage("capture_image", binalized);

        x=mx;
        y=my;
        visible=true;
        
    }else{
        visible=false;
    }

}

Object obj[E_NUM_OBJECTS];

int main(int argc, char *argv[]) {

    CvCapture *capture;
    IplImage *image;

    capture = cvCaptureFromCAM(1);

    {

        obj[E_ENEMY1].init(100, 200, 100, 200, 100, 200);
        obj[E_ENEMY2].init(100, 200, 100, 200, 100, 200);
        obj[E_ENEMY3].init(100, 200, 100, 200, 100, 200);
        obj[E_ITEM1].init(100, 200, 100, 200, 100, 200);


        bool loop_flag = true;
        while (loop_flag) {

            image = cvQueryFrame(capture);

            detectAllObject(image);

            //cvShowImage("capture_image", image);

            // 5. process according to input key
            int k = cvWaitKey(1);
            switch (k) {
                case 'q':
                case 'Q':
                    loop_flag = false;
                    break;

            }
        }
    }
    cvDestroyWindow("capture_image");
    cvReleaseImage(&image);

    return 0;
}


void  detectAllObject(IplImage *image) {
    for(int i = 0;i<E_NUM_OBJECTS;i++){
        obj[i].detect(image);
    }
}
