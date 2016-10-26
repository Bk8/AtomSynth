/*
 * BuiltinWaveformOsc.h
 *
 *  Created on: Aug 17, 2016
 *      Author: josh
 */

#ifndef SOURCE_BUILTINWAVEFORMOSC_H_
#define SOURCE_BUILTINWAVEFORMOSC_H_

#include "Atom.h"
#include "AtomGui.h"
#include "BuiltinWaveform.h"
#include "WaveformPlot.h"
#include "../JuceLibraryCode/JuceHeader.h"

#include <array>
#include <vector>
#include "Controls.h"

namespace AtomSynth
{

class SaveState;

class BuiltinWaveformOscController : public AtomController, public Button::Listener, public AtomKnob::Listener, public SimpleKnob::Listener
{
private:
	Waveform::WaveShape m_shape;
	TextButton m_sineButton, m_squareButton, m_triangleButton;
	Label m_v1v2Label, m_v3PhaseLabel, m_fmLabel, m_detuneLabel, m_semitoneOctaveLabel;
	Waveform::WaveformPlot m_plot;
	AtomKnob m_topKnob, m_bottomKnob, m_centerKnob, m_parmKnob;
	AtomKnob m_phaseKnob, m_amplitudeKnob, m_panningKnob;
	CentsKnob m_fmKnob;
	SemitonesKnob m_semitoneDetuneKnob;
	OctavesKnob m_octaveDetuneKnob;
	SimpleKnob m_unisonVoicesKnob;
	CentsKnob m_unisonDetuneKnob;
	AtomKnob m_unisonPanningKnob, m_unisonPhaseKnob;
	Label m_unisonLabel;
	std::array<AtomKnob *, 13> m_knobSet;
public:
	BuiltinWaveformOscController();
	virtual ~BuiltinWaveformOscController();

	void updateGraph();
	void updateVoices();

	void atomKnobChanged(AtomKnob * knob) { updateGraph(); }
	void buttonClicked(Button * button);
	void simpleKnobChanged(SimpleKnob * knob) { updateVoices(); }

	virtual Atom * createAtom(int index);
	virtual AtomControllerPreview getPreview();
	virtual std::string getName() { return "Waveform Oscillator"; }
	virtual int getNumAuxiliaryBuffers() { return 13; } //need 13 buffers to calculate all the animatable parameters

	virtual void loadSaveState(SaveState state);
	virtual SaveState saveSaveState();

	friend class BuiltinWaveformOscAtom;
};

AtomController * createBuiltinWaveformOscController();

class BuiltinWaveformOscAtom : public Atom
{
private:
	BuiltinWaveformOscController & m_parent;
	std::vector<std::vector<double>> m_phases;
	void updateVoices(int numVoices);
public:
	BuiltinWaveformOscAtom(BuiltinWaveformOscController & parent, int index);
	virtual ~BuiltinWaveformOscAtom();
	virtual void execute();

	friend class BuiltinWaveformOscController;
};

} /* namespace AtomSynth */

#endif /* SOURCE_BUILTINWAVEFORMOSC_H_ */
