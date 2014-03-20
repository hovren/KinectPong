/*
 * KinectPongGame.cpp
 *
 *  Created on: Mar 19, 2014
 *      Author: hannes
 */

#include <iostream>
#include <unistd.h>
#include <opencv2/core/core.hpp>
#include <libfreenect/libfreenect.hpp>
#include "KinectPongGame.h"


SDL_Texture* texture_from_mat(cv::Mat& image, SDL_Renderer* renderer) {
	SDL_Surface* surf = SDL_CreateRGBSurfaceFrom(image.data, image.cols, image.rows,
												 24, image.cols*image.channels(),
												 0,0,0,0);
	if (surf == NULL) {
		std::cout << "failed to create surface: " << SDL_GetError() << std::endl;
		return NULL;
	}
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
	if (tex == NULL) {
			std::cout << "failed to create texture: " << SDL_GetError() << std::endl;
			return NULL;
	}
	SDL_FreeSurface(surf);
	return tex;
}

KinectPongGame::KinectPongGame(){

}

KinectPongGame::~KinectPongGame() {

}

void KinectPongGame::run(void) {
	bool quit = false;
	SDL_Event e;
	std::cout << "Entering game loop" << std::endl;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
			case SDL_KEYDOWN:
				quit = true;
				std::cout << "Quitting" << std::endl;
				break;
			}
		} // end SDL_PollEvent

		cv::Mat depth, rgb;
		if (m_kinect.poll_data(rgb, depth)) {
			std::cout << "New frame at time " << SDL_GetTicks() / 1000.0 << std::endl;
		}
	}
	m_kinect.stop();
}

bool KinectPongGame::init(void) {
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return false;
	}

	m_window = SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
	if (m_window == NULL) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return false;
	}

	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (m_renderer == NULL) {
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		return false;
	}

	std::cout << "Starting Kinect input source" << std::endl;
	m_kinect.start();

	return true;
}
