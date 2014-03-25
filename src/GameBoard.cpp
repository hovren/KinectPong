/*
 * GameBoard.cpp
 *
 *  Created on: Mar 24, 2014
 *      Author: hannes
 */

#include "GameBoard.h"
#include "Player.h"

#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

GameBoard::GameBoard(KinectPongGame* game) {
	m_game = game;

	for (int i=0; i < 2; ++i) {
		m_players[i] = new Player(this);
	}

	m_players[0]->set_name("Player 1");
	m_players[1]->set_name("Player 2");

	m_ball_pos.x = 0.85;
	m_ball_pos.y = 0.12;
	m_ball_velocity.x = -1.8;
	m_ball_velocity.y = 0.0;
	m_ball_radius = 0.01;

	had_collision = false;

	// Graphics related
	m_ball_tex = IMG_LoadTexture(m_game->renderer(), "window_icon.png");
	if (!m_ball_tex) {
		std::cout << "Error: IMG_LoadTexture: " << SDL_GetError() << std::endl;
	}
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

	//std::cout << "Ball pos " << m_ball_pos.x << ", " << m_ball_pos.y << " velocity " << m_ball_velocity.x << ", " << m_ball_velocity.y << std::endl;

	// Collide with walls
	if (m_ball_pos.x - m_ball_radius < 0) {
		m_ball_pos.x = m_ball_radius;
		m_ball_velocity.x = -m_ball_velocity.x;
		//m_ball_vel_x = m_ball_vel_y = 0;
		//std::cout << "Hit wall " << m_ball_pos_x << ", " << m_ball_pos_y << " radius " << m_ball_radius << std::endl;
		had_collision = true;
	}
	if (m_ball_pos.x + m_ball_radius > 1.0) {
		m_ball_pos.x = 1.0 - m_ball_radius;
		m_ball_velocity.x = -m_ball_velocity.x;
		//m_ball_vel_x = m_ball_vel_y = 0;
		//std::cout << "Hit wall " << m_ball_pos_x << ", " << m_ball_pos_y << " radius " << m_ball_radius << std::endl;
		had_collision = true;
	}
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

	///////////////////////
	// OBS
	// OBS
	// OBS
	////////////////////////
	had_collision = false;


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
