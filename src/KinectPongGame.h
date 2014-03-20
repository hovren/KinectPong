/*
 * KinectPongGame.h
 *
 *  Created on: Mar 19, 2014
 *      Author: hannes
 */

#ifndef KINECTPONGGAME_H_
#define KINECTPONGGAME_H_

#include <SDL2/SDL.h>
#include "KinectInput.h"

class KinectPongGame {
public:
	KinectPongGame();
	virtual ~KinectPongGame();
	bool init(void);
	void run(void);

	void on_new_rgb_frame(cv::Mat& image);
	void on_new_depth_frame(cv::Mat& image);

private:
	KinectInput m_kinect;

	// Rendering primitives
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
	SDL_Texture* m_rgb_tex;
};

#endif /* KINECTPONGGAME_H_ */
