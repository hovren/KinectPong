/*
 * playserve.cpp
 *
 *  Created on: Mar 25, 2014
 *      Author: hannes
 */

#include "GameStates.h"

#include <iostream>
#include <algorithm>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <opencv2/imgproc/imgproc.hpp>

#include "../Player.h"

#define CB_BOX_WIDTH 0.05
#define CB_BOX_PADDING 0.01

ServePlayState::ServePlayState(KinectPongGame* game, float countdown) {
	m_game = game;
	m_countdown = countdown;
	m_tick_start = 0;
	std::cout << "Countdown started at " << m_tick_start << std::endl;

	// Reset ball
	m_game->get_gameboard()->reset_ball();
}

ServePlayState::~ServePlayState() {
	// Do nothing
}

void ServePlayState::handle_events(KinectInput* kinect) {
	default_event_handler();
}

void ServePlayState::handle_logic() {


	if(m_tick_start == 0){
		if(m_game->has_roboref()){
			std::stringstream ss;
			ss << "Score is " <<  m_game->get_gameboard()->get_player(0)->score() << " to  " << m_game->get_gameboard()->get_player(1)->score() << std::endl;
			std::string cmdstr;
			std::string substr;
			while(ss >> substr)
				cmdstr += substr;

			m_game->get_roboref()->speak(cmdstr);
		}
		m_tick_start = SDL_GetTicks();
	}


	if (SDL_GetTicks() > (m_tick_start + m_countdown*1000)) {
		m_game->set_next_state(STATE_PLAYING);
		std::cout << "Countdown completed at " << SDL_GetTicks() << std::endl;
	}
}

void render_countdown_box(SDL_Renderer* renderer, SDL_Rect* rect, bool filled, SDL_Color color) {
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	if (filled) {
		SDL_RenderFillRect(renderer, rect);
	}
	else {
		SDL_RenderDrawRect(renderer, rect);
	}
}

void ServePlayState::render() {
	SDL_Renderer* renderer = m_game->renderer();
	GameBoard* gameboard = m_game->get_gameboard();

	// Board background
	gameboard->render_board_background();

	// Render score
	gameboard->render_scores();

	// Draw countdown circles
	int seconds_left = m_countdown - (SDL_GetTicks() - m_tick_start) / 1000.0;


	SDL_Rect cb_rects[3];
	float x_min = 0.5 - 0.5*CB_BOX_WIDTH - CB_BOX_PADDING - 2*CB_BOX_WIDTH;
	float x_step = 2 * CB_BOX_WIDTH + CB_BOX_PADDING;
	for (int i=0; i < 3; ++i) {
		cb_rects[i].w = m_game->norm2pixel_x(CB_BOX_WIDTH);
		cb_rects[i].h = m_game->norm2pixel_y(CB_BOX_WIDTH);
		cb_rects[i].x = m_game->norm2pixel_x(x_min + i*x_step);
		cb_rects[i].y = m_game->norm2pixel_y(0.2 - 0.5*CB_BOX_WIDTH);
	}


	SDL_Color color_three = {255, 0, 0, 255};
	SDL_Color color_two = {255, 255, 0, 255};
	SDL_Color color_one = {0, 255, 0, 255};

	render_countdown_box(renderer, &cb_rects[0], false, color_three);
	render_countdown_box(renderer, &cb_rects[1], false, color_two);
	render_countdown_box(renderer, &cb_rects[2], false, color_one);
	// Note: All "break" are left out on purpose
	switch (seconds_left) {
	case 0:
		render_countdown_box(renderer, &cb_rects[2], true, color_one);
	case 1:
		render_countdown_box(renderer, &cb_rects[1], true, color_two);
	case 2:
		render_countdown_box(renderer, &cb_rects[0], true, color_three);
	}

	// Render the ball
	gameboard->render_ball();

	// Present and delay
	SDL_RenderPresent(renderer);
	SDL_Delay(10);
}



