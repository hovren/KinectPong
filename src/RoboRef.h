/*
 * RoboRef.h
 *
 *  Created on: Mar 20, 2014
 *      Author: wallenberg
 */

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <math.h>
#include "PanTiltInterface.h"
#include "EspeakInterface.h"

#ifndef ROBOREF_H_
#define ROBOREF_H_

class RoboRef {
public:
	RoboRef();
	~RoboRef();
	void connect(std::string portname);
	void set_intrinsics(cv::Mat intrinsic_camera_matrix);
	void set_pan_tilt_angles(double pan_angle, double tilt_angle);
	void look_at(cv::Point2f pixel_position);
	void speak(std::string text);

private:
	PanTiltInterface m_pan_tilt_interface;
	EspeakInterface m_espeak_interface;
	cv::Mat m_intrinsic_camera_matrix;
};

#endif /* ROBOREF_H_ */
