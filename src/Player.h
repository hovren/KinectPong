/*
 * Player.h
 *
 *  Created on: Mar 24, 2014
 *      Author: hannes
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include<opencv2/core/core.hpp>
#include <SDL2/SDL.h>
#include <string>
#include "GameBoard.h"

class Player {
public:
	Player(GameBoard* gb);
	virtual ~Player();

	void paddle_input(cv::Mat mask);
	void player_input(cv::Mat mask);
	bool collision(cv::Point2f ball_pos, float ball_radius, cv::Vec2f& collision_normal); // (x,y,r) -> (has_collided, normal)
	bool collision_line(cv::Point2f ball_pos, cv::Point2f ball_velocity, float ball_radius, float dt, cv::Vec2f& collision_normal, cv::Point2f& collision_point);
	int score() { return m_score; }
	std::string name() { return m_name; }
	void increase_score() { ++m_score; }
	void set_name(std::string name) { m_name = name; }
	cv::Rect_<float> get_paddle_rect() { return m_paddle_rect; }
	cv::Rect_<float> get_player_rect() { return m_player_rect; }
	cv::Mat get_paddle_mask() { return m_paddle_mask; }
	SDL_Texture* get_paddle_texture() { return m_paddle_tex; }
	SDL_Texture* get_player_texture() { return m_player_tex; }
private:
	int m_score;
	GameBoard* m_gameboard;
	std::string m_name;
	cv::Rect_<float> m_paddle_rect;
	cv::Rect_<float> m_player_rect;
	cv::Mat m_paddle_mask;
	cv::Mat m_paddle_gradient;
	cv::Mat m_player_mask;
	std::vector<cv::Point> m_paddle_contour;
	cv::Point m_input_dimensions;
	cv::Point m_player_dimensions;
	SDL_Texture* m_paddle_tex;
	SDL_Texture* m_player_tex;
};

#endif /* PLAYER_H_ */
