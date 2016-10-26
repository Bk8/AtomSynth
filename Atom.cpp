/*
 * Atom.cpp
 *
 *  Created on: Jul 22, 2016
 *      Author: josh
 */

#include "Atom.h"
#include "AudioBuffer.h"
#include "Controls.h"
#include "GlobalNoteStates.h"
#include "SaveState.h"

#include <array>
#include <iostream>

namespace AtomSynth
{

void AtomController::init()
{
	for(int i = 0; i < GlobalNoteStates::getPolyphony(); i++)
	{
		m_atoms.push_back(createAtom(i));
	}
}

AtomController::AtomController(int inputs, int outputs)
	: m_inputs(std::vector<std::pair<AtomController *, int>>()),
	  m_atoms(std::vector<Atom *>()),
	  m_numInputs(inputs),
	  m_numOutputs(outputs),
	  m_x(0), m_y(0),
	  m_gui(new AtomGui())
{
	m_inputs.resize(inputs, std::pair<AtomController *, int>(nullptr, 0));
}

AtomController::~AtomController()
{
}

Atom * AtomController::createAtom(int index)
{
	return new Atom(MAX_INPUTS, MAX_OUTPUTS, index);
}

Atom * AtomController::getAtom(int index)
{
	return m_atoms[index];
}

AtomControllerPreview AtomController::getPreview()
{
	AtomControllerPreview toReturn;
	toReturn.m_name = this->getName();
	toReturn.m_inputs = m_numInputs;
	toReturn.m_outputs = m_numOutputs;
	return toReturn;
}

void AtomController::linkInput(int index, AtomController* output, int outputIndex)
{
	m_inputs[index].first = output;
	m_inputs[index].second = outputIndex;
	int atomIndex = 0;
	for(Atom * atom : m_atoms)
	{
		atom->linkInput(index, output->getAtom(atomIndex)->getOutput(outputIndex));
		atomIndex++;
	}
}

void AtomController::unlinkInput(int index)
{
	m_inputs[index].first = nullptr;
	for(Atom * atom : m_atoms)
	{
		atom->unlinkInput(index);
	}
}

void AtomController::loadSaveState(SaveState state)
{
	m_x = int(state.getValue(0));
	m_y = int(state.getValue(1));
}

SaveState AtomController::saveSaveState()
{
	SaveState state = SaveState();
	state.addValue(m_x);
	state.addValue(m_y);
	return state;
}

void AtomController::execute()
{
	//TODO: Change this so only active notes are calculated.
	for(int i = 0; i < GlobalNoteStates::getPolyphony(); i++)
	{
		if(GlobalNoteStates::getIsActive(i)) //Only bother calculating active notes
		{
			m_atoms[i]->execute();
		}
	}
}

Atom::Atom(int index, int inputs, int outputs)
	: m_sampleRate(0),
	  m_index(index),
	  m_sampleRate_f(0.0f)
{
	m_inputs.resize(inputs, nullptr);
	m_outputs.resize(outputs, AudioBuffer());
}

Atom::~Atom()
{
	// TODO Auto-generated destructor stub
}

void Atom::execute()
{
	m_sampleRate = GlobalNoteStates::s_sampleRate;
	m_sampleRate_f = double(m_sampleRate);
}

} /* namespace AtomSynth */
