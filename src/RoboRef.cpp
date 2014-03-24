/*
 * RoboRef.cpp
 *
 *  Created on: Mar 20, 2014
 *      Author: wallenberg
 */

#include "RoboRef.h"

RoboRef::RoboRef() {
	//set default Kinect intrinsics
	//default matrices for the Kinect
	float nir_intrinsics[] = {582.67750309, 0.0, 314.96824757, 0.0, 584.65055308, 248.16240365, 0.0, 0.0,1.0};
	float rgb_intrinsics[] = {519.83879135, 0.0, 313.55797842, 0.0, 520.71387, 267.59027502, 0.0, 0.0, 1.0};

	m_intrinsic_camera_matrix.create(3, 3, CV_32F);
	for(int k=0;k<m_intrinsic_camera_matrix.rows*m_intrinsic_camera_matrix.cols;k++){
		m_intrinsic_camera_matrix.at<float>(k) = rgb_intrinsics[k];
	}

	std::cout << m_intrinsic_camera_matrix << std::endl;

	std::cout << m_intrinsic_camera_matrix.inv() << std::endl;

}

RoboRef::~RoboRef() {

}

void RoboRef::connect(std::string portname)
{
	m_pan_tilt_interface.connect(portname);
	m_pan_tilt_interface.set_base_speed_ticks(57, 57);
	m_pan_tilt_interface.set_speed_ticks(3000, 3000);
}

void RoboRef::set_intrinsics(cv::Mat intrinsic_camera_matrix)
{
	intrinsic_camera_matrix.copyTo(m_intrinsic_camera_matrix);
}

void RoboRef::set_pan_tilt_angles(double pan_angle, double tilt_angle)
{
	m_pan_tilt_interface.set_position_angles(pan_angle, tilt_angle);
}

void RoboRef::look_at(cv::Point2f pixel_position)
{
	cv::Mat ph = cv::Mat::ones(3, 1, CV_32F);
	ph.at<float>(0, 0) = pixel_position.x;
	ph.at<float>(1, 0) = pixel_position.y;
	std::cout << ph << std::endl;
	cv::Mat pn = (m_intrinsic_camera_matrix.inv()) * ph;
	std::cout << pn << std::endl;
	double norm = sqrt(pow(pn.at<float>(0, 0), 2) + pow(pn.at<float>(1, 0), 2) + pow(pn.at<float>(2, 0), 2));
	std::cout << "NORM: " << norm << std::endl;
	pn /= norm;
	double pan_angle = -180.0/M_PI*pn.at<float>(0, 0); //pan axis is flipped
	double tilt_angle = -180.0/M_PI*pn.at<float>(1, 0); //so is tilt
	std::cout << "Setting angle to target: " << pan_angle << ", " << tilt_angle << std::endl;
	std::vector<double> current_angles = m_pan_tilt_interface.get_position_angles();
	set_pan_tilt_angles(current_angles.at(0) + pan_angle, current_angles.at(1) + tilt_angle);
}

void RoboRef::speak(std::string text)
{
	m_espeak_interface.speak(text);
}
