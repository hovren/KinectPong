#include <iostream>

#include "KinectPongGame.h"

int main(int argc, char** argv) {
	std::cout << "KinectPong" << std::endl;
	KinectPongGame game;
	game.run();
	std::cout << "Done" << std::endl;
	return 0;
}
