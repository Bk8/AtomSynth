/*
 * AudioBuffer.cpp
 *
 *  Created on: Jul 22, 2016
 *      Author: josh
 */

#include "AudioBuffer.h"

#include <algorithm>
#include <cmath>
#include <iostream>

namespace AtomSynth
{

int AudioBuffer::s_size = 256, AudioBuffer::s_channels = 2;

AudioBuffer::AudioBuffer()
	: m_constant(false)
{
	m_data.resize(s_channels * s_size);
}

AudioBuffer::~AudioBuffer()
{
	// TODO Auto-generated destructor stub
}

void AtomSynth::AudioBuffer::copyData(AudioBuffer & copyFrom)
{
	m_data = copyFrom.getData();
}

void AudioBuffer::fill(double value)
{
	std::fill(m_data.begin(), m_data.end(), value);
}

} /* namespace AtomSynth */
