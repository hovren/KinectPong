/*
 * KinectPongGame.h
 *
 *  Created on: Mar 19, 2014
 *      Author: hannes
 */

#ifndef KINECTPONGGAME_H_
#define KINECTPONGGAME_H_

#include "KinectSource.h"

class KinectPongGame {
public:
	KinectPongGame();
	virtual ~KinectPongGame();
	void run(void);

	enum State {Init, WaitForPlayers, IdentifyPlayers, Playing, ShowResults, Exit};

private:
	void change_state(enum State new_state);
	void handle_init(void);
	void handle_wait_for_players(void);
	void handle_identify_players(void);

	enum State m_state;
	KinectSource m_kinect;
};

#endif /* KINECTPONGGAME_H_ */
