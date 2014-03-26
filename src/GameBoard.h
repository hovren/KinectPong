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

enum GameBoardEvent {
	GAMEBOARD_EVENT_NONE,
	GAMEBOARD_EVENT_PLAYER_1_SCORED,
	GAMEBOARD_EVENT_PLAYER_2_SCORED
};

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
	GameBoardEvent get_event() { return m_event; }
	cv::Point2f game2screen(cv::Point2f gp);
	cv::Point2f screen2game(cv::Point2f sp);
	cv::Point game2pixel(cv::Point2f gp);

	bool gameover(); // True if game ended by player score

	void reset();
	void reset_ball();

	void render_board_background();
	void render_ball();
	void render_scores();
	void render_board_all();

private:
	KinectPongGame* m_game;
	const int m_max_score;
	cv::Point2f m_game_screen_pos; // Board position in world coordinate space [0, 1]
	cv::Point2f m_game_screen_dims; // Board dimensions in pixels (640, 480)
	SDL_Texture* m_ball_tex;

	GameBoardEvent m_event;

	// Players
	Player* m_players[2];
	SDL_Surface* m_player_1_score_surf;
	SDL_Surface* m_player_2_score_surf;
	void update_score_textures();

	// Ball
	cv::Point2f m_ball_pos;
	float m_ball_radius;
	cv::Point2f m_ball_velocity;
};

#endif /* GAMEBOARD_H_ */
