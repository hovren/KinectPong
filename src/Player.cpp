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
	deriv[0] = cv::abs(deriv[0]);
	deriv[1] = cv::abs(deriv[1]);

	cv::imwrite("sobelx.png", deriv[0]);
	cv::imwrite("sobely.png", deriv[1]);


	//cv::blur(deriv[0], deriv[0], cv::Size(8,8));
	//cv::blur(deriv[1], deriv[1], cv::Size(8,8));

	//cv::imwrite("sobelx_blur.png", deriv[0]);
	//cv::imwrite("sobely_blur.png", deriv[1]);

	cv::merge(deriv, m_paddle_gradient);
#if 0
	std::cout << "c = array([" << std::endl;
	for (int i=0; i < m_paddle_contour.size(); ++i) {
		std::cout << m_paddle_contour[i] << ", " << std::endl;
	}
	std::cout << "])"<<std::endl;
#endif

	std::cout << m_paddle_gradient.depth() << " is CV_32F " << CV_32F << " two channels " << m_paddle_gradient.channels() << std::endl;

	cv::Mat sobx_out, soby_out;
	double max_val, min_val;
	cv::minMaxLoc(deriv[0], &min_val, &max_val);
	deriv[0].convertTo(sobx_out, CV_8UC1, 255.0 / (max_val - min_val), -min_val*255.0/(max_val - min_val));
	cv::cvtColor(sobx_out, sobx_out, CV_GRAY2RGB);

	for (int i=0; i < m_paddle_contour.size(); ++i) {
		sobx_out.at<cv::Vec3b>(m_paddle_contour[i].y, m_paddle_contour[i].x)[0] = 0;
		sobx_out.at<cv::Vec3b>(m_paddle_contour[i].y, m_paddle_contour[i].x)[1] = 0;
		sobx_out.at<cv::Vec3b>(m_paddle_contour[i].y, m_paddle_contour[i].x)[2] = 255;
	}

	cv::imwrite("sobelx_rgb.png", sobx_out);



#if 0
	std::cout << "size and channels in gradient image " << m_paddle_gradient.cols << " x " << m_paddle_gradient.rows << " channels " << m_paddle_gradient.channels() << std::endl;
	for (int r=0; r < m_paddle_gradient.rows; ++r) {
		for (int c=0; c < m_paddle_gradient.cols; ++c) {
			cv::Vec2f pixel = m_paddle_gradient.at<cv::Vec2f>(c, r);
			std::cout << c << ", " << r << " : " << pixel << std::endl;
		}
	}
#endif
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
	for (int i=0; i < m_paddle_contour.size(); ++i) {
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
