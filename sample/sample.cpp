#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/legacy/legacy.hpp"

#define NCORNERS 4

using namespace std;
using namespace cv;

Point corners[NCORNERS];

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


/*
 * corners are ordered as below
 *
 *  0 ------ 3
 *  |        |
 *  |        |
 *  |        |
 *  1 ------ 2
 */
class Field {
    Point2f corners[4];
    Point2f center;
public:
    Field() {};
    Field(Point corners[NCORNERS]);
    void setCorners(Point corners[NCORNERS]);
    void drawBoundaries(Mat image);
    Point2f getLocation(Point2f p);
};

Field f;

void Field::setCorners(Point corners[NCORNERS]) {
    // find digonal
    int diagnal = 0;
    double max_dist = 0.0;
    for (int i = 1; i < NCORNERS; ++i) {
        double dist = norm(corners[i] - corners[0]);
        if (dist > max_dist) {
            diagnal = i;
            dist = max_dist;
        }
    }

    this->corners[0] = corners[0];
    if (diagnal == 1) {
        this->corners[1] = corners[2];
        this->corners[2] = corners[1];
        this->corners[3] = corners[3];
    } else if (diagnal == 2) {
        for (int i = 1; i < NCORNERS; ++i) {
            this->corners[i] = corners[i];
        }
    } else if (diagnal == 3) {
        this->corners[1] = corners[1];
        this->corners[2] = corners[3];
        this->corners[3] = corners[2];
    }

    this->center = Point(0, 0);
    for (int i = 0; i < NCORNERS; ++i) {
        this->center += this->corners[i];
    }
    this->center = Point2f(this->center.x / 4.0, this->center.y / 4.0);
}


Field::Field(Point corners[NCORNERS]) {
    // find digonal
    int diagnal = 0;
    double max_dist = 0.0;
    for (int i = 1; i < NCORNERS; ++i) {
        double dist = norm(corners[i] - corners[0]);
        if (dist > max_dist) {
            diagnal = i;
            dist = max_dist;
        }
    }

    this->corners[0] = corners[0];
    if (diagnal == 1) {
        this->corners[1] = corners[2];
        this->corners[2] = corners[1];
        this->corners[3] = corners[3];
    } else if (diagnal == 2) {
        for (int i = 1; i < NCORNERS; ++i) {
            this->corners[i] = corners[i];
        }
    } else if (diagnal == 3) {
        this->corners[1] = corners[1];
        this->corners[2] = corners[3];
        this->corners[3] = corners[2];
    }

    this->center = Point(0, 0);
    for (int i = 0; i < NCORNERS; ++i) {
        this->center += this->corners[i];
    }
    this->center = Point2f(this->center.x / 4.0, this->center.y / 4.0);
}

void Field::drawBoundaries(Mat image) {
    for (int i = 0; i < NCORNERS; ++i) {
        cout << "corners[" << i << "] : " << corners[i].x << ", " << corners[i].y << endl;
        line(image, corners[i], corners[(i+1)%4], Scalar(255, 255, 255));
    }
    for (int i = 0; i < NCORNERS; ++i) {
        line(image, corners[i], center, Scalar(255, 255, 255));
    }
    cout << "center : " << center.x << " , " << center.y << endl;
}

Point2f Field::getLocation(Point2f p) {
    Point2f dirs[NCORNERS];
    for (int i = 0; i < NCORNERS; ++i) {
        dirs[i] = corners[i] - center;
    }

    Point2f relative = p - center;

    for (int i = 0; i < NCORNERS; ++i) {
        Point2f d1 = dirs[i];
        Point2f d2 = dirs[(i+1)%NCORNERS];
        if (d1.dot(relative) > -0.3 && d2.dot(relative) > -0.3) {
            cout << "i: " << i << endl;
            cout << "relative: " << relative.x << ", " << relative.y << endl;
            cout << "d1: " << d1.x << ", " << d1.y << endl;
            cout << "d2: " << d2.x << ", " << d2.y << endl;
            double det = d1.x * d2.y - d1.y * d2.x;
            if (det == 0) {
                return 100 * Point2f(0.5, 0.5);
            } else {
                double s = (d2.y * relative.x - d2.x * relative.y) / det;
                double t = (d1.x * relative.y - d1.y * relative.x) / det;
                cout << "s: " << s << ", t: " << t << endl;
                if (s < 0 || t < 0) continue;
                switch(i) {
                    case 0:
                        return 100 * Point2f(0.5 * (1 - s - t), 0.5 * (1 - s + t));
                    case 1:
                        return 100 * Point2f(0.5 * (1 - s + t), 0.5 * (1 + s + t));
                    case 2: 
                        return 100 * Point2f(0.5 * (1 + s + t), 0.5 * (1 + s - t));
                    case 3:
                        return 100 * Point2f(0.5 * (1 + s - t), 0.5 * (1 - s - t));
                }
            }
        }
    }
    return 100 * Point2f(0.5, 0.5);
}

// Field f();

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
    std::vector<std::vector<Point> > contours;
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

    std::sort(contours.begin(), contours.end(),
        [](const std::vector<Point> &a, const std::vector<Point> &b) {
        return fabs(contourArea(a)) > fabs(contourArea(b));
    });

    Point corners[4];
    Rect rect;

    // cout << contours.size() << endl;
    for (int i = 0; i < NCORNERS; ++i) {
        Moments moments = cv::moments(contours[i], true);

        y = (int)(moments.m01 / moments.m00);
        x = (int)(moments.m10 / moments.m00);
        corners[i] = Point(x, y);

        rect = boundingRect(contours[i]);
        // cout << fabs(contourArea(contours[i])) << endl;
        drawContours(image, contours, i, Scalar(0, 255, 0));
    }

    f.setCorners(corners);

    // // マーカが見つかった
    // if (contour_index >= 0 && max_area > 150) {
    //     // 重心
    //     Moments moments = cv::moments(contours[contour_index], true);
    //     y = (int)(moments.m01 / moments.m00);
    //     x = (int)(moments.m10 / moments.m00);

    //     // 表示
    //     rect = boundingRect(contours[contour_index]);

    //     //drawContours(image, contours, contour_index, Scalar(0,255,0));

    //     visible = true;
    // } else {
    //     visible = false;
    // }

    Mat output(image.size(),image.type(), Scalar::all(0));
    image.copyTo(output ,binalized);
    rectangle(output, rect, Scalar(0, 255, 0));

    // find digonal
    int diagnal = 0;
    double max_dist = 0.0;
    for (int i = 1; i < NCORNERS; ++i) {
        double dist = norm(corners[i] - corners[0]);
        if (dist > max_dist) {
            diagnal = i;
            dist = max_dist;
        }
    }
    // draw boundaries
    // for (int i = 1; i < NCORNERS; ++i) {
    //     if (i != diagnal) {
    //         line(output, corners[0], corners[i], Scalar(255, 255, 255));
    //         line(output, corners[diagnal], corners[i], Scalar(255, 255, 255));
    //     }
    // }
    f.drawBoundaries(output);

    // 表示
    imshow("Captured Image", output);
}

// Point getLocation(Point x) {

// }

Object obj;

void notifyTrackbarChange() {
    cout << "minH: " << obj.minH << endl;
    cout << "maxH: " << obj.maxH << endl;
    cout << "minS: " << obj.minS << endl;
    cout << "maxS: " << obj.maxS << endl;
    cout << "minV: " << obj.minV << endl;
    cout << "maxV: " << obj.maxV << endl;
}

void callbackForLowerH(int lowerH, void *data) {
    obj.minH = lowerH;
    notifyTrackbarChange();
}

void callbackForUpperH(int upperH, void *data) {
    obj.maxH = upperH;
    notifyTrackbarChange();
}

void callbackForLowerS(int lowerS, void *data) {
    obj.minS = lowerS;
    notifyTrackbarChange();
}

void callbackForUpperS(int upperS, void *data) {
    obj.maxS = upperS;
    notifyTrackbarChange();
}

void callbackForLowerV(int lowerV, void *data) {
    obj.minV = lowerV;
    notifyTrackbarChange();
}

void callbackForUpperV(int upperV, void *data) {
    obj.maxV = upperV;
    notifyTrackbarChange();
}

void mouseCallback(int event, int x, int y, int flags, void *data) {
    if (event == EVENT_LBUTTONDOWN || event == EVENT_RBUTTONDOWN) {
        cout << "x: " << x << " y: " << y << endl;
        Point2f p = f.getLocation(Point2f((double)x, (double)y));
        cout << "px: " << p.x << "py: " << p.y << endl;
    }
}

int main(int argc, char *argv[]) {

    Mat image;

    // VideoCapture cap(0); // デフォルトカメラをオープン
    // if(!cap.isOpened())  // 成功したかどうかをチェック
    //     return -1;

    int lowerH = 0;
    int upperH = 143;
    int lowerS = 0;
    int upperS = 255;
    int lowerV = 189;
    int upperV = 255;
    obj.init(lowerH, upperH, lowerS, upperS, lowerV, upperV);
    namedWindow("Captured Image", WINDOW_AUTOSIZE);

    // createTrackbar("Lower Hue/2", "Captured Image", &lowerH, 180, callbackForLowerH);
    // createTrackbar("Upper Hue/2", "Captured Image", &upperH, 180, callbackForUpperH);
    // createTrackbar("Lower Sat", "Captured Image", &lowerS, 255, callbackForLowerS);
    // createTrackbar("Upper Sat", "Captured Image", &upperS, 255, callbackForUpperS);
    // createTrackbar("Lower Val", "Captured Image", &lowerV, 255, callbackForLowerV);
    // createTrackbar("Upper Val", "Captured Image", &upperV, 255, callbackForUpperV);

    setMouseCallback("Captured Image", mouseCallback, NULL);

    for (;;) {
        // cap >> image; // カメラから新しいフレームを取得

        image = imread("image2.png"); 
        // obj.detect(image);
        obj.detect(image);

        int k = waitKey(30);
        if (k == 'q' || k == 'Q') break;
    }

    return 0;
}
