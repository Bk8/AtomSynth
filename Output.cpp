/*
 * OutputAtom.cpp
 *
 *  Created on: Aug 17, 2016
 *      Author: josh
 */

#include "Output.h"

#include <iostream>

namespace AtomSynth
{

AtomController * createOutputAtomController()
{
	return new OutputAtomController();
}

AtomControllerPreview OutputAtomController::getPreview()
{
	AtomControllerPreview toReturn = AtomController::getPreview();
	toReturn.m_createFunction = createOutputAtomController;
	return toReturn;
}

OutputAtomController::OutputAtomController()
	: AtomController(1, 0)
{
	init();
}

OutputAtomController::~OutputAtomController()
{

}

Atom* OutputAtomController::createAtom(int index)
{
	return new OutputAtom(* this, index);
}

OutputAtom::OutputAtom(OutputAtomController & parent, int index)
	: Atom(index, 1, 0),
	  m_parent(parent)
{

}

OutputAtom::~OutputAtom()
{

}

void OutputAtom::execute()
{
	Atom::execute();
}

}

