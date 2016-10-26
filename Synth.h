/*
 * Synth.h
 *
 *  Created on: Sep 30, 2016
 *      Author: josh
 */

#ifndef SOURCE_SYNTH_H_
#define SOURCE_SYNTH_H_

#include <vector>
#include "Atom.h"
#include "AudioBuffer.h"

namespace AtomSynth
{

class Atom;
class AtomController;
class AtomControllerPreview;
class AudioBuffer;
class SaveState;

class Synth
{
private:
	std::vector<AtomController *> m_atoms;
	std::vector<int> m_atomExecutionOrder;
	std::vector<AtomControllerPreview> m_previews;
	bool m_atomsReloaded;
	AtomSynth::AudioBuffer m_output;
public:
	static Synth * s_instance;

	Synth();
	virtual ~Synth();

	void setup();

	AudioBuffer execute();
	void updateExecutionOrder();

	void setAtomsReloaded(bool reloaded = true) { m_atomsReloaded = reloaded; }
	bool getAtomsReloaded() { return m_atomsReloaded; }

	void addAtom(AtomController * atom);
	std::vector<AtomController *> & getAtoms() { return m_atoms; }
	std::vector<AtomControllerPreview> & getPreviews() { return m_previews; }

	void loadSaveState(SaveState state);
	SaveState saveSaveState();
};

} /* namespace AtomSynth */

#endif /* SOURCE_SYNTH_H_ */
