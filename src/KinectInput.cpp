/*
 * KinectInput.cpp
 *
 *  Created on: Mar 19, 2014
 *      Author: hannes
 */
#include <iostream>
#include <opencv2/core/core.hpp>
#include "KinectInput.h"

KinectInputDevice::KinectInputDevice(freenect_context *_ctx, int _index) :
		Freenect::FreenectDevice(_ctx, _index), m_buffer_depth(
				FREENECT_DEPTH_11BIT), m_buffer_rgb(FREENECT_VIDEO_RGB), m_gamma(
				2048), m_new_depth(false), m_new_rgb(false){
	for (unsigned int i = 0; i < 2048; i++) {
		float v = i / 2048.0;
		v = std::pow(v, 3) * 6;
		m_gamma[i] = v * 6 * 256;
	}

	pthread_mutex_init(&m_mutex, NULL);
}

//KinectInputDevice::~KinectInputDevice() {
//	std::cout << "Destroying instance" << std::endl;
//}

// Do not call directly even in child
void KinectInputDevice::VideoCallback(void* _rgb, uint32_t timestamp) {
	//std::cout << "RGB callback" << std::endl;
	uint8_t* rgb = static_cast<uint8_t*>(_rgb);
	cv::Mat rgbMat(cv::Size(640,480),CV_8UC3, cv::Scalar(0));
	rgbMat.data = rgb;
	pthread_mutex_lock(&m_mutex);
	m_rgb = rgbMat;
	m_new_rgb = true;
	pthread_mutex_unlock(&m_mutex);
};
// Do not call directly even in child
void KinectInputDevice::DepthCallback(void* _depth, uint32_t timestamp) {
	//std::cout << "Depth callback" << std::endl;
	cv::Mat depthMat(cv::Size(640,480),CV_16UC1);
	uint16_t* depth = static_cast<uint16_t*>(_depth);
	depthMat.data = (uchar*) depth;
	pthread_mutex_lock(&m_mutex);
	m_depth = depthMat;
	m_new_depth = true;
	pthread_mutex_unlock(&m_mutex);
}

static Freenect::Freenect freenect;

KinectInput::KinectInput(bool registered) : m_device(freenect.createDevice<KinectInputDevice>(0)){
	m_registered = registered;
}

void KinectInput::start() {
	m_device.startDepth();
	if(m_registered)
		m_device.setDepthFormat(FREENECT_DEPTH_REGISTERED);
	m_device.startVideo();
}

void KinectInput::stop() {
	m_device.stopDepth();
	m_device.stopVideo();
}

bool KinectInputDevice::poll_data(cv::Mat& rgb, cv::Mat& depth) {
	pthread_mutex_lock(&m_mutex);
	bool retval = m_new_depth && m_new_rgb;
	if(retval) {
		m_rgb.copyTo(rgb);
		m_depth.copyTo(depth);
		m_new_rgb = m_new_depth = false;
	}
	pthread_mutex_unlock(&m_mutex);
	return retval;
}

bool KinectInput::poll_data(cv::Mat& rgb, cv::Mat& depth) {
	return m_device.poll_data(rgb, depth);
}
