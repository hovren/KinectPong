/*
 * KinectPongGame.cpp
 *
 *  Created on: Mar 19, 2014
 *      Author: hannes
 */

#include <iostream>
#include <unistd.h>

#include "KinectPongGame.h"

KinectPongGame::KinectPongGame() {
	m_state = Init;
}

KinectPongGame::~KinectPongGame() {

}

void KinectPongGame::run(void) {

	while (m_state != Exit) {
		switch (m_state) {
		case Init:
			handle_init();
			break;
		case WaitForPlayers:
			handle_wait_for_players();
			break;
		case IdentifyPlayers:
			handle_identify_players();
			break;
		case Exit:
			std::cout << "Exiting" << std::endl;
			break;
		default:
			m_state = Exit;
		}
	}

	m_kinect.stop();
}

void KinectPongGame::handle_init(void) {
	std::cout << "Initialising" << std::endl;
	m_kinect.start();
	std::cout << "Kinect started" << std::endl;
	cv::namedWindow("rgb", CV_WINDOW_AUTOSIZE);
	change_state(WaitForPlayers);
}

void KinectPongGame::handle_wait_for_players(void) {
	cv::Mat rgb, depth;
	m_kinect.get_pair(rgb, depth);
	cv::imshow("rgb", rgb);
	sleep(1);
	std::cout << "Loopeliloop" << std::endl;
}

void KinectPongGame::change_state(enum State new_state) {
	m_state = new_state;
}

void KinectPongGame::handle_identify_players(void) {
}
