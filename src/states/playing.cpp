/*
 * playing.cpp
 *
 *  Created on: Mar 21, 2014
 *      Author: hannes
 */

#include "GameStates.h"

#include <iostream>
#include <algorithm>

#define DEFAULT_BALL_RADIUS 0.015
#define MIN_BALL_RADIUS_PIXELS 10

PlayingState::PlayingState(KinectPongGame* game) {
	m_game = game;
	m_ball_pos_x = m_ball_pos_y = 0.5;
	m_ball_vel_x = 0.1;
	m_ball_vel_y = 0.2;
	m_last_tick = 0;
	m_ball_radius = std::max<float>(DEFAULT_BALL_RADIUS, m_game->pixel2norm_x(MIN_BALL_RADIUS_PIXELS));
	int r = m_game->norm2pixel_x(m_ball_radius);
	int surf_length = 2*r + 1;
	SDL_Surface* surf = SDL_CreateRGBSurface(0, surf_length, surf_length, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	if (!surf) {
		std::cout << "Could not create ball surface: " << SDL_GetError() << std::endl;
		return;
	}
	std::cout << "Ball surface created of size " << surf->w << " x " << surf->h << " pitch " << surf->pitch << std::endl;
	Uint32* pixels = (Uint32*) surf->pixels;
	for (int y=-r; y <= r; ++y) {
		int py = y + r;
		for (int x=-r; x <= r; ++x) {
			int px = x + r;
			int index = py*(surf->pitch/sizeof(Uint32)) + px; // For 32-bit surfaces I believe pitch is defined differently
			int dd = y*y + x*x;
			if (dd < r*r) {
				Uint32 alpha = ((Uint32) (128 + 128.0 * dd / (r*r))) & 0x000000ff;
				//Uint32 alpha = (255 * (1.0 * dd / r*r)) & 0x000000ff;
				Uint32 color = 0xff000000 | alpha;
				pixels[index] = color; //0xff0000ff;
			}
			else {
				pixels[index] = 0x00ff0000;
			}
		}
	}
	//SDL_SetSurfacePalette(surf, m_game->get_gray_palette());
	m_ball_tex = SDL_CreateTextureFromSurface(m_game->renderer(), surf);
	if (!m_ball_tex) {
		std::cout << "Failed to create ball texture: " << SDL_GetError() << std::endl;
	}
	SDL_FreeSurface(surf);
}

PlayingState::~PlayingState() {
	// Do nothing
}

void PlayingState::handle_events(KinectInput* kinect) {
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

void PlayingState::handle_logic() {
	if (!m_last_tick) {
		m_last_tick = SDL_GetTicks();
		return;
	}

	Uint32 curr_tick = SDL_GetTicks();
	float dt = (curr_tick - m_last_tick) / 1000.0;

	m_ball_pos_x += m_ball_vel_x * dt;
	m_ball_pos_y += m_ball_vel_y * dt;

	if (m_ball_pos_x - m_ball_radius < 0) {
		m_ball_pos_x = m_ball_radius;
		m_ball_vel_x = -m_ball_vel_x;
		//m_ball_vel_x = m_ball_vel_y = 0;
		std::cout << "Hit wall " << m_ball_pos_x << ", " << m_ball_pos_y << " radius " << m_ball_radius << std::endl;
	}
	if (m_ball_pos_x + m_ball_radius > 1.0) {
		m_ball_pos_x = 1.0 - m_ball_radius;
		m_ball_vel_x = -m_ball_vel_x;
		//m_ball_vel_x = m_ball_vel_y = 0;
		std::cout << "Hit wall " << m_ball_pos_x << ", " << m_ball_pos_y << " radius " << m_ball_radius << std::endl;
	}
	if (m_ball_pos_y - m_ball_radius < 0) {
		m_ball_pos_y = m_ball_radius;
		m_ball_vel_y = -m_ball_vel_y;
		//m_ball_vel_x = m_ball_vel_y = 0;
		std::cout << "Hit wall " << m_ball_pos_x << ", " << m_ball_pos_y << " radius " << m_ball_radius << std::endl;
	}
	if (m_ball_pos_y + m_ball_radius > 1.0) {
		m_ball_pos_y = 1.0 - m_ball_radius;
		m_ball_vel_y = -m_ball_vel_y;
		//m_ball_vel_x = m_ball_vel_y = 0;
		std::cout << "Hit wall " << m_ball_pos_x << ", " << m_ball_pos_y << " radius " << m_ball_radius << std::endl;
	}
	m_last_tick = curr_tick;
}

void PlayingState::render() {
	SDL_Renderer* renderer = m_game->renderer();

	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 40, 40, 70, 255);
	SDL_Rect ball_rect;
	ball_rect.x = m_game->norm2pixel_x(m_ball_pos_x - m_ball_radius);
	ball_rect.y = m_game->norm2pixel_y(m_ball_pos_y - m_ball_radius);
	ball_rect.h = ball_rect.w = m_game->norm2pixel_x(2*m_ball_radius);
	//std::cout << m_ball_pos_x << ", " << m_ball_pos_y << ", " << ball_rect.x << ", " << ball_rect.y << ", " << ball_rect.w << ", " << ball_rect.h << std::endl;
	SDL_RenderCopy(renderer, m_ball_tex, NULL, &ball_rect);
	SDL_RenderPresent(renderer);
	SDL_Delay(30);
}



