#ifndef _AR_MINIMANP_H_
#define _AR_MINIMAP_H_

#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#define NCORNERS 4

using namespace std;
using namespace cv;

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
	int minH, maxH;
    int minS, maxS;
    int minV, maxV;
public:
    Field() {};
    Field(Point corners[NCORNERS]);
	void init(int minH_, int maxH_, int minS_, int maxS_, int minV_, int maxV_);
    void setCorners(Mat image);
    void drawBoundaries(Mat image);
    Point2f getLocation(Point2f p);
};

//Point corners[NCORNERS];

class Object {

public:
    int x, y,normalized_x,normalized_y;

    int minH, maxH;
    int minS, maxS;
    int minV, maxV;

    bool visible; //Ž‹ŠE‚É“ü‚Á‚Ä‚¢‚é‚©‚Ìƒtƒ‰ƒO

    void init(int minH_, int maxH_, int minS_, int maxS_, int minV_, int maxV_) {
        minH = minH_;
        minS = minS_;
        minV = minV_;
        maxH = maxH_;
        maxS = maxS_;
        maxV = maxV_;

        x = 0;
        y = 0;
		normalized_x = 0;
		normalized_y = 0;

        visible = false;
    }

   void detect(Mat image, Field *f);
};

#endif