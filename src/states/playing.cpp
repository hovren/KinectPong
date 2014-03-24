/*
 * playing.cpp
 *
 *  Created on: Mar 21, 2014
 *      Author: hannes
 */

#include "GameStates.h"

#include <iostream>
#include <algorithm>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <opencv2/imgproc/imgproc.hpp>

#define DEFAULT_BALL_RADIUS 0.015
#define MIN_BALL_RADIUS_PIXELS 10



SDL_Texture* create_ball_texture(int pixel_radius, SDL_Renderer* renderer) {
	int r = pixel_radius;
	int surf_length = 2*r + 1;
	SDL_Surface* surf = SDL_CreateRGBSurface(0, surf_length, surf_length, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	if (!surf) {
		std::cout << "Could not create ball surface: " << SDL_GetError() << std::endl;
		return NULL;
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
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
	if (!tex) {
		std::cout << "Failed to create texture: " << SDL_GetError() << std::endl;
	}
	SDL_FreeSurface(surf);
	return tex;
}

// ------------------------------------------------------


PlayingState::PlayingState(KinectPongGame* game) {
	std::cout << "Run constructor" << std::endl;
	m_game = game;
//	m_ball_tex = create_ball_texture(m_game->norm2pixel_x(m_ball_radius), m_game->renderer());
	m_ball_tex = IMG_LoadTexture(m_game->renderer(), "window_icon.png");
	//cv::Mat test_mask; //(cv::Size(640, 480), CV_8U);

	cv::Mat test_mask = cv::Mat::zeros(cv::Size(640, 480), CV_8U);
	cv::circle(test_mask, cv::Point(100, 70), 50, cv::Scalar(255), -1);
	cv::rectangle(test_mask, cv::Point(100, 20), cv::Point(350, 450), cv::Scalar(0), -1);
	cv::Mat rot_mat = cv::getRotationMatrix2D(cv::Point2f(100, 70), 20.0, 1.0);
	cv::warpAffine(test_mask, test_mask, rot_mat, test_mask.size());
	for (int i=0; i < 2; ++i) {
		Player* player = m_game->get_gameboard()->get_player(i);
		player->paddle_input(test_mask);
		cv::Mat mask = player->get_paddle_mask();
		m_player_tex[i] = m_game->texture_from_mat(mask);
	}
	m_last_tick = 0;
}

PlayingState::~PlayingState() {
	// Do nothing
	SDL_DestroyTexture(m_ball_tex);
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

	GameBoard* gameboard = m_game->get_gameboard();

	gameboard->update(dt);

	// Update tick count (not safe for overflows, I guess, but we can play for 1200 hours before that happens...)
	m_last_tick = curr_tick;
}

void PlayingState::render() {
	SDL_Renderer* renderer = m_game->renderer();
	GameBoard* gameboard = m_game->get_gameboard();

	// Render background
	if (gameboard->had_collision) {
		SDL_SetRenderDrawColor(renderer, 255, 40, 70, 255);
	}
	else {
		SDL_SetRenderDrawColor(renderer, 40, 40, 70, 255);
	}
	SDL_RenderClear(renderer);

	// Render paddle
	for (int i=0; i < 2; ++i) {
		Player* player = gameboard->get_player(i);
		SDL_Texture* paddle_tex = m_player_tex[i];
		if (paddle_tex) {
			SDL_Rect paddle_rect;
			//cv::Point2f paddle_pos(m_player_data[player].paddle_x, m_player_data[player].paddle_y);
			cv::Rect_<float> paddle_rect_norm = player->get_paddle_rect();
			paddle_rect.x = m_game->norm2pixel_x(paddle_rect_norm.x);
			paddle_rect.y = m_game->norm2pixel_y(paddle_rect_norm.y);
			paddle_rect.w = m_game->norm2pixel_x(paddle_rect_norm.width);
			paddle_rect.h = m_game->norm2pixel_y(paddle_rect_norm.height);
			SDL_RenderCopy(renderer, paddle_tex, NULL, &paddle_rect);
		}
	}

	// Render ball
	SDL_Rect ball_rect;
	cv::Point2f ball_pos = gameboard->get_ball_position();
	float ball_radius = gameboard->get_ball_radius();
	ball_rect.x = m_game->norm2pixel_x(ball_pos.x - ball_radius);
	ball_rect.y = m_game->norm2pixel_y(ball_pos.y - ball_radius);
	ball_rect.h = ball_rect.w = m_game->norm2pixel_x(2*ball_radius);
	//std::cout << "Ball Pos: " << ball_rect.x << ", " << ball_rect.y << " dim " << ball_rect.w << ", " << ball_rect.h << std::endl;
	SDL_RenderCopy(renderer, m_ball_tex, NULL, &ball_rect);

	// Render game board
	cv::Point2f gb0(0.0, 0.0);
	cv::Point2f gbdim(1.0, 1.0);
	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // yellow
	cv::Point2f gb0_s = game2screen(gb0);
	cv::Point2f gbdim_s = game2screen(gbdim);
	SDL_RenderDrawLine(renderer, m_game->norm2pixel_x(gb0_s.x), m_game->norm2pixel_y(gb0_s.y), m_game->norm2pixel_x(gbdim_s.x), m_game->norm2pixel_y(gb0_s.y));
	SDL_RenderDrawLine(renderer, m_game->norm2pixel_x(gb0_s.x), m_game->norm2pixel_y(gb0_s.y), m_game->norm2pixel_x(gb0_s.x), m_game->norm2pixel_y(gbdim_s.y));
	SDL_RenderDrawLine(renderer, m_game->norm2pixel_x(gbdim_s.x), m_game->norm2pixel_y(gb0_s.y), m_game->norm2pixel_x(gbdim_s.x), m_game->norm2pixel_y(gbdim_s.y));
	SDL_RenderDrawLine(renderer, m_game->norm2pixel_x(gb0_s.x), m_game->norm2pixel_y(gbdim_s.y), m_game->norm2pixel_x(gbdim_s.x), m_game->norm2pixel_y(gbdim_s.y));

	// Present and delay
	SDL_RenderPresent(renderer);
	SDL_Delay(10);
}

cv::Point2f PlayingState::game2screen(cv::Point2f gp) {
	cv::Point2f sp;
	sp.x = m_game_screen_pos.x + m_game_screen_dims.x * gp.x;
	sp.y = m_game_screen_pos.y + m_game_screen_dims.y * gp.y;
	return sp;
}

cv::Point2f PlayingState::screen2game(cv::Point2f sp) {
	cv::Point2f gp;
	gp.x = (sp.x - m_game_screen_pos.x) / m_game_screen_dims.x;
	gp.y = (sp.y - m_game_screen_pos.y) / m_game_screen_dims.y;
	return gp;
}
