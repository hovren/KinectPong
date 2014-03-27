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

#define SCORE_WAIT_PERIOD 2.0

PlayerScoredState::PlayerScoredState(KinectPongGame* game) {
	m_game = game;

	SDL_Color text_color = {255, 255, 255, 255};
	TTF_Font* font = TTF_OpenFont("font.ttf", 150);
	if (!font) {
		std::cout << "Error loading font: " << TTF_GetError() << std::endl;
	}

	for (int i = 0; i < 2; ++i) {
		// Name
		m_player_names[i] = TTF_RenderText_Blended(font,
				m_game->get_gameboard()->get_player(i)->name().c_str(),
				text_color);
		if (!m_player_names[i]) {
			std::cout << "Error: " << TTF_GetError() << std::endl;
		}

		// Score
		char score_buf[8];
		sprintf(score_buf, "%d", m_game->get_gameboard()->get_player(i)->score());
		m_player_scores[i] = TTF_RenderText_Blended(font, score_buf,
				text_color);
		if (!m_player_scores[i]) {
			std::cout << "Error: " << TTF_GetError() << std::endl;
		}
	}


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


	// Scored
	m_text_scored = TTF_RenderText_Blended(font, "scored", text_color);
	if (!m_text_scored) {
		std::cout << "Error: " << TTF_GetError() << std::endl;
	}

	m_tick_start = SDL_GetTicks();
}

PlayerScoredState::~PlayerScoredState() {
	for (int i=0; i < 2; ++i) {
		SDL_DestroyTexture(m_player_faces[i].texture);
		SDL_FreeSurface(m_player_names[i]);
		SDL_FreeSurface(m_player_scores[i]);
	}
	SDL_FreeSurface(m_text_scored);
}

void PlayerScoredState::handle_events(KinectInput* kinect) {
	default_event_handler();
}

void PlayerScoredState::handle_logic() {
	bool is_done = false;
	if (m_game->has_roboref()) {
		cv::Rect player_rect;
		std::string cmdstr;
		if (m_game->get_gameboard()->get_event()
				== GAMEBOARD_EVENT_PLAYER_1_SCORED) {
			player_rect =
					m_game->get_image_processor()->get_right_player_face_roi();
			cmdstr = "Point to player 1";
		} else {
			player_rect =
					m_game->get_image_processor()->get_left_player_face_roi();
			cmdstr = "Point to player 2";
		}
		m_game->get_roboref()->look_at(
				cv::Point2f(player_rect.x + player_rect.width / 2,
						player_rect.y + player_rect.height / 2));
		m_game->get_roboref()->speak(cmdstr);
		m_game->get_roboref()->set_pan_tilt_angles(0, 0);
		is_done = true;
	} else {
		if ((SDL_GetTicks() - m_tick_start) / 1000.0 > SCORE_WAIT_PERIOD) {
			is_done = true;
		}
	}

	// Change state if roboref done talking or countdown timer exceeded
	if (is_done) {
		if (m_game->get_gameboard()->gameover()) {
			m_game->set_next_state(STATE_FINAL_SCORE);
		} else {
			m_game->set_next_state(STATE_PLAY_SERVE);
		}
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

	int winner = gameboard->get_event() == GAMEBOARD_EVENT_PLAYER_1_SCORED ? 0 : 1;
	float midx = 0.5;
	int w, h;

	// Face
	SDL_Rect face_rect;
	w = m_player_faces[winner].w;
	h = m_player_faces[winner].h;
	//std::cout << i << " face surface dims " << w << " x " << h << std::endl;
	float face_scale = FACE_WIDTH / m_game->pixel2norm_y(h);
	face_rect.x = m_game->norm2pixel_x(midx) - face_scale*w/2;
	face_rect.y = m_game->norm2pixel_y(FACE_POS_Y);
	face_rect.w = face_scale*w;
	face_rect.h = face_scale*h;
	//std::cout << i << " Face rect " << face_rect.x << ", " << face_rect.y << " " << face_rect.w << " x " << face_rect.h << std::endl;
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(renderer, &face_rect); // Draw rectangle, because there might not be a face
	SDL_RenderCopy(renderer, m_player_faces[winner].texture, NULL, &face_rect);
	//SDL_DestroyTexture(face_tex);

	// Name
	SDL_Rect name_rect;
	w = m_player_names[winner]->w;
	h = m_player_names[winner]->h;
	name_rect.x = m_game->norm2pixel_x(midx) - w/2;
	name_rect.y = m_game->norm2pixel_y(FACE_POS_Y + FACE_WIDTH + 0.1);
	name_rect.w = w;
	name_rect.h = h;
	SDL_Texture* text_tex = SDL_CreateTextureFromSurface(renderer, m_player_names[winner]);
	SDL_RenderCopy(renderer, text_tex, NULL, &name_rect);
	SDL_DestroyTexture(text_tex);

	//text_rect = fit_texture_inside(m_text_scored, &scores_text_rect);
	SDL_Rect scored_rect;
	w = m_text_scored->w;
	h = m_text_scored->h;
	name_rect.x = m_game->norm2pixel_x(midx) - w/2;
	name_rect.y = name_rect.y + name_rect.h + m_game->norm2pixel_y(0.05);
	name_rect.w = w;
	name_rect.h = h;
	text_tex = SDL_CreateTextureFromSurface(renderer, m_text_scored);
	SDL_RenderCopy(renderer, text_tex, NULL, &name_rect);
	SDL_DestroyTexture(text_tex);

	// Present and delay
	SDL_RenderPresent(renderer);
	SDL_Delay(10);
}



