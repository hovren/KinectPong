/*
 * KinectPongGame.h
 *
 *  Created on: Mar 19, 2014
 *      Author: hannes
 */

#ifndef KINECTPONGGAME_H_
#define KINECTPONGGAME_H_

//temporary definition
//#define BUILD_ROBOREF

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
class GameBoard;

class KinectPongGame {
public:
	KinectPongGame();
	virtual ~KinectPongGame();
	bool init(bool fullscreen = false);
	void run(void);

	void set_next_state(int new_state);
	SDL_Renderer* renderer() { return m_renderer; }
	KinectInput* get_kinect() { return &m_kinect; }
	SDL_Surface* get_window_surface() { return SDL_GetWindowSurface(m_window); }
	PlayerImageProcessor* get_image_processor() { return &m_image_processor;}
	#ifdef BUILD_ROBOREF
	RoboRef* get_roboref() { return &m_roboref;}
	#endif
	SDL_Texture* texture_from_mat(cv::Mat&);
	SDL_Palette* get_gray_palette() { return m_gray_palette; }
	float get_aspect_ratio();
	int norm2pixel_x(float x);
	int norm2pixel_y(float y);
	float pixel2norm_x(int pixel_x);
	float pixel2norm_y(int pixel_y);

	void window_size(int& width, int& height);
	GameBoard* get_gameboard() { return m_gameboard; }



private:
	KinectInput m_kinect;
	GameBoard* m_gameboard;
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
	int m_screen_height;
	int m_screen_width;
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
	SDL_Texture* m_rgb_tex;
	SDL_Palette* m_gray_palette;
};

#endif /* KINECTPONGGAME_H_ */
