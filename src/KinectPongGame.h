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
#include "PlayerImageProcessor.h"

//build with or without RoboRef support
#ifdef BUILD_ROBOREF
#include "RoboRef.h"
#endif

//#include "states/GameStates.h"

class GameState;

class KinectPongGame {
public:
	KinectPongGame();
	virtual ~KinectPongGame();
	bool init(void);
	void run(void);

	void set_next_state(int new_state);
	SDL_Renderer* renderer() { return m_renderer; }
	KinectInput* get_kinect() { return &m_kinect; }
	PlayerImageProcessor* get_image_processor() { return &m_image_processor;}
	#ifdef BUILD_ROBOREF
	RoboRef* get_roboref() { return &m_roboref;}
	#endif
	SDL_Texture* texture_from_mat(cv::Mat&);


private:
	KinectInput m_kinect;
	PlayerImageProcessor m_image_processor;
	#ifdef BUILD_ROBOREF
	RoboRef m_roboref;
	#endif

	// Game state handling (credit to lazyfoo.net)
	GameState* m_current_state;
	int m_state_id;
	int m_next_state;
	void change_state();

	// Rendering
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
	SDL_Texture* m_rgb_tex;
	SDL_Palette* m_gray_palette;
};

#endif /* KINECTPONGGAME_H_ */
