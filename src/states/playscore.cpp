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

#define CB_BOX_WIDTH 0.05
#define CB_BOX_PADDING 0.01

PlayerScoredState::PlayerScoredState(KinectPongGame* game) {
	m_game = game;
	m_tick_start = 0;
}

PlayerScoredState::~PlayerScoredState() {
	// Do nothing
}

void PlayerScoredState::handle_events(KinectInput* kinect) {

}

void PlayerScoredState::handle_logic() {
	if(m_tick_start == 0){
		if(m_game->has_roboref()){
			cv::Rect player_rect = m_game->get_image_processor()->get_left_player_face_roi();
			m_game->get_roboref()->look_at(cv::Point2f(player_rect.x + player_rect.width/2, player_rect.y + player_rect.height/2));
			m_game->get_roboref()->speak("Point to someone");
			m_game->get_roboref()->set_pan_tilt_angles(0, 0);
		}
		m_tick_start = SDL_GetTicks();
	}

	if ((SDL_GetTicks() - m_tick_start) / 1000.0 > 5.0) {
		m_game->set_next_state(STATE_PLAY_SERVE);
	}
}

void PlayerScoredState::render() {
	SDL_Renderer* renderer = m_game->renderer();
	GameBoard* gameboard = m_game->get_gameboard();

	// Board background
	gameboard->render_board_background();

	// Gray it out
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 50);
	SDL_RenderFillRect(renderer, NULL);

	// Draw score text
	SDL_Rect meh;
	meh.x = m_game->norm2pixel_x(0.5-0.2);
	meh.y = m_game->norm2pixel_y(0.5-0.05);
	meh.w = m_game->norm2pixel_x(0.4);
	meh.h = m_game->norm2pixel_y(0.1);
	SDL_SetRenderDrawColor(renderer, 255, 0, 0 , 255);
	SDL_RenderFillRect(renderer, &meh);

	// Present and delay
	SDL_RenderPresent(renderer);
	SDL_Delay(10);
}



