/*
 * SimpleEnvelope.h
 *
 *  Created on: Aug 25, 2016
 *      Author: josh
 */

#ifndef SOURCE_SIMPLEENVELOPE_H_
#define SOURCE_SIMPLEENVELOPE_H_

#include "../JuceLibraryCode/JuceHeader.h"
#include "Atom.h"
#include "Controls.h"
#include "WaveformPlot.h"

#include <array>

namespace AtomSynth
{

class SimpleEnvelopeDisplay : public Component
{
private:
	std::vector<Point<double>> m_points;
public:
	SimpleEnvelopeDisplay();
	virtual ~SimpleEnvelopeDisplay();
	virtual void paint(Graphics & g);
	void setPoint(int index, double x, double y) { m_points[index] = Point<double>(x * 390.0f, (1.0f - y) * 100.0f); }
};

class SimpleEnvelopeController : public AtomController, public BpmMultiple::Listener, public AtomKnob::Listener
{
private:
	SimpleEnvelopeDisplay m_plot;
	Label m_sectionLabel, m_beatLabel;
	BpmMultiple m_delayTime, m_attackTime, m_holdTime, m_sustainTime, m_releaseTime;
	AtomKnob m_holdLevel, m_sustainLevel;
	std::array<AtomKnob *, 2> m_knobSet;
public:
	SimpleEnvelopeController();
	virtual ~SimpleEnvelopeController();

	void updatePlot();

	virtual void bpmMultipleChanged(BpmMultiple * bpmMultiple) { updatePlot(); }
	virtual void atomKnobChanged(AtomKnob * knob) { updatePlot(); }

	virtual Atom * createAtom(int index);
	virtual AtomControllerPreview getPreview();
	virtual std::string getName() { return "Simple Envelope"; }

	virtual void loadSaveState(SaveState state);
	virtual SaveState saveSaveState();

	friend class SimpleEnvelopeAtom;
};

class SimpleEnvelopeAtom : public Atom
{
private:
	SimpleEnvelopeController & m_parent;
	std::vector<double> m_releaseLevel;
	std::vector<bool> m_releasing;
	double m_releaseStart;
public:
	SimpleEnvelopeAtom(SimpleEnvelopeController & parent, int index);
	virtual ~SimpleEnvelopeAtom();
	virtual void execute();
};

} /* namespace AtomSynth */

#endif /* SOURCE_SIMPLEENVELOPE_H_ */
