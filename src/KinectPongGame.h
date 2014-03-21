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
	SDL_Texture* texture_from_mat(cv::Mat&);
	SDL_Palette* get_gray_palette() { return m_gray_palette; }
	int norm2pixel_x(float x);
	int norm2pixel_y(float y);
	float pixel2norm_x(int pixel_x);
	float pixel2norm_y(int pixel_y);
private:
	KinectInput m_kinect;

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
