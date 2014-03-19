/*
 * KinectSource.h
 *
 *  Created on: Mar 19, 2014
 *      Author: hannes
 */

#ifndef KINECTSOURCE_H_
#define KINECTSOURCE_H_

#include <libfreenect/libfreenect.h>
#include <opencv2/core/core.hpp>

#include "KinectDevice.h"

class KinectSource {
public:
	KinectSource();
	virtual ~KinectSource();
	void start(void);


	KinectDevice* m_device;
	bool m_running; // Set private and move to getter?
	void thread_func();
	cv::Mat m_depth_mat;
	cv::Mat m_rgb_mat;
};

#endif /* KINECTSOURCE_H_ */
