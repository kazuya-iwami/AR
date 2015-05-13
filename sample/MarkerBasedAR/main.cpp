#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
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
    string templateMatch(Mat& in);
    Mat templates[3];

private:
    void reorder(vector<Point>& boundingBox);
};

void MarkerDetector::reorder(vector<Point>& boundingBox) {
    Point v1 = boundingBox[1] - boundingBox[0];
    Point v2 = boundingBox[2] - boundingBox[0];
    if (v1.x * v2.y - v1.y * v2.x > 0) {
        swap(boundingBox[1], boundingBox[3]);
    }
    Moments mu = moments(boundingBox);
    Point center = Point(mu.m10 / mu.m00, mu.m01 / mu.m00);

    // find top left corner
    int topLeftCornerIdx = -1;
    for (int i = 0; i < 4; ++i) {
        if (boundingBox[i].x < center.x && boundingBox[i].y < center.y)
            topLeftCornerIdx = i;
    }
    if (topLeftCornerIdx >= 0) {
        vector<Point> buf;
        for (int i = 0; i < 4; ++i) {
            buf.push_back(boundingBox[(topLeftCornerIdx+i)%4]);
        }
        boundingBox.clear();
        for (int i = 0; i < 4; ++i) {
            boundingBox.push_back(buf[i]);
        }
    }
}

string MarkerDetector::templateMatch(Mat& in)
{
    Mat m[3];
    m[0] = imread("bulb_template.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    m[1] = imread("bullet_template.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    m[2] = imread("target_template.jpg", CV_LOAD_IMAGE_GRAYSCALE);

    double scores[3];
    Mat fajfaj = m[0] - m[1];
    cout << "hello" << endl;
    cout << m[0].size() << endl;
    for (int i = 0; i < 3; ++i) {
        scores[i] = norm(m[i] - in);
    }
    cout << "hello" << endl;
    switch(min_element(scores, scores + 3) - scores) {
        case 0: return "bulb";
        case 1: return "bullet";
        case 2: return "target";
    }
    return "None";
}

MarkerDetector::MarkerDetector(Mat& src)
{
    Mat gray;
    cvtColor(src, gray, CV_BGRA2GRAY);
    threshold(gray, preprocessed, 150, 255, THRESH_BINARY_INV);
    // imshow("preprocessed", preprocessed);
    // templates[0] = imread("bulb_template.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    // templates[1] = imread("bullet_template.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    // templates[2] = imread("target_template.jpg", CV_LOAD_IMAGE_GRAYSCALE);
}

void MarkerDetector::findCandidates()
{
    vector<vector<Point> > allContours;
    vector<Vec4i> hierarchy;
    findContours(preprocessed.clone(), allContours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
    // for (vector<vector<Point> >::iterator it = allContours.begin();
    //      it != allContours.end();
    //      ++it) {
    //     if ((*it).size() > 10) {
    //         contours.push_back(*it);
    //     }
    // }
    for (int i = 0; i < allContours.size(); ++i) {
        if (allContours[i].size() > 10 && hierarchy[i][3] != -1) {
            contours.push_back(allContours[i]);
        }
    }

    // Mat allContourImage = Mat::zeros(preprocessed.size(), CV_8UC3);
    // for (int i = 0; i < allContours.size(); ++i) {
    //     drawContours(allContourImage, allContours, i, Scalar(255, 255, 255));
    // }
    // imshow("all contours", allContourImage);

    // Mat countourImage = Mat::zeros(preprocessed.size(), CV_8UC3);
    // Mat im = Mat::zeros(preprocessed.size(), CV_8UC3);
    // for (int i = 0; i < contours.size(); ++i) {
    //     drawContours(countourImage, contours, i, Scalar(255, 255, 255));
    //     drawContours(im, contours, i, Scalar(255, 255, 255));
    // }

    cout << "# of all contours found: " << allContours.size() << endl;
    cout << "# of contours found: " << contours.size() << endl;
    // imshow("contours", countourImage);
    vector<Point> approxCurve;

    vector<vector<Point> > candidates;
    // cout << "hierarchy size: " << hierarchy.size() << endl;
    for (int i = 0; i < contours.size(); ++i) {
        double epsilon = contours[i].size() * 0.01;
        approxPolyDP(contours[i], approxCurve, epsilon, true);

        if (approxCurve.size() != 4) continue;
        if (!isContourConvex(approxCurve)) continue;

        reorder(approxCurve);
        candidates.push_back(approxCurve);
    }

    // for (vector<vector<Point> >::iterator it = candidates.begin();
    //      it != candidates.end();
    //      ++it) {
    //     for (int i = 0; i < 4; ++i) {
    //         line(im, (*it)[i], (*it)[(i+1)%4], Scalar(255, 0, 0));
    //     }
    // }
    // imshow("cands", im);

    // for (vector<vector<Point> >::iterator it = candidates.begin();
    //      it != candidates.end();
    //      ++it) {
    //     cout << *it << endl;
    // }

    cout << "# of candidates found : " << candidates.size() << endl;

    Mat image = Mat::zeros(200, 200, CV_8UC3);
    vector<Point2f> corners;
    corners.push_back(Point(0, 0));
    corners.push_back(Point(0, 200));
    corners.push_back(Point(200, 200));
    corners.push_back(Point(200, 0));
    vector<Point2f> copied;
    Mat(candidates[0]).copyTo(copied);
    Mat m = getPerspectiveTransform(copied, corners);
    warpPerspective(preprocessed, image, m, image.size());
    imshow("transformed", image);
    // imshow("preprocessed", preprocessed);
    cout << "Template matched: " << templateMatch(image) << endl;
    waitKey(0);
}

int main(int argc, char** argv)
{
    Mat src = imread(argv[1], 1);
    MarkerDetector m(src);
    m.findCandidates();
}