#pragma once

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/legacy/legacy.hpp"

using namespace std;
using namespace cv;

class CImage_processer {

public:
    int x, y;

    int minH, maxH;
    int minS, maxS;
    int minV, maxV;

    bool visible; //視界に入っているかのフラグ

    void init(int minH_, int maxH_, int minS_, int maxS_, int minV_, int maxV_);

	Mat detect(Mat image);
};
