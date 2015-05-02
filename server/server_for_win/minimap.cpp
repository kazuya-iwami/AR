#include "minimap.h"

void Field::setCorners(Mat image) {

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
		int x,y;

        y = (int)(moments.m01 / moments.m00);
        x = (int)(moments.m10 / moments.m00);
        corners[i] = Point(x, y);

        rect = boundingRect(contours[i]);
        // cout << fabs(contourArea(contours[i])) << endl;
        drawContours(image, contours, i, Scalar(0, 255, 0));
    }


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
void Field::init(int minH_, int maxH_, int minS_, int maxS_, int minV_, int maxV_) {
        minH = minH_;
        minS = minS_;
        minV = minV_;
        maxH = maxH_;
        maxS = maxS_;
        maxV = maxV_;
}
// Field f();

void Object::detect(Mat image,Field *f) {

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

    //std::sort(contours.begin(), contours.end(),
    //    [](const std::vector<Point> &a, const std::vector<Point> &b) {
    //    return fabs(contourArea(a)) > fabs(contourArea(b));
    //});

    //Point corners[4];
    //Rect rect;

    //// cout << contours.size() << endl;
    //for (int i = 0; i < NCORNERS; ++i) {
    //    Moments moments = cv::moments(contours[i], true);

    //    y = (int)(moments.m01 / moments.m00);
    //    x = (int)(moments.m10 / moments.m00);
    //    corners[i] = Point(x, y);

    //    rect = boundingRect(contours[i]);
    //    // cout << fabs(contourArea(contours[i])) << endl;
    //    drawContours(image, contours, i, Scalar(0, 255, 0));
    //}

    //f.setCorners(corners);

    // マーカが見つかった
    if (contour_index >= 0 && max_area > 150) {
         // 重心
         Moments moments = cv::moments(contours[contour_index], true);
         y = (int)(moments.m01 / moments.m00);
         x = (int)(moments.m10 / moments.m00);

		 Point2f pos(x,y);
		 Point2f normalized_pos = f->getLocation(pos);

		 normalized_x = (int)normalized_pos.x;
		 normalized_y = (int)normalized_pos.y;

         // 表示
         //rect = boundingRect(contours[contour_index]);

         //drawContours(image, contours, contour_index, Scalar(0,255,0));

         visible = true;
     } else {
         visible = false;
     }

    /*Mat output(image.size(),image.type(), Scalar::all(0));
    image.copyTo(output ,binalized);
    rectangle(output, rect, Scalar(0, 255, 0));*/

    // find digonal
    /*int diagnal = 0;
    double max_dist = 0.0;
    for (int i = 1; i < NCORNERS; ++i) {
        double dist = norm(corners[i] - corners[0]);
        if (dist > max_dist) {
            diagnal = i;
            dist = max_dist;
        }
    }*/
    // draw boundaries
    // for (int i = 1; i < NCORNERS; ++i) {
    //     if (i != diagnal) {
    //         line(output, corners[0], corners[i], Scalar(255, 255, 255));
    //         line(output, corners[diagnal], corners[i], Scalar(255, 255, 255));
    //     }
    // }
    //f.drawBoundaries(output);

    //// 表示
    //imshow("Captured Image", output);
}

