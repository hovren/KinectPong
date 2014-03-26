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

#include "../Player.h"

#define DEFAULT_BALL_RADIUS 0.015
#define MIN_BALL_RADIUS_PIXELS 10

PlayingState::PlayingState(KinectPongGame* game) {
	std::cout << "Run constructor" << std::endl;
	m_game = game;
//	m_ball_tex = create_ball_texture(m_game->norm2pixel_x(m_ball_radius), m_game->renderer());

	//cv::Mat test_mask; //(cv::Size(640, 480), CV_8U);

	cv::Mat test_mask = cv::Mat::zeros(cv::Size(640, 480), CV_8U);
	/*cv::circle(test_mask, cv::Point(100, 70), 50, cv::Scalar(255), -1);
	cv::rectangle(test_mask, cv::Point(100, 20), cv::Point(350, 450), cv::Scalar(0), -1);
	cv::Mat rot_mat = cv::getRotationMatrix2D(cv::Point2f(100, 70), 30.0, 1.0);
	cv::warpAffine(test_mask, test_mask, rot_mat, test_mask.size());*/
	for (int i=0; i < 2; ++i) {
		Player* player = m_game->get_gameboard()->get_player(i);
		player->paddle_input(test_mask);
		cv::Mat mask = player->get_paddle_mask();
	}
	m_last_tick = 0;
}

PlayingState::~PlayingState() {
	// Do nothing
}

void PlayingState::handle_events(KinectInput* kinect) {
	cv::Mat depth, rgb;
	if (m_game->get_kinect()->poll_data(rgb, depth)) {
		cv::flip(rgb, rgb, 1);
		cv::flip(depth, depth, 1);

		m_game->get_image_processor()->set_player_masks(depth);

		//face detector is too slow to run in this loop
	    //m_game->get_image_processor()->find_player_faces(rgb, depth);

		cv::Mat right_player_mask;
		m_game->get_image_processor()->get_right_player_mask(right_player_mask);
		m_game->get_gameboard()->get_player(0)->player_input(right_player_mask);

		cv::Mat left_player_mask;
		m_game->get_image_processor()->get_left_player_mask(left_player_mask);
		m_game->get_gameboard()->get_player(1)->player_input(left_player_mask);

		cv::Mat right_contact_mask;
		m_game->get_image_processor()->get_right_player_contact_mask(right_contact_mask);
		m_game->get_gameboard()->get_player(0)->paddle_input(right_contact_mask);

		cv::Mat left_contact_mask;
		m_game->get_image_processor()->get_left_player_contact_mask(left_contact_mask);
		m_game->get_gameboard()->get_player(1)->paddle_input(left_contact_mask);
	}

	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_QUIT:
		case SDL_KEYDOWN:
			if (e.key.keysym.sym == SDLK_ESCAPE)
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

	int p1_score_prev = gameboard->get_player(0)->score();
	int p2_score_prev = gameboard->get_player(1)->score();
	gameboard->update(dt);
	int p1_score = gameboard->get_player(0)->score();
	int p2_score = gameboard->get_player(1)->score();

	if (p1_score > p1_score_prev) {
		m_game->set_next_state(STATE_PLAY_SCORE);
	}
	else if (p2_score > p2_score_prev) {
		m_game->set_next_state(STATE_PLAY_SCORE);
	}


	// Update tick count (not safe for overflows, I guess, but we can play for 1200 hours before that happens...)
	m_last_tick = curr_tick;
}

void PlayingState::render() {
	SDL_Renderer* renderer = m_game->renderer();
	GameBoard* gameboard = m_game->get_gameboard();

	gameboard->render_board_all();

	// Present and delay
	SDL_RenderPresent(renderer);
	SDL_Delay(10);
}
