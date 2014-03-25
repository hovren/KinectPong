#include <iostream>
#include <cstdlib>
#include <ctime>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "KinectPongGame.h"

int main(int argc, char** argv) {
	srand(time(NULL));
	KinectPongGame game;
	//std::cout << "" << argc << " " << argv[1] << std::endl;
	bool fullscreen = (argc > 1) && (strcmp(argv[1], "fullscreen") == 0);
	std::cout << "Starting in " << (fullscreen ? "fullscreen" : "normal") << " window mode" << std::endl;
	game.init(fullscreen);
	game.run();

	SDL_Quit();

	std::cout << "Done" << std::endl;
	return 0;
}
