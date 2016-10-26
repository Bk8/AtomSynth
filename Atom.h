/*
 * Atom.h
 *
 *  Created on: Jul 22, 2016
 *      Author: josh
 */

#ifndef SOURCE_ATOM_H_
#define SOURCE_ATOM_H_

#include "AtomGui.h"
#include "AudioBuffer.h"
#include "GlobalNoteStates.h"

#include <iostream>
#include <functional>
#include <string>
#include <vector>

#define ATOM_TIME ((double(sample) / m_sampleRate_f) + m_time)

namespace AtomSynth
{

class Atom;
class AtomController;
class AtomKnob;
class SaveState;

struct AtomControllerPreview
{
	std::function<AtomController * ()> m_createFunction;
	std::string m_name;
	int m_inputs, m_outputs;
};

class AtomController
{
private:
	std::vector<std::pair<AtomController *, int>> m_inputs;
	std::vector<int> m_inputIndexes;
	std::vector<Atom *> m_atoms;
	int m_numInputs, m_numOutputs;
	int m_x, m_y;
protected:
	AtomGui * m_gui;
	void init();
	std::vector<Atom *> & getAtoms() { return m_atoms; }
public:
	AtomController(int inputs, int outputs);
	virtual ~AtomController();

	virtual Atom * createAtom(int index);
	virtual Atom * getAtom(int index);

	virtual void loadSaveState(SaveState state);
	virtual SaveState saveSaveState();

	virtual AtomControllerPreview getPreview();
	virtual std::string getName() { return ""; }

	void linkInput(int index, AtomController * output, int outputIndex);
	void unlinkInput(int index);
	int getNumInputs() { return m_numInputs; }
	int getNumOutputs() { return m_numOutputs; }
	std::pair<AtomController *, int> getInput(int index) { return m_inputs[index]; }
	std::vector<std::pair<AtomController *, int> > getInputs() { return m_inputs; }

	void execute();

	void setPosition(int x, int y) { m_x = x; m_y = y; }
	AtomGui * getGui() { return m_gui; }
	int getX() { return m_x; }
	int getY() { return m_y; }

	friend class Atom;
};

#define MAX_INPUTS 12
#define MAX_OUTPUTS MAX_INPUTS

class Atom
{
protected:
	int m_sampleRate, m_index;
	double m_sampleRate_f;
	std::vector<AudioBuffer *> m_inputs;
	std::vector<AudioBuffer> m_outputs;
public:
	Atom(int index, int inputs, int outputs);
	virtual ~Atom();

	virtual void execute();

	void linkInput(int index, AudioBuffer * output) { m_inputs[index] = output; }
	void unlinkInput(int index) { m_inputs[index] = nullptr; }
	AudioBuffer * getOutput(int index) { return & m_outputs[index]; }

	friend class AtomKnob;

};

/*template<std::size_t N>
void Atom::calculateKnobs(std::array<AtomKnob *, N> knobs, MultiBuffer & buffer) //Because for some reason template functions need to be in the header.
{
	std::array<double, N> baseValues;
	std::array<bool, N> shouldCalculate;
	std::array<std::array<double, 4>, N> multiply;
	std::array<std::array<double, 4>, N> add;
	std::array<std::array<int, 4>, N> sources;

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
				sources[i][infIndex] = m_parent.m_inputIndexes[inf.m_inputIndex];
				shouldCalculate[i] = true;
			}
			else
			{
				multiply[i][infIndex] = 0.0;
				add[i][infIndex] = 0.0;
				sources[i][infIndex] = 0;
			}
			infIndex++;
		}
		if(shouldCalculate[i])
		{
			calculateIndexes.push_back(i);
		}
		else
		{
			basicIndexes.push_back(i);
		}
	}

	buffer.resetIterator();
	double value;
	int offset = m_parent.m_auxiliaryBufferOffset;
	for(int i = 0; i < AudioBuffer::getChannels() * AudioBuffer::getSize(); i++)
	{
		for(int n : calculateIndexes)
		{
			value = 0.0;
			for(int s = 0; s < 4; s++)
			{
				//Values come in in range -1.0 to 1.0
				//Highly simplified version of converting from -1.0 - 1.0 to 0.0 - 1.0 to min - max to knob top - knob bottom
				//Original equation is ((((sample + 1) / 2) * (max - min) + min) * (hi - low) + low ) * percent
				//Algebra is really handy
				value += buffer.getValue(sources[n][s]) * multiply[n][s] + add[n][s];
			}
			buffer.setValue(n + m_parent.m_auxiliaryBufferOffset, value);
		}
		for(int n : basicIndexes)
		{
			buffer.setValue(n + offset, baseValues[n]);
		}
		buffer.incrementIterator();
	}
}*/

} /* namespace AtomSynth */

#endif /* SOURCE_ATOM_H_ */
