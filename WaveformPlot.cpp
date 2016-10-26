/*
 * WaveformPlot.cpp
 *
 *  Created on: Aug 17, 2016
 *      Author: josh
 */

#include "WaveformPlot.h"
#include "Colours.h"

namespace AtomSynth
{

namespace Waveform
{

WaveformPlot::WaveformPlot()
	: Component(),
	  m_xLines(8),
	  m_yLines(8),
	  m_values(std::vector<double>())
{

}

WaveformPlot::~WaveformPlot()
{
	// TODO Auto-generated destructor stub
}

void WaveformPlot::paint(Graphics& g)
{
	double w = g.getClipBounds().getWidth(), h = g.getClipBounds().getHeight();
	g.fillAll(ATOM_BG);
	g.setColour(ATOM_TEXT_BG);
	g.fillRoundedRectangle(0, 0, w, h, 4.0f);
	g.setColour(ATOM_BG);
	double xo = (w + 1.0f) / double(m_xLines + 1), yo = (h + 1.0f) / double(m_yLines + 1);
	for(int x = 0; x < m_xLines; x++)
		g.drawLine((x + 1) * xo, 0, (x + 1) * xo, h);
	for(int y = 0; y < m_yLines; y++)
		g.drawLine(0, (y + 1) * yo, w, (y + 1) * yo);
	if(m_values.size() == 0)
		return;
	xo = w / m_values.size();
	g.setColour(WHITE);
	for(int index = 1; index < m_values.size(); index++)
		g.drawLine((index - 1) * xo, (-m_values[index - 1] + 1.0f) / 2.0f * h, index * xo, (-m_values[index] + 1.0f) / 2.0f * h, 2.0f);
}

void WaveformPlot::addPoint(double value)
{
	m_values.push_back(value);
}

void WaveformPlot::clear()
{
	m_values.clear();
}

void WaveformPlot::finish()
{
	repaint();
}

} /* namespace Waveform */

} /* namespace AtomSynth */
