/*
 * AtomKnob.h
 *
 *  Created on: Aug 24, 2016
 *      Author: josh
 */

#ifndef SOURCE_CONTROLS_H_
#define SOURCE_CONTROLS_H_

#include <array>
#include <functional>
#include <string>

#include "../JuceLibraryCode/JuceHeader.h"
#include "Atom.h"
#include "AtomGui.h"
#include "AudioBuffer.h"
#include "GlobalNoteStates.h"

#define M_PI 3.141592653589793236

namespace AtomSynth
{

class SaveState;

#define CENTS_INTERVAL 1.00057778951f //1200th root of 2
#define SEMITONES_INTERVAL 1.05946309436f //12th root of 2
#define OCTAVES_INTERVAL 2 //1th root of 2

class SimpleKnob : public juce::Component
{
public:
	class JUCE_API Listener
	{
	public:
		virtual ~Listener() { };
		virtual void simpleKnobChanged(SimpleKnob * knob) = 0;
	};
private:
	Point<int> m_prevMousePos;
	std::vector<Listener *> m_listeners;
protected:
	double m_value, m_min, m_max;
	std::string m_suffix;
	bool m_int, m_bounded;
public:
	SimpleKnob();
	SimpleKnob(double min, double max, bool bounded = false, std::string suffix = "", bool isInt = true);
	virtual ~SimpleKnob();

	virtual void paint(Graphics & g);

	virtual void mouseDown(const MouseEvent & event);
	virtual void mouseDrag(const MouseEvent & event);
	void addListener(Listener * listener) { m_listeners.push_back(listener); }

	void setRange(double min, double max) { m_min = min; m_max = max; }
	void setValue(double value) { m_value = value; }
	void setBounded(bool bounded) { m_bounded = bounded; }
	void setInt(bool isInt) { m_int = isInt; }
	void setSuffix(std::string suffix) { m_suffix = suffix; }

	double getValue() { return (m_int) ? round(m_value) : m_value; }
	double getAngle() { return -(((getValue() - m_min) / (m_max - m_min)) * M_PI * 2.0f); }

	void loadSaveState(SaveState state);
	SaveState saveSaveState();
};

class AtomGui;
class GlobalNoteStates;

struct KnobInfluence
{
	int m_inputIndex;
	double m_minRange, m_maxRange, m_influencePercent;
};

class AtomKnob : public juce::Component
{
public:
	class JUCE_API Listener
	{
	public:
		virtual ~Listener() { };
		virtual void atomKnobChanged(AtomKnob * knob) = 0;
	};
private:
	double m_min, m_max, m_value;
	Point<int> m_prevMousePos;
	std::vector<Listener *> m_listeners;
	KnobInfluence m_influences[4];
	bool m_dragging, m_editing, m_bounded, m_int;
	std::string m_suffix;
	AtomGui * m_detailedEditGui;
	AudioBuffer m_result;
public:
	AtomKnob();
	AtomKnob(std::string suffix, bool bounded = false, int isInt = false);
	virtual ~AtomKnob();

	virtual void paint(Graphics & g);

	virtual void mouseDown(const MouseEvent & event);
	virtual void mouseDrag(const MouseEvent & event);
	void addListener(Listener * listener) { m_listeners.push_back(listener); }

	void setRange(double min, double max) { m_min = min; m_max = max; }
	void setValue(double value) { m_value = value; repaint(); }
	void setEditing(bool editing) { m_editing = editing; }
	void enableDetailedEditing(AtomGui * parent) { m_detailedEditGui = parent; }

	KnobInfluence & getInfluence(int index) { return m_influences[index]; }
	double getAngle();
	double getValue() { return m_int ? round(m_value) : m_value; }

	void loadSaveState(SaveState state);
	SaveState saveSaveState();

	template<int N>
	static void calculateKnobs(std::array<AtomKnob *, N> & knobs, Atom & source);
	AudioBuffer & getResult() { return m_result; }

	friend class Atom;
};

template<int N>
void AtomKnob::calculateKnobs(std::array<AtomKnob *, N> & knobs, Atom & source)
{
	std::array<double, N> baseValues;
	std::array<bool, N> shouldCalculate;
	std::array<std::array<double, 4>, N> multiply;
	std::array<std::array<double, 4>, N> add;
	std::array<std::array<AudioBuffer *, 4>, N> sources;

	std::vector<int> calculateIndexes;
	std::vector<int> basicIndexes;

	bool calculate;
	int infIndex = 0;
	double divisor = 0.0, kmax, kmin, imax, imin;
	for(int i = 0; i < N; i++)
	{
		shouldCalculate[i] = false;
		baseValues[i] = knobs[i]->getValue();
		infIndex = 0;
		divisor = 0.0;
		for(KnobInfluence inf : knobs[i]->m_influences)
		{
			if(inf.m_inputIndex != -1)
			{
				divisor += inf.m_influencePercent;
			}
		}
		if(divisor != 0.0)
		{
			divisor = 1.0 / divisor;
		}

		kmax = knobs[i]->m_max;
		kmin = knobs[i]->m_min;
		for(KnobInfluence inf : knobs[i]->m_influences)
		{
			//Simplifies the computation, more detail below.
			imax = inf.m_maxRange;
			imin = inf.m_minRange;
			multiply[i][infIndex] = (((kmax * (imax - imin)) + (kmin * (imin - imax))) / 2.0) * inf.m_influencePercent * divisor;
			add[i][infIndex] = (((kmax - kmin) * (imax + imin) / 2.0) + kmin) * inf.m_influencePercent * divisor;
			if(inf.m_inputIndex != -1)
			{
				sources[i][infIndex] = source.m_inputs[inf.m_inputIndex];
				shouldCalculate[i] = true;
			}
			else
			{
				multiply[i][infIndex] = 0.0;
				add[i][infIndex] = 0.0;
				sources[i][infIndex] = nullptr;
			}
			infIndex++;
		}
		if(shouldCalculate[i])
		{
			calculateIndexes.push_back(i);
			knobs[i]->m_result.fill(0.0);
			knobs[i]->m_result.setConstant(false);
		}
		else
		{
			basicIndexes.push_back(i);
			knobs[i]->m_result.fill(baseValues[i]);
			knobs[i]->m_result.setConstant(true);
		}
	}

	double value;
	std::vector<double>::iterator input, output;
	double mValue, aValue;
	for(int n : calculateIndexes)
	{
		output = knobs[n]->m_result.getData().begin();
		for(int s = 0; s < 4; s++)
		{
			if(sources[n][s] != nullptr)
			{
				input = sources[n][s]->getData().begin();
				mValue = multiply[n][s];
				aValue = add[n][s];
				for(int i = 0; i < AudioBuffer::getChannels() * AudioBuffer::getSize(); i++)
				{
					//Values come in in range -1.0 to 1.0
					//Highly simplified version of converting from -1.0 - 1.0 to 0.0 - 1.0 to min - max to knob top - knob bottom
					//Original equation is ((((sample + 1) / 2) * (max - min) + min) * (hi - low) + low ) * percent
					//Algebra is really handy
					(* output) += (* input) * mValue + aValue;
					input++;
					output++;
				}
			}
		}
	}
} //Because template functions must be in the header...

class BpmMultiple : public Component
{
public:
	class JUCE_API Listener
	{
	public:
		virtual ~Listener() {};
		virtual void bpmMultipleChanged(BpmMultiple * bpmMultiple) = 0;
	};
private:
	double m_numerator, m_denominator;
	bool m_editingTop;
	std::vector<Listener *> m_listeners;
	Point<int> m_prevMousePos;
public:
	BpmMultiple();
	virtual ~BpmMultiple();

	virtual void paint(Graphics & g);

	virtual void mouseDown(const MouseEvent & event);
	virtual void mouseDrag(const MouseEvent & event);
	void addListener(Listener * listener) { m_listeners.push_back(listener); }

	void setNumerator(int numerator) { m_numerator = numerator; }
	void setDenominator(int denominator) { m_denominator = denominator; }
	void setFraction(int numerator, int denominator) { m_numerator = numerator, m_denominator = denominator; }

	int getNumerator() { return floor(m_numerator); }
	int getDenominator() { return floor(m_denominator); }
	double getFraction() { return floor(m_numerator) / floor(m_denominator); }
	double getSeconds() { return getFraction() * (60.0f / GlobalNoteStates::s_bpm); } //How long it would take for 1 cycle to elapse.
	double getHertz() { return 1.0f / getSeconds(); }

	void loadSaveState(SaveState state);
	SaveState saveSaveState();
};

class DetuneKnob : public AtomKnob
{
public:
	DetuneKnob(int range, std::string suffix);
	virtual ~DetuneKnob();
	virtual double detune(double hz) = 0;
};

class CentsKnob : public DetuneKnob
{
public:
	CentsKnob();
	virtual ~CentsKnob();
	virtual double detune(double hz) { return hz * pow(CENTS_INTERVAL, getValue()); }
	static double detune(double hz, double cents) { return hz * pow(CENTS_INTERVAL, cents); }
};

class SemitonesKnob : public DetuneKnob
{
public:
	SemitonesKnob();
	virtual ~SemitonesKnob();
	virtual double detune(double hz) { return hz * pow(SEMITONES_INTERVAL, getValue()); }
	static double detune(double hz, double semitones) { return hz * pow(SEMITONES_INTERVAL, semitones); }
};

class OctavesKnob : public DetuneKnob
{
public:
	OctavesKnob();
	virtual ~OctavesKnob();
	virtual double detune(double hz) { return hz * pow(OCTAVES_INTERVAL, getValue()); }
	static double detune(double hz, double octaves) { return hz * pow(OCTAVES_INTERVAL, octaves); }
};

} /* namespace AtomSynth */

#endif /* SOURCE_CONTROLS_H_ */
