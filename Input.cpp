/*
 * InputAtom.cpp
 *
 *  Created on: Aug 17, 2016
 *      Author: josh
 */

#include <iostream>

#include "AudioBuffer.h"
#include "GlobalNoteStates.h"
#include "Input.h"

namespace AtomSynth
{

AtomController * createInputAtomController()
{
	return new InputAtomController();
}

AtomControllerPreview InputAtomController::getPreview()
{
	AtomControllerPreview toReturn = AtomController::getPreview();
	toReturn.m_createFunction = createInputAtomController;
	return toReturn;
}

InputAtomController::InputAtomController()
	: AtomController(0, 3)
{
	init();
}

InputAtomController::~InputAtomController()
{

}

Atom * InputAtomController::createAtom(int index)
{
	return new InputAtom(* this, index); //Increment afterwards, hopefully.
}

InputAtom::InputAtom(InputAtomController & parent, int index)
	: Atom(index, 0, 3),
	  m_parent(parent)
{

}

InputAtom::~InputAtom()
{

}

void InputAtom::execute()
{
	Atom::execute();
	//m_outputs[0].fill(GlobalNoteStates::getNoteState(m_index).frequency);
	int multiplier = 0;
	switch(GlobalNoteStates::getNoteState(m_index).status)
	{
	case NoteState::ACTIVE:
		multiplier = 1;
		break;
	case NoteState::RELEASING:
		multiplier = -1;
		break;
	case NoteState::SILENT:
		multiplier = 0;
	}

	unsigned long int base = GlobalNoteStates::s_currentTimestamp - GlobalNoteStates::getNoteState(m_index).timestamp;
	double time, frequency = GlobalNoteStates::getNoteState(m_index).frequency;
	for(int c = 0; c < AudioBuffer::getChannels(); c++)
	{
		for(int s = 0; s < AudioBuffer::getSize(); s++)
		{
			time = (double(s + base) / m_sampleRate_f) * multiplier;
			m_outputs[0].setValue(c, s, frequency);
			m_outputs[1].setValue(c, s, time);
		}
	}
}


} /* namespace AtomSynth */
