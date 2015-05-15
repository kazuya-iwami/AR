#include "image_processing.h"


void CImage_processer::init(int minH_, int maxH_, int minS_, int maxS_, int minV_, int maxV_) {
	minH = minH_;
	minS = minS_;
	minV = minV_;
	maxH = maxH_;
	maxS = maxS_;
	maxV = maxV_;

	ip_x = 0;
	ip_y = 0;
	for(int i=0;i<4;i++){
		for(int j=0;j<2;j++){
			for(int k=0;k<10;k++){
				ip[j][k]=-100;
			}
		}
	}
	visible = false;
}  

Mat CImage_processer::detect(Mat image) {

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
	std::vector<std::vector<Point>> contours;
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

	Rect rect;

	// マーカが見つかった
	if (contour_index >= 0 && max_area > 150) {
		int xx=0,yy=0;	
		for(int j=0;j<10;j++){	
		xx+=ip[0][j];
		yy+=ip[1][j];
		}
		// 重心
		Moments moments = cv::moments(contours[contour_index], true);
		ip_y = (int)(moments.m01 / moments.m00);
		ip_x = (int)(moments.m10 / moments.m00);
		if((xx-ip_x)*(xx-ip_x)+(yy-ip_y)*(yy-ip_y)>1000){
			//敵の位置がこれまでの平均よりも極端に離れていたら履歴を削除して作り直し.適当に閾値は調整すること
			for(int j=0;j<10;j++){
				ip[0][j]=ip_x;
				ip[1][j]=ip_y;
			}
		}else{
			for(int j=8;j>=0;j--){
				ip[0][j+1]=ip[0][j];
				ip[1][j+1]=ip[1][j];
			}
			ip[0][0]=ip_x;
			ip[1][0]=ip_y;
			for(int j=0;j<10;j++){	
				xx+=ip[0][j];
				yy+=ip[1][j];
			}
			ip_x=xx/10;	
			ip_y=yy/10;
		}
		// 表示
		rect = boundingRect(contours[contour_index]);

		//drawContours(image, contours, contour_index, Scalar(0,255,0));

		visible = true;
	} else {
		for(int j=8;j>=0;j--){
			ip[0][j+1]=ip[0][j];
			ip[1][j+1]=ip[1][j];
		}
		ip[0][0]=-100;
		ip[0][1]=-100;
		visible = false;
	}


	Mat output;
	image.copyTo(output);
	rectangle(output, rect, Scalar(0, 255, 0));

	 //表示
	return output;
}


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

void MarkerDetector::init(){
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
    //cout << "Template matched: " << templateMatch(image) << endl;
	int result = templateMatch(image);
	if(result != -1){
		visible = true;
		marker_id = result;

		Moments moment = moments(candidates[0]);
		Point center = Point(moment.m10 / moment.m00, moment.m01 / moment.m00);
		marker_x = center.x*1000/320;
		marker_y = center.y*750/240; //画面を引き延ばしている分の補正
	}else{
		visible = false;
	}

    //imshow("transformed", image);
    //waitKey(0);
}

//int main(int argc, char** argv)
//{
//    Mat src = imread(argv[1], 1);
//    MarkerDetector m;
//    m.findMarker(src);
//}
