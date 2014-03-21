/*
 * PlayerImageProcessor.cpp
 *
 *  Created on: Mar 20, 2014
 *      Author: wallenberg
 */

#include "PlayerImageProcessor.h"

PlayerImageProcessor::PlayerImageProcessor() {
	m_near_limit = 0;
	m_far_limit = 0;
	m_min_region_size = 640*480;
}

PlayerImageProcessor::~PlayerImageProcessor() {

}

void PlayerImageProcessor::init_detector(std::string data_path)
{
	m_face_detector.load(data_path + "/haarcascade_frontalface_default.xml");
}

bool PlayerImageProcessor::find_player_face(cv::Mat rgb_frame, cv::Mat& face_image, cv::Rect roi)
{
	cv::Mat frame_roi(rgb_frame, roi);
	cv::Mat frame_roi_gray;
	cv::cvtColor(frame_roi, frame_roi_gray, CV_RGB2GRAY);
	std::vector<cv::Rect> detections;
	m_face_detector.detectMultiScale(frame_roi_gray, detections);
	if(detections.size() > 0){
		cv::Mat face_roi(frame_roi, detections.at(0));
		face_roi.copyTo(face_image);
		return true;
	}
	else{
		return false;
	}
}

void PlayerImageProcessor::set_near_limit(double near_limit)
{
	m_near_limit = near_limit;
}

void PlayerImageProcessor::set_far_limit(double far_limit)
{
	m_far_limit = far_limit;
}

double PlayerImageProcessor::get_near_limit()
{
	return m_near_limit;
}

double PlayerImageProcessor::get_far_limit()
{
	return m_far_limit;
}

void PlayerImageProcessor::get_left_player_mask(cv::Mat& retmask)
{
	m_left_player_mask.copyTo(retmask);
}

void PlayerImageProcessor::get_right_player_mask(cv::Mat& retmask)
{
	m_right_player_mask.copyTo(retmask);
}

void PlayerImageProcessor::set_player_masks(cv::Mat depth_frame)
{
	cv::Mat threshold_image_low;
	cv::Mat threshold_image_high;
	cv::Mat float_depth;
	depth_frame.convertTo(float_depth, CV_32F);

	cv::threshold(float_depth, threshold_image_low, m_near_limit, 1.0, cv::THRESH_BINARY);
	cv::threshold(float_depth, threshold_image_high, m_far_limit, 1.0, cv::THRESH_BINARY_INV);
	cv::Mat player_mask = threshold_image_low.mul(threshold_image_high);

	player_mask = player_mask.mul(float_depth);

	player_mask.copyTo(m_left_player_mask);

}
