/*
 * BuiltinWaveform.cpp
 *
 *  Created on: Aug 17, 2016
 *      Author: josh
 */

#include "BuiltinWaveform.h"

#define _USE_MATH_DEFINES
#include <cmath>

namespace AtomSynth
{

namespace Waveform
{


double calculate(WaveShape shape, double phase, double top, double bottom, double center, double parm)
{
	double value = 0.0f;
	switch(shape)
	{
	case SINE:
		if(center == 0.0f)
			return (-sin(phase * M_PI) / 2.0f + 0.5f) * (top - bottom) + bottom;
		else
			phase = (phase < center) ? (phase + 1.0f) * (1.0f / (center + 1.0000001f)) : (phase - 1.0f) * (1.0f / (1.0000001f - center));
			return (sin(phase * M_PI) / 2.0f + 0.5f) * (top - bottom) + bottom;
	case SQUARE:
		return (phase < center) ? (top) : (bottom);
	case TRIANGLE:
		value = (phase < center) ? (phase + 1.0f) * (1.0f / (center + 1.0000001f)) : -(phase - 1.0f) * (1.0f / (1.0000001f - center));
		return value * (top - bottom) + bottom;
	default:
		return 0.0f;
	}
}

} /* namespace Waveform */

} /* namespace AtomSynth */

