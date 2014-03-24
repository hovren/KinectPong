/*
 * GameBoard.h
 *
 *  Created on: Mar 24, 2014
 *      Author: hannes
 */

#ifndef GAMEBOARD_H_
#define GAMEBOARD_H_

#include <opencv2/core/core.hpp>
#include "Player.h"

class GameBoard {
public:
	GameBoard();
	virtual ~GameBoard();
	void update(float dt);
	Player* get_player(int player) { return m_players[player]; };

	bool had_collision;
	float get_ball_radius() { return m_ball_radius; };
	cv::Point2f get_ball_position() { return m_ball_pos; };
	cv::Point2f get_ball_velocity() { return m_ball_velocity; };


private:
	// Players
	Player* m_players[2];

	// Ball
	cv::Point2f m_ball_pos;
	float m_ball_radius;
	cv::Point2f m_ball_velocity;
};

#endif /* GAMEBOARD_H_ */
