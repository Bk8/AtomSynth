/*
 * InputAtom.h
 *
 *  Created on: Aug 17, 2016
 *      Author: josh
 */

#ifndef SOURCE_INPUT_H_
#define SOURCE_INPUT_H_

#include "Atom.h"
#include "AtomGui.h"

namespace AtomSynth
{

class InputAtomController : public AtomController
{
public:
	InputAtomController();
	virtual ~InputAtomController();
	virtual Atom * createAtom(int index);
	virtual AtomControllerPreview getPreview();
	virtual std::string getName() { return "Note Input"; }
};

class InputAtom: public Atom
{
private:
	InputAtomController & m_parent;
public:
	InputAtom(InputAtomController & parent, int index);
	virtual ~InputAtom();
	virtual void execute();
};

} /* namespace AtomSynth */

#endif /* SOURCE_INPUT_H_ */
