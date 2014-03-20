#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "KinectPongGame.h"

int main(int argc, char** argv) {
	KinectPongGame game;
	game.init();
	game.run();

	SDL_Quit();

	std::cout << "Done" << std::endl;
	return 0;
}
