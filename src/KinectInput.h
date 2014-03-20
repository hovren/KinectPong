/*
 * KinectInput.h
 *
 *  Created on: Mar 19, 2014
 *      Author: hannes
 */

#ifndef KINECTINPUT_H_
#define KINECTINPUT_H_

#include <libfreenect/libfreenect.hpp>
#include <opencv2/core/core.hpp>
#include <pthread.h>

class KinectInputDevice : public Freenect::FreenectDevice {
  public:
	KinectInputDevice(freenect_context *_ctx, int _index);
//	virtual ~KinectInputDevice();

	void VideoCallback(void* _rgb, uint32_t timestamp);
	void DepthCallback(void* _depth, uint32_t timestamp);

	std::vector<uint8_t> m_buffer_depth;
	std::vector<uint8_t> m_buffer_rgb;
	std::vector<uint16_t> m_gamma;
	cv::Mat m_rgb;
	cv::Mat m_depth;
	bool m_new_depth;
	bool m_new_rgb;
	pthread_mutex_t m_mutex;

	bool poll_data(cv::Mat& rgb, cv::Mat& depth);
};

class KinectInput {
public:
	KinectInput(bool registered = false);
	void start();
	void stop();
	bool poll_data(cv::Mat& rgb, cv::Mat& depth);
	bool is_running() { return m_running;};
private:
	KinectInputDevice& m_device;
	bool m_running;
	bool m_registered;
};

#endif /* KINECTINPUT_H_ */
