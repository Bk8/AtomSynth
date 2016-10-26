/*
 * SimpleEnvelope.cpp
 *
 *  Created on: Aug 25, 2016
 *      Author: josh
 */

#include "Colours.h"
#include "GlobalNoteStates.h"
#include "SaveState.h"
#include "SimpleEnvelope.h"

#include <iostream>

namespace AtomSynth
{

SimpleEnvelopeDisplay::SimpleEnvelopeDisplay()
	: Component()
{
	setSize(390, 100);
	m_points.resize(4, Point<double>(0.0, 0.0));
}

SimpleEnvelopeDisplay::~SimpleEnvelopeDisplay()
{

}

void SimpleEnvelopeDisplay::paint(Graphics& g)
{
	g.fillAll(MID_LAYER);
	g.setColour(BACK_LAYER);
	g.fillRoundedRectangle(0.0f, 0.0f, 390.0f, 100.0f, 4.0f);
	g.setColour(FORE_LAYER);
	g.drawLine(m_points[0].getX(), m_points[0].getY(), m_points[1].getX(), m_points[1].getY(), 2.0f);
	g.drawLine(m_points[1].getX(), m_points[1].getY(), m_points[2].getX(), m_points[2].getY(), 2.0f);
	g.drawLine(m_points[2].getX(), m_points[2].getY(), m_points[3].getX(), m_points[3].getY(), 2.0f);
	g.drawLine(m_points[3].getX(), m_points[3].getY(), 390.0f, 100.0f, 2.0f);
}

SimpleEnvelopeController::SimpleEnvelopeController()
	: AtomController(MAX_INPUTS, 1)
{
	init();

	m_gui->addComponent(& m_plot);
	m_plot.setTopLeftPosition(5, 35);
	m_plot.setPoint(0, 0.0f, 0.0f);
	m_plot.setPoint(1, 0.25f, 1.0f);
	m_plot.setPoint(2, 0.5f, 1.0f);
	m_plot.setPoint(3, 0.75f, 0.75f);

	m_gui->addComponent(& m_sectionLabel);
	m_sectionLabel.setBounds(30, 140, 350, 20);
	m_sectionLabel.setText("DLAY           ATTK          HOLD           SUST          RELS", NotificationType::dontSendNotification);

	m_gui->addComponent(& m_delayTime);
	m_delayTime.setBounds(25, 160, 50, 100);
	m_delayTime.setFraction(0, 2);
	m_delayTime.addListener(this);

	m_gui->addComponent(& m_attackTime);
	m_attackTime.setBounds(100, 160, 50, 100);
	m_attackTime.setFraction(1, 2);
	m_attackTime.addListener(this);

	m_gui->addComponent(& m_holdTime);
	m_holdTime.setBounds(175, 160, 50, 100);
	m_holdTime.setFraction(1, 2);
	m_holdTime.addListener(this);

	m_gui->addComponent(& m_holdLevel);
	m_holdLevel.setBounds(168, 285, 64, 64);
	m_holdLevel.setRange(0.0f, 1.0f);
	m_holdLevel.setValue(1.0f);
	m_holdLevel.enableDetailedEditing(m_gui);
	m_holdLevel.addListener(this);

	m_gui->addComponent(& m_sustainTime);
	m_sustainTime.setBounds(250, 160, 50, 100);
	m_sustainTime.setFraction(1, 2);
	m_sustainTime.addListener(this);

	m_gui->addComponent(& m_sustainLevel);
	m_sustainLevel.setBounds(243, 285, 64, 64);
	m_sustainLevel.setRange(0.0f, 1.0f);
	m_sustainLevel.setValue(0.75f);
	m_sustainLevel.enableDetailedEditing(m_gui);
	m_sustainLevel.addListener(this);

	m_gui->addComponent(& m_releaseTime);
	m_releaseTime.setBounds(325, 160, 50, 100);
	m_releaseTime.setFraction(1, 2);
	m_releaseTime.addListener(this);

	m_gui->addComponent(& m_beatLabel);
	m_beatLabel.setBounds(25, 260, 350, 20);
	m_beatLabel.setText("BEATS        BEATS         BEATS         BEATS        BEATS", NotificationType::dontSendNotification);

	m_knobSet[0] = & m_holdLevel;
	m_knobSet[1] = & m_sustainLevel;
}

SimpleEnvelopeController::~SimpleEnvelopeController()
{
	// TODO Auto-generated destructor stub
}

void SimpleEnvelopeController::loadSaveState(SaveState state)
{
	AtomController::loadSaveState(state.getNextState());
	state = state.getNextState();

	m_delayTime.loadSaveState(state.getNextState());
	m_attackTime.loadSaveState(state.getNextState());
	m_holdTime.loadSaveState(state.getNextState());
	m_sustainTime.loadSaveState(state.getNextState());
	m_releaseTime.loadSaveState(state.getNextState());

	m_holdLevel.loadSaveState(state.getNextState());
	m_sustainLevel.loadSaveState(state.getNextState());
}

SaveState SimpleEnvelopeController::saveSaveState()
{
	SaveState toReturn = SaveState();
	SaveState basicState = AtomController::saveSaveState();
	toReturn.addState(basicState);
	SaveState state = SaveState();

	state.addState(m_delayTime.saveSaveState());
	state.addState(m_attackTime.saveSaveState());
	state.addState(m_holdTime.saveSaveState());
	state.addState(m_sustainTime.saveSaveState());
	state.addState(m_releaseTime.saveSaveState());

	state.addState(m_holdLevel.saveSaveState());
	state.addState(m_sustainLevel.saveSaveState());

	toReturn.addState(state);
	return toReturn;
}

void SimpleEnvelopeController::updatePlot()
{
	double p1 = m_delayTime.getSeconds(), p2 = p1 + m_attackTime.getSeconds(),
			p3 = p2 + m_holdTime.getSeconds(), p4 = p3 + m_sustainTime.getSeconds();
	double total = p4 + m_releaseTime.getSeconds();
	p1 /= total; p2 /= total; p3 /= total; p4 /= total;
	m_plot.setPoint(0, p1, 0.0f);
	m_plot.setPoint(1, p2, m_holdLevel.getValue());
	m_plot.setPoint(2, p3, m_holdLevel.getValue());
	m_plot.setPoint(3, p4, m_sustainLevel.getValue());
	m_plot.repaint();
}

Atom * AtomSynth::SimpleEnvelopeController::createAtom(int index)
{
	return new SimpleEnvelopeAtom(* this, index);
}

AtomController * createSimpleEnvelopeController()
{
	return new SimpleEnvelopeController();
}

AtomControllerPreview SimpleEnvelopeController::getPreview()
{
	AtomControllerPreview toReturn = AtomController::getPreview();
	toReturn.m_createFunction = createSimpleEnvelopeController;
	return toReturn;
}

AtomSynth::SimpleEnvelopeAtom::SimpleEnvelopeAtom(SimpleEnvelopeController& parent, int index)
	: Atom(index, MAX_INPUTS, 1),
	  m_parent(parent),
	  m_releaseStart(0.0f)
{
	m_releaseLevel.resize(AudioBuffer::getChannels(), 0.0f);
	m_releasing.resize(AudioBuffer::getChannels(), false);
}

AtomSynth::SimpleEnvelopeAtom::~SimpleEnvelopeAtom()
{

}

void AtomSynth::SimpleEnvelopeAtom::execute()
{
	if(m_inputs[0] != nullptr)
	{
		double delayTime = m_parent.m_delayTime.getSeconds();
		double attackTime = m_parent.m_attackTime.getSeconds();
		double holdTime = m_parent.m_holdTime.getSeconds();
		double sustainTime = m_parent.m_sustainTime.getSeconds();
		double releaseTime = m_parent.m_releaseTime.getSeconds();

		AtomKnob::calculateKnobs<2>(m_parent.m_knobSet, * this);

		std::vector<double>::iterator holdIterator = m_parent.m_holdLevel.getResult().getData().begin();
		std::vector<double>::iterator sustainIterator = m_parent.m_sustainLevel.getResult().getData().begin();

		double time = 0.0f, ltime = 0.0f;
		bool triggerRelease = false;
		for(int c = 0; c < AudioBuffer::getChannels(); c++)
		{
			for(int s = 0; s < AudioBuffer::getSize(); s++)
			{
				time = m_inputs[0]->getValue(c, s);
				triggerRelease = (time < 0.0); //Negative time = key not being pressed, waiting for note to finish.
				time = fabs(time);
				if((!triggerRelease) && m_releasing[c])
					m_releasing[c] = false;
				if(m_releasing[c])
				{
					if((time - m_releaseStart) < releaseTime)
					{
						ltime = time - m_releaseStart;
						ltime /= releaseTime / 2.0;
						ltime = 2.0 - ltime; //go from 0 - 2 to 2 - 0
						m_outputs[0].setValue(c, s, ltime * m_releaseLevel[c] - 1.0);
					}
					else
					{
						m_outputs[0].setValue(c, s, -1.0);
					}
				}
				else
				{
					if(time < delayTime)
					{
						m_outputs[0].setValue(c, s, -1.0);
					}
					else if(time < delayTime + attackTime)
					{
						ltime = time - delayTime; //Time window between 0s and [attack length] seconds
						ltime /= attackTime / 2.0; //Between 0 and 2
						m_outputs[0].setValue(c, s, ltime * (* holdIterator) - 1.0);
					}
					else if(time < delayTime + attackTime + holdTime)
					{
						m_outputs[0].setValue(c, s, (* holdIterator) * 2.0 - 1.0);
					}
					else if(time < delayTime + attackTime + holdTime + sustainTime)
					{
						ltime = time - (delayTime + attackTime + holdTime); //Time window between 0s and [sustain length] seconds
						ltime /= sustainTime / 2.0; //Between 0 and 2
						ltime = 2.0 - ltime;
						ltime *= ((* holdIterator) - (* sustainIterator));
						ltime += (* sustainIterator) * 2.0;
						m_outputs[0].setValue(c, s, ltime - 1.0);
					}
					else
					{
						m_outputs[0].setValue(c, s, (* sustainIterator) * 2.0 - 1.0);
					}

					if(triggerRelease)
					{
						m_releasing[c] = true;
						m_releaseStart = time;
						m_releaseLevel[c] = m_outputs[0].getValue(c, s);
					}
				}
			}
		}
	}
	else
	{
		m_outputs[0].fill(-1.0);
	}
}

} /* namespace AtomSynth */

