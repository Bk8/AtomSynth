/*
 * OutputAtom.hpp
 *
 *  Created on: Aug 17, 2016
 *      Author: josh
 */

#ifndef SOURCE_OUTPUT_H_
#define SOURCE_OUTPUT_H_

#include "Atom.h"
#include "AtomGui.h"

namespace AtomSynth
{

#define OUTPUT_IO 1, 1

class OutputAtomController : public AtomController
{
public:
	OutputAtomController();
	virtual ~OutputAtomController();
	virtual Atom * createAtom(int index);
	virtual AtomControllerPreview getPreview();
	std::string getName() { return "Output"; }
};

class OutputAtom : public Atom
{
private:
	OutputAtomController & m_parent;
public:
	OutputAtom(OutputAtomController & parent, int index);
	virtual ~OutputAtom();
	virtual void execute();
};

} /* namespace AtomSynth */

#endif /* SOURCE_OUTPUT_H_ */
