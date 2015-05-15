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
	int ip[2][10];//敵の位置の履歴を格納ip[x=0,y=1][tコマ前]=座標

	int minH, maxH;
	int minS, maxS;
	int minV, maxV;

	bool visible; //視界に入っているかのフラグ

	void init(int minH_, int maxH_, int minS_, int maxS_, int minV_, int maxV_);

	Mat detect(Mat image);
};

class MarkerDetector
{
public:
	void init();
    void findMarker(Mat &src);
    int templateMatch(Mat &src);
    Mat templates[3];

	bool visible;
	int marker_x,marker_y;
	int marker_id;
private:
    void reorder(vector<Point>& boundingBox);
	
};