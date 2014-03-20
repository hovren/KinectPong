/*
 * KinectPongGame.h
 *
 *  Created on: Mar 19, 2014
 *      Author: hannes
 */

#ifndef KINECTPONGGAME_H_
#define KINECTPONGGAME_H_

#include <SDL2/SDL.h>
#include <opencv2/core/core.hpp>
#include "KinectInput.h"

class KinectPongGame;

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
	STATE_WAITFORPLAYERS,
	STATE_PLAYING,
	STATE_SHOWSCORE,
	STATE_EXIT
};

class KinectPongGame {
public:
	KinectPongGame();
	virtual ~KinectPongGame();
	bool init(void);
	void run(void);

	void set_next_state(int new_state);
	SDL_Renderer* renderer() { return m_renderer; }

private:
	KinectInput m_kinect;

	// Game state handling (credit to lazyfoo.net)
	GameState* m_current_state;
	int m_state_id;
	int m_next_state;
	void change_state();

	// Rendering
	SDL_Texture* texture_from_mat(cv::Mat&);
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
	SDL_Texture* m_rgb_tex;
};


class IntroState : public GameState {
public:
	IntroState(KinectPongGame*);
	~IntroState();
	void handle_events(KinectInput*);
	void handle_logic(); // No logic required
	void render();

private:
	SDL_Surface* m_background;
};

#endif /* KINECTPONGGAME_H_ */
