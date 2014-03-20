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
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

KinectPongGame::KinectPongGame() {

}

KinectPongGame::~KinectPongGame() {

}

void KinectPongGame::run(void) {
	m_state_id = STATE_INTRO;
	m_next_state = STATE_NULL;
	m_current_state = new IntroState(this);

	std::cout << "Entering game loop" << std::endl;
	while (m_state_id != STATE_EXIT) {
		m_current_state->handle_events(&m_kinect);
		m_current_state->handle_logic();
		change_state();
		m_current_state->render();
	}
	std::cout << "Game loop ended" << std::endl;

	m_kinect.stop();
//	bool quit = false;
//	SDL_Event e;
//	std::cout << "Entering game loop" << std::endl;
//	SDL_Texture* background = NULL;
//
//	while (!quit) {
//		while (SDL_PollEvent(&e)) {
//			switch (e.type) {
//			case SDL_QUIT:
//			case SDL_KEYDOWN:
//				quit = true;
//				std::cout << "Quitting" << std::endl;
//				break;
//			}
//		} // end SDL_PollEvent
//
//		cv::Mat depth, rgb;
//		if (m_kinect.poll_data(rgb, depth)) {
//			//std::cout << "New frame at time " << SDL_GetTicks() / 1000.0 << std::endl;
//			background = texture_from_mat(rgb);
//		}
//
//		if (background) {
//			SDL_RenderClear(m_renderer);
//			SDL_RenderCopy(m_renderer, background, NULL, NULL);
//			SDL_RenderPresent(m_renderer);
//		}
//	}
//	m_kinect.stop();
}


bool KinectPongGame::init() {
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return false;
	}

	if (TTF_Init() != 0) {
		std::cout << "TTF_Init Error: " << SDL_GetError() << std::endl;
		return false;
	}

	m_window = SDL_CreateWindow("Hello World!", 100, 100, 1280, 960, SDL_WINDOW_SHOWN);
	if (m_window == NULL) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return false;
	}

	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (m_renderer == NULL) {
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		return false;
	}
	return true;
}

void KinectPongGame::set_next_state(int new_state) {
	// If someone wanted exit, we do not override
	if (m_next_state != STATE_EXIT) {
		m_next_state = new_state;
	}
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
			break;
		}

		// Change ID
		m_state_id = m_next_state;

		// Null next state
		m_next_state = STATE_NULL;
	}
}

SDL_Texture* KinectPongGame::texture_from_mat(cv::Mat& image) {
	SDL_Surface* surf = SDL_CreateRGBSurfaceFrom(image.data, image.cols, image.rows,
												 24, image.cols*image.channels(),
												 0x0000ff,0x00ff00,0xff0000,0);
	if (surf == NULL) {
		std::cout << "failed to create surface: " << SDL_GetError() << std::endl;
		return NULL;
	}
	SDL_Texture* tex = SDL_CreateTextureFromSurface(m_renderer, surf);
	if (tex == NULL) {
			std::cout << "failed to create texture: " << SDL_GetError() << std::endl;
			return NULL;
	}
	SDL_FreeSurface(surf);
	return tex;
}

IntroState::IntroState(KinectPongGame* game) {
	m_background = IMG_Load("window_icon.png");
	if (!m_background) {
		std::cout << "Error loading image: " << SDL_GetError() << std::endl;
	}
	else {
		std::cout << "Image loaded" << std::endl;
	}

	TTF_Font* font = TTF_OpenFont("font.ttf", 40);
	if (!font) {
		std::cout << "Error loading font: " << TTF_GetError() << std::endl;
	}

	SDL_Color text_color = {255, 0, 0, 255};
	m_text = TTF_RenderText_Blended(font, "Initializing Kinect", text_color);
	if (!m_text) {
		std::cout << "Error: " << TTF_GetError() << std::endl;
	}

	m_game = game;
}

IntroState::~IntroState() {
	SDL_FreeSurface(m_background);
	SDL_FreeSurface(m_text);
}

void IntroState::handle_events(KinectInput* kinect) {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_QUIT:
		case SDL_KEYDOWN:
			m_game->set_next_state(STATE_EXIT);
			break;
		}
	} // end SDL_PollEvent
}

void IntroState::handle_logic() {
	if (!m_game->get_kinect()->is_running() && SDL_GetTicks() > 1000) {
		std::cout << "Starting Kinect input source" << std::endl;
		m_game->get_kinect()->start(); // This takes a while
	}
	else if (m_game->get_kinect()->is_running()) {
		m_game->set_next_state(STATE_EXIT);
	}
}

void IntroState::render() {
	SDL_Renderer* renderer = m_game->renderer();
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, m_background);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	//SDL_RenderCopy(renderer, tex, NULL, NULL);
	if (m_game->get_kinect()->is_running()) {
		SDL_Rect top_left;
		top_left.x = 1280 / 2 - 100/2;
		top_left.y = 960 / 2 - 100/2;
		top_left.w = 100;
		top_left.h = 100;
		SDL_RenderCopy(renderer, tex, NULL, &top_left);
	}
	else {
		SDL_Texture* text_tex = SDL_CreateTextureFromSurface(renderer, m_text);
		SDL_Rect dst;
		dst.x = 1280 / 2 - m_text->w / 2;
		dst.y = 960 / 2 - m_text->h / 2;
		dst.w = m_text->w;
		dst.h = m_text->h;
		SDL_RenderCopy(renderer, text_tex, NULL, &dst);
	}
	SDL_RenderPresent(renderer);
}
