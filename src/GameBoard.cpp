/*
 * GameBoard.cpp
 *
 *  Created on: Mar 24, 2014
 *      Author: hannes
 */

#include "GameBoard.h"
#include "Player.h"
#include "states/GameStates.h"

#include <iostream>
#include <cstdlib>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#define GAMEBOARD_MAX_SCORE 5

static Uint32 last_update = 0;

GameBoard::GameBoard(KinectPongGame* game) : m_max_score(GAMEBOARD_MAX_SCORE) {
	m_game = game;

	for (int i = 0; i < 2; ++i) {
		m_players[i] = NULL;
	}

	m_player_1_score_surf = NULL;
	m_player_2_score_surf = NULL;

	m_ball_pos.x = 0.85;
	m_ball_pos.y = 0.12;
	m_ball_velocity.x = -1.8;
	m_ball_velocity.y = 0.0;
	m_ball_radius = 0.01;

	// Screen size and position in normed world coordinates
	m_game_screen_dims.x = 1.0;
	m_game_screen_dims.y = 1.0;
	m_game_screen_pos.x = 0.0;
	m_game_screen_pos.y = 0.0;

	had_collision = false;

	// Graphics related
	m_ball_tex = IMG_LoadTexture(m_game->renderer(), "window_icon.png");
	if (!m_ball_tex) {
		std::cout << "Error: IMG_LoadTexture: " << SDL_GetError() << std::endl;
	}


	// Reset the game
	reset();
}

GameBoard::~GameBoard() {
	// TODO Auto-generated destructor stub
}


cv::Point2f reflect(cv::Point2f in, cv::Vec2f normal) {
	float dn = in.x * normal.val[0] + in.y * normal.val[1];
	cv::Point2f out;
	out.x = in.x - 2 * dn * normal.val[0];
	out.y = in.y - 2 * dn * normal.val[1];

	return out;
}

void GameBoard::update(float dt) {
	had_collision = false;
	m_ball_pos.x += m_ball_velocity.x * dt;
	m_ball_pos.y += m_ball_velocity.y * dt;

	// Collide with score zones
	if (m_ball_pos.x - m_ball_radius < 0) {
		m_ball_pos.x = m_ball_radius;
		//m_ball_velocity.x = -m_ball_velocity.x;
		//had_collision = true;
		m_players[0]->increase_score();
		m_event = GAMEBOARD_EVENT_PLAYER_1_SCORED;
	}
	if (m_ball_pos.x + m_ball_radius > 1.0) {
		m_ball_pos.x = 1.0 - m_ball_radius;
		//m_ball_velocity.x = -m_ball_velocity.x;
		//had_collision = true;
		m_players[1]->increase_score();
		m_event = GAMEBOARD_EVENT_PLAYER_2_SCORED;
	}

	// Collide with upper wall
	if (m_ball_pos.y - m_ball_radius < 0) {
		m_ball_pos.y = m_ball_radius;
		m_ball_velocity.y = -m_ball_velocity.y;
		//m_ball_vel_x = m_ball_vel_y = 0;
		//std::cout << "Hit wall " << m_ball_pos_x << ", " << m_ball_pos_y << " radius " << m_ball_radius << std::endl;
		had_collision = true;
	}
	if (m_ball_pos.y + m_ball_radius > 1.0) {
		m_ball_pos.y = 1.0 - m_ball_radius;
		m_ball_velocity.y = -m_ball_velocity.y;
		//m_ball_vel_x = m_ball_vel_y = 0;
		//std::cout << "Hit wall " << m_ball_pos_x << ", " << m_ball_pos_y << " radius " << m_ball_radius << std::endl;
		had_collision = true;
	}

	// Collide with players
	cv::Vec2f collision_normal;
	cv::Point2f normed_velocity = m_ball_velocity * (1.0 / cv::norm(m_ball_velocity));
	if (normed_velocity.dot(collision_normal) < 0) {
		normed_velocity *= -1;
	}
	for (int i=0; i < 2; ++i) {
		cv::Vec2f collision_normal;
		cv::Point2f collision_point;
		if (m_players[i]->collision_line(m_ball_pos, m_ball_velocity, m_ball_radius, dt, collision_normal, collision_point)) {
			m_ball_pos = collision_point;
			cv::Point2f reflection = reflect(normed_velocity, collision_normal);
			std::cout << "Collided. Input velocity (normed)" << normed_velocity << " speed="<< cv::norm(m_ball_velocity) << " dot=" << reflection.dot(normed_velocity) << " collision normal " << collision_normal <<  " reflection " << reflection << std::endl;
			m_ball_velocity = cv::norm(m_ball_velocity) * reflection;
			std::cout << "New velocity " << m_ball_velocity << std::endl;
			had_collision = true;
		}
	}

	// Check out of bounds and reset
	if ((m_ball_pos.x > 1.0) || (m_ball_pos.y > 1.0) || (m_ball_pos.x < 0) || (m_ball_pos.y < 0)) {
		std::cout << "Ball out of bounds, re-serve" << std::endl;
		m_game->set_next_state(STATE_PLAY_SERVE);
	}

	if (SDL_GetTicks() > (last_update + 1000)) {
		std::cout << "Current ball position " << m_ball_pos << " ball velocity " << m_ball_velocity << std::endl;
		last_update = SDL_GetTicks();
	}
}

cv::Point2f GameBoard::game2screen(cv::Point2f gp) {
	cv::Point2f sp;
	sp.x = m_game_screen_pos.x + m_game_screen_dims.x * gp.x;
	sp.y = m_game_screen_pos.y + m_game_screen_dims.y * gp.y;
	return sp;
}

cv::Point2f GameBoard::screen2game(cv::Point2f sp) {
	cv::Point2f gp;
	gp.x = (sp.x - m_game_screen_pos.x) / m_game_screen_dims.x;
	gp.y = (sp.y - m_game_screen_pos.y) / m_game_screen_dims.y;
	return gp;
}

cv::Point GameBoard::game2pixel(cv::Point2f gp) {
	cv::Point p(m_game->norm2pixel_x(gp.x), m_game->norm2pixel_y(gp.y));
	return p;
}

void GameBoard::render_board_background() {
	SDL_Renderer* renderer = m_game->renderer();

	// Background color
	SDL_SetRenderDrawColor(renderer, 60, 60, 80, 255);
	SDL_RenderClear(renderer);

	// Draw middle line
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 128);
	cv::Point l0, l1;
	l0 = game2pixel(cv::Point2f(0.5, 0.0));
	l1 = game2pixel(cv::Point2f(0.5, 1.0));
	SDL_RenderDrawLine(renderer, l0.x, l0.y, l1.x, l1.y);
}

void GameBoard::render_ball() {
	SDL_Rect ball_rect;
	cv::Point2f ball_pos = game2screen(cv::Point2f(m_ball_pos.x - m_ball_radius, m_ball_pos.y - m_ball_radius));
	//std::cout << "Ball was at " << m_ball_pos << " and is now at " << ball_pos << std::endl;
	ball_rect.x = m_game->norm2pixel_x(ball_pos.x);
	ball_rect.y = m_game->norm2pixel_y(ball_pos.y);
	cv::Point2f ball_dim = game2screen(cv::Point2f(m_ball_radius, 0));
	ball_rect.h = ball_rect.w = m_game->norm2pixel_x(2*ball_dim.x);
	SDL_Texture* ball_tex = m_game->get_gameboard()->get_ball_texture();
	//std::cout << "Rendering " << ball_tex << " at " << ball_rect.x << ", " << ball_rect.y << " size " << ball_rect.w << " x " << ball_rect.h << std::endl;
	SDL_RenderCopy(m_game->renderer(), ball_tex, NULL, &ball_rect);
}

void GameBoard::render_board_all() {
	SDL_Renderer* renderer = m_game->renderer();

	// Background
	render_board_background();

	// Current Score
	render_scores();

	// Render paddle
	for (int i=0; i < 2; ++i) {
		Player* player = get_player(i);

		// Render player silhouettes
		SDL_Texture* player_tex = player->get_player_texture();
		if (player_tex) {
			SDL_Rect player_rect;
			//cv::Point2f paddle_pos(m_player_data[player].paddle_x, m_player_data[player].paddle_y);
			cv::Rect_<float> player_rect_norm = player->get_player_rect();
			cv::Point padd_pos = game2pixel(cv::Point2f(player_rect_norm.x, player_rect_norm.y));
			cv::Point padd_dim = game2pixel(cv::Point2f(player_rect_norm.width, player_rect_norm.height));
			player_rect.x = padd_pos.x;
			player_rect.y = padd_pos.y;
			player_rect.w = padd_dim.x;
			player_rect.h = padd_dim.y;
			SDL_RenderCopy(renderer, player_tex, NULL, &player_rect);
		}

		//Render paddle area
		SDL_Texture* paddle_tex = player->get_paddle_texture();
		if (paddle_tex) {
			SDL_Rect paddle_rect;
			//cv::Point2f paddle_pos(m_player_data[player].paddle_x, m_player_data[player].paddle_y);
			cv::Rect_<float> paddle_rect_norm = player->get_paddle_rect();
			cv::Point padd_pos = game2pixel(cv::Point2f(paddle_rect_norm.x, paddle_rect_norm.y));
			cv::Point padd_dim = game2pixel(cv::Point2f(paddle_rect_norm.width, paddle_rect_norm.height));
			paddle_rect.x = padd_pos.x;
			paddle_rect.y = padd_pos.y;
			paddle_rect.w = padd_dim.x;
			paddle_rect.h = padd_dim.y;
			SDL_RenderCopy(renderer, paddle_tex, NULL, &paddle_rect);
		}
	}




	// Render ball
	render_ball();

}

static float random_between(float a, float b) {
	float random = ((float) rand()) / (float) RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

void GameBoard::reset_ball() {
	m_event = GAMEBOARD_EVENT_NONE;
	m_ball_pos = cv::Point2f(0.5, 0.5);
	m_ball_velocity = cv::Point2f(random_between(-1,1), random_between(-1,1));
	const float start_speed = 0.5;
	m_ball_velocity = (start_speed / cv::norm(m_ball_velocity)) * m_ball_velocity; // Normalize to starting speed
	update_score_textures();
}

void GameBoard::render_scores() {
	SDL_Renderer* renderer = m_game->renderer();

	const float score_height = 0.1;
	const float score_margin = 0.01;
	for (int i=0; i < 2; ++i) {
		SDL_Surface* surf = (i == 0 ? m_player_1_score_surf : m_player_2_score_surf);
		SDL_Rect dst;
		float scale = ((float) m_game->norm2pixel_y(score_height)) / surf->h;
		// Note: Player 1 is on the right, and player 2 is on the left
		dst.x = m_game->norm2pixel_x(0.5) + (i == 1 ? -scale*surf->w - m_game->norm2pixel_x(score_margin) : m_game->norm2pixel_x(score_margin));
		dst.y = m_game->norm2pixel_y(0.05);
		dst.w = scale * surf->w;
		dst.h = scale * surf->h;
		SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
		SDL_RenderCopy(renderer, tex, NULL, &dst);
		SDL_DestroyTexture(tex);
	}
}

void GameBoard::update_score_textures() {
	TTF_Font* font = TTF_OpenFont("font.ttf", 200);
	if (!font) {
		std::cout << "Error loading font: " << TTF_GetError() << std::endl;
	}

	if (m_player_1_score_surf) {
		SDL_FreeSurface(m_player_1_score_surf);
	}
	if (m_player_2_score_surf) {
		SDL_FreeSurface(m_player_2_score_surf);
	}

	SDL_Color text_color;
	text_color.r = text_color.g = text_color.b = text_color.a = 255;
	char score_buf[8];

	sprintf(score_buf, "%d", get_player(0)->score());
	std::cout << "Player 1 score " << get_player(0)->score() << " and " << score_buf << std::endl;
	m_player_1_score_surf = TTF_RenderText_Blended(font, score_buf, text_color);
	if (!m_player_1_score_surf) {
		std::cout << "Error: " << TTF_GetError() << std::endl;
	}

	sprintf(score_buf, "%d", get_player(1)->score());
	std::cout << "Player 2 score " << get_player(1)->score() << " and " << score_buf << std::endl;
	m_player_2_score_surf = TTF_RenderText_Blended(font, score_buf, text_color);
	if (!m_player_2_score_surf) {
		std::cout << "Error: " << TTF_GetError() << std::endl;
	}
}

void GameBoard::reset() {
	for (int i=0; i < 2; ++i) {
		if (m_players[i]) {
			delete m_players[i];
		}

		m_players[i] = new Player(this);
	}

	m_players[0]->set_name("Player 1");
	m_players[1]->set_name("Player 2");

	if (m_player_1_score_surf) {
		delete m_player_1_score_surf;
	}
	if (m_player_2_score_surf) {
		delete m_player_2_score_surf;
	}
	m_player_1_score_surf = NULL;
	m_player_2_score_surf = NULL;

	m_event = GAMEBOARD_EVENT_NONE;

}

bool GameBoard::gameover() {
	for (int i=0; i < 2; ++i) {
		if (get_player(i)->score() >= m_max_score) {
			return true;
		}
	}
	return false;
}
