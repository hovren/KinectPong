/*
 * kinectview.cpp
 *
 *  Created on: Mar 20, 2014
 *      Author: hannes
 */

#include "GameStates.h"

#include <iostream>

#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

KinectViewState::KinectViewState(KinectPongGame* game) {
	m_game = game;
	m_rgb_tex = m_depth_tex = NULL;
}

KinectViewState::~KinectViewState() {
	// Do nothing
}

void KinectViewState::handle_events(KinectInput* kinect) {
	cv::Mat depth, rgb;
	if (kinect->poll_data(rgb, depth)) {
		//std::cout << "New frame at time " << SDL_GetTicks() / 1000.0 << std::endl;
		SDL_DestroyTexture(m_rgb_tex);
		SDL_DestroyTexture(m_depth_tex);
		m_rgb_tex = m_game->texture_from_mat(rgb);
		cv::Mat depth_show;
		double min_val, max_val;
		cv::minMaxLoc(depth, &min_val, &max_val);
		depth.convertTo(depth_show, CV_8UC1, 255.0 / (max_val - min_val), -min_val*255.0/(max_val - min_val));
		m_depth_tex = m_game->texture_from_mat(depth_show);
	}

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

void KinectViewState::handle_logic() {

}

void KinectViewState::render() {
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
//		SDL_RenderCopy(renderer, m_depth_tex, NULL, NULL);
//		SDL_RenderCopy(renderer, m_rgb_tex, NULL, &depth_dst);
		SDL_RenderPresent(renderer);
	}
}




