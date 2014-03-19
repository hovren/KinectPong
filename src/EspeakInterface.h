/*
 * EspeakInterface.h
 *
 *  Created on: Mar 19, 2014
 *      Author: wallenberg
 */

#include <string>
#include <malloc.h>
#include <espeak/speak_lib.h>
#include <iostream>

#ifndef ESPEAKINTERFACE_H_
#define ESPEAKINTERFACE_H_

class EspeakInterface{
public:
	EspeakInterface();
	~EspeakInterface();
	void speak(std::string text);

private:
	espeak_POSITION_TYPE 	m_position_type;
	espeak_AUDIO_OUTPUT 	m_audio_output;
	espeak_PARAMETER		m_parameter;
	t_espeak_callback*		m_synth_callback;
	std::string				m_path;
	std::string				m_voice;
	unsigned int			m_position;
	unsigned int			m_end_position;
	unsigned int			m_flags;
	unsigned int* 			m_identifier;
	int						m_buf_length;
	int						m_options;
	void*					m_user_data;

};



#endif /* ESPEAKINTERFACE_H_ */


