/*
 * player_image_test.cpp
 *
 *  Created on: Mar 20, 2014
 *      Author: wallenberg
 */

#include "PlayerImageProcessor.h"
#include "KinectInput.h"
#include <opencv2/opencv.hpp>
#include <string>

int main(int argc, char** argv)
{
	KinectInput input(true);
	PlayerImageProcessor processor;
	processor.init_detector("/usr/share/OpenCV/haarcascades");
	processor.set_near_limit(600);
	processor.set_far_limit(2000);

	input.start();
	cv::Mat rgb_frame, bgr_frame, depth_frame, raw_depth_frame;
	std::string win_name_rgb = "RGB";
	std::string win_name_depth = "Depth";
	std::string win_name_face = "face";

	cv::namedWindow(win_name_rgb, CV_WINDOW_KEEPRATIO);
	cv::namedWindow(win_name_depth, CV_WINDOW_KEEPRATIO);
	cv::namedWindow(win_name_face, CV_WINDOW_KEEPRATIO);

	char key = 0;
	while(key != 27){
		bool datafl = false;
		while(!datafl){
				datafl = input.poll_data(rgb_frame, raw_depth_frame);
		}
		double min, max;

		processor.set_player_masks(raw_depth_frame);
		cv::Mat left_player_mask;
		processor.get_left_player_mask(left_player_mask);

		cv::Mat vismask;
		cv::convertScaleAbs(left_player_mask, vismask, 255/max);
		cv::minMaxIdx(left_player_mask, &min, &max);
		cv::cvtColor(rgb_frame, bgr_frame, CV_RGB2BGR);

		cv::Mat face_image_rgb, face_image_bgr;
		cv::Rect face_box(50, 50, 200, 200);
		bool facefl = processor.find_player_face(rgb_frame, face_image_rgb, face_box);
		if(facefl){
			cv::cvtColor(face_image_rgb, face_image_bgr, CV_RGB2BGR);
			cv::imshow(win_name_face, face_image_bgr);
		}
		cv::rectangle(bgr_frame, face_box, cv::Scalar(0.0, 1.0, 0.0), 5);
		cv::imshow(win_name_rgb, bgr_frame);
		cv::imshow(win_name_depth, left_player_mask);
		key = cv::waitKey(1);
	}
	std::cout << "Stopping input device..." << std::endl;
	input.stop();
	std::cout << "Destroying windows..." << std::endl;
	cv::destroyAllWindows();
	for(int k=0;k<10;k++)
		cv::waitKey(1);
	std::cout << "Done." << std::endl;
	return 0;
}


