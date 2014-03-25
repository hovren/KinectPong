/*
 * KinectPongGame.cpp
 *
 *  Created on: Mar 19, 2014
 *      Author: hannes
 */

#include "KinectPongGame.h"

#include <iostream>
#include <unistd.h>
#include <opencv2/core/core.hpp>
#include <libfreenect/libfreenect.hpp>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "states/GameStates.h"

KinectPongGame::KinectPongGame() : m_kinect(true){
	m_gray_palette = SDL_AllocPalette(256);
	for (int i=0; i < 256; ++i) {
		m_gray_palette->colors[i].a = 255;
		m_gray_palette->colors[i].r = i;
		m_gray_palette->colors[i].g = i;
		m_gray_palette->colors[i].b = i;
	}

	m_gameboard = new GameBoard();
}

KinectPongGame::~KinectPongGame() {
	SDL_FreePalette(m_gray_palette);
}

void KinectPongGame::run(void) {

	m_next_state = STATE_NULL;

	m_state_id = STATE_INTRO;
	//m_state_id = STATE_PLAYING;

	switch (m_state_id) {
	case STATE_INTRO:
		m_current_state = new IntroState(this);
		break;
	case STATE_PLAYING:
		m_current_state = new PlayingState(this);
		break;
	}

	std::cout << "Entering game loop" << std::endl;
	while (m_state_id != STATE_EXIT) {
		m_current_state->handle_events(&m_kinect);
		m_current_state->handle_logic();
		change_state();
		m_current_state->render();
	}
	std::cout << "Game loop ended" << std::endl;

	m_kinect.stop();
}


bool KinectPongGame::init(bool fullscreen) {
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return false;
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	if (TTF_Init() != 0) {
		std::cout << "TTF_Init Error: " << SDL_GetError() << std::endl;
		return false;
	}

	//m_window = SDL_CreateWindow("Hello World!", 100, 100, 1280, 960, SDL_WINDOW_SHOWN);
	int flags = (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
	m_window = SDL_CreateWindow("Kinect Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 960, flags);
	if (m_window == NULL) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return false;
	}

	SDL_GetWindowSize(m_window, &m_screen_width, &m_screen_height);
	std::cout << "Created Window of size " << m_screen_width << " x " << m_screen_height << std::endl;
	std::cout << "Aspect ratio is " << get_aspect_ratio() << std::endl;

	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (m_renderer == NULL) {
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		return false;
	}

	//Initialise feace detector
	m_image_processor.init_detector("/usr/share/OpenCV/haarcascades");
	m_image_processor.set_near_limit(600);
	m_image_processor.set_far_limit(2000);

	//Initialise RoboRef if present
	#ifdef BUILD_ROBOREF
	m_roboref.connect("/dev/ttyUSB0");
	#endif

	return true;
}

void KinectPongGame::set_next_state(int new_state) {
	// If someone wanted exit, we do not override
	if (m_next_state != STATE_EXIT) {
		m_next_state = new_state;
	}
}

int KinectPongGame::norm2pixel_x(float x) {
	return (int) (x * m_screen_width);
}

int KinectPongGame::norm2pixel_y(float y) {
	return (int) (y * m_screen_height);
}

float KinectPongGame::pixel2norm_x(int pixel_x) {
	return (1.0 * pixel_x) / m_screen_width;
}

float KinectPongGame::pixel2norm_y(int pixel_y) {
	return (1.0 * pixel_y) / m_screen_height;
}

void KinectPongGame::window_size(int& width, int& height) {
	width = m_screen_width;
	height = m_screen_height;
}

void KinectPongGame::change_state() {
	if (m_next_state != STATE_NULL) {
		// Remove old state instance
		if (m_next_state != STATE_EXIT) {
			delete m_current_state;
		}

		// Change the state
		switch (m_next_state) {
		case STATE_INTRO:
			m_current_state = new IntroState(this);
			break;
		case STATE_WAITFORPLAYERS:
			m_current_state = new WaitForPlayerState(this);
			break;
		case STATE_KINECTVIEW:
			m_current_state = new KinectViewState(this);
			break;
		case STATE_PLAYING:
			m_current_state = new PlayingState(this);
			break;
		}

		std::cout << "Switching state: " << m_state_id << " to " << m_next_state << std::endl;

		// Change ID
		m_state_id = m_next_state;

		// Null next state
		m_next_state = STATE_NULL;
	}
}

SDL_Texture* KinectPongGame::texture_from_mat(cv::Mat& image) {
	SDL_Surface* surf;

	switch(image.channels()){
	case 4:
		surf = SDL_CreateRGBSurfaceFrom(image.data, image.cols, image.rows,
														 32, image.cols*image.channels(),
														 0x0000ff00,0x00ff0000,0xff000000,0x000000ff);
		break;
	case 3:
		surf = SDL_CreateRGBSurfaceFrom(image.data, image.cols, image.rows,
												 24, image.cols*image.channels(),
												 0x0000ff,0x00ff00,0xff0000,0);
		break;
	default:
		surf = SDL_CreateRGBSurfaceFrom(image.data, image.cols, image.rows,
														 8, image.cols,
														 0,0,0,0);
		SDL_SetSurfacePalette(surf, m_gray_palette);
		break;
	}

	if (surf == NULL) {
		std::cout << "failed to create surface: " << SDL_GetError() << std::endl;
		return NULL;
	}
	SDL_Texture* tex = SDL_CreateTextureFromSurface(m_renderer, surf);
	if (tex == NULL) {
			std::cout << "failed to create texture: " << SDL_GetError() << std::endl;
	}
	SDL_FreeSurface(surf);
	return tex;
}

float KinectPongGame::get_aspect_ratio() {
	return 1.0 * m_screen_width / m_screen_height;
}
