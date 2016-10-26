/*
 * AtomGui.cpp
 *
 *  Created on: Aug 15, 2016
 *      Author: josh
 */

#include "AtomGui.h"
#include "Colours.h"
#include "Controls.h"

namespace AtomSynth
{

KnobFooter::KnobFooter()
	: Component(),
	  m_currentKnob(nullptr),
	  m_sliderStatus(SLIDER_NOT_SLIDING),
	  m_index(0)
{

}

KnobFooter::~KnobFooter()
{

}

#define INPUT_INCREMENT double(ATOM_GUI_X - 112) / 12.0f
void KnobFooter::mouseDown(const MouseEvent& event)
{
	if(m_currentKnob == nullptr)
		return;
	bool rightClick = ModifierKeys::getCurrentModifiersRealtime().isRightButtonDown();
	int x = event.getPosition().getX(), y = event.getPosition().getY();
	if((x >= 86) && (x <= ATOM_GUI_X - 26) && (y >= 2))
	{
		y -= 2;
		if((y % 20) > 17)
			return;
		y /= 20;
		KnobInfluence inf = m_currentKnob->getInfluence(y);
		if(inf.m_inputIndex == -1)
		{
			x -= 86 + 9;
			x /= INPUT_INCREMENT;
			x = std::min(x, 11);
			if(m_currentKnob != nullptr)
				m_currentKnob->getInfluence(y).m_inputIndex = x;
			m_currentKnob->repaint();
			repaint();
		}
		else
		{
			if(rightClick)
			{
				m_currentKnob->getInfluence(y).m_inputIndex = -1;
				m_currentKnob->repaint();
				repaint();
			}
			else
			{
				x -= 86;
				if(x < (inf.m_minRange * (ATOM_GUI_X - 94) + 18))
				{
					m_sliderStatus = SLIDER_MIN;
				}
				else if(x > (inf.m_maxRange * (ATOM_GUI_X - 94) - 18.0f))
				{
					m_sliderStatus = SLIDER_MAX;
				}
				else
				{
					m_sliderStatus = SLIDER_MOVE;
				}
				m_previousLocation = event.getPosition();
				m_index = y;
			}
		}
	}
}

void KnobFooter::paint(Graphics & g)
{
	g.fillAll(MID_LAYER);
	if(m_currentKnob == nullptr)
		return;
	g.setColour(BACK_LAYER);
	g.fillEllipse(10, 10, 64, 64);
	g.fillRoundedRectangle(84, 0, ATOM_GUI_X - 90, 82, 9);
	double angle = m_currentKnob->getAngle();
	double x = std::sin(angle) * 32.0f + 42.0f, y = std::cos(angle) * 32.0f + 42.0f;
	g.setColour(FORE_LAYER);
	g.drawLine(x, y, 42.0f, 42.0f, 4.0f);
	g.setColour(MID_LAYER);
	g.fillEllipse(16.0f, 16.0f, 52.0f, 52.0f);

	g.setColour(MID_LAYER);
	double increment = INPUT_INCREMENT;
	for(int i = 0; i < 4; i++)
	{
		KnobInfluence inf = m_currentKnob->getInfluence(i);
		if(inf.m_inputIndex == -1)
		{
			for(int c = 0; c < 12; c++)
			{
				g.setColour(ATOM_COLOURS[c]);
				g.fillRect(95.0f + (increment * c), 2.0f + (i * 20.0f), increment, 18.0f);
			}
			g.setColour(ATOM_COLOURS[0]);
			g.fillEllipse(86.0f, 2.0f + (i * 20.0f), 18.0f, 18.0f);
			g.setColour(ATOM_COLOURS[11]);
			g.fillEllipse(ATOM_GUI_X - 26.0f, 2.0f + (i * 20.0f), 18.0f, 18.0f);
		}
		else
		{
			g.setColour(MID_LAYER);
			g.fillRoundedRectangle(86.0f, 2.0f + (i * 20.0f), ATOM_GUI_X - 94, 18.0f, 9.0f);
			g.setColour(ATOM_COLOURS[inf.m_inputIndex]);
			double low = inf.m_minRange * (ATOM_GUI_X - 94) + 86.0f, high = inf.m_maxRange * (ATOM_GUI_X - 94) + 86.0f;
			g.fillRoundedRectangle(low, 2.0f + (i * 20.0f), high - low, 18.0f, 9.0f);
			g.setColour(FORE_LAYER);
			if(inf.m_inputIndex == 0)
				g.setColour(ATOM_COLOURS[1]);
			g.fillEllipse(low + 2.0f, (i * 20.0f) + 4.0f, 14.0f, 14.0f);
			g.fillEllipse(high - 16.0f, (i * 20.0f) + 4.0f, 14.0f, 14.0f);
		}
	}
}

void KnobFooter::mouseDrag(const MouseEvent& event)
{
	double dx = event.getPosition().getX() - m_previousLocation.getX();
	dx /= double(ATOM_GUI_X - 112);
	m_previousLocation = event.getPosition();
	KnobInfluence & inf = m_currentKnob->getInfluence(m_index);
	switch(m_sliderStatus)
	{
	case SLIDER_MIN:
		inf.m_minRange += dx;
		break;
	case SLIDER_MAX:
		inf.m_maxRange += dx;
		break;
	case SLIDER_MOVE:
		inf.m_minRange += dx;
		inf.m_maxRange += dx;
		break;
	default:
		break;
	}
	inf.m_minRange = std::max(inf.m_minRange, 0.0);
	inf.m_maxRange = std::min(inf.m_maxRange, 1.0);
	repaint();
}

void KnobFooter::setActiveKnob(AtomKnob* knob)
{
	if(m_currentKnob != nullptr)
		m_currentKnob->setEditing(false);
	m_currentKnob = knob;
	knob->setEditing(true);
	repaint();
}

AtomGui::AtomGui()
	: m_shown(false)
{
	m_root.setBounds(0, 0, ATOM_GUI_X, ATOM_GUI_Y);
	m_root.setVisible(m_shown);
	m_root.addAndMakeVisible(m_footer);
	m_footer.setBounds(0, ATOM_GUI_Y - 84, ATOM_GUI_X, 84);
}

AtomGui::~AtomGui()
{
	// TODO Auto-generated destructor stub
}

void AtomGui::addComponent(Component * toAdd)
{
	m_root.addAndMakeVisible(toAdd);
}

void AtomGui::setShown(bool shown)
{
	if(shown != m_shown) 
		m_root.setVisible(shown);
	m_shown = shown;
}

void AtomGui::setParent(Component* parent)
{
	parent->addAndMakeVisible(getRoot());
	if(!m_shown) m_root.setVisible(false); //There is no 'add and don't make visible'
}

Component * AtomGui::getRoot()
{
	return & m_root;
}

} /*namespace AtomSynth*/

