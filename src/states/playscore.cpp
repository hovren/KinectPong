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
#include <SDL2/SDL_ttf.h>
#include <opencv2/imgproc/imgproc.hpp>

#include "../Player.h"

#define CB_BOX_WIDTH 0.05
#define CB_BOX_PADDING 0.01

#define FACE_WIDTH 0.3
#define FACE_POS_Y 0.20

#define SCORE_WAIT_PERIOD 3.0

PlayerScoredState::PlayerScoredState(KinectPongGame* game) {
	m_game = game;
	m_tick_start = SDL_GetTicks();
	TTF_Font* font = TTF_OpenFont("font.ttf", 200);
	if (!font) {
		std::cout << "Error loading font: " << TTF_GetError() << std::endl;
	}

	SDL_Color text_color = {255, 255, 255, 255};
	SDL_Surface* surf;
	// Player 1
	surf = TTF_RenderText_Blended(font, m_game->get_gameboard()->get_player(0)->name().c_str(), text_color);
	if (!surf) {
		std::cout << "Error: " << TTF_GetError() << std::endl;
	}
	m_text_player_1 = SDL_CreateTextureFromSurface(m_game->renderer(), surf);
	SDL_FreeSurface(surf);

	// Player 2
	surf = TTF_RenderText_Blended(font, m_game->get_gameboard()->get_player(1)->name().c_str(), text_color);
	if (!surf) {
		std::cout << "Error: " << TTF_GetError() << std::endl;
	}
	m_text_player_2 = SDL_CreateTextureFromSurface(m_game->renderer(), surf);
	SDL_FreeSurface(surf);

	// Scored
	surf = TTF_RenderText_Blended(font, "scored", text_color);
	if (!surf) {
		std::cout << "Error: " << TTF_GetError() << std::endl;
	}
	m_text_scored = SDL_CreateTextureFromSurface(m_game->renderer(), surf);
	SDL_FreeSurface(surf);

	cv::Mat face;
	m_game->get_image_processor()->get_left_player_face_image(face);
	m_face_player_1 = m_game->texture_from_mat(face);
	m_game->get_image_processor()->get_right_player_face_image(face);
	m_face_player_2 = m_game->texture_from_mat(face);
}

PlayerScoredState::~PlayerScoredState() {
	// Do nothing
	SDL_DestroyTexture(m_text_player_1);
	SDL_DestroyTexture(m_text_player_2);
	SDL_DestroyTexture(m_face_player_1);
	SDL_DestroyTexture(m_face_player_2);
	SDL_DestroyTexture(m_text_scored);
}

void PlayerScoredState::handle_events(KinectInput* kinect) {

}

void PlayerScoredState::handle_logic() {
	if ((SDL_GetTicks() - m_tick_start) / 1000.0 > SCORE_WAIT_PERIOD) {
		m_game->set_next_state(STATE_PLAY_SERVE);
	}
}

SDL_Rect fit_texture_inside(SDL_Texture* tex, SDL_Rect* rect) {
	Uint32 format;
	int access, w, h;
	SDL_QueryTexture(tex, &format, &access, &w, &h);

	int mx = rect->x + rect->w/2;
	int my = rect->y + rect->h/2;

	SDL_Rect out_rect;
	out_rect.x = mx - w/2;
	out_rect.y = my - h/2;
	out_rect.w = w;
	out_rect.h = h;

	return out_rect;
}

void PlayerScoredState::render() {
	SDL_Renderer* renderer = m_game->renderer();
	GameBoard* gameboard = m_game->get_gameboard();

	// Board background
	gameboard->render_board_background();

	// Gray it out
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
	SDL_SetRenderDrawColor(renderer, 80, 80, 80, 80);
	SDL_RenderFillRect(renderer, NULL);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

	// Draw score text
	SDL_Rect score_box_rect;
	score_box_rect.x = m_game->norm2pixel_x(0.5-0.2);
	score_box_rect.y = m_game->norm2pixel_y(0.5-0.05);
	score_box_rect.w = m_game->norm2pixel_x(0.4);
	score_box_rect.h = m_game->norm2pixel_y(0.2);

	SDL_Rect text_box_rect;
	text_box_rect.x = m_game->norm2pixel_x(0.5-0.2);
	text_box_rect.y = m_game->norm2pixel_y(0.5-0.05);
	text_box_rect.w = m_game->norm2pixel_x(0.4);
	text_box_rect.h = m_game->norm2pixel_y(0.1);

	SDL_Rect scores_text_rect = text_box_rect;
	scores_text_rect.y += m_game->norm2pixel_y(0.15);

	SDL_Rect face_rect;
	face_rect.x = m_game->norm2pixel_x(0.5-0.5*FACE_WIDTH);
	face_rect.y = m_game->norm2pixel_y(FACE_POS_Y-0.5*FACE_WIDTH);
	face_rect.w = m_game->norm2pixel_x(FACE_WIDTH);
	face_rect.h = m_game->norm2pixel_y(FACE_WIDTH);

	SDL_Rect text_rect;
	switch (gameboard->get_event()) {
	case GAMEBOARD_EVENT_PLAYER_1_SCORED:
		text_rect = fit_texture_inside(m_text_player_1, &text_box_rect);
		SDL_RenderCopy(renderer, m_text_player_1, NULL, &text_rect);
		SDL_RenderCopy(renderer, m_face_player_1, NULL, &face_rect);
		break;
	case GAMEBOARD_EVENT_PLAYER_2_SCORED:
		text_rect = fit_texture_inside(m_text_player_2, &text_box_rect);
		SDL_RenderCopy(renderer, m_text_player_2, NULL, &text_rect);
		SDL_RenderCopy(renderer, m_face_player_2, NULL, &face_rect);
		break;
	}
	//text_rect = fit_texture_inside(m_text_scored, &scores_text_rect);
	text_rect.y += m_game->norm2pixel_y(0.15);
	SDL_RenderCopy(renderer, m_text_scored, NULL, &text_rect);

	// Present and delay
	SDL_RenderPresent(renderer);
	SDL_Delay(10);
}



