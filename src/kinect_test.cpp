/*
 * kinect_test.cpp
 *
 *  Created on: Mar 20, 2014
 *      Author: hannes
 */


#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include "KinectInput.h"

int main() {
	KinectInput kinect(true);
	std::cout << "Starting Kinect interface" << std::endl;
	kinect.start();

	bool quit = false;

	cv::Mat rgb, depth, depth_show;
	cv::namedWindow("rgb", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("depth", CV_WINDOW_AUTOSIZE);
	while (!quit) {
		if (kinect.poll_data(rgb, depth)) {
			cv::imshow("rgb", rgb);
			double min_val, max_val;
			cv::minMaxLoc(depth, &min_val, &max_val);
			//depth.convertTo(depth_show, CV_8UC1, 255.0 / (max_val - min_val), -min_val*255.0/(max_val - min_val));
			depth.convertTo(depth_show, CV_8UC3, 255.0 / (max_val - min_val), -min_val*255.0/(max_val - min_val));

			std::cout << "Before " << depth.channels() << " channels " << depth.depth() << " depth " << depth.cols << "x" << depth.rows;
			std::cout << "  Now " << depth_show.channels() << " channels " << depth_show.depth() << "depth " << depth_show.cols << "x" << depth_show.rows << std::endl;

			cv::imshow("depth", depth_show);
			if (cv::waitKey(100) > 0) {
				std::cout << "Breaking by user" << std::endl;
				break;
			}
		}
	}
	std::cout << "Closing windows" << std::endl;
	cv::destroyAllWindows();

	std::cout << "Stopping Kinect interface" << std::endl;
	kinect.stop();
}
