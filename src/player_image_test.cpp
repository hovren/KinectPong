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
	cv::Mat rgb_frame, bgr_frame, depth_frame;
	std::string win_name_rgb = "RGB";
	std::string win_name_depth = "Depth";
	std::string win_name_face = "face";

	cv::namedWindow(win_name_rgb, CV_WINDOW_KEEPRATIO);
	cv::namedWindow(win_name_depth, CV_WINDOW_KEEPRATIO);

	bool facesfl = false;

	char key = 0;
	while(key != 27){
		bool datafl = false;
		while(!datafl){
				datafl = input.poll_data(rgb_frame, depth_frame);
		}

		cv::flip(rgb_frame, rgb_frame, 1);
		cv::flip(depth_frame, depth_frame, 1);

		processor.set_player_masks(depth_frame);
		cv::Mat left_player_mask;
		cv::Mat right_player_mask;
		cv::Mat left_contact_mask;
		cv::Mat right_contact_mask;

		processor.get_left_player_mask(left_player_mask);
		processor.get_right_player_mask(right_player_mask);
		processor.get_left_player_contact_mask(left_contact_mask);
		processor.get_right_player_contact_mask(right_contact_mask);

		cv::Mat dummy = cv::Mat::zeros(left_player_mask.rows, left_player_mask.cols, left_player_mask.type());
		std::vector<cv::Mat> channels;
		channels.push_back(right_player_mask);
		channels.push_back(left_contact_mask + right_contact_mask);
		channels.push_back(left_player_mask);

		cv::Mat visimg;
		cv::merge(channels, visimg);

		processor.find_player_faces(rgb_frame, depth_frame);

		if(processor.got_left_face()){
			cv::Rect left_roi = processor.get_left_player_face_roi();
			cv::Rect left_face_window(0, 0, left_roi.width, left_roi.height);
			cv::Mat left_player_face;
			processor.get_left_player_face_image(left_player_face);
			cv::Mat left_face_region(visimg, left_face_window);
			left_player_face.copyTo(left_face_region);
			cv::rectangle(visimg, left_roi, cv::Scalar(0, 255, 0), 5);
			cv::rectangle(visimg, left_face_window, cv::Scalar(0, 255, 0), 5);
		}

		if(processor.got_right_face()){
			cv::Rect right_roi = processor.get_right_player_face_roi();
			cv::Rect right_face_window(visimg.cols - right_roi.width, 0, right_roi.width, right_roi.height);
			cv::Mat right_player_face;
			processor.get_right_player_face_image(right_player_face);
			cv::Mat right_face_region(visimg, right_face_window);
			right_player_face.copyTo(right_face_region);
			cv::rectangle(visimg, right_roi, cv::Scalar(0, 255, 255), 5);
			cv::rectangle(visimg, right_face_window, cv::Scalar(0, 255, 255), 5);
		}

		cv::cvtColor(rgb_frame, bgr_frame, CV_RGB2BGR);



		cv::imshow(win_name_rgb, bgr_frame);
		cv::imshow(win_name_depth, visimg);
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


