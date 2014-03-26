/*
 * PlayerImageProcessor.cpp
 *
 *  Created on: Mar 20, 2014
 *      Author: wallenberg
 */

#include "PlayerImageProcessor.h"

PlayerImageProcessor::PlayerImageProcessor() {
	m_near_limit = 600;
	m_far_limit = 2000;
	m_min_contour_size = 500;

	m_min_player_area = 500;
	m_max_player_area = 640*480/3;

	m_left_player_face_depth = 0;
	m_left_player_face_position.x = 0;
	m_left_player_face_position.y = 0;
	m_left_player_face_position.width = 0;
	m_left_player_face_position.height = 0;
	m_tracking_left = false;

	m_right_player_face_depth = 0;
	m_right_player_face_position.x = 0;
	m_right_player_face_position.y = 0;
	m_right_player_face_position.width = 0;
	m_right_player_face_position.height = 0;
	m_tracking_right = false;

	m_min_face_size = cv::Size(20, 30);
	m_max_face_size = cv::Size(60, 90);

	cv::Mat empty_frame = cv::Mat::zeros(cv::Size(640, 480), CV_8UC1);
	empty_frame.copyTo(m_left_player_mask);
	empty_frame.copyTo(m_right_player_mask);
	empty_frame.copyTo(m_left_player_contact_mask);
	empty_frame.copyTo(m_right_player_contact_mask);

	cv::Mat empty_face = cv::Mat::zeros(cv::Size(50, 50), CV_8UC3);
	empty_face.copyTo(m_left_player_face_image);
	empty_face.copyTo(m_right_player_face_image);

}

PlayerImageProcessor::~PlayerImageProcessor() {

}

void PlayerImageProcessor::init_detector(std::string data_path)
{
	m_face_detector.load(data_path + "/haarcascade_frontalface_default.xml");
}

bool PlayerImageProcessor::init_player_faces(cv::Mat rgb_frame, cv::Mat depth_frame)
{
	cv::Mat frame_gray;
	cv::cvtColor(rgb_frame, frame_gray, CV_RGB2GRAY);

	cv::Mat float_depth;
	depth_frame.convertTo(float_depth, CV_32FC1);

	cv::Mat threshold_image;
	cv::add(m_left_player_mask, m_right_player_mask, threshold_image, cv::noArray(), CV_8U);

	cv::Mat weight_image;
	threshold_image.convertTo(weight_image, CV_8UC1);
	weight_image /= 255;
	frame_gray = frame_gray.mul(weight_image);

	std::vector<cv::Rect> detections;
	m_face_detector.detectMultiScale(frame_gray, detections, 2, 3, 0, m_min_face_size, m_max_face_size);

	std::vector<cv::Rect> left_detections;
	std::vector<cv::Rect> right_detections;

	for(unsigned int k=0;k<detections.size();k++){
		if((detections.at(k).width > 30)&&(detections.at(k).height > 30)
				&&(detections.at(k).y < rgb_frame.rows/2)){
			if((detections.at(k).x + detections.at(k).width/2) < rgb_frame.cols/2)
				left_detections.push_back(detections.at(k));
			else
				right_detections.push_back(detections.at(k));
		}
	}

	if(left_detections.size() > 0){
		m_left_player_face_position = left_detections.at(0);
		int row = (int)round(m_left_player_face_position.y+m_left_player_face_position.height/2);
		int col = (int)round(m_left_player_face_position.x+m_left_player_face_position.width/2);
		m_left_player_face_depth = depth_frame.at<int>(row, col);

		//Increase face ROI size by 3x
		cv::Rect roi(m_left_player_face_position);
		roi.x = std::max(0, roi.x - roi.width);
		int max_width = frame_gray.cols - roi.x;
		roi.width = std::min(3*roi.width, max_width);
		roi.y = std::max(0, roi.y - roi.height);
		int max_height = frame_gray.rows - roi.y;
		roi.height = std::min(3*roi.height, max_height);

		m_left_player_face_position = roi;

		m_tracking_left = true;
		//std::cout << "Got left face" << std::endl;
	}
	else{
		m_tracking_left = false;
	}


	if(right_detections.size() > 0){
		m_right_player_face_position = right_detections.at(0);
		int row = (int)round(m_right_player_face_position.y+m_right_player_face_position.height/2);
		int col = (int)round(m_right_player_face_position.x+m_right_player_face_position.width/2);
		m_right_player_face_depth = depth_frame.at<int>(row, col);

		//Increase face ROI size by 3x
		cv::Rect roi(m_right_player_face_position);
		roi.x = std::max(0, roi.x - roi.width);
		int max_width = frame_gray.cols - roi.x;
		roi.width = std::min(3*roi.width, max_width);
		roi.y = std::max(0, roi.y - roi.height);
		int max_height = frame_gray.rows - roi.y;
		roi.height = std::min(3*roi.height, max_height);

		m_right_player_face_position = roi;

		m_tracking_right = true;
		//std::cout << "Got right face" << std::endl;
	}
	else{
		m_tracking_right = false;
	}

	if(m_tracking_left && m_tracking_right){
		std::cout << "Both faces found" << std::endl;
		return true;
	}
	else{
		return false;
	}
}

void PlayerImageProcessor::find_player_faces(cv::Mat rgb_frame, cv::Mat depth_frame)
{
	cv::Mat frame_gray;
	cv::cvtColor(rgb_frame, frame_gray, CV_RGB2GRAY);

	cv::Mat float_depth;
	depth_frame.convertTo(float_depth, CV_32FC1);

	cv::Mat threshold_image;
	cv::add(m_left_player_mask, m_right_player_mask, threshold_image, cv::noArray(), CV_8UC1);

	cv::Mat weight_image;
	threshold_image.convertTo(weight_image, CV_8UC1);
	weight_image /= 255;
	frame_gray = frame_gray.mul(weight_image);

	bool redetectfl = (!m_tracking_left || !m_tracking_right);

	if(redetectfl){
		init_player_faces(rgb_frame, depth_frame);
	}
	if(m_tracking_left && !redetectfl)
		find_left_player_face(frame_gray, depth_frame);
	if(m_tracking_right && !redetectfl)
		find_right_player_face(frame_gray, depth_frame);

	cv::Mat left_face_image(rgb_frame, m_left_player_face_position);
	left_face_image.copyTo(m_left_player_face_image);
	cv::Mat right_face_image(rgb_frame, m_right_player_face_position);
	right_face_image.copyTo(m_right_player_face_image);
}

void PlayerImageProcessor::find_left_player_face(cv::Mat frame_gray, cv::Mat depth_frame)
{
	cv::Rect roi(m_left_player_face_position);
	roi.x = std::max(0, roi.x - roi.width);
	int max_width = frame_gray.cols - roi.x;
	roi.width = std::min(3*roi.width, max_width);
	roi.y = std::max(0, roi.y - roi.height);
	int max_height = frame_gray.rows - roi.y;
	roi.height = std::min(3*roi.height, max_height);

	cv::Mat face_roi(frame_gray, roi);
	std::vector<cv::Rect> detections;
	m_face_detector.detectMultiScale(face_roi, detections, 2, 3, 0, m_min_face_size, m_max_face_size);

	if(detections.size() > 0){
		m_left_player_face_position = detections.at(0);
		m_left_player_face_position.x += roi.x;
		m_left_player_face_position.y += roi.y;
		int row = (int)round(m_left_player_face_position.y+m_left_player_face_position.height/2);
		int col = (int)round(m_left_player_face_position.x+m_left_player_face_position.width/2);
		m_left_player_face_depth = depth_frame.at<int>(row, col);

		//Increase face ROI size by 3x
		cv::Rect large_roi(m_left_player_face_position);
		large_roi.x = std::max(0, large_roi.x - large_roi.width);
		int max_width = frame_gray.cols - large_roi.x;
		large_roi.width = std::min(3*large_roi.width, max_width);
		large_roi.y = std::max(0, large_roi.y - large_roi.height);
		int max_height = frame_gray.rows - large_roi.y;
		large_roi.height = std::min(3*large_roi.height, max_height);

		m_left_player_face_position = large_roi;

		m_tracking_left = true;
	}
	else{
		m_tracking_left = false;
	}

}

void PlayerImageProcessor::find_right_player_face(cv::Mat frame_gray, cv::Mat depth_frame)
{
	cv::Rect roi(m_right_player_face_position);
	roi.x = std::max(0, roi.x - roi.width);
	int max_width = frame_gray.cols - roi.x;
	roi.width = std::min(3*roi.width, max_width);
	roi.y = std::max(0, roi.y - roi.height);
	int max_height = frame_gray.rows - roi.y;
	roi.height = std::min(3*roi.height, max_height);

	cv::Mat face_roi(frame_gray, roi);
	std::vector<cv::Rect> detections;
	m_face_detector.detectMultiScale(face_roi, detections, 1.1, 3, 0, m_min_face_size, m_max_face_size);

	if(detections.size() > 0){
		m_right_player_face_position = detections.at(0);
		m_right_player_face_position.x += roi.x;
		m_right_player_face_position.y += roi.y;
		int row = (int)round(m_right_player_face_position.y+m_right_player_face_position.height/2);
		int col = (int)round(m_right_player_face_position.x+m_right_player_face_position.width/2);
		m_right_player_face_depth = depth_frame.at<int>(row, col);

		//Increase face ROI size by 3x
		cv::Rect large_roi(m_right_player_face_position);
		large_roi.x = std::max(0, large_roi.x - large_roi.width);
		int max_width = frame_gray.cols - large_roi.x;
		large_roi.width = std::min(3*large_roi.width, max_width);
		large_roi.y = std::max(0, large_roi.y - large_roi.height);
		int max_height = frame_gray.rows - large_roi.y;
		large_roi.height = std::min(3*large_roi.height, max_height);

		m_right_player_face_position = large_roi;

		m_tracking_right = true;
	}
	else{
		m_tracking_right = false;
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

void PlayerImageProcessor::get_left_player_contact_mask(cv::Mat& retmask)
{
	m_left_player_contact_mask.copyTo(retmask);
}

void PlayerImageProcessor::get_right_player_contact_mask(cv::Mat& retmask)
{
	m_right_player_contact_mask.copyTo(retmask);
}

void PlayerImageProcessor::set_player_masks(cv::Mat depth_frame)
{
	cv::Mat float_depth;
	depth_frame.convertTo(float_depth, CV_32F);

	cv::Mat player_mask;
	threshold_depth_frame(float_depth, player_mask);

	//find contours and label them
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::Mat contour_image;
	player_mask.convertTo(contour_image, CV_8UC1, 255);
	//label regions
	cv::Mat label_image;
	contour_image.copyTo(label_image);
	cv::findContours(contour_image, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	for(unsigned int k=0;k<contours.size();k++){
		if(contours.at(k).size() >= m_min_contour_size)
			cv::floodFill(label_image, contours.at(k).at(0), cv::Scalar(254 - k));
		else
			cv::floodFill(label_image, contours.at(k).at(0), cv::Scalar(0));
	}

	cv::Rect left_half(0, 0, depth_frame.cols/2, depth_frame.rows);
	cv::Mat left_player_half_image(m_left_player_mask, left_half);
	cv::Mat left_half_mask(label_image, left_half);
	cv::Rect right_half(depth_frame.cols/2, 0, depth_frame.cols/2, depth_frame.rows);
	cv::Mat right_player_half_image(m_right_player_mask, right_half);
	cv::Mat right_half_mask(label_image, right_half);

	select_largest(left_half_mask, contours.size());
	select_largest(right_half_mask, contours.size());


	int left_area = calc_area(left_half_mask);
	int right_area = calc_area(right_half_mask);


	cv::Mat strel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(11, 11), cv::Point(5, 5));

	if(left_area > 0){
		left_half_mask.copyTo(left_player_half_image);
		double left_min_val, left_max_val;
		cv::Point left_min, left_max;
		cv::Mat left_depth(float_depth, left_half);
		cv::Mat left_contact_half(m_left_player_contact_mask, left_half);

		cv::minMaxLoc(left_depth, &left_min_val, &left_max_val, &left_min, &left_max, left_player_half_image);
		cv::Mat left_contact_threshold_float, left_contact_threshold;
		cv::threshold(left_depth, left_contact_threshold_float, left_min_val+200, 255, CV_THRESH_BINARY_INV);
		left_contact_threshold_float.convertTo(left_contact_threshold, CV_8UC1, 1);
		left_contact_half.setTo(0);

		if((left_min.x > 0) && (left_min.y > 0)){
			left_contact_half.at<uchar>(left_min.y, left_min.x) = 255;
			int left_area = calc_area(left_player_half_image);
			int left_contact_area = 0;
			int old_left_contact_area = 1;
			while((left_contact_area < left_area/10) && (old_left_contact_area != left_contact_area)){
				old_left_contact_area = left_contact_area;
				cv::Mat new_mask;
				cv::dilate(left_contact_half, new_mask, strel, cv::Point(-1, -1), 5);
				new_mask.copyTo(left_contact_half, left_contact_threshold.mul(left_player_half_image));
				left_contact_area = calc_area(left_contact_half);
			}
		}
		//smooth outlines
		cv::dilate(left_contact_half, left_contact_half, strel, cv::Point(-1, -1), 3);
		cv::erode(left_contact_half, left_contact_half, strel, cv::Point(-1, -1), 3);
	}

	if(right_area > 0){
		right_half_mask.copyTo(right_player_half_image);
		double right_min_val, right_max_val;
		cv::Point right_min, right_max;
		cv::Mat right_depth(float_depth, right_half);
		cv::Mat right_contact_half(m_right_player_contact_mask, right_half);

		cv::minMaxLoc(right_depth, &right_min_val, &right_max_val, &right_min, &right_max, right_player_half_image);
		cv::Mat right_contact_threshold_float, right_contact_threshold;
		cv::threshold(right_depth, right_contact_threshold_float, right_min_val+200, 255, CV_THRESH_BINARY_INV);
		right_contact_threshold_float.convertTo(right_contact_threshold, CV_8UC1, 1);
		right_contact_half.setTo(0);

		if((right_min.x > 0) && (right_min.y > 0)){
			right_contact_half.at<uchar>(right_min.y, right_min.x) = 255;
			int right_area = calc_area(right_player_half_image);
			int right_contact_area = 0;
			int old_right_contact_area = 1;
			while((right_contact_area < right_area/10) && (old_right_contact_area != right_contact_area)){
				old_right_contact_area = right_contact_area;
				cv::Mat new_mask;
				cv::dilate(right_contact_half, new_mask, strel, cv::Point(-1, -1), 5);
				new_mask.copyTo(right_contact_half, right_contact_threshold.mul(right_player_half_image));
				right_contact_area = calc_area(right_contact_half);
			}
		}
		//smooth outlines
		cv::dilate(right_contact_half, right_contact_half, strel, cv::Point(-1, -1), 3);
		cv::erode(right_contact_half, right_contact_half, strel, cv::Point(-1, -1), 3);
	}

}

void PlayerImageProcessor::select_largest(cv::Mat& label_image, int n_regions)
{
	//for each region, calculate area and return mask with only the largest one
	//assume label image regions are numbered as 254 - k
	int max_area = 0;
	int max_index = 0;
	for(int k=0;k<n_regions;k++){
		cv::Mat region_mask(label_image == (254 -k));
		int area = calc_area(region_mask);
		if(area > max_area){
			max_area = area;
			max_index = 254 - k;
		}
	}
	if((max_area > m_min_player_area) && (max_area <= m_max_player_area))
		label_image = (label_image == max_index);
	else
		label_image.setTo(0);
}


void PlayerImageProcessor::threshold_depth_frame(cv::Mat depth_frame, cv::Mat& threshold_frame)
{
	cv::Mat threshold_image_low, threshold_image_high;
	cv::threshold(depth_frame, threshold_image_low, m_near_limit, 1.0, cv::THRESH_BINARY);
	cv::threshold(depth_frame, threshold_image_high, m_far_limit, 1.0, cv::THRESH_BINARY_INV);
	threshold_frame = threshold_image_low.mul(threshold_image_high);
}

cv::Rect PlayerImageProcessor::get_left_player_face_roi()
{
	return m_left_player_face_position;
}

int PlayerImageProcessor::get_left_player_face_depth()
{
	return m_left_player_face_depth;
}

cv::Rect PlayerImageProcessor::get_right_player_face_roi()
{
	return m_right_player_face_position;
}

int PlayerImageProcessor::get_right_player_face_depth()
{
	return m_right_player_face_depth;
}

int PlayerImageProcessor::calc_area(cv::Mat mask)
{
	cv::Scalar area = cv::sum(mask);
	if(mask.type() == CV_8UC1)
		area.val[0] /= 255;
	return area.val[0];
}

bool PlayerImageProcessor::got_left_face()
{
	return m_tracking_left;
}

bool PlayerImageProcessor::got_right_face()
{
	return m_tracking_right;
}

void PlayerImageProcessor::get_left_player_face_image(cv::Mat& face_image)
{
	m_left_player_face_image.copyTo(face_image);
}

void PlayerImageProcessor::get_right_player_face_image(cv::Mat& face_image)
{
	m_right_player_face_image.copyTo(face_image);
}

void PlayerImageProcessor::reset() {
	m_tracking_left = false;
	m_tracking_right = false;
}
