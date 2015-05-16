
#include "sample.h"

using namespace std;
using namespace cv;

//hsv.csv読み込み
void Data::open(){
	std::ifstream ifs( "../../player/player/data/hsv.csv" );
	std::string str;

	ifs >> str;
	int result = sscanf_s(str.c_str(),"1p:%d,%d,%d,%d,%d,%d,2p:%d,%d,%d,%d,%d,%d,3p:%d,%d,%d,%d,%d,%d,4p:%d,%d,%d,%d,%d,%d,corner:%d,%d,%d,%d,%d,%d",
		&(player[0].minH),&(player[0].maxH),&(player[0].minS),&(player[0].maxS),&(player[0].minV),&(player[0].maxV),
		&(player[1].minH),&(player[1].maxH),&(player[1].minS),&(player[1].maxS),&(player[1].minV),&(player[1].maxV),
		&(player[2].minH),&(player[2].maxH),&(player[2].minS),&(player[2].maxS),&(player[2].minV),&(player[2].maxV),
		&(player[3].minH),&(player[3].maxH),&(player[3].minS),&(player[3].maxS),&(player[3].minV),&(player[3].maxV),
		&(corner.minH),&(corner.maxH),&(corner.minS),&(corner.maxS),&(corner.minV),&(corner.maxV));
	if(result != 30){ //30個の入力どれかに失敗した場合
			cout << "hsv.csv load failed" <<endl;

			for(int i=0;i<4;i++){
				player[i].minH=0;
				player[i].maxH=180;
				player[i].minS=0;
				player[i].maxS=255;
				player[i].minV=0;
				player[i].maxV=255;
			}
			corner.minH=0;
			corner.maxH=180;
			corner.minS=0;
			corner.maxS=255;
			corner.minV=0;
			corner.maxV=255;
				
	}else{
		cout<< "load succeeded!" << endl;
	}
}
//hsv.csv書き込み
void Data::save(){
	 std::ofstream ofs("../../player/player/data/hsv.csv");
	 char str_c[400];

	 sprintf_s(str_c,"1p:%d,%d,%d,%d,%d,%d,2p:%d,%d,%d,%d,%d,%d,3p:%d,%d,%d,%d,%d,%d,4p:%d,%d,%d,%d,%d,%d,corner:%d,%d,%d,%d,%d,%d",
		(player[0].minH),(player[0].maxH),(player[0].minS),(player[0].maxS),(player[0].minV),(player[0].maxV),
		(player[1].minH),(player[1].maxH),(player[1].minS),(player[1].maxS),(player[1].minV),(player[1].maxV),
		(player[2].minH),(player[2].maxH),(player[2].minS),(player[2].maxS),(player[2].minV),(player[2].maxV),
		(player[3].minH),(player[3].maxH),(player[3].minS),(player[3].maxS),(player[3].minV),(player[3].maxV),
		(corner.minH),(corner.maxH),(corner.minS),(corner.maxS),(corner.minV),(corner.maxV));

	 string str = str_c;
	 ofs << str;
}

void Object::detect(Mat image) {

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
        // 重心
        Moments moments = cv::moments(contours[contour_index], true);
        y = (int)(moments.m01 / moments.m00);
        x = (int)(moments.m10 / moments.m00);

        // 表示
        rect = boundingRect(contours[contour_index]);

        //drawContours(image, contours, contour_index, Scalar(0,255,0));

        visible = true;
    } else {
        visible = false;
    }

    Mat output(image.size(),image.type(), Scalar::all(0));
    image.copyTo(output ,binalized);
    rectangle(output, rect, Scalar(0, 255, 0));

    // 表示
    imshow("Captured Image", output);
}

Object obj;

void callbackForLowerH(int lowerH, void *data) {
    obj.minH = lowerH;
}

void callbackForUpperH(int upperH, void *data) {
    obj.maxH = upperH;
}

void callbackForLowerS(int lowerS, void *data) {
    obj.minS = lowerS;
}

void callbackForUpperS(int upperS, void *data) {
    obj.maxS = upperS;
}

void callbackForLowerV(int lowerV, void *data) {
    obj.minV = lowerV;
}

void callbackForUpperV(int upperV, void *data) {
    obj.maxV = upperV;
}

int main(int argc, char *argv[]) {

    Mat image;
	Data data;

	VideoCapture cap;


	const std::string videoStreamAddress =std::string("http://")+"192.168.10.114"+":8080/?action=stream.mjpeg";

	if(!cap.open(videoStreamAddress)) { //ラズパイからの取得はこちら
		 return -1;
	}

	//hsv.csv開く
	data.open();

	
	int lowerH = data.player[0].minH;
	int upperH = data.player[0].maxH;
	int lowerS = data.player[0].minS;
	int upperS = data.player[0].maxS;
	int lowerV = data.player[0].minV;
	int upperV = data.player[0].maxV;
    obj.init(lowerH, upperH, lowerS, upperS, lowerV, upperV);
    namedWindow("Captured Image", WINDOW_NORMAL);
	namedWindow("Captured Image2", WINDOW_NORMAL);

    createTrackbar("Lower Hue/2", "Captured Image2", &lowerH, 180, callbackForLowerH);
    createTrackbar("Upper Hue/2", "Captured Image2", &upperH, 180, callbackForUpperH);
    createTrackbar("Lower Sat", "Captured Image2", &lowerS, 255, callbackForLowerS);
    createTrackbar("Upper Sat", "Captured Image2", &upperS, 255, callbackForUpperS);
    createTrackbar("Lower Val", "Captured Image2", &lowerV, 255, callbackForLowerV);
    createTrackbar("Upper Val", "Captured Image2", &upperV, 255, callbackForUpperV);

	int status = 0;//現在どのパラメーターをセットしているかを示すフラグ

    for (;;) {
        cap >> image; // カメラから新しいフレームを取得

        obj.detect(image);

        int k = waitKey(30);
		if(status == 0 && k == '1'){
			data.player[0].minH = lowerH;
			data.player[0].maxH = upperH;
			data.player[0].minS = lowerS;
			data.player[0].maxS = upperS;
			data.player[0].minV = lowerV;
			data.player[0].maxV = upperV;

			cout <<"set player1"<< endl;

			setTrackbarPos("Lower Hue/2", "Captured Image2", data.player[1].minH);
			setTrackbarPos("Upper Hue/2", "Captured Image2", data.player[1].maxH);
			setTrackbarPos("Lower Sat", "Captured Image2", data.player[1].minS);
			setTrackbarPos("Upper Sat", "Captured Image2", data.player[1].maxS);
			setTrackbarPos("Lower Val", "Captured Image2", data.player[1].minV);
			setTrackbarPos("Upper Val", "Captured Image2", data.player[1].maxV);

			status++;
		}else if(status == 1 && k == '2'){
			data.player[1].minH = lowerH;
			data.player[1].maxH = upperH;
			data.player[1].minS = lowerS;
			data.player[1].maxS = upperS;
			data.player[1].minV = lowerV;
			data.player[1].maxV = upperV;
			cout << "set player2" << endl;
			setTrackbarPos("Lower Hue/2", "Captured Image2", data.player[2].minH);
			setTrackbarPos("Upper Hue/2", "Captured Image2", data.player[21].maxH);
			setTrackbarPos("Lower Sat", "Captured Image2", data.player[2].minS);
			setTrackbarPos("Upper Sat", "Captured Image2", data.player[2].maxS);
			setTrackbarPos("Lower Val", "Captured Image2", data.player[2].minV);
			setTrackbarPos("Upper Val", "Captured Image2", data.player[2].maxV);
			status++;
		}else if(status == 2 && k == '3'){
			data.player[2].minH = lowerH;
			data.player[2].maxH = upperH;
			data.player[2].minS = lowerS;
			data.player[2].maxS = upperS;
			data.player[2].minV = lowerV;
			data.player[2].maxV = upperV;
			cout << "set player3" << endl;
			setTrackbarPos("Lower Hue/2", "Captured Image2", data.player[3].minH);
			setTrackbarPos("Upper Hue/2", "Captured Image2", data.player[3].maxH);
			setTrackbarPos("Lower Sat", "Captured Image2", data.player[3].minS);
			setTrackbarPos("Upper Sat", "Captured Image2", data.player[3].maxS);
			setTrackbarPos("Lower Val", "Captured Image2", data.player[3].minV);
			setTrackbarPos("Upper Val", "Captured Image2", data.player[3].maxV);
			status++;
		}else if(status == 3 && k == '4'){
			data.player[3].minH = lowerH;
			data.player[3].maxH = upperH;
			data.player[3].minS = lowerS;
			data.player[3].maxS = upperS;
			data.player[3].minV = lowerV;
			data.player[3].maxV = upperV;
			cout << "set player4" << endl;
			setTrackbarPos("Lower Hue/2", "Captured Image2", data.corner.minH);
			setTrackbarPos("Upper Hue/2", "Captured Image2", data.corner.maxH);
			setTrackbarPos("Lower Sat", "Captured Image2", data.corner.minS);
			setTrackbarPos("Upper Sat", "Captured Image2", data.corner.maxS);
			setTrackbarPos("Lower Val", "Captured Image2", data.corner.minV);
			setTrackbarPos("Upper Val", "Captured Image2", data.corner.maxV);
			status++;
		}else if(status == 4 && k == '5'){
			data.corner.minH = lowerH;
			data.corner.maxH = upperH;
			data.corner.minS = lowerS;
			data.corner.maxS = upperS;
			data.corner.minV = lowerV;
			data.corner.maxV = upperV;
			std::cout << "set corner" << std::endl;
			
			data.save();
			break;

		}
        if (k == 'q' || k == 'Q') break;
    }

    return 0;
}

