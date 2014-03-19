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

class SomeMutex {
public:
	SomeMutex() {
		pthread_mutex_init( &m_mutex, NULL );
	}
	void lock() {
		pthread_mutex_lock( &m_mutex );
	}
	void unlock() {
		pthread_mutex_unlock( &m_mutex );
	}
private:
	pthread_mutex_t m_mutex;
};

class MyFreenectDevice : public Freenect::FreenectDevice {
  public:
	MyFreenectDevice(freenect_context *_ctx, int _index)
		: Freenect::FreenectDevice(_ctx, _index), m_buffer_depth(FREENECT_DEPTH_11BIT),m_buffer_rgb(FREENECT_VIDEO_RGB), m_gamma(2048), m_new_rgb_frame(false), m_new_depth_frame(false),
		  depthMat(Size(640,480),CV_16UC1), rgbMat(Size(640,480),CV_8UC3,Scalar(0)), ownMat(Size(640,480),CV_8UC3,Scalar(0))
	{
		for( unsigned int i = 0 ; i < 2048 ; i++) {
			float v = i/2048.0;
			v = std::pow(v, 3)* 6;
			m_gamma[i] = v*6*256;
		}
	}
	// Do not call directly even in child
	void VideoCallback(void* _rgb, uint32_t timestamp) {
		std::cout << "RGB callback" << std::endl;
		m_rgb_mutex.lock();
		uint8_t* rgb = static_cast<uint8_t*>(_rgb);
		rgbMat.data = rgb;
		m_new_rgb_frame = true;
		m_rgb_mutex.unlock();
	};
	// Do not call directly even in child
	void DepthCallback(void* _depth, uint32_t timestamp) {
		std::cout << "Depth callback" << std::endl;
		m_depth_mutex.lock();
		uint16_t* depth = static_cast<uint16_t*>(_depth);
		depthMat.data = (uchar*) depth;
		m_new_depth_frame = true;
		m_depth_mutex.unlock();
	}

	bool getVideo(Mat& output) {
		m_rgb_mutex.lock();
		if(m_new_rgb_frame) {
			cv::cvtColor(rgbMat, output, CV_RGB2BGR);
			m_new_rgb_frame = false;
			m_rgb_mutex.unlock();
			return true;
		} else {
			m_rgb_mutex.unlock();
			return false;
		}
	}

	bool getDepth(Mat& output) {
			m_depth_mutex.lock();
			if(m_new_depth_frame) {
				depthMat.copyTo(output);
				m_new_depth_frame = false;
				m_depth_mutex.unlock();
				return true;
			} else {
				m_depth_mutex.unlock();
				return false;
			}
		}

  private:
	std::vector<uint8_t> m_buffer_depth;
	std::vector<uint8_t> m_buffer_rgb;
	std::vector<uint16_t> m_gamma;
	Mat depthMat;
	Mat rgbMat;
	Mat ownMat;
	SomeMutex m_rgb_mutex;
	SomeMutex m_depth_mutex;
	bool m_new_rgb_frame;
	bool m_new_depth_frame;
};

void KinectSource::start(void) {
    //The next two lines must be changed as Freenect::Freenect isn't a template but the method createDevice:
    //Freenect::Freenect<MyFreenectDevice> freenect;
    //MyFreenectDevice& device = freenect.createDevice(0);
    //by these two lines:
    Freenect::Freenect freenect;

    MyFreenectDevice& m_device = freenect.createDevice<MyFreenectDevice>(0);

	namedWindow("rgb",CV_WINDOW_AUTOSIZE);
	namedWindow("depth",CV_WINDOW_AUTOSIZE);
	m_device.startVideo();
	m_device.startDepth();
	cv::Mat depth_show_mat;
	int iter = 0;
	while (m_running) {
		m_device.getVideo(m_rgb_mat);
		m_device.getDepth(m_depth_mat);
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

	m_device.stopVideo();
	m_device.stopDepth();
}

void KinectSource::thread_func() {
	while (m_running) {
		std::cout << "Hello" << std::endl;
	}
}
