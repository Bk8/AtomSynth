/*
 * Synth.cpp
 *
 *  Created on: Sep 30, 2016
 *      Author: josh
 */

#include "Atom.h"
#include "AudioBuffer.h"
#include "BuiltinWaveformOsc.h"
#include "GlobalNoteStates.h"
#include "Input.h"
#include "Output.h"
#include "SaveState.h"
#include "SimpleEnvelope.h"
#include "Synth.h"

namespace AtomSynth
{

Synth * Synth::s_instance = nullptr;

Synth::Synth()
	: m_atomsReloaded(false),
	  m_output(AtomSynth::AudioBuffer())
{
	// TODO Auto-generated constructor stub
	s_instance = this;
}

Synth::~Synth()
{
	// TODO Auto-generated destructor stub
}

void Synth::setup()
{
	m_atoms.clear();
	/*m_atoms.push_back(new AtomSynth::InputAtomController());
	m_atoms.push_back(new AtomSynth::SimpleEnvelopeController());
	m_atoms.push_back(new AtomSynth::BuiltinWaveformOscController());
	m_atoms.push_back(new AtomSynth::OutputAtomController());
	m_atoms[1]->linkInput(0, m_atoms[0], 1);
	m_atoms[2]->linkInput(0, m_atoms[0], 0);
	m_atoms[2]->linkInput(2, m_atoms[1], 0);
	m_atoms[3]->linkInput(0, m_atoms[2], 0);
	m_atoms[0]->setPosition(0, 0);
	m_atoms[1]->setPosition(0, 100);
	m_atoms[2]->setPosition(0, 200);
	m_atoms[3]->setPosition(0, 300);*/
	m_previews.push_back(AtomSynth::InputAtomController().getPreview());
	m_previews.push_back(AtomSynth::SimpleEnvelopeController().getPreview());
	m_previews.push_back(AtomSynth::BuiltinWaveformOscController().getPreview());
	m_previews.push_back(AtomSynth::OutputAtomController().getPreview());
	AtomSynth::GlobalNoteStates::init();
	updateExecutionOrder();
	m_atomsReloaded = true;
}

void Synth::addAtom(AtomSynth::AtomController* controller)
{
	m_atoms.push_back(controller);
	m_atomExecutionOrder.resize(m_atoms.size(), 0);
	updateExecutionOrder();
}

struct ReallySimpleAtomProxy
{
	std::vector<ReallySimpleAtomProxy *> m_inputs;
	bool m_requirementsSatisfied;

	ReallySimpleAtomProxy();
	bool update();
};

ReallySimpleAtomProxy::ReallySimpleAtomProxy()
	: m_inputs(std::vector<ReallySimpleAtomProxy *>()),
	  m_requirementsSatisfied(false)
{

}

void Synth::updateExecutionOrder()
{
	std::vector<ReallySimpleAtomProxy> proxies = std::vector<ReallySimpleAtomProxy>();
	proxies.resize(m_atoms.size(), ReallySimpleAtomProxy());
	m_atomExecutionOrder.clear();
	for(int i = 0; i < m_atoms.size(); i++)
	{
		for(std::pair<AtomSynth::AtomController *, int> & input : m_atoms[i]->getInputs())
		{
			if(input.first != nullptr)
			{
				std::vector<AtomSynth::AtomController *>::const_iterator index = std::find(m_atoms.begin(), m_atoms.end(), input.first);
				if(index != m_atoms.end())
				{
					proxies[i].m_inputs.push_back(& proxies[index - m_atoms.begin()]);
				}
			}
		}
	}

	bool running = true;
	int remaining = proxies.size();
	while(running)
	{
		int index = 0;
		bool progress = false;
		for(ReallySimpleAtomProxy & proxy : proxies)
		{
			if(!proxy.m_requirementsSatisfied)
			{
				progress = true;
				proxy.m_requirementsSatisfied = true;
				for(ReallySimpleAtomProxy * input : proxy.m_inputs)
				{
					proxy.m_requirementsSatisfied = proxy.m_requirementsSatisfied && input->m_requirementsSatisfied; //Perform an && operation with all inputs. If one is not satisfied, the current proxy is not satisfied.
				}
				if(proxy.m_requirementsSatisfied)
				{
					remaining--;
					m_atomExecutionOrder.push_back(index);
				}
			}
			index++;
		}
//#define PRINT_COMPILE
		if(remaining == 0)
		{
#ifdef PRINT_COMPILE
			std::cout << "Compile succeeded" << std::endl;
			for(int i : m_atomExecutionOrder)
				std::cout << i << std::endl;
#endif
			running = false;
		}
		if(progress == false)
		{
#ifdef PRINT_COMPILE
			std::cout << "Compile failed" << std::endl;
			for(int i : m_atomExecutionOrder)
				std::cout << i << std::endl;
#endif
			running = false;
		}
	}
}

AudioBuffer Synth::execute()
{
	if(m_atoms.size() != 0)
	{
		AtomSynth::AtomController * output = nullptr;
		for(AtomSynth::AtomController * controller: m_atoms)
		{
			if(controller->getName() == "Output")
				output = controller;
		}
		std::pair<AtomController *, int> input;
		if(output == nullptr)
			return m_output;
		else
		{
			input = output->getInput(0);
			if(input.first == nullptr)
			{
				return m_output;
			}
		}
		for(int index : m_atomExecutionOrder)
		{
			m_atoms[index]->execute();
		}

		double sample = 0.0f;
		std::vector<double> totals;
		totals.resize(AtomSynth::GlobalNoteStates::getPolyphony(), 0.0);
		m_output.fill(0.0);
		//Sum up all polyphony.
		for(int note = 0; note < GlobalNoteStates::getPolyphony(); note++)
		{
			std::vector<double>::iterator bufferIterator = input.first->getAtom(note)->getOutput(input.second)->getData().begin();
			if(AtomSynth::GlobalNoteStates::getIsActive(note))
			{
				for(int s = 0; s < AudioBuffer::getChannels() * AudioBuffer::getSize(); s++)
				{
					sample = (*bufferIterator);
					totals[note] += fabs(sample);
					m_output.getData()[s] += sample;
					bufferIterator++;
				}
			}
		}

		//Copy values to output.
		double polyphony = AtomSynth::GlobalNoteStates::getPolyphony();
		for(int s = 0; s < AudioBuffer::getChannels() * AudioBuffer::getSize(); s++)
		{
			m_output.getData()[s] /= polyphony;
		}

		int index = 0;
		for(double total : totals)
		{
			if(AtomSynth::GlobalNoteStates::getIsActive(index))
			{
				if(total <= 0.00001)
				{
					AtomSynth::GlobalNoteStates::stop(index);
				}
			}
			index++;
		}
	}
	return m_output;
}

void Synth::loadSaveState(SaveState state)
{
	for(SaveState & atomState : state.getStates())
	{
		AtomController * atom = m_previews[int(atomState.getValue(0))].m_createFunction();
		atom->loadSaveState(atomState.getState(1));
		m_atoms.push_back(atom);
	}

	int index = 0;
	for(SaveState & atomState : state.getStates())
	{
		int input = 0;
		for(SaveState & inputState : atomState.getState(0).getStates())
		{
			if(inputState.getValue(0) != -1.0)
			{
				m_atoms[index]->linkInput(input, m_atoms[int(inputState.getValue(0))], int(inputState.getValue(1)));
			}
			input++;
		}
		index++;
	}

	updateExecutionOrder();
}

SaveState Synth::saveSaveState()
{
	SaveState state = SaveState();

	for(AtomController * atom : m_atoms)
	{
		SaveState atomState = SaveState();
		std::string name = atom->getName();
		int index = 0;
		for(AtomControllerPreview & preview : m_previews)
		{
			if(preview.m_name == name)
			{
				atomState.addValue(index);
				break;
			}
			index++;
		}

		SaveState inputStates = SaveState();
		for(std::pair<AtomController *, int> & input : atom->getInputs())
		{
			if(input.first == nullptr)
			{
				SaveState inputState = SaveState();
				inputState.addValue(-1.0);
				inputState.addValue(-1.0);
				inputStates.addState(inputState);
			}
			else
			{
				index = 0;
				for(AtomController * inputAtom : m_atoms)
				{
					if(inputAtom == input.first)
					{
						SaveState inputState = SaveState();
						inputState.addValue(index);
						inputState.addValue(input.second);
						inputStates.addState(inputState);
						break;
					}
					index++;
				}
			}
		}

		atomState.addState(inputStates);
		atomState.addState(atom->saveSaveState());
		state.addState(atomState);
	}

	return state;
}

} /* namespace AtomSynth */

