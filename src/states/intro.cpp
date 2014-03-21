/*
 * intro.cpp
 *
 *  Created on: Mar 20, 2014
 *      Author: hannes
 */

#include "GameStates.h"

#include <iostream>

#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

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
			m_game->set_next_state(STATE_EXIT);
			break;
		case SDL_KEYDOWN:
			m_game->set_next_state(STATE_KINECTVIEW);
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
		m_game->set_next_state(STATE_KINECTVIEW);
	}
}

void IntroState::render() {
	SDL_Renderer* renderer = m_game->renderer();
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, m_background);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	//SDL_RenderCopy(renderer, tex, NULL, NULL);
	if (!m_game->get_kinect()->is_running()) {
		SDL_Texture* text_tex = SDL_CreateTextureFromSurface(renderer, m_text);
		SDL_Rect dst;
		dst.x = m_game->norm2pixel_x(0.5) - m_text->w / 2;
		dst.y = m_game->norm2pixel_y(0.5) - m_text->h / 2;
		dst.w = m_text->w;
		dst.h = m_text->h;
		SDL_RenderCopy(renderer, text_tex, NULL, &dst);
	}
	SDL_RenderPresent(renderer);
}

