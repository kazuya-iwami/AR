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
				ip[i][j][k]=-100;
			}
		}
	}
	visible = false;
}  

Mat CImage_processer::detect(Mat image, int id) {

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
		xx+=ip[id][0][j];
		yy+=ip[id][1][j];
		}
		// 重心
		Moments moments = cv::moments(contours[contour_index], true);
		ip_y = (int)(moments.m01 / moments.m00);
		ip_x = (int)(moments.m10 / moments.m00);
		if((xx-ip_x)*(xx-ip_x)+(yy-ip_y)*(yy-ip_y)>1000){//敵の位置がこれまでの平均よりも極端に離れていたら履歴を削除して作り直し.適当に閾値は調整すること
			for(int j=0;j<10;j++){
				ip[id][0][j]=ip_x;
				ip[id][1][j]=ip_y;
			}
		}else{
			for(int j=8;j>=0;j--){
				ip[id][0][j+1]=ip[id][0][j];
				ip[id][1][j+1]=ip[id][1][j];
			}
			ip[id][0][0]=ip_x;
			ip[id][1][0]=ip_y;
			for(int j=0;j<10;j++){	
				xx+=ip[id][0][j];
				yy+=ip[id][1][j];
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
			ip[id][0][j+1]=ip[id][0][j];
			ip[id][1][j+1]=ip[id][1][j];
		}
		ip[id][0][0]=-100;
		ip[id][0][1]=-100;
		visible = false;
	}


	Mat output;
	image.copyTo(output);
	rectangle(output, rect, Scalar(0, 255, 0));

	// 表示
	return output;
}
