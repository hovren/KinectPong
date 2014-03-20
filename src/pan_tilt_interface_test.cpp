/*
 * pan_tilt_interface_test.cpp
 *
 *  Created on: Mar 19, 2014
 *      Author: wallenberg
 */



#include "PanTiltInterface.h"
#include <iostream>

int main(int argc, char** argv)
{
	std::cout << "Testing the PTU interface" << std::endl;

	PanTiltInterface my_interface;
	my_interface.connect("/dev/ttyUSB0");
	std::vector<int> pan_tilt_ticks = my_interface.get_position_ticks();
	std::vector<double> pan_tilt_angles = my_interface.get_position_angles();
	std::cout << "Pan ticks: " << pan_tilt_ticks.at(0) << ", Tilt ticks: " << pan_tilt_ticks.at(1) << std::endl;
	std::cout << "Pan angle: " << pan_tilt_angles.at(0) << ", Tilt angle: " << pan_tilt_angles.at(1) << std::endl;

	//set speed
	my_interface.set_base_speed_ticks(57, 57);
	my_interface.set_speed_ticks(3000, 3000);

	my_interface.set_position_angles(90, 25);
	pan_tilt_ticks = my_interface.get_position_ticks();
	pan_tilt_angles = my_interface.get_position_angles();

	std::cout << "Pan ticks: " << pan_tilt_ticks.at(0) << ", Tilt ticks: " << pan_tilt_ticks.at(1) << std::endl;
	std::cout << "Pan angle: " << pan_tilt_angles.at(0) << ", Tilt angle: " << pan_tilt_angles.at(1) << std::endl;
	my_interface.set_position_angles(0, 0);

	pan_tilt_ticks = my_interface.get_position_ticks();
	pan_tilt_angles = my_interface.get_position_angles();
	std::cout << "Pan ticks: " << pan_tilt_ticks.at(0) << ", Tilt ticks: " << pan_tilt_ticks.at(1) << std::endl;
	std::cout << "Pan angle: " << pan_tilt_angles.at(0) << ", Tilt angle: " << pan_tilt_angles.at(1) << std::endl;
}
