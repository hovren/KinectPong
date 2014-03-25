/*
 * Player.cpp
 *
 *  Created on: Mar 24, 2014
 *      Author: hannes
 */

#include "Player.h"

#include <cmath>
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>

Player::Player() {
	// TODO Auto-generated constructor stub
	m_name = "Player";
}

Player::~Player() {
	// TODO Auto-generated destructor stub
}

void Player::paddle_input(cv::Mat mask) {
	// Construct texture
	std::vector< std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::Mat mask_for_contours;
	mask.copyTo(mask_for_contours);
	cv::findContours(mask_for_contours, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, cv::Point(0,0));
	//std::cout << "Found " << contours.size() << " contours for player " << player << std::endl;
	cv::Rect bb(0, 0, 1, 1);
	if(contours.size() > 0)
		bb = cv::boundingRect(contours[0]);

	cv::Mat mask_roi = mask(bb).clone();
	m_input_dimensions.x = mask.cols;
	m_input_dimensions.y = mask.rows;
	m_paddle_rect.x = ((float) bb.x) / mask.cols;
	m_paddle_rect.y = ((float) bb.y) / mask.rows;
	m_paddle_rect.width = ((float) bb.width) / mask.cols;
	m_paddle_rect.height = ((float) bb.height) / mask.rows;

	// clear previous contours
	m_paddle_contour.clear();

	if(contours.size() == 0){
		//set mask and gradients to zero and return
		cv::Mat empty_gradient = cv::Mat::zeros(mask.rows, mask.cols, CV_32FC2);
		empty_gradient.copyTo(m_paddle_gradient);
		mask_roi.copyTo(m_paddle_mask);
		return;
	}

	// Update the paddle contours coordinates to lie within bounding box
	for (unsigned int i=0; i < contours[0].size(); ++i) {
		cv::Point contour_point_updated;
		contour_point_updated.x = contours[0][i].x - bb.x;
		contour_point_updated.y = contours[0][i].y - bb.y;
		m_paddle_contour.push_back(contour_point_updated);
	}

	std::cout << "Found bounding box at " << bb.x << ", " << bb.y << " dim " << bb.width << " x " << bb.height;
	std::cout << " of image size " << mask.cols << ", " << mask.rows << std::endl;
	std::cout << "Player paddle set at " << m_paddle_rect.x << ", " << m_paddle_rect.y << std::endl;
	mask_roi.copyTo(m_paddle_mask); // = mask_roi;

	// Filter the mask to find the normals
	cv::Mat mask_blurred, dx, dy;
	std::vector<cv::Mat> deriv(2);
	cv::blur(mask_roi, mask_blurred, cv::Size(5,5));
	cv::Sobel(mask_blurred, deriv[0], CV_32F, 1, 0, 5);
	cv::Sobel(mask_blurred, deriv[1], CV_32F, 0, 1, 5);
	cv::blur(deriv[0], deriv[0], cv::Size(5,5));
	cv::blur(deriv[1], deriv[1], cv::Size(5,5));
	cv::merge(deriv, m_paddle_gradient);
}

bool in_circle(cv::Point2f pos, cv::Point2f circle_pos, float radius) {
	float dx = pos.x - circle_pos.x;
	float dy = pos.y - circle_pos.y;
	float dd = dx*dx + dy*dy;
	return dd < (radius*radius);
}

bool Player::collision(cv::Point2f ball_pos, float ball_radius,
		cv::Vec2f& collision_normal) {

	collision_normal.val[0] = 0;
	collision_normal.val[1] = 1;
	int num_gradients = 0;

	for (unsigned int i=0; i < m_paddle_contour.size(); ++i) {
		cv::Point2f pos;
		pos.x = m_paddle_rect.x + ((float) m_paddle_contour[i].x) / m_input_dimensions.x;
		pos.y = m_paddle_rect.y + ((float) m_paddle_contour[i].y) / m_input_dimensions.y;

		// Is the point inside the circle?
		if (in_circle(pos, ball_pos, ball_radius)) {
			cv::Vec2f gradient = m_paddle_gradient.at<cv::Vec2f>(m_paddle_contour[i]);
			collision_normal.val[0] += gradient.val[0];
			collision_normal.val[1] += gradient.val[1];
			++num_gradients;
		}
	}

	if (num_gradients > 0) {
		float norm = sqrt(collision_normal.val[0]*collision_normal.val[0] + collision_normal.val[1]*collision_normal.val[1]);
		collision_normal.val[0] /= norm;
		collision_normal.val[1] /= norm;
		return true;
	}
	else {
		return false;
	}
}
