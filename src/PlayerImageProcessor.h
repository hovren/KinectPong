/*
 * PlayerImageProcessor.h
 *
 *  Created on: Mar 20, 2014
 *      Author: wallenberg
 */

#include <iostream>
#include <opencv2/opencv.hpp>

#ifndef PLAYERIMAGEPROCESSOR_H_
#define PLAYERIMAGEPROCESSOR_H_

class PlayerImageProcessor {
public:
	PlayerImageProcessor();
	~PlayerImageProcessor();
	void init_detector(std::string data_path);
	bool find_player_face(cv::Mat rgb_frame, cv::Mat& face_image, cv::Rect roi);
	void set_near_limit(double near_limit);
	void set_far_limit(double far_limit);
	double get_near_limit();
	double get_far_limit();
	void get_left_player_mask(cv::Mat& retmask);
	void get_right_player_mask(cv::Mat& retmask);
	void set_player_masks(cv::Mat depth_frame);

private:
	cv::CascadeClassifier m_face_detector;
	cv::Mat m_left_player_face;
	cv::Mat m_right_player_face;
	cv::Mat m_left_player_mask;
	cv::Mat m_right_player_mask;
	cv::Mat m_left_player_contact_mask;
	cv::Mat m_right_player_contact_mask;
	double m_near_limit;
	double m_far_limit;
	int m_min_region_size;

};

#endif /* PLAYERIMAGEPROCESSOR_H_ */
