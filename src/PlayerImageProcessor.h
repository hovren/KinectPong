/*
 * PlayerImageProcessor.h
 *
 *  Created on: Mar 20, 2014
 *      Author: wallenberg
 */

#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>

#ifndef PLAYERIMAGEPROCESSOR_H_
#define PLAYERIMAGEPROCESSOR_H_

class PlayerImageProcessor {
public:
	PlayerImageProcessor();
	~PlayerImageProcessor();
	void init_detector(std::string data_path);
	bool init_player_faces(cv::Mat rgb_frame, cv::Mat depth_frame);
	void find_player_faces(cv::Mat rgb_frame, cv::Mat depth_frame);
	void find_left_player_face(cv::Mat frame_gray, cv::Mat depth_frame);
	void find_right_player_face(cv::Mat frame_gray, cv::Mat depth_frame);

	void set_near_limit(double near_limit);
	void set_far_limit(double far_limit);
	double get_near_limit();
	double get_far_limit();

	cv::Rect get_left_player_face_roi();
	void get_left_player_face_image(cv::Mat& face_image);
	int get_left_player_face_depth();
	cv::Rect get_right_player_face_roi();
	void get_right_player_face_image(cv::Mat& face_image);
	int get_right_player_face_depth();
	bool got_left_face();
	bool got_right_face();

	void get_left_player_mask(cv::Mat& retmask);
	void get_right_player_mask(cv::Mat& retmask);

	void get_left_player_contact_mask(cv::Mat& retmask);
	void get_right_player_contact_mask(cv::Mat& retmask);

	void set_player_masks(cv::Mat depth_frame);

	void threshold_depth_frame(cv::Mat depth_frame, cv::Mat& threshold_frame);
	int calc_area(cv::Mat mask);
	void select_largest(cv::Mat& label_image, int n_regions);

private:
	cv::CascadeClassifier m_face_detector;
	cv::Rect m_left_player_face_position;
	cv::Rect m_right_player_face_position;
	cv::Mat m_left_player_face_image;
	cv::Mat m_right_player_face_image;
	int m_left_player_face_depth;
	int m_right_player_face_depth;
	bool m_tracking_left;
	bool m_tracking_right;
	cv::Mat m_left_player_mask;
	cv::Mat m_right_player_mask;
	cv::Mat m_left_player_contact_mask;
	cv::Mat m_right_player_contact_mask;
	double m_near_limit;
	double m_far_limit;
	unsigned int m_min_contour_size;
	cv::Size m_min_face_size;
	cv::Size m_max_face_size;
	int m_min_player_area;
	int m_max_player_area;

};

#endif /* PLAYERIMAGEPROCESSOR_H_ */
