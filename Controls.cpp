/*
 * AtomKnob.cpp
 *
 *  Created on: Aug 24, 2016
 *      Author: josh
 */

#include "Controls.h"

#include "Atom.h"
#include "AudioBuffer.h"
#include "Colours.h"
#include "SaveState.h"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <vector>

namespace AtomSynth
{

SimpleKnob::SimpleKnob()
	: m_listeners(std::vector<SimpleKnob::Listener *>()),
	  m_value(0.0f),
	  m_min(-10.0f),
	  m_max(10.0f),
	  m_suffix(""),
	  m_int(true),
	  m_bounded(true)
{

}

SimpleKnob::SimpleKnob(double min, double max, bool bounded, std::string suffix, bool isInt)
	: m_listeners(std::vector<SimpleKnob::Listener *>()),
	  m_value(0.0f),
	  m_min(min),
	  m_max(max),
	  m_suffix(suffix),
	  m_int(isInt),
	  m_bounded(bounded)
{

}

SimpleKnob::~SimpleKnob()
{

}

void SimpleKnob::mouseDown(const MouseEvent& event)
{
	m_prevMousePos = event.getPosition();
}

void SimpleKnob::mouseDrag(const MouseEvent& event)
{
	double delta = event.getPosition().getY() - m_prevMousePos.getY();
	delta *= (m_max - m_min) * 0.005;
	m_prevMousePos = event.getPosition();
	m_value -= delta;
	if(m_bounded)
		m_value = std::max(std::min(m_value, m_max), m_min);
	if(delta != 0)
		for(SimpleKnob::Listener * listener : m_listeners)
			listener->simpleKnobChanged(this);
	repaint();
}

void SimpleKnob::paint(Graphics& g)
{
	int w = getWidth(), h = getHeight();
	g.setColour(BACK_LAYER);
	g.fillEllipse(0, 0, w, h);
	double angle = getAngle();
	if(m_bounded) //Slightly shrink angle, so -1 and 1 do not look the same.
	{
		angle *= 0.96;
		angle -= M_PI * 0.04;
	}
	double x = (std::sin(angle) * w * 0.5f) + (w * 0.5f), y = (std::cos(angle) * h * 0.5f) + (h * 0.5f);
	g.setColour(FORE_LAYER);
	g.drawLine(x, y, w * 0.5f, h * 0.5f, 4.0f);
	g.setColour(MID_LAYER);
	if(m_bounded) //Draw a barrier between -1 and 1.
		g.drawLine(w / 2, h / 2, w / 2, h, 4.0f);
	g.fillEllipse(6.0f, 6.0f, w - 12.0f, h - 12.0f);
	y = h / 2;
	y -= 15;
	g.setColour(BACK_LAYER);
	g.setFont(17.0f);
	std::stringstream stream;
	stream << std::fixed << std::setprecision(m_int ? 0 : 2) << m_value;
	g.drawFittedText(stream.str() + m_suffix, 0, 0, w, h, Justification::centred, 1, 1.0f);
}

void SimpleKnob::loadSaveState(SaveState state)
{
	m_value = state.getValue(0);
}

SaveState SimpleKnob::saveSaveState()
{
	SaveState state = SaveState();
	state.addValue(m_value);
	return state;
}

DetuneKnob::DetuneKnob(int range, std::string suffix)
	: AtomKnob(suffix, false, true)
{
	setRange(-range, range);
}

DetuneKnob::~DetuneKnob()
{

}

CentsKnob::CentsKnob()
	: DetuneKnob(100.0f, "c")
{

}

CentsKnob::~CentsKnob()
{

}

SemitonesKnob::SemitonesKnob()
	: DetuneKnob(12.0f, "s")
{

}

SemitonesKnob::~SemitonesKnob()
{

}

OctavesKnob::OctavesKnob()
	: DetuneKnob(4.0f, "o")
{

}

OctavesKnob::~OctavesKnob()
{

}

AtomKnob::AtomKnob()
	: Component(),
	  m_min(-1.0f),
	  m_max(1.0f),
	  m_value(0.0f),
	  m_dragging(false),
	  m_editing(false),
	  m_bounded(true),
	  m_int(false),
	  m_suffix(""),
	  m_detailedEditGui(nullptr)
{
	for(int i = 0; i < 4; i++)
	{
		m_influences[i].m_inputIndex = -1;
		m_influences[i].m_minRange = 0.0f;
		m_influences[i].m_maxRange = 1.0f;
		m_influences[i].m_influencePercent = 1.0f;
	}
}

AtomKnob::AtomKnob(std::string suffix, bool bounded, int isInt)
	: Component(),
	  m_min(-1.0f),
	  m_max(1.0f),
	  m_value(0.0f),
	  m_dragging(false),
	  m_editing(false),
	  m_bounded(bounded),
	  m_int(isInt),
	  m_suffix(suffix),
	  m_detailedEditGui(nullptr)
{
	for(int i = 0; i < 4; i++)
	{
		m_influences[i].m_inputIndex = -1;
		m_influences[i].m_minRange = 0.0f;
		m_influences[i].m_maxRange = 1.0f;
		m_influences[i].m_influencePercent = 1.0f;
	}
}

AtomKnob::~AtomKnob()
{
	// TODO Auto-generated destructor stub
}

void AtomKnob::mouseDown(const MouseEvent& event)
{
	m_prevMousePos = event.getMouseDownPosition();
	m_dragging = !ModifierKeys::getCurrentModifiersRealtime().isRightButtonDown();
	if(m_detailedEditGui != nullptr)
		m_detailedEditGui->setActiveKnob(this);
}

void AtomKnob::mouseDrag(const MouseEvent& event)
{
	if(!m_dragging)
		return;
	double offset = event.getPosition().getY() - m_prevMousePos.getY();
	m_prevMousePos = event.getPosition();
	offset /= 200.0f;
	offset *= -(m_max - m_min);
	m_value += offset;
	if(m_bounded) //Clamp value by minimum and maximum.
		m_value = std::min(std::max(m_value, m_min), m_max);
	for(AtomKnob::Listener * listener : m_listeners)
		listener->atomKnobChanged(this);
	if(m_editing)
		m_detailedEditGui->repaintFooter();
	repaint();
}

void AtomKnob::paint(Graphics & g)
{
	int w = getWidth(), h = getHeight();
	g.setColour(BACK_LAYER);
	g.fillEllipse(0, 0, w, h);
	double angle = getAngle();
	if(m_bounded) //Slightly shrink angle, so -1 and 1 do not look the same.
	{
		angle *= 0.96;
		angle -= M_PI * 0.04;
	}
	double x = (std::sin(angle) * w * 0.5f) + (w * 0.5f), y = (std::cos(angle) * h * 0.5f) + (h * 0.5f);
	g.setColour(FORE_LAYER);
	g.drawLine(x, y, w * 0.5f, h * 0.5f, 4.0f);
	g.setColour(MID_LAYER);
	if(m_bounded) //Draw a barrier between -1 and 1.
		g.drawLine(w / 2, h / 2, w / 2, h, 4.0f);
	g.fillEllipse(6.0f, 6.0f, w - 12.0f, h - 12.0f);

	x = w / 2; y = h - 16;

	if(m_influences[0].m_inputIndex == -1)
		g.setColour(BACK_LAYER);
	else
		g.setColour(ATOM_COLOURS[m_influences[0].m_inputIndex]);
	g.fillEllipse(x - 16, y - 4, 7.0f, 7.0f);

	if(m_influences[1].m_inputIndex == -1)
		g.setColour(BACK_LAYER);
	else
		g.setColour(ATOM_COLOURS[m_influences[1].m_inputIndex]);
	g.fillEllipse(x - 8, y, 7.0f, 7.0f);

	if(m_influences[2].m_inputIndex == -1)
		g.setColour(BACK_LAYER);
	else
		g.setColour(ATOM_COLOURS[m_influences[2].m_inputIndex]);
	g.fillEllipse(x + 0, y, 7.0f, 7.0f);

	if(m_influences[3].m_inputIndex == -1)
		g.setColour(BACK_LAYER);
	else
		g.setColour(ATOM_COLOURS[m_influences[3].m_inputIndex]);
	g.fillEllipse(x + 8, y - 4, 7.0f, 7.0f);

	y = h / 2;
	y -= 15;
	g.setColour(BACK_LAYER);
	g.setFont(17.0f);
	std::stringstream stream;
	stream << std::fixed << std::setprecision(m_int ? 0 : 2) << m_value;
	g.drawFittedText(stream.str() + m_suffix, 0, 0, w, h, Justification::centred, 1, 1.0f);
}

void AtomKnob::loadSaveState(SaveState state)
{
	m_value = state.getValue(0);
	for(KnobInfluence & i : m_influences)
	{
		SaveState knobState = state.getNextState();
		i.m_inputIndex = int(knobState.getValue(0));
		i.m_minRange = knobState.getValue(1);
		i.m_maxRange = knobState.getValue(2);
		i.m_influencePercent = knobState.getValue(3);
	}
}

SaveState AtomKnob::saveSaveState()
{
	SaveState state = SaveState();
	state.addValue(m_value);
	for(KnobInfluence & i : m_influences)
	{
		SaveState knobState = SaveState();
		knobState.addValue(i.m_inputIndex);
		knobState.addValue(i.m_minRange);
		knobState.addValue(i.m_maxRange);
		knobState.addValue(i.m_influencePercent);
		state.addState(knobState);
	}
	return state;
}

/*
void AtomKnob::addToResult(Atom * source, int index)
{
	std::vector<std::vector<double>> & data = m_result.getData();
	KnobInfluence & inf = m_influences[index];
	AudioBuffer * input = source->getInput(inf.m_inputIndex);
	for(int c = 0; c < AudioBuffer::getChannels(); c++)
	{
		std::vector<double>::iterator inputIterator = input->getChannel(c).begin();
		std::vector<double>::iterator outputIterator = m_result.getChannel(c).begin();
		for(int i = 0; i < AudioBuffer::getSize(); i++)
		{
			(*outputIterator) += (((*inputIterator) * (inf.m_maxRange - inf.m_minRange)) + inf.m_minRange) * inf.m_influencePercent;
			inputIterator++;
			outputIterator++;
		}
	}
}

void AtomKnob::calculate(Atom* source)
{
	bool v0 = m_influences[0].m_inputIndex != -1,
		 v1 = m_influences[1].m_inputIndex != -1,
		 v2 = m_influences[2].m_inputIndex != -1,
		 v3 = m_influences[3].m_inputIndex != -1;
	if(!(v0 || v1 || v2 || v3)) //If none are active...
	{
		m_result.fill(m_value);
	}
	else
	{
		m_result.fill(0.0f);
		double divisor = 0.0;
		if(v0 && (source->getInput(m_influences[0].m_inputIndex) != nullptr))
		{
			addToResult(source, 0);
			divisor += m_influences[0].m_influencePercent;
		}
		if(v1 && (source->getInput(m_influences[1].m_inputIndex) != nullptr))
		{
			addToResult(source, 1);
			divisor += m_influences[1].m_influencePercent;
		}
		if(v2 && (source->getInput(m_influences[2].m_inputIndex) != nullptr))
		{
			addToResult(source, 2);
			divisor += m_influences[2].m_influencePercent;
		}
		if(v3 && (source->getInput(m_influences[3].m_inputIndex) != nullptr))
		{
			addToResult(source, 3);
			divisor += m_influences[3].m_influencePercent;
		}

		if(divisor == 0.0) //Basically if everything turned out to be nullptrs
		{
			m_result.fill(m_value);
		}
		else //Otherwise finish the weighted average calculation.
		{
			std::vector<std::vector<double>> & data = m_result.getData();
			if(m_bounded) //If bounded, use min and max as range.
				for(int c = 0; c < AudioBuffer::getChannels(); c++)
					for(int i = 0; i < AudioBuffer::getSize(); i++)
						data[c][i] = (((data[c][i] + 1.0f) / 2.0f) / divisor) * (m_max - m_min) + m_min;
			else //If not bounded, use the knob's value as the range.
				for(int c = 0; c < AudioBuffer::getChannels(); c++)
					for(int i = 0; i < AudioBuffer::getSize(); i++)
						data[c][i] = (data[c][i] / divisor) * m_value;

		}
	}
}
*/

double AtomKnob::getAngle()
{
	return -(((getValue() - m_min) / (m_max - m_min)) * M_PI * 2.0f);
}

BpmMultiple::BpmMultiple()
	: m_numerator(1.0f),
	  m_denominator(1.0f),
	  m_editingTop(false)
{

}

BpmMultiple::~BpmMultiple()
{

}

void BpmMultiple::mouseDown(const MouseEvent & event)
{
	m_prevMousePos = event.getPosition();
	m_editingTop = (event.getMouseDownY() <= (getHeight() / 2));
}

void BpmMultiple::mouseDrag(const MouseEvent & event)
{
	double delta = event.getPosition().getY() - m_prevMousePos.getY();
	delta /= -5.0f;
	if(m_editingTop)
		m_numerator = std::max(std::min(m_numerator + delta, 99.9), 0.0);
	else
		m_denominator = std::max(std::min(m_denominator + delta, 99.9), 1.0);
	for(Listener * listener : m_listeners)
		listener->bpmMultipleChanged(this);
	m_prevMousePos = event.getPosition();
	repaint();
}

void BpmMultiple::paint(Graphics & g)
{
	double w = getWidth(), h = getHeight() / 2;
	g.setColour(BACK_LAYER);
	g.fillRoundedRectangle(0.0f, 0.0f, w, h - 6.0f, 4.0f);
	g.fillRoundedRectangle(0.0f, h + 6.0f, w, h - 6.0f, 4.0f);
	g.setColour(FORE_LAYER);
	g.fillRoundedRectangle(1.0f, h - 4.0f, w - 2.0f, 8.0f, 4.0f);
	g.setFont(Font("Monospace", w - 10.0f, 0));
	g.drawFittedText(std::to_string(getNumerator()), 0, 0, int(w), int(h * 0.6), Justification::centred, 1, 1.0);
	g.drawFittedText(std::to_string(getDenominator()), 0, int(h) + 6, int(w), int(h * 0.6), Justification::centred, 1, 1.0);
}

void BpmMultiple::loadSaveState(SaveState state)
{
	m_numerator = state.getValue(0);
	m_denominator = state.getValue(1);
}

SaveState BpmMultiple::saveSaveState()
{
	SaveState state = SaveState();
	state.addValue(m_numerator);
	state.addValue(m_denominator);
	return state;
}

} /* namespace AtomSynth */
