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
#include <opencv2/highgui/highgui.hpp>

#define BB_PADDING 16

Player::Player(GameBoard* gb) {
	// TODO Auto-generated constructor stub
	m_name = "Player";
	m_paddle_tex = NULL;
	m_gameboard = gb;
	m_score = 0;
}

Player::~Player() {
	// TODO Auto-generated destructor stub
	if (m_paddle_tex) {
		SDL_DestroyTexture(m_paddle_tex);
	}
}

void Player::paddle_input(cv::Mat mask) {
	// Construct texture
	std::vector< std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::Mat mask_for_contours;
	mask.copyTo(mask_for_contours);
	cv::findContours(mask_for_contours, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, cv::Point(0,0));
	//std::cout << "Found " << contours.size() << " contours for player " << player << std::endl;
	cv::Rect bb = cv::boundingRect(contours[0]);
	std::cout << "bb was " << bb << std::endl;
	bb.x = std::max<int>(0, bb.x - BB_PADDING);
	bb.y = std::max<int>(0, bb.y - BB_PADDING);
	bb.width = bb.x + bb.width + 2*BB_PADDING > mask.cols ? mask.cols - bb.x : bb.width + 2*BB_PADDING;
	bb.height = bb.y + bb.height + 2*BB_PADDING > mask.rows ? mask.rows - bb.y : bb.height + 2*BB_PADDING;
	std::cout << "bb is " << bb << std::endl;
	cv::Mat mask_roi = mask(bb).clone();
	m_input_dimensions.x = mask.cols;
	m_input_dimensions.y = mask.rows;
	m_paddle_rect.x = ((float) bb.x) / mask.cols;
	m_paddle_rect.y = ((float) bb.y) / mask.rows;
	m_paddle_rect.width = ((float) bb.width) / mask.cols;
	m_paddle_rect.height = ((float) bb.height) / mask.rows;

	// Update the paddle contours coordinates to lie within bounding box
	m_paddle_contour.clear();
	for (int i=0; i < contours[0].size(); ++i) {
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
	cv::Mat mask_blurred;
	std::vector<cv::Mat> deriv(2);
	cv::imwrite("mask_roi.png", mask_roi);
	cv::blur(mask_roi, mask_blurred, cv::Size(3,3));
	cv::Sobel(mask_blurred, deriv[0], CV_32F, 1, 0, 3);
	cv::Sobel(mask_blurred, deriv[1], CV_32F, 0, 1, 3);

	cv::blur(deriv[0], deriv[0], cv::Size(5,5));
	cv::blur(deriv[1], deriv[1], cv::Size(5,5));

	cv::merge(deriv, m_paddle_gradient);

	// Set new texture
	if (m_paddle_tex) {
		SDL_DestroyTexture(m_paddle_tex);
	}
	m_paddle_tex = m_gameboard->get_game()->texture_from_mat(mask_roi);
}

bool in_circle(cv::Point2f pos, cv::Point2f circle_pos, float radius) {
	float dx = pos.x - circle_pos.x;
	float dy = pos.y - circle_pos.y;
	float dd = dx*dx + dy*dy;
	return dd < (radius*radius);
}

float line_to_point_distance(cv::Point2f p, cv::Point2f a, cv::Point2f n, float& t) {
	cv::Point2f q = (a - p) - (a - p).dot(n) * n;
	float d = cv::norm(q);
	return d;
}

bool Player::collision_line(cv::Point2f ball_pos, cv::Point2f ball_velocity,
		float ball_radius, float dt, cv::Vec2f& collision_normal,
		cv::Point2f& collision_point) {
	cv::Point2f n = ball_velocity * (1.0 / cv::norm(ball_velocity));
	float best_t = std::numeric_limits<float>::max();
	float max_t = cv::norm(ball_velocity*dt);
	bool collided = false;
	for (int i=0; i < m_paddle_contour.size(); ++i) {
		cv::Point2f pos;
		pos.x = m_paddle_rect.x + ((float) m_paddle_contour[i].x) / m_input_dimensions.x;
		pos.y = m_paddle_rect.y + ((float) m_paddle_contour[i].y) / m_input_dimensions.y;

		float s;
		float d = line_to_point_distance(pos, ball_pos, n, s);

		if (d < ball_radius) {
			cv::Point2f ap = pos - ball_pos;
			float a = ap.x*n.x + ap.y*n.y;
			float b = a*a - ap.x*ap.x - ap.y*ap.y + ball_radius*ball_radius;
			float t1 = a + sqrt(b);
			float t2 = a - sqrt(b);
			float t = ((t1 > 0) && (t1 < t2)) ? t1 : t2;
			if ((t > 0) && (t <= max_t) && (t < best_t)) {
				best_t = t;
				collision_point = ball_pos + t*n;
				cv::Point cpos = m_paddle_contour[i];
				//std::cout << "testing gradient pixel at " << cpos << " ";
				cv::Vec2f gradient = m_paddle_gradient.at<cv::Vec2f>(cpos.y, cpos.x);
				float gradient_norm = cv::norm(gradient);
				//std::cout << "value " << gradient << " norm " << gradient_norm << " mask val " << (unsigned int) m_paddle_mask.at<unsigned char>(cpos.x, cpos.y) << std::endl;
				collision_normal.val[0] = gradient.val[0] / gradient_norm;
				collision_normal.val[1] = gradient.val[1] / gradient_norm;
				collided = true;
			}
		}
	}

	collision_normal *= (1.0 / cv::norm(collision_normal));

	return collided;
}
