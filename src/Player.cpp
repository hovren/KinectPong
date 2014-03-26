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
	m_player_tex = NULL;
	m_gameboard = gb;
	m_score = 0;
}

Player::~Player() {
	// TODO Auto-generated destructor stub
	if (m_paddle_tex) {
		SDL_DestroyTexture(m_paddle_tex);
	}
	if(m_player_tex){
		SDL_DestroyTexture(m_player_tex);
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
	cv::Rect bb(0, 0, 1, 1);
	if(contours.size() > 0)
		bb = cv::boundingRect(contours[0]);

	//std::cout << "bb was " << bb << std::endl;
	bb.x = std::max<int>(0, bb.x - BB_PADDING);
	bb.y = std::max<int>(0, bb.y - BB_PADDING);
	bb.width = bb.x + bb.width + 2*BB_PADDING > mask.cols ? mask.cols - bb.x : bb.width + 2*BB_PADDING;
	bb.height = bb.y + bb.height + 2*BB_PADDING > mask.rows ? mask.rows - bb.y : bb.height + 2*BB_PADDING;
	//std::cout << "bb is " << bb << std::endl;
	cv::Mat mask_roi = mask(bb).clone();
	m_input_dimensions.x = mask.cols;
	m_input_dimensions.y = mask.rows;
	m_paddle_rect.x = ((float) bb.x) / mask.cols;
	m_paddle_rect.y = ((float) bb.y) / mask.rows;
	m_paddle_rect.width = ((float) bb.width) / mask.cols;
	m_paddle_rect.height = ((float) bb.height) / mask.rows;

	// clear previous contours and gradietns
	m_paddle_contour.clear();
	m_paddle_gradient.clear();

	if(contours.size() == 0){
		//set mask and gradients to zero and return
		cv::Mat empty_gradient = cv::Mat::zeros(mask.rows, mask.cols, CV_32FC2);
		//empty_gradient.copyTo(m_paddle_gradient);
		mask_roi.copyTo(m_paddle_mask);
		return;
	}

//	std::cout << "Found bounding box at " << bb.x << ", " << bb.y << " dim " << bb.width << " x " << bb.height;
//	std::cout << " of image size " << mask.cols << ", " << mask.rows << std::endl;
//	std::cout << "Player paddle set at " << m_paddle_rect.x << ", " << m_paddle_rect.y << std::endl;
	mask_roi.copyTo(m_paddle_mask); // = mask_roi;

	// Filter the mask to find the normals
	cv::Mat mask_blurred;
	std::vector<cv::Mat> deriv(2);
	cv::blur(mask_roi, mask_blurred, cv::Size(3,3));
	cv::Sobel(mask_blurred, deriv[0], CV_32F, 1, 0, 3);
	cv::Sobel(mask_blurred, deriv[1], CV_32F, 0, 1, 3);

	cv::GaussianBlur(deriv[0], deriv[0], cv::Size(11,11), 1.0, 1.0);
	cv::GaussianBlur(deriv[1], deriv[1], cv::Size(11,11), 1.0, 1.0);
//	cv::blur(deriv[0], deriv[0], cv::Size(16,16));
//	cv::blur(deriv[1], deriv[1], cv::Size(16,16));
	cv::Mat gradient_image;
	cv::merge(deriv, gradient_image);

	// Extract normals at contour points
	// Remove contour points with zero gradient
	for (unsigned int i=0; i < contours[0].size(); ++i) {
		cv::Point abs_pos = cv::Point(contours[0][i].x, contours[0][i].y); // Position inside bounding box
		cv::Point rel_pos = cv::Point(abs_pos.x - bb.x, abs_pos.y - bb.y); // Position inside bounding box
		cv::Point2f gpos((float) abs_pos.x / m_input_dimensions.x, (float) abs_pos.y / m_input_dimensions.y); // Game position
		cv::Vec2f g = gradient_image.at<cv::Vec2f>(rel_pos.y, rel_pos.x);
		float norm = cv::norm(g);

		if (norm > std::numeric_limits<float>::epsilon()) {
			g *= (1.0 / norm);
			m_paddle_gradient.push_back(cv::Point2f(g.val[0], g.val[1]));
			m_paddle_contour.push_back(gpos);
		}
		else {
			//std::cout << "Skipped point with norm " << norm << std::endl;
		}
	}


	// Set new texture
	if (m_paddle_tex) {
		SDL_DestroyTexture(m_paddle_tex);
	}
	std::vector<cv::Mat> channels0;
	for(unsigned int k=0;k<4;k++) {
		channels0.push_back(mask_roi);
	}
	cv::Mat argb_texture_mat;
	cv::merge(channels0, argb_texture_mat);
	m_paddle_tex = m_gameboard->get_game()->texture_from_mat(argb_texture_mat);
}

void Player::player_input(cv::Mat mask) {
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

	//std::cout << "bb was " << bb << std::endl;
	bb.x = std::max<int>(0, bb.x - BB_PADDING);
	bb.y = std::max<int>(0, bb.y - BB_PADDING);
	bb.width = bb.x + bb.width + 2*BB_PADDING > mask.cols ? mask.cols - bb.x : bb.width + 2*BB_PADDING;
	bb.height = bb.y + bb.height + 2*BB_PADDING > mask.rows ? mask.rows - bb.y : bb.height + 2*BB_PADDING;
	//std::cout << "bb is " << bb << std::endl;
	cv::Mat mask_roi = mask(bb).clone();
	m_player_dimensions.x = mask.cols;
	m_player_dimensions.y = mask.rows;
	m_player_rect.x = ((float) bb.x) / mask.cols;
	m_player_rect.y = ((float) bb.y) / mask.rows;
	m_player_rect.width = ((float) bb.width) / mask.cols;
	m_player_rect.height = ((float) bb.height) / mask.rows;

	mask_roi.copyTo(m_player_mask);

	if(contours.size() == 0){
		return;
	}

	// Set new texture
	if (m_player_tex) {
		SDL_DestroyTexture(m_player_tex);
	}
	std::vector<cv::Mat> channels0;
	for(unsigned int k=0;k<4;k++) {
		if((k==0) || (k==2))
			channels0.push_back(mask_roi);
		else
			channels0.push_back(cv::Mat::zeros(mask_roi.rows, mask_roi.cols, CV_8UC1));
	}
	cv::Mat rgba_texture_mat;
	cv::merge(channels0, rgba_texture_mat);
	m_player_tex = m_gameboard->get_game()->texture_from_mat(rgba_texture_mat);
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
	for (unsigned int i=0; i < m_paddle_contour.size(); ++i) {
		cv::Point2f gpos = m_paddle_contour[i];
		float s;
		float d = line_to_point_distance(gpos, ball_pos, n, s);

		if (d < ball_radius) {
			cv::Point2f ap = gpos - ball_pos;
			float a = ap.x*n.x + ap.y*n.y;
			float b = a*a - ap.x*ap.x - ap.y*ap.y + ball_radius*ball_radius;
			float t1 = a + sqrt(b);
			float t2 = a - sqrt(b);
			float t = ((t1 > 0) && (t1 < t2)) ? t1 : t2;
			if ((t > 0) && (t <= max_t) && (t < best_t)) {
				best_t = t;
				collision_point = ball_pos + t*n;
				collision_normal.val[0] = m_paddle_gradient[i].x;
				collision_normal.val[1] = m_paddle_gradient[i].y;
				collided = true;
			}
		}
	}

	collision_normal *= (1.0 / cv::norm(collision_normal));

	return collided;
}

void Player::get_paddle_gradients(std::vector<cv::Point2f>& contour,
		std::vector<cv::Point2f>& gradient) {
	contour.assign(m_paddle_contour.begin(), m_paddle_contour.end());
	gradient.assign(m_paddle_gradient.begin(), m_paddle_gradient.end());
#if 0
	//contour.assign(m_paddle_contour.begin(), m_paddle_contour.end());
	gradient.clear();
	contour.clear();
	for (int i=0; i < m_paddle_contour.size(); ++i) {
		cv::Point pos = m_paddle_contour[i];
		cv::Point2f gpos((float) pos.x / m_input_dimensions.x, (float) pos.y / m_input_dimensions.y);
		cv::Vec2f g = m_paddle_gradient.at<cv::Vec2f>(pos.y, pos.x);
		g *= (1.0 / cv::norm(g));
		gradient.push_back(cv::Point2f(g.val[0], g.val[1]));
		contour.push_back(gpos);
	}
#endif
}
