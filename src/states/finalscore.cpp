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

FinalScoreState::FinalScoreState(KinectPongGame* game) {
	m_game = game;
	m_tick_start = 0;
	TTF_Font* font = TTF_OpenFont("font.ttf", 80);
	if (!font) {
		std::cout << "Error loading font: " << TTF_GetError() << std::endl;
	}

	SDL_Color text_color = {255, 255, 255, 255};
	SDL_Color text_winner_color = {0, 255, 0, 255 };
	SDL_Color text_loser_color = { 255, 0, 0, 255 };
	int winner = (m_game->get_gameboard()->get_player(0)->score() > m_game->get_gameboard()->get_player(1)->score() ? 0 : 1);
	for (int i=0; i < 2; ++i) {
		// Name
		m_player_names[i] = TTF_RenderText_Blended(font, m_game->get_gameboard()->get_player(i)->name().c_str(), text_color);
		if (!m_player_names[i]) {
			std::cout << "Error: " << TTF_GetError() << std::endl;
		}
		// Score
		char score_buf[8];
		SDL_Color score_color = (i == winner ? text_winner_color : text_loser_color);
		sprintf(score_buf, "%d", m_game->get_gameboard()->get_player(i)->score());
		m_player_scores[i] = TTF_RenderText_Blended(font, score_buf, score_color);
		if (!m_player_scores[i]) {
			std::cout << "Error: " << TTF_GetError() << std::endl;
		}
	}

	// Scored
	SDL_Surface* surf;
	surf = TTF_RenderText_Blended(font, "scored", text_color);
	if (!surf) {
		std::cout << "Error: " << TTF_GetError() << std::endl;
	}
	m_text_score = SDL_CreateTextureFromSurface(m_game->renderer(), surf);
	SDL_FreeSurface(surf);

	// Faces
	cv::Mat face;
	m_game->get_image_processor()->get_right_player_face_image(face);
	m_player_faces[0].texture = m_game->texture_from_mat(face);
	m_player_faces[0].w = face.cols;
	m_player_faces[0].h = face.rows;

	m_game->get_image_processor()->get_left_player_face_image(face);
	m_player_faces[1].texture = m_game->texture_from_mat(face);
	m_player_faces[1].w = face.cols;
	m_player_faces[1].h = face.rows;
}

FinalScoreState::~FinalScoreState() {
	// Do nothing
	for (int i=0; i < 2; ++i) {
		SDL_DestroyTexture(m_player_faces[i].texture);
		SDL_FreeSurface(m_player_names[i]);
	}
	SDL_DestroyTexture(m_text_score);
}

void FinalScoreState::handle_events(KinectInput* kinect) {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_QUIT:
			m_game->set_next_state(STATE_EXIT);
			break;
		case SDL_KEYDOWN:
			if (e.key.keysym.sym == SDLK_ESCAPE) {
				std::cout << "Game over, ESC pressed, exiting" << std::endl;
				m_game->set_next_state(STATE_EXIT);
			}
			else {
				std::cout << "Game over, moving to reset state" << std::endl;
				m_game->set_next_state(STATE_RESET);
			}
			break;
		}
	} // end SDL_PollEvent
}

void FinalScoreState::handle_logic() {
	if(m_tick_start == 0){
		if(m_game->has_roboref()){
			cv::Rect player_rect;
			std::string cmdstr;
			if(m_game->get_gameboard()->get_event() == GAMEBOARD_EVENT_PLAYER_1_SCORED){
				player_rect = m_game->get_image_processor()->get_right_player_face_roi();
				cmdstr = "Player 1 wins";
			}
			else{
				player_rect = m_game->get_image_processor()->get_left_player_face_roi();
				cmdstr = "Player 2 wins";
			}
			m_game->get_roboref()->look_at(cv::Point2f(player_rect.x + player_rect.width/2, player_rect.y + player_rect.height/2));
			m_game->get_roboref()->speak(cmdstr);
			m_game->get_roboref()->set_pan_tilt_angles(0, 0);
		}
		m_tick_start = SDL_GetTicks();
	}
}

void FinalScoreState::render() {
	SDL_Renderer* renderer = m_game->renderer();
	GameBoard* gameboard = m_game->get_gameboard();

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	int winner = (gameboard->get_player(0)->score() > gameboard->get_player(1)->score()) ? 0 : 1;

	int w, h;
	for (int i=0; i < 2; ++i) {
		float midx = 0.75 - 0.5*i;
		// Face
		SDL_Rect face_rect;
		w = m_player_faces[i].w;
		h = m_player_faces[i].h;
		std::cout << i << " face surface dims " << w << " x " << h << std::endl;
		float face_scale = FACE_WIDTH / m_game->pixel2norm_y(h);
		face_rect.x = m_game->norm2pixel_x(midx) - face_scale*w/2;
		face_rect.y = m_game->norm2pixel_y(FACE_POS_Y);
		face_rect.w = face_scale*w;
		face_rect.h = face_scale*h;
		std::cout << i << " Face rect " << face_rect.x << ", " << face_rect.y << " " << face_rect.w << " x " << face_rect.h << std::endl;
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderDrawRect(renderer, &face_rect); // Draw rectangle, because there might not be a face
		SDL_RenderCopy(renderer, m_player_faces[i].texture, NULL, &face_rect);
		//SDL_DestroyTexture(face_tex);

		// Name
		SDL_Rect name_rect;
		w = m_player_names[i]->w;
		h = m_player_names[i]->h;
		name_rect.x = m_game->norm2pixel_x(midx) - w/2;
		name_rect.y = face_rect.y = m_game->norm2pixel_y(FACE_POS_Y + FACE_WIDTH + 0.1);
		name_rect.w = w;
		name_rect.h = h;
		SDL_Texture* text_tex = SDL_CreateTextureFromSurface(renderer, m_player_names[i]);
		SDL_RenderCopy(renderer, text_tex, NULL, &name_rect);
		SDL_DestroyTexture(text_tex);

		// Name
		SDL_Rect score_rect;
		w = m_player_scores[i]->w;
		h = m_player_scores[i]->h;
		score_rect.x = m_game->norm2pixel_x(midx) - w/2;
		score_rect.y = name_rect.y + name_rect.h + m_game->pixel2norm_y(0.02);
		score_rect.w = w;
		score_rect.h = h;
		SDL_Texture* score_tex = SDL_CreateTextureFromSurface(renderer, m_player_scores[i]);
		SDL_RenderCopy(renderer, text_tex, NULL, &score_rect);
		SDL_DestroyTexture(score_tex);
	}

	// Present and delay
	SDL_RenderPresent(renderer);
	SDL_Delay(10);
}



