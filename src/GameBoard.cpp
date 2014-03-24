/*
 * GameBoard.cpp
 *
 *  Created on: Mar 24, 2014
 *      Author: hannes
 */

#include "GameBoard.h"

#include <iostream>

GameBoard::GameBoard() {
	// TODO Auto-generated constructor stub
	for (int i=0; i < 2; ++i) {
		m_players[i] = new Player();
	}

	m_players[0]->set_name("Player 1");
	m_players[1]->set_name("Player 2");

	m_ball_pos.x = 0.5;
	m_ball_pos.y = 0.12;
	m_ball_velocity.x = -0.2;
	m_ball_velocity.y = 0.0;
	m_ball_radius = 0.01;
}

GameBoard::~GameBoard() {
	// TODO Auto-generated destructor stub
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

	// Collide with players
	cv::Vec2f collision_normal;
	for (int i=0; i < 2; ++i) {
		if (m_players[i]->collision(m_ball_pos, m_ball_radius, collision_normal)) {
			cv::Point2f r;
			float dn = m_ball_velocity.x * collision_normal.val[0] + m_ball_velocity.y * collision_normal.val[1];
			r.x = m_ball_velocity.x - 2 * dn * collision_normal.val[0];
			r.y = m_ball_velocity.y - 2 * dn * collision_normal.val[1];
			m_ball_velocity.x = r.x; //0.2 * collision_normal.val[0];
			m_ball_velocity.y = r.y; //0.2 * collision_normal.val[1];
			std::cout << "Input direction " << m_ball_velocity.x << ", " << m_ball_velocity.y << std::endl;
			std::cout << "Surface normal " << collision_normal.val[0] << ", " << collision_normal.val[1] << std::endl;
			std::cout << "New velocity " << m_ball_velocity.x << ", " << m_ball_velocity.y << std::endl;
			had_collision = true;
			break; // No need to check other player
		}
	}
}
