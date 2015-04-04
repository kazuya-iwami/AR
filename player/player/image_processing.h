#pragma once

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/legacy/legacy.hpp"

using namespace std;
using namespace cv;

//画像処理クラス
//読まなくていい

class CImage_processer {

public:
    int ip_x, ip_y;//ip : image_prosesser

    int minH, maxH;
    int minS, maxS;
    int minV, maxV;

    bool visible; //視界に入っているかのフラグ

    void init(int minH_, int maxH_, int minS_, int maxS_, int minV_, int maxV_);

	Mat detect(Mat image);
};

//ここ見なくていい