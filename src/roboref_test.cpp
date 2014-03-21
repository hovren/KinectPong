/*
 * test_roboref.cpp
 *
 *  Created on: Mar 20, 2014
 *      Author: wallenberg
 */

#include <opencv2/opencv.hpp>
#include "RoboRef.h"

void mouse_callback(int event, int mousex, int mousey, int flags, void* userdata);
int targetx=0, targety=0;
bool resetfl=false, movefl = false;

int main(int argc, char** argv)
{
	RoboRef my_ref;
	my_ref.connect("/dev/ttyUSB0");

	cv::VideoCapture my_capture;
	my_capture.open(0);
	std::string win_name = "target test";
	cv::namedWindow(win_name, CV_WINDOW_KEEPRATIO);

	cv::setMouseCallback(win_name, mouse_callback, NULL);

	cv::Mat img;
	if(my_capture.isOpened()){
		char key = 0;
		while(key != 27){
			if(movefl){
				my_ref.look_at(cv::Point2f((float)targetx, (float)targety));
				movefl = false;
			}
			if(resetfl){
				my_ref.set_pan_tilt_angles(0.0, 0.0);
				resetfl = false;
			}
			my_capture >> img;
			cv::imshow(win_name, img);
			key = cv::waitKey(1);
		}

	}
	else{
		std::cout << "Could not initialise camera..." << std::endl;
	}

	cv::destroyAllWindows();

	return 0;
}

void mouse_callback(int event, int mousex, int mousey, int flags, void* userdata){
	if  (event == cv::EVENT_LBUTTONDOWN ){
		targetx = mousex;
		targety = mousey;
		movefl = true;
	}
	if  ( event == cv::EVENT_RBUTTONDOWN ){
		resetfl = true;
	}
}



