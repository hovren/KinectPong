/*
 * KinectSource.h
 *
 *  Created on: Mar 19, 2014
 *      Author: hannes
 */

#ifndef KINECTSOURCE_H_
#define KINECTSOURCE_H_

#include <pthread.h>
#include <libfreenect/libfreenect.h>
#include <opencv2/core/core.hpp>

#include "KinectDevice.h"

class KinectSource {
public:
	KinectSource();
	virtual ~KinectSource();
	void start(void);
	void stop(void);

	void get_pair(cv::Mat& rgb, cv::Mat& depth);

	//KinectDevice* m_device;
	bool m_running; // Set private and move to getter?
	pthread_t m_thread;
	pthread_mutex_t m_depth_lock;
	pthread_mutex_t m_rgb_lock;
	void thread_func();
	cv::Mat m_depth_mat;
	cv::Mat m_rgb_mat;

};

#endif /* KINECTSOURCE_H_ */
