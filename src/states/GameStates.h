/*
 * GameStates.h
 *
 *  Created on: Mar 20, 2014
 *      Author: hannes
 */

#ifndef GAMESTATES_H_
#define GAMESTATES_H_

#include <SDL2/SDL.h>
#include <opencv2/core/core.hpp>

#include "../KinectInput.h"
#include "../KinectPongGame.h"
#include "../GameBoard.h"
//class KinectPongGame; // Forward declare this

class GameState {
public:
	virtual void handle_events(KinectInput* kinect_input) = 0;
	virtual void handle_logic() = 0;
	virtual void render() = 0;
	virtual ~GameState(){};
protected:
	KinectPongGame* m_game;
};

enum GameStates {
	STATE_NULL,
	STATE_INTRO,
	STATE_KINECTVIEW,
	STATE_WAITFORPLAYERS,
	STATE_PLAYING,
	STATE_SHOWSCORE,
	STATE_EXIT
};

//----------------------------------
// Declare game states below here
//------------------------------------

class IntroState : public GameState {
public:
	IntroState(KinectPongGame*);
	~IntroState();
	void handle_events(KinectInput*);
	void handle_logic(); // No logic required
	void render();

private:
	SDL_Surface* m_background;
	SDL_Surface* m_text;
};

//--------------------------------------------------

class KinectViewState : public GameState {
public:
	KinectViewState(KinectPongGame*);
	~KinectViewState();
	void handle_events(KinectInput*);
	void handle_logic(); // No logic required
	void render();

private:
	SDL_Texture* m_rgb_tex;
	SDL_Texture* m_depth_tex;
};

//--------------------------------------------------

class PlayingState : public GameState {
public:
	PlayingState(KinectPongGame*);
	~PlayingState();
	void handle_events(KinectInput*);
	void handle_logic(); // No logic required
	void render();

private:
	GameBoard m_gameboard;
//
//	struct {
//		cv::Mat binary_mask;
//		cv::Mat gradient;
//		SDL_Texture* paddle_tex;
//		float paddle_x; // x, y are top left corner of mask Mat
//		float paddle_y;
//		float paddle_width;
//		float paddle_height;
//		bool has_collided;
//	} m_player_data[2];
//
//	void collide_player(int player_num);
//	void set_paddle_mask(int player, cv::Mat& mask);
	cv::Point2f game2screen(cv::Point2f gp);
	cv::Point2f screen2game(cv::Point2f sp);
//
	cv::Point2f m_game_screen_pos;
	cv::Point2f m_game_screen_dims;
//	float m_ball_pos_x;
//	float m_ball_pos_y;
//	float m_ball_vel_x;
//	float m_ball_vel_y;
//	float m_ball_radius;
	SDL_Texture* m_player_tex[2];
	SDL_Texture* m_ball_tex;
	Uint32 m_last_tick;
};

#endif /* GAMESTATES_H_ */
