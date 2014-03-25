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
	std::cout << "Waiting for players..." << std::endl;
	cv::Mat depth, rgb;
	if (m_game->get_kinect()->poll_data(rgb, depth)) {
		cv::flip(rgb, rgb, 1);
		cv::flip(depth, depth, 1);

		m_game->get_image_processor()->set_player_masks(depth);
		m_game->get_image_processor()->find_player_faces(rgb, depth);

		SDL_DestroyTexture(m_rgb_tex);
		SDL_DestroyTexture(m_depth_tex);

		std::vector<cv::Mat> rgb_channels;
		cv::split(rgb, rgb_channels);
		cv::Mat left_player_mask;
		m_game->get_image_processor()->get_left_player_mask(left_player_mask);
		cv::Mat right_player_mask;
		m_game->get_image_processor()->get_right_player_mask(right_player_mask);
		cv::Mat left_player_contact_mask;
		m_game->get_image_processor()->get_left_player_contact_mask(left_player_contact_mask);
		cv::Mat right_player_contact_mask;
		m_game->get_image_processor()->get_right_player_contact_mask(right_player_contact_mask);
		cv::max(rgb_channels.at(0), left_player_mask, rgb_channels.at(0));
		cv::max(rgb_channels.at(0), right_player_mask, rgb_channels.at(0));
		cv::max(rgb_channels.at(1), left_player_contact_mask, rgb_channels.at(1));
		cv::max(rgb_channels.at(1), right_player_contact_mask, rgb_channels.at(1));
		cv::Mat rgb_tex;
		cv::merge(rgb_channels, rgb_tex);
		m_rgb_tex = m_game->texture_from_mat(rgb_tex);

		cv::Mat depth_show;
		double min_val, max_val;
		cv::minMaxLoc(depth, &min_val, &max_val);
		depth.convertTo(depth_show, CV_8UC1, 255.0 / (max_val - min_val), -min_val*255.0/(max_val - min_val));
		m_depth_tex = m_game->texture_from_mat(depth_show);
	}


	if(m_game->get_image_processor()->got_left_face() && m_game->get_image_processor()->got_right_face()){
		#ifdef BUILD_ROBOREF
		cv::Rect left_face = m_game->get_image_processor()->get_left_player_face_roi();

		//look at left player
		m_game->get_roboref()->look_at(cv::Point(left_face.x + left_face.width/2, left_face.y + left_face.height/2));
		m_game->get_roboref()->speak("First player detected");
		m_game->get_roboref()->set_pan_tilt_angles(0, 0);

		//look at right player
		cv::Rect right_face = m_game->get_image_processor()->get_right_player_face_roi();
		m_game->get_roboref()->look_at(cv::Point(right_face.x + right_face.width/2, right_face.y + right_face.height/2));
		m_game->get_roboref()->speak("Second player detected");
		m_game->get_roboref()->set_pan_tilt_angles(0, 0);

		m_game->get_roboref()->speak("Let's play Pong");
		#endif
		m_game->set_next_state(STATE_PLAYING);
	}

	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_QUIT:
			m_game->set_next_state(STATE_EXIT);
			break;
		case SDL_KEYDOWN:
			m_game->set_next_state(STATE_PLAYING);
			break;
		}
	} // end SDL_PollEvent
}

void WaitForPlayerState::handle_logic()
{
	//Nothing here yet...
}

void WaitForPlayerState::render()
{
	SDL_Renderer* renderer = m_game->renderer();
	if (m_rgb_tex && m_depth_tex) {
		SDL_Rect depth_dst;
		float aspect = 0.75;
		depth_dst.x = m_game->norm2pixel_x(0.8);
		depth_dst.w = m_game->norm2pixel_x(0.2);
		depth_dst.h = depth_dst.w * aspect;
		depth_dst.y = m_game->norm2pixel_y(1.0 - m_game->pixel2norm_y(depth_dst.h));
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, m_rgb_tex, NULL, NULL);
		SDL_RenderCopy(renderer, m_depth_tex, NULL, &depth_dst);
		SDL_RenderPresent(renderer);
	}
}
