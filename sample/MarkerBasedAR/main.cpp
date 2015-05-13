#include <iostream>
#include <vector>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

class MarkerDetector
{
public:
    MarkerDetector(Mat& src);
    void findCandidates();

    Mat src, preprocessed;
    vector<vector<Point> > contours;

// private:

//     class Bounding
//     {
//         vector<Point> points;
//         bool contain(Bounding b);
//     };
};

MarkerDetector::MarkerDetector(Mat& src)
{
    Mat gray;
    cvtColor(src, gray, CV_BGRA2GRAY);
    threshold(gray, preprocessed, 180, 255, THRESH_BINARY_INV);
}

void MarkerDetector::findCandidates()
{
    vector<vector<Point> > allContours;
    vector<Vec4i> hierarchy;
    findContours(preprocessed.clone(), allContours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    for (vector<vector<Point> >::iterator it = allContours.begin();
         it != allContours.end();
         ++it) {
        if ((*it).size() > 10) {
            contours.push_back(*it);
        }
    }

    Mat countourImage = Mat::zeros(preprocessed.size(), CV_8UC3);
    for (int i = 0; i < contours.size(); ++i) {
        drawContours(countourImage, contours, i, Scalar(255, 0, 0));
    }

    cout << "# of contours found: " << contours.size() << endl;
    imshow("contours", countourImage);
    vector<Point> approxCurve;

    vector<vector<Point> > candidates;
    for (int i = 0; i < hierarchy.size(); ++i) {
        double epsilon = contours[i].size() * 0.03;
        approxPolyDP(contours[i], approxCurve, epsilon, true);

        if (approxCurve.size() != 4) continue;
        if (!isContourConvex(approxCurve)) continue;

        candidates.push_back(approxCurve);

    }

    Mat im = Mat::zeros(preprocessed.size(), CV_8UC3);
    for (vector<vector<Point> >::iterator it = candidates.begin();
         it != candidates.end();
         ++it) {
        for (int i = 0; i < 4; ++i) {
            line(im, (*it)[i], (*it)[(i+1)%4], Scalar(255, 0, 0));
        }
    }
    imshow("cands", im);

    for (vector<vector<Point> >::iterator it = candidates.begin();
         it != candidates.end();
         ++it) {
        cout << *it << endl;
    }

    cout << "# of candidates found : " << candidates.size() << endl;

    Mat image = Mat::zeros(200, 200, CV_8UC3);
    vector<Point2f> corners;
    corners.push_back(Point(0, 0));
    corners.push_back(Point(0, 200));
    corners.push_back(Point(200, 200));
    corners.push_back(Point(200, 0));
    vector<Point2f> copied;
    Mat(candidates[5]).copyTo(copied);
    cout << candidates[0] << endl;
    Mat m = getPerspectiveTransform(copied, corners);
    warpPerspective(preprocessed, image, m, image.size());
    imshow("transformed", image);
    imshow("preprocessed", preprocessed);
    waitKey(0);
}

int main(int argc, char** argv)
{
    Mat src = imread(argv[1], 1);
    MarkerDetector m(src);
    m.findCandidates();
}