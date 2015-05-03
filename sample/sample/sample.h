#include <iostream>
#include <fstream>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/legacy/legacy.hpp"

class Hsv {
public:
	int minH, maxH;
    int minS, maxS;
    int minV, maxV;
};

class Data{
public:
	Hsv player[4];
	Hsv corner;
	void open();
	void save();
};

class Object {

public:
    int x, y;

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

        visible = false;
    }

   void detect(cv::Mat image);
};