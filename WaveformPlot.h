/*
 * WaveformPlot.h
 *
 *  Created on: Aug 17, 2016
 *      Author: josh
 */

#ifndef SOURCE_WAVEFORMPLOT_H_
#define SOURCE_WAVEFORMPLOT_H_

#include "../JuceLibraryCode/JuceHeader.h"

#include <vector>

namespace AtomSynth
{

namespace Waveform
{

class WaveformPlot : public Component
{
private:
	int m_xLines, m_yLines;
	std::vector<double> m_values;
public:
	WaveformPlot();
	virtual ~WaveformPlot();
	virtual void paint(Graphics & g);
	void setLines(int xLines, int yLines) { m_xLines = xLines; m_yLines = yLines; }
	void addPoint(double value);
	void clear();
	void finish();
};

} /* namespace Waveform */

} /* namespace AtomSynth */

#endif /* SOURCE_WAVEFORMPLOT_H_ */
