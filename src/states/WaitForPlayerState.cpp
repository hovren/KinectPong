/*
 * WaitForPlayerState.cpp
 *
 *  Created on: Mar 24, 2014
 *      Author: wallenberg
 */

#include "GameStates.h"

#include <iostream>

#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

WaitForPlayerState::WaitForPlayerState(KinectPongGame* game) {
	m_game = game;
	m_rgb_tex = NULL;
	m_depth_tex = NULL;

}

WaitForPlayerState::~WaitForPlayerState() {
	//Nothing
}

void WaitForPlayerState::handle_events(KinectInput* input)
{
	std::cout << "In state WaitForPlayer" << std::endl;
	m_game->set_next_state(STATE_PLAYING);
}

void WaitForPlayerState::handle_logic()
{
	//if(m)
}

void WaitForPlayerState::render()
{
	//Nothing yet
}
