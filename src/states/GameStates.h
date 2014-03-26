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

enum GameStates {
	STATE_NULL,
	STATE_INTRO,
	STATE_RESET,
	STATE_KINECTVIEW,
	STATE_WAITFORPLAYERS,
	STATE_PLAY_SERVE,
	STATE_PLAY_SCORE,
	STATE_PLAYING,
	STATE_FINAL_SCORE,
	STATE_EXIT
};

class GameState {
public:
	virtual void handle_events(KinectInput* kinect_input) = 0;
	virtual void handle_logic() = 0;
	virtual void render() = 0;
	virtual ~GameState() {}
protected:
	KinectPongGame* m_game;
	void default_event_handler() {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				m_game->set_next_state(STATE_EXIT);
				break;
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_ESCAPE) {
					std::cout << "Escape pressed" << std::endl;
					switch (m_game->current_state()) {
					case STATE_PLAYING:
					case STATE_PLAY_SERVE:
					case STATE_PLAY_SCORE:
						m_game->set_next_state(STATE_FINAL_SCORE);
						break;
					default:
						m_game->set_next_state(STATE_EXIT);
					}
				}
				break;
			}
		} // end SDL_PollEvent
	}
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
	void handle_logic();
	void render();

private:
	float m_countdown;
	Uint32 m_tick_start;
};

class PlayerScoredState : public GameState {
public:
	PlayerScoredState(KinectPongGame*);
	~PlayerScoredState();
	void handle_events(KinectInput*);
	void handle_logic();
	void render();
private:
	Uint32 m_tick_start;
	SDL_Texture* m_text_player_1;
	SDL_Texture* m_text_player_2;
	SDL_Texture* m_face_player_1;
	SDL_Texture* m_face_player_2;
	SDL_Texture* m_text_scored;
};

class FinalScoreState : public GameState {
public:
	FinalScoreState(KinectPongGame*);
	~FinalScoreState();
	void handle_events(KinectInput*);
	void handle_logic();
	void render();
private:
	Uint32 m_tick_start;
	struct { SDL_Texture* texture;
			 int w;
			 int h;
		} m_player_faces[2];
	SDL_Surface* m_player_names[2];
	SDL_Surface* m_player_scores[2];
	SDL_Texture* m_tmp;

	SDL_Texture* m_text_score;
};

class ResetState : public GameState {
public:
	ResetState(KinectPongGame*);
	~ResetState();
	void handle_events(KinectInput*);
	void handle_logic();
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
	bool m_first_run;

};

#endif /* GAMESTATES_H_ */
