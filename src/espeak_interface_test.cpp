/*
 * espeak_interface_test.cpp
 *
 *  Created on: Mar 19, 2014
 *      Author: wallenberg
 */

#include "EspeakInterface.h"
#include <iostream>
#include <unistd.h>

int main(int argc, char** argv)
{
	std::cout << "Testing the espeak synth..." << std::endl;
	EspeakInterface my_espeak_interface;
	my_espeak_interface.speak("First phrase");
	my_espeak_interface.speak("Second phrase");
	return 0;
}


