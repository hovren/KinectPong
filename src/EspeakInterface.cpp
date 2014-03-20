/*
 * EspeakInterface.cpp
 *
 *  Created on: Mar 19, 2014
 *      Author: wallenberg
 */

#include "EspeakInterface.h"

EspeakInterface::EspeakInterface()
{
	m_audio_output = AUDIO_OUTPUT_PLAYBACK;
	m_synth_callback = NULL;
	m_path = "/usr/share";
	m_voice = "default";
	m_position = 0;
	m_end_position = 0;
	m_flags = espeakCHARS_8BIT;
	m_identifier = NULL;
	m_buf_length = 500;
	m_options = 0;
	espeak_Initialize(m_audio_output, m_buf_length, m_path.c_str(), m_options );
	espeak_SetVoiceByName(m_voice.c_str());
	speak("voice synth active");
}

EspeakInterface::~EspeakInterface()
{
	//Nothing to do here (I think...)
}

void EspeakInterface::speak(std::string text)
{
    espeak_Synth(text.c_str(), text.size(), m_position, m_position_type, m_end_position, m_flags, m_identifier, m_user_data );
    espeak_Synchronize();
}


espeak_POSITION_TYPE position_type;
espeak_AUDIO_OUTPUT output;
t_espeak_callback *SynthCallback;
espeak_PARAMETER Parm;
unsigned int Size,position=0, end_position=0, flags=espeakCHARS_AUTO, *unique_identifier;
