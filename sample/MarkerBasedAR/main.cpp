#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

class MarkerDetector
{
public:
    MarkerDetector();
    void findMarker(Mat &src);
    int templateMatch(Mat &src);
    Mat templates[3];
private:
    void reorder(vector<Point>& boundingBox);
};

void MarkerDetector::reorder(vector<Point> &boundingBox)
{
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

int MarkerDetector::templateMatch(Mat &src)
{
    double scores[3];
    for (int i = 0; i < 3; ++i) {
        scores[i] = norm(templates[i] - src);
        // cout << "scores[" << i << "] = " << scores[i] << endl;
    }
    int minIdx = min_element(scores, scores + 3) - scores;
    return scores[minIdx] < 5000 ? minIdx : -1;
}

MarkerDetector::MarkerDetector()
{
    templates[0] = imread("bulb_template.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    templates[1] = imread("bullet_template.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    templates[2] = imread("target_template.jpg", CV_LOAD_IMAGE_GRAYSCALE);
}

void MarkerDetector::findMarker(Mat &src)
{
    Mat gray, preprocessed;
    cvtColor(src, gray, CV_BGRA2GRAY);
    threshold(gray, preprocessed, 150, 255, THRESH_BINARY_INV);

    vector<vector<Point> > allContours, contours;
    vector<Vec4i> hierarchy;
    findContours(preprocessed.clone(), allContours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
    for (int i = 0; i < allContours.size(); ++i) {
        if (allContours[i].size() > 10 && hierarchy[i][3] != -1) {
            contours.push_back(allContours[i]);
        }
    }

    vector<Point> approxCurve;
    vector<vector<Point> > candidates;
    for (int i = 0; i < contours.size(); ++i) {
        double epsilon = contours[i].size() * 0.01;
        approxPolyDP(contours[i], approxCurve, epsilon, true);

        if (approxCurve.size() != 4) continue;
        if (!isContourConvex(approxCurve)) continue;

        reorder(approxCurve);
        candidates.push_back(approxCurve);
    }

    vector<vector<Point> > markers;

    vector<Point2f> corners;
    corners.push_back(Point(0, 0));
    corners.push_back(Point(0, 200));
    corners.push_back(Point(200, 200));
    corners.push_back(Point(200, 0));

    // converts to Point2f 'cause getPerspectiveTransform requires
    // vector of Point2f
    vector<Point2f> copied;
    Mat image = Mat::zeros(200, 200, CV_8UC3);
    Mat(candidates[0]).copyTo(copied);

    Mat m = getPerspectiveTransform(copied, corners);
    warpPerspective(preprocessed, image, m, image.size());
    cout << "Template matched: " << templateMatch(image) << endl;
    imshow("transformed", image);
    waitKey(0);
}

int main(int argc, char** argv)
{
    Mat src = imread(argv[1], 1);
    MarkerDetector m;
    m.findMarker(src);
}
