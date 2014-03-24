/*
 * Player.h
 *
 *  Created on: Mar 24, 2014
 *      Author: hannes
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include<opencv2/core/core.hpp>
#include <string>

class Player {
public:
	Player();
	virtual ~Player();

	void paddle_input(cv::Mat mask);
	bool collision(cv::Point2f ball_pos, float ball_radius, cv::Vec2f& collision_normal); // (x,y,r) -> (has_collided, normal)
	bool collision_line(cv::Point2f ball_pos, cv::Point2f ball_velocity, float ball_radius, float dt, cv::Vec2f& collision_normal, cv::Point2f& collision_point);

	void set_name(std::string name) { m_name = name; };
	cv::Rect_<float> get_paddle_rect() { return m_paddle_rect; }
	cv::Mat get_paddle_mask() { return m_paddle_mask; };
private:
	std::string m_name;
	cv::Rect_<float> m_paddle_rect;
	cv::Mat m_paddle_mask;
	cv::Mat m_paddle_gradient;
	std::vector<cv::Point> m_paddle_contour;
	cv::Point m_input_dimensions;
};

#endif /* PLAYER_H_ */
