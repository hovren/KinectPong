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
	STATE_PLAY_SERVE,
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
	Uint32 m_last_tick;
};

class ServePlayState : public GameState {
public:
	ServePlayState(KinectPongGame*, float countdown);
	~ServePlayState();
	void handle_events(KinectInput*);
	void handle_logic(); // No logic required
	void render();

private:
	float m_countdown;
	Uint32 m_tick_start;
};

class PlayerScoredState : public GameState {
	PlayerScoredState(KinectPongGame*);
	~PlayerScoredState();
	void handle_events(KinectInput*);
	void handle_logic(); // No logic required
	void render();
};

class WaitForPlayerState : public GameState{
public:
	WaitForPlayerState(KinectPongGame*);
	~WaitForPlayerState();
	void handle_events(KinectInput*);
	void handle_logic();
	void render();

private:
	SDL_Texture* m_rgb_tex;
	SDL_Texture* m_depth_tex;

};

#endif /* GAMESTATES_H_ */
