/*
 * KinectSource.cpp
 *
 *  Created on: Mar 19, 2014
 *      Author: hannes
 */
#include "KinectSource.h"

#include <pthread.h>
#include <iostream>
#include <libfreenect/libfreenect.hpp>
#include <opencv2/highgui/highgui.hpp>

void *freenect_threadfunc(void *arg)
{
	KinectSource* kinect_source = (KinectSource*) arg;
	kinect_source->thread_func();
	return NULL;
}

KinectSource::KinectSource() {
	// TODO Auto-generated constructor stub
	m_running = true;
	m_depth_mat.create(cv::Size(640, 480), CV_16UC1);
	m_rgb_mat.create(cv::Size(640, 480), CV_8UC3);
}

KinectSource::~KinectSource() {
	// TODO Auto-generated destructor stub
}

void KinectSource::start(void) {
    //The next two lines must be changed as Freenect::Freenect isn't a template but the method createDevice:
    //Freenect::Freenect<MyFreenectDevice> freenect;
    //MyFreenectDevice& device = freenect.createDevice(0);
    //by these two lines:
    Freenect::Freenect freenect;
    m_device = &freenect.createDevice<KinectDevice>(0);

	namedWindow("rgb",CV_WINDOW_AUTOSIZE);
	namedWindow("depth",CV_WINDOW_AUTOSIZE);
	m_device->startVideo();
	m_device->startDepth();
	cv::Mat depth_show_mat;
	int iter = 0;
	while (m_running) {
		m_device->getVideo(m_rgb_mat);
		m_device->getDepth(m_depth_mat);
		cv::imshow("rgb", m_rgb_mat);
		m_depth_mat.convertTo(depth_show_mat, CV_8UC1, 255.0/2048.0);
		cv::imshow("depth",depth_show_mat);
		char k = cvWaitKey(5);
		if( k == 27 ){
			cvDestroyWindow("rgb");
			cvDestroyWindow("depth");
			break;
		}

		if(iter >= 1000)
			m_running = false;

		iter++;
	}

	m_device->stopVideo();
	m_device->stopDepth();
}

void KinectSource::thread_func() {
	while (m_running) {
		std::cout << "Hello" << std::endl;
	}
}
