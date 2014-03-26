/*
 * PanTiltInterface.cpp
 *
 *  Created on: Mar 19, 2014
 *      Author: wallenberg
 */

#include "PanTiltInterface.h"

PanTiltInterface::PanTiltInterface() {

	m_port = NULL;
	m_io_service = NULL;
	m_min_pan_ticks = 0;
	m_max_pan_ticks = 0;
	m_min_tilt_ticks = 0;
	m_max_tilt_ticks = 0;
	m_pan_resolution = 0;
	m_tilt_resolution = 0;
	m_connected = false;
}

PanTiltInterface::~PanTiltInterface() {
	if (m_connected) {
		write_command("PP0\n");
		write_command("TP0\n");

		m_port->close();
		m_io_service->stop();
	}

	if (m_port)
		delete m_port;
	if (m_io_service)
		delete m_io_service;
}

void PanTiltInterface::connect(std::string portname)
{
	m_io_service = new boost::asio::io_service;
	m_port = new boost::asio::serial_port(*m_io_service, portname);
	m_portname = portname;
	m_port->set_option(boost::asio::serial_port_base::baud_rate(9600));

	//enable limit checks
	write_command("LE\n");

	std::cout << "getting limits" << std::endl;

	//get limits and resolution
	std::string retstr;
	write_command("PN\n");
	retstr = read_response();
	std::cout << retstr << std::endl;
	m_min_pan_ticks = atoi(retstr.substr(25).c_str());
	write_command("PX\n");
	retstr = read_response();
	std::cout << retstr << std::endl;
	m_max_pan_ticks = atoi(retstr.substr(25).c_str());
	write_command("TN\n");
	retstr = read_response();
	std::cout << retstr << std::endl;
	m_min_tilt_ticks = atoi(retstr.substr(26).c_str());
	write_command("TX\n");
	retstr = read_response();
	std::cout << retstr << std::endl;
	m_max_tilt_ticks = atoi(retstr.substr(26).c_str());
	std::cout << "Got limits" << std::endl;

	std::cout << "Getting resolution" << std::endl;
	write_command("PR\n");
	retstr = read_response();
	std::string valuestr;
	unsigned int idx = 1;
	while((retstr[idx] != ' ') && (idx < retstr.size())){
		valuestr += retstr[idx];
		idx++;
	}
	std::cout << valuestr << std::endl;
	m_pan_resolution = atof(valuestr.c_str());
	write_command("TR\n");
	retstr = read_response();
	valuestr = "";
	idx = 1;
	while((retstr[idx] != ' ') && (idx < retstr.size())){
		valuestr += retstr[idx];
		idx++;
	}
	m_tilt_resolution = atof(valuestr.c_str());

	std::cout << "Got resolution" << std::endl;

	std::cout << "Limits:" << std::endl;
	std::cout << "Pan: " << m_min_pan_ticks << ":" << m_max_pan_ticks << ", resolution: " << m_pan_resolution << std::endl;
	std::cout << "Tilt: " << m_min_tilt_ticks << ":" << m_max_tilt_ticks  << ", resolution: " << m_tilt_resolution << std::endl;

	// Mark as connected
	m_connected = true;
}


void PanTiltInterface::write_command(std::string cmdstr)
{
	boost::asio::write(*m_port, boost::asio::buffer(cmdstr.c_str(), cmdstr.size()));
	await_command_completion();
}

std::string PanTiltInterface::read_response()
{
	std::string response_string;
	unsigned char c = 0;
	while((boost::asio::read(*m_port, boost::asio::buffer(&c, 1))) && (c != '\n'))
		response_string += c;

	return response_string;
}

void PanTiltInterface::clear_read_buffer()
{
	unsigned char c = 0;
	while(c != '\n')
		boost::asio::read(*m_port, boost::asio::buffer(&c, 1));
	std::cout << "Read buffer cleared" << std::endl;
}

std::vector<int> PanTiltInterface::get_position_ticks()
{
	std::vector<int> pan_tilt_ticks(2, 0);

	std::string response_string;
	//request pan ticks
	write_command("PP\n");
	response_string = read_response();
	//pan ticks start at 26
	if(response_string.size() >= 27)
		pan_tilt_ticks.at(0) = atoi(response_string.substr(25).c_str());
	else
		std::cout << "Error getting pan ticks" << std::endl;
	//request tilt ticks
	write_command("TP\n");
	response_string = read_response();
	//tilt ticks start at 26
	if(response_string.size() >= 27)
		pan_tilt_ticks.at(1) = atoi(response_string.substr(25).c_str());
	else
		std::cout << "Error getting tilt ticks" << std::endl;

	return pan_tilt_ticks;
}

std::vector<double> PanTiltInterface::get_position_angles()
{
	std::vector<int> pan_tilt_ticks = get_position_ticks();
	std::vector<double> pan_tilt_angles = pan_tilt_ticks_to_angles(pan_tilt_ticks.at(0), pan_tilt_ticks.at(1));
	return pan_tilt_angles;
}

void PanTiltInterface::await_command_completion()
{
	unsigned char c = 0;
	while((c != '*') && (c !='!'))
		boost::asio::read(*m_port, boost::asio::buffer(&c, 1));
	if(c == '!'){
		std::string errmsg = read_response();
		std::cout << "ERROR: " << errmsg << std::endl;
	}

}

std::string PanTiltInterface::int_to_string(int arg)
{
	std::stringstream ss;
	ss << arg;
	std::string arg_string;
	ss >> arg_string;
	return arg_string;
}

void PanTiltInterface::set_position_ticks(int pan_ticks, int tilt_ticks)
{

	if(pan_ticks < m_min_pan_ticks)
		pan_ticks = m_min_pan_ticks;
	if(pan_ticks > m_max_pan_ticks)
		pan_ticks = m_max_pan_ticks;
	if(tilt_ticks < m_min_tilt_ticks)
		tilt_ticks = m_min_tilt_ticks;
	if(tilt_ticks > m_max_tilt_ticks)
		tilt_ticks = m_max_tilt_ticks;

	std::string pan_string = int_to_string(pan_ticks);
	std::string tilt_string = int_to_string(tilt_ticks);

	//set slaved mode
	write_command("S\n");
	//set pan ticks
	write_command("PP" + pan_string + "\n");
	//set tilt ticks
	write_command("TP" + tilt_string + "\n");
	// execute move
	clear_read_buffer();
	write_command("A\n");

}

void PanTiltInterface::set_base_speed_ticks(int pan_speed, int tilt_speed)
{
	std::string pan_string = int_to_string(pan_speed);
	std::string tilt_string = int_to_string(tilt_speed);
	write_command("PB" + pan_string + "\n");
	write_command("TB" + tilt_string+ "\n");
}

void PanTiltInterface::set_speed_ticks(int pan_speed, int tilt_speed)
{
	std::string pan_string = int_to_string(pan_speed);
	std::string tilt_string = int_to_string(tilt_speed);
	write_command("PS" + pan_string + "\n");
	write_command("TS" + tilt_string + "\n");
}

void PanTiltInterface::set_acceleration_ticks(int pan_acc, int tilt_acc)
{
	std::string pan_string = int_to_string(pan_acc);
	std::string tilt_string = int_to_string(tilt_acc);
	write_command("PA" + pan_string + "\n");
	write_command("TA" + tilt_string + "\n");
}

void PanTiltInterface::reset()
{
	write_command("R\n");
}

std::vector<int> PanTiltInterface::pan_tilt_angles_to_ticks(double pan_angle, double tilt_angle)
{
	std::vector<int> pan_tilt_ticks(2, 0);
	double pan_degrees_per_tick = m_pan_resolution*0.0002778; //approximate value from PTU manual
	double tilt_degrees_per_tick = m_tilt_resolution*0.0002778; //approximate value from PTU manual
	pan_tilt_ticks.at(0) = (int)round(pan_angle/pan_degrees_per_tick);
	pan_tilt_ticks.at(1) = (int)round(tilt_angle/tilt_degrees_per_tick);
	return pan_tilt_ticks;
}

std::vector<double> PanTiltInterface::pan_tilt_ticks_to_angles(int pan_ticks, int tilt_ticks)
{
	std::vector<double> pan_tilt_angles(2, 0);
	double pan_degrees_per_tick = m_pan_resolution*0.0002778; //approximate value from PTU manual
	double tilt_degrees_per_tick = m_tilt_resolution*0.0002778; //approximate value from PTU manual
	pan_tilt_angles.at(0) = ((double)pan_ticks)*pan_degrees_per_tick;
	pan_tilt_angles.at(1) = ((double)tilt_ticks)*tilt_degrees_per_tick;
	return pan_tilt_angles;
}

void PanTiltInterface::set_position_angles(double pan_angle, double tilt_angle)
{
	std::vector<int> pan_tilt_ticks = pan_tilt_angles_to_ticks(pan_angle, tilt_angle);

	set_position_ticks(pan_tilt_ticks.at(0), pan_tilt_ticks.at(1));
}
