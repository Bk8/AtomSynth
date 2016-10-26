/*
 * AudioBuffer.h
 *
 *  Created on: Jul 22, 2016
 *      Author: josh
 */

#ifndef SOURCE_AUDIOBUFFER_H_
#define SOURCE_AUDIOBUFFER_H_

#include <vector>

namespace AtomSynth
{

class AudioBuffer
{
private:
	std::vector<double> m_data;
	bool m_constant;
	static int s_size, s_channels;
public:
	AudioBuffer();
	virtual ~AudioBuffer();
	void copyData(AudioBuffer & copyFrom);
	void fill(double value);
	std::vector<double> & getData() { return m_data; }
	double getValue(int channel, int sample) { return m_data[channel * s_size + sample]; }
	void setValue(int channel, int sample, double value) { m_data[channel * s_size + sample] = value; }
	void setConstant(bool constant) { m_constant = constant; }
	bool getConstant() { return m_constant; }
	static void setSize(int size) { s_size = size; }
	static void setChannels(int channels) { s_channels = channels; }
	static int getSize() { return s_size; }
	static int getChannels() { return s_channels; }
};

} /* namespace AtomSynth */

#endif /* SOURCE_AUDIOBUFFER_H_ */
