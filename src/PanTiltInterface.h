/*
 * PanTiltInterface.h
 *
 *  Created on: Mar 19, 2014
 *      Author: wallenberg
 */

#include <boost/asio.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifndef PANTILTINTERFACE_H_
#define PANTILTINTERFACE_H_

class PanTiltInterface {
public:
	PanTiltInterface();
	~PanTiltInterface();
	void 				connect(std::string portname);
	std::vector<int> 	get_position_ticks();
	std::vector<double> get_position_angles();
	void 				set_position_ticks(int pan_ticks, int tilt_ticks);
	void 				set_position_angles(double pan_angle, double tilt_angle);
	void 				set_base_speed_ticks(int pan_speed, int tilt_speed);
	void 				set_speed_ticks(int pan_speed, int tilt_speed);
	void 				set_acceleration_ticks(int pan_acc, int tilt_acc);
	void				reset();

	void 				write_command(std::string cmdstr);
	void 				await_command_completion();
	std::string 		read_response();
	void 				clear_read_buffer();
	static std::string 	int_to_string(int arg);
	std::vector<int> 	pan_tilt_angles_to_ticks(double pan_angle, double tilt_angle);
	std::vector<double> pan_tilt_ticks_to_angles(int pan_ticks, int tilt_ticks);


private:
	boost::asio::serial_port*   m_port;
	std::string 				m_portname;
	boost::asio::io_service* 	m_io_service;
	int 						m_min_pan_ticks;
	int 						m_max_pan_ticks;
	int 						m_min_tilt_ticks;
	int 						m_max_tilt_ticks;
	double 						m_pan_resolution;
	double 						m_tilt_resolution;

};

#endif /* PANTILTINTERFACE_H_ */
