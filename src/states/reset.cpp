/*
 * reset.cpp
 *
 *  Created on: Mar 26, 2014
 *      Author: hannes
 */

#include "GameStates.h"

ResetState::ResetState(KinectPongGame* game) {
	m_game = game;
}

ResetState::~ResetState() {
	// Nothing to see, move along
}

void ResetState::handle_logic() {
	// Reset the game and go to next suitable state
	m_game->get_gameboard()->reset();
	m_game->get_image_processor()->reset();
	m_game->set_next_state(STATE_KINECTVIEW);
}

void ResetState::handle_events(KinectInput* kinect) {

}

void ResetState::render() {
	// Nothing to render
	SDL_Renderer* renderer = m_game->renderer();
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	SDL_Delay(30);
}
