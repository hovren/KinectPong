/*
 * GameBoard.h
 *
 *  Created on: Mar 24, 2014
 *      Author: hannes
 */

#ifndef GAMEBOARD_H_
#define GAMEBOARD_H_

#include <opencv2/core/core.hpp>
//#include "Player.h"
#include "KinectPongGame.h"

class Player;

class GameBoard {
public:
	GameBoard(KinectPongGame* game);
	virtual ~GameBoard();
	void update(float dt);
	Player* get_player(int player) { return m_players[player]; }

	bool had_collision;
	float get_ball_radius() { return m_ball_radius; }
	cv::Point2f get_ball_position() { return m_ball_pos; }
	cv::Point2f get_ball_velocity() { return m_ball_velocity; }
	SDL_Texture* get_ball_texture() {return m_ball_tex; }
	KinectPongGame* get_game() { return m_game; }
	cv::Point2f game2screen(cv::Point2f gp);
	cv::Point2f screen2game(cv::Point2f sp);

private:
	KinectPongGame* m_game;

	cv::Point2f m_game_screen_pos;
	cv::Point2f m_game_screen_dims;
	SDL_Texture* m_ball_tex;

	// Players
	Player* m_players[2];

	// Ball
	cv::Point2f m_ball_pos;
	float m_ball_radius;
	cv::Point2f m_ball_velocity;
};

#endif /* GAMEBOARD_H_ */
