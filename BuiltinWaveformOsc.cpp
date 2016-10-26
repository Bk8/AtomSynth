/*
 * BuiltinWaveformOsc.cpp
 *
 *  Created on: Aug 17, 2016
 *      Author: josh
 */

#include "BuiltinWaveformOsc.h"

#include <cmath>
#include <iostream>

#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioBuffer.h"
#include "SaveState.h"

namespace AtomSynth
{

BuiltinWaveformOscController::BuiltinWaveformOscController()
	: AtomController(MAX_INPUTS, 1),
	  m_shape(Waveform::TRIANGLE)
{
	init();

	m_gui->addComponent(& m_sineButton);
	m_sineButton.setButtonText("SINE");
	m_sineButton.setBounds(5, 36, 80, 30);
	m_sineButton.addListener(this);

	m_gui->addComponent(& m_squareButton);
	m_squareButton.setButtonText("SQUARE");
	m_squareButton.setBounds(89, 36, 80, 30);
	m_squareButton.addListener(this);

	m_gui->addComponent(& m_triangleButton);
	m_triangleButton.setButtonText("TRIANGLE");
	m_triangleButton.setBounds(173, 36, 80, 30);
	m_triangleButton.addListener(this);

	m_gui->addComponent(& m_plot);
	m_plot.setBounds(5, 70, 247, 247);
	m_plot.setLines(7, 7);

	for(int i = 0; i < 248; i++)
		m_plot.addPoint(double(i) / 248.0f);
	m_plot.finish();

	m_gui->addComponent(& m_detuneLabel);
	m_detuneLabel.setBounds(300, 35, 200, 20);
	m_detuneLabel.setText("Detune", NotificationType::dontSendNotification);

	m_gui->addComponent(& m_octaveDetuneKnob);
	m_octaveDetuneKnob.setBounds(260, 55, 64, 64);
	m_octaveDetuneKnob.enableDetailedEditing(m_gui);

	m_gui->addComponent(& m_semitoneDetuneKnob);
	m_semitoneDetuneKnob.setBounds(329, 55, 64, 64);
	m_semitoneDetuneKnob.enableDetailedEditing(m_gui);

	m_gui->addComponent(& m_semitoneOctaveLabel);
	m_semitoneOctaveLabel.setBounds(256, 118, 300, 20);
	m_semitoneOctaveLabel.setText("Octaves   Semitones", NotificationType::dontSendNotification);

	m_gui->addComponent(& m_topKnob);
	m_topKnob.setBounds(260, 170, 64, 64);
	m_topKnob.setValue(1.0f);
	m_topKnob.addListener(this);
	m_topKnob.enableDetailedEditing(m_gui);

	m_gui->addComponent(& m_bottomKnob);
	m_bottomKnob.setBounds(329, 170, 64, 64);
	m_bottomKnob.setValue(-1.0f);
	m_bottomKnob.addListener(this);
	m_bottomKnob.enableDetailedEditing(m_gui);

	m_gui->addComponent(& m_centerKnob);
	m_centerKnob.setBounds(260, 239, 64, 64);
	m_centerKnob.setValue(0.0f);
	m_centerKnob.addListener(this);
	m_centerKnob.enableDetailedEditing(m_gui);

	m_gui->addComponent(& m_parmKnob);
	m_parmKnob.setBounds(329, 239, 64, 64);
	m_parmKnob.setValue(0.0f);
	m_parmKnob.addListener(this);
	m_parmKnob.enableDetailedEditing(m_gui);

	m_gui->addComponent(& m_v1v2Label);
	m_v1v2Label.setBounds(275, 150, 300, 20);
	m_v1v2Label.setText("Top          Bottom ", NotificationType::dontSendNotification);

	m_gui->addComponent(& m_v3PhaseLabel);
	m_v3PhaseLabel.setBounds(263, 302, 300, 20);
	m_v3PhaseLabel.setText("Center    Parameter", NotificationType::dontSendNotification);

	m_gui->addComponent(& m_fmLabel);
	m_fmLabel.setBounds(10, 320, 300, 20);
	m_fmLabel.setText("Amplitude", NotificationType::dontSendNotification);

	m_gui->addComponent(& m_amplitudeKnob);
	m_amplitudeKnob.setBounds(5, 342, 64, 64);
	m_amplitudeKnob.setRange(0.0f, 1.0f);
	m_amplitudeKnob.setValue(1.0f);
	m_amplitudeKnob.addListener(this);
	m_amplitudeKnob.enableDetailedEditing(m_gui);

	m_gui->addComponent(& m_fmKnob);
	m_fmKnob.setBounds(74, 342, 64, 64);
	m_fmKnob.setValue(0.0f);
	m_fmKnob.enableDetailedEditing(m_gui);

	m_gui->addComponent(& m_panningKnob);
	m_panningKnob.setBounds(143, 342, 64, 64);
	m_panningKnob.setRange(-1.0, 1.0);
	m_panningKnob.setValue(0.0);
	m_panningKnob.enableDetailedEditing(m_gui);

	m_gui->addComponent(& m_phaseKnob);
	m_phaseKnob.setBounds(212, 342, 64, 64);
	m_phaseKnob.addListener(this);
	m_phaseKnob.enableDetailedEditing(m_gui);

	m_gui->addComponent(& m_unisonVoicesKnob);
	m_unisonVoicesKnob.setBounds(5, 433, 64, 64);
	m_unisonVoicesKnob.setRange(1.0f, 20.0f);
	m_unisonVoicesKnob.setValue(1.0f);
	m_unisonVoicesKnob.setInt(true);
	m_unisonVoicesKnob.addListener(this);

	m_gui->addComponent(& m_unisonDetuneKnob);
	m_unisonDetuneKnob.setBounds(74, 433, 64, 64);
	m_unisonDetuneKnob.setValue(20.0f);
	m_unisonDetuneKnob.enableDetailedEditing(m_gui);

	m_gui->addComponent(& m_unisonPanningKnob);
	m_unisonPanningKnob.setBounds(143, 433, 64, 64);
	m_unisonPanningKnob.setRange(0.0f, 1.0f);
	m_unisonPanningKnob.setValue(0.5f);
	m_unisonPanningKnob.enableDetailedEditing(m_gui);

	m_gui->addComponent(& m_unisonPhaseKnob);
	m_unisonPhaseKnob.setBounds(212, 433, 64, 64);
	m_unisonPhaseKnob.setRange(-1.0, 1.0);
	m_unisonPhaseKnob.setValue(0.0);
	m_unisonPhaseKnob.enableDetailedEditing(m_gui);

	m_gui->addComponent(& m_unisonLabel);
	m_unisonLabel.setBounds(13, 409, 300, 20);
	m_unisonLabel.setText("Voices     (Detuning     Panning      Phase)", NotificationType::dontSendNotification);

	m_knobSet[0] = & m_octaveDetuneKnob;
	m_knobSet[1] = & m_semitoneDetuneKnob;
	m_knobSet[2] = & m_fmKnob;
	m_knobSet[3] = & m_topKnob;
	m_knobSet[4] = & m_bottomKnob;
	m_knobSet[5] = & m_centerKnob;
	m_knobSet[6] = & m_parmKnob;
	m_knobSet[7] = & m_phaseKnob;
	m_knobSet[8] = & m_amplitudeKnob;
	m_knobSet[9] = & m_panningKnob;
	m_knobSet[10] = & m_unisonDetuneKnob;
	m_knobSet[11] = & m_unisonPanningKnob;
	m_knobSet[12] = & m_unisonPhaseKnob;
}

BuiltinWaveformOscController::~BuiltinWaveformOscController()
{

}

void BuiltinWaveformOscController::loadSaveState(SaveState state)
{
	AtomController::loadSaveState(state.getNextState());
	state = state.getNextState();

	m_shape = static_cast<Waveform::WaveShape>(int(state.getValue(0)));

	m_octaveDetuneKnob.loadSaveState(state.getNextState());
	m_semitoneDetuneKnob.loadSaveState(state.getNextState());

	m_topKnob.loadSaveState(state.getNextState());
	m_bottomKnob.loadSaveState(state.getNextState());
	m_centerKnob.loadSaveState(state.getNextState());
	m_parmKnob.loadSaveState(state.getNextState());

	m_amplitudeKnob.loadSaveState(state.getNextState());
	m_fmKnob.loadSaveState(state.getNextState());
	m_panningKnob.loadSaveState(state.getNextState());
	m_phaseKnob.loadSaveState(state.getNextState());

	m_unisonVoicesKnob.loadSaveState(state.getNextState());
	m_unisonDetuneKnob.loadSaveState(state.getNextState());
	m_unisonPanningKnob.loadSaveState(state.getNextState());
	m_unisonPhaseKnob.loadSaveState(state.getNextState());

	updateGraph();
	updateVoices();
}

SaveState BuiltinWaveformOscController::saveSaveState()
{
	SaveState basicState = AtomController::saveSaveState();
	SaveState state = SaveState();
	SaveState toReturn = SaveState();
	toReturn.addState(basicState);

	state.addValue(static_cast<int>(m_shape));

	state.addState(m_octaveDetuneKnob.saveSaveState());
	state.addState(m_semitoneDetuneKnob.saveSaveState());

	state.addState(m_topKnob.saveSaveState());
	state.addState(m_bottomKnob.saveSaveState());
	state.addState(m_centerKnob.saveSaveState());
	state.addState(m_parmKnob.saveSaveState());

	state.addState(m_amplitudeKnob.saveSaveState());
	state.addState(m_fmKnob.saveSaveState());
	state.addState(m_panningKnob.saveSaveState());
	state.addState(m_phaseKnob.saveSaveState());

	state.addState(m_unisonVoicesKnob.saveSaveState());
	state.addState(m_unisonDetuneKnob.saveSaveState());
	state.addState(m_unisonPanningKnob.saveSaveState());
	state.addState(m_unisonPhaseKnob.saveSaveState());

	toReturn.addState(state);

	return toReturn;
}

void BuiltinWaveformOscController::buttonClicked(Button* button)
{
	if(button == &m_sineButton)
		m_shape = Waveform::SINE;
	else if(button == &m_squareButton)
		m_shape = Waveform::SQUARE;
	else if(button == &m_triangleButton)
		m_shape = Waveform::TRIANGLE;
	updateGraph();
}

void BuiltinWaveformOscController::updateGraph()
{
	m_plot.clear();
	double phase = 0.0f;
	for(int i = 0; i < 248; i++)
	{
		phase = (double(i) / 124.0f);
		phase -= m_phaseKnob.getValue();
		phase = phase - 2.0f * floor(phase / 2.0f); // Wrap around. Works on negative numbers, unlike fmod.
		phase -= 1.0f;
		m_plot.addPoint(Waveform::calculate(m_shape, phase, m_topKnob.getValue(), m_bottomKnob.getValue(), m_centerKnob.getValue(), m_parmKnob.getValue()) * m_amplitudeKnob.getValue());
	}
	m_plot.finish();
}

void BuiltinWaveformOscController::updateVoices()
{
	for(Atom * atom : getAtoms())
		dynamic_cast<BuiltinWaveformOscAtom *>(atom)->updateVoices(int(m_unisonVoicesKnob.getValue()));
}

Atom* BuiltinWaveformOscController::createAtom(int index)
{
	return new BuiltinWaveformOscAtom(* this, index);
}

AtomController * createBuiltinWaveformOscController()
{
	return new BuiltinWaveformOscController();
}

AtomControllerPreview BuiltinWaveformOscController::getPreview()
{
	AtomControllerPreview toReturn = AtomController::getPreview();
	toReturn.m_createFunction = createBuiltinWaveformOscController;
	return toReturn;
}

BuiltinWaveformOscAtom::BuiltinWaveformOscAtom(BuiltinWaveformOscController& parent, int index)
	: Atom(index, MAX_INPUTS, 1),
	  m_parent(parent),
	  m_phases(std::vector<std::vector<double>>(1, std::vector<double>(AudioBuffer::getChannels(), -1.0)))
{
	for(int channel = 0; channel < AudioBuffer::getChannels(); channel++)
		m_phases[0][channel] = 0.0f;
}

BuiltinWaveformOscAtom::~BuiltinWaveformOscAtom()
{

}

void BuiltinWaveformOscAtom::updateVoices(int numVoices)
{
	if(numVoices > m_phases.size()) //Add more voices.
	{
		for(int i = 0; i < (numVoices - m_phases.size()); i++)
		{
			m_phases.push_back(std::vector<double>(AudioBuffer::getChannels(), -1.0));
		}
	}
	else if(numVoices < m_phases.size()) //Remove extra voices.
	{
		for(int i = 0; i < (m_phases.size() - numVoices); i++)
		{
			m_phases.erase(m_phases.end());
		}
	}
	else
	{
		return;
	}

	for(int i = 0; i < m_phases.size(); i++)
	{
		m_phases[i][0]= -1.0;
		m_phases[i][1]= -1.0;
	}

}

double getUnisonFactor(int voice, int maxVoices)
{
	if(maxVoices == 1)
		return 0.0;
	else
		return (double(voice) / double(maxVoices - 1)) * 2.0 - 1.0;
}

void BuiltinWaveformOscAtom::execute()
{
	Atom::execute();
	AtomKnob::calculateKnobs<13>(m_parent.m_knobSet, * this);
	double phase = 0.0, frequency = 0.0, baseFrequency = 0.0;
	double baseDetune = pow(OCTAVES_INTERVAL, m_parent.m_octaveDetuneKnob.getValue()) + pow(SEMITONES_INTERVAL, m_parent.m_semitoneDetuneKnob.getValue()) - 1.0f;
	double detune = 0.0;
	double pan = 0.0, panAmplitude = 0.0, basePan = 0.0, panMaxing = sin(M_PI / 4.0);
	double unisonFactor = 0.0f;
	double voices = double(m_phases.size());
	bool manyVoices = (m_phases.size() > 1);
	double finalValue = 0.0;

	std::vector<double>::iterator octaveIterator	= m_parent.m_octaveDetuneKnob.getResult().getData().begin();
	std::vector<double>::iterator semitoneIterator	= m_parent.m_semitoneDetuneKnob.getResult().getData().begin();
#define IS_CONSTANT(A) (m_parent.A.getResult().getConstant())
	bool animatedCoarseDetune = !(IS_CONSTANT(m_octaveDetuneKnob) && IS_CONSTANT(m_semitoneDetuneKnob));
	std::vector<double>::iterator centsIterator		= m_parent.m_fmKnob.getResult().getData().begin();
	bool animatedCents = !IS_CONSTANT(m_fmKnob);
	std::vector<double>::iterator topIterator		= m_parent.m_topKnob.getResult().getData().begin();
	std::vector<double>::iterator bottomIterator	= m_parent.m_bottomKnob.getResult().getData().begin();
	std::vector<double>::iterator centerIterator	= m_parent.m_centerKnob.getResult().getData().begin();
	std::vector<double>::iterator parameterIterator	= m_parent.m_parmKnob.getResult().getData().begin();
	std::vector<double>::iterator phaseIterator		= m_parent.m_phaseKnob.getResult().getData().begin();
	std::vector<double>::iterator amplitudeIterator	= m_parent.m_amplitudeKnob.getResult().getData().begin();
	std::vector<double>::iterator panningIterator	= m_parent.m_panningKnob.getResult().getData().begin();
	bool animatedPanning = !m_parent.m_panningKnob.getResult().getConstant();
	std::vector<double>::iterator uCentsIterator	= m_parent.m_unisonDetuneKnob.getResult().getData().begin();
	bool nonZeroUnisonCents = (m_parent.m_unisonDetuneKnob.getValue() != 0.0) || !IS_CONSTANT(m_unisonDetuneKnob);
	std::vector<double>::iterator uPanningIterator	= m_parent.m_unisonPanningKnob.getResult().getData().begin();
	std::vector<double>::iterator uPhaseIterator	= m_parent.m_unisonPhaseKnob.getResult().getData().begin();

	baseDetune = 1.0;

	if(!animatedCoarseDetune)
	{
		baseDetune *= OctavesKnob::detune(1.0, (* octaveIterator)); //Detuning 1 hz up 2 octaves = 2 hz. This is the same as getting the detune factor for the number of octaves. Used later.
		baseDetune *= SemitonesKnob::detune(1.0, (* semitoneIterator));
	}

	if(!animatedCents)
	{
		baseDetune *= CentsKnob::detune(1.0, (* centsIterator));
	}

	if(m_inputs[0] != nullptr)
	{
		baseFrequency = m_inputs[0]->getValue(0, 0) * baseDetune;
	}
	else
	{
		baseFrequency = 440.0 * baseDetune;
	}

	std::vector<double> panAmplitudes;
	if(!animatedPanning)
	{
		if(manyVoices)
		{
			for(int voice = 0; voice < m_phases.size(); voice++)
			{
				pan = (* panningIterator) + (getUnisonFactor(voice, m_phases.size()) * (* uPanningIterator));
				pan = (pan + 1) * 0.5;
				panAmplitudes.push_back(std::min(1.0, cos(pan * M_PI / 2.0) / panMaxing));
				panAmplitudes.push_back(std::min(1.0, sin(pan * M_PI / 2.0) / panMaxing));
			}
		}
		else
		{
			pan = ((* panningIterator) + 1) * 0.5;
			panAmplitudes.push_back(std::min(1.0, cos(pan * M_PI / 2.0) / panMaxing));
			panAmplitudes.push_back(std::min(1.0, sin(pan * M_PI / 2.0) / panMaxing));
		}
	}

	std::vector<double> voiceDetunes;
	if(manyVoices)
	{
		for(int voice = 0; voice < m_phases.size(); voice++)
		{
			voiceDetunes.push_back(CentsKnob::detune(1.0, (* uCentsIterator) * getUnisonFactor(voice, m_phases.size())));
		}
	}

	m_outputs[0].fill(0.0);
	for(int voice = 0; voice < voices; voice++) //Iterate over each voice.
	{
		std::vector<double>::iterator octaveIterator	= m_parent.m_octaveDetuneKnob.getResult().getData().begin();
		std::vector<double>::iterator semitoneIterator	= m_parent.m_semitoneDetuneKnob.getResult().getData().begin();
		std::vector<double>::iterator centsIterator		= m_parent.m_fmKnob.getResult().getData().begin();
		std::vector<double>::iterator topIterator		= m_parent.m_topKnob.getResult().getData().begin();
		std::vector<double>::iterator bottomIterator	= m_parent.m_bottomKnob.getResult().getData().begin();
		std::vector<double>::iterator centerIterator	= m_parent.m_centerKnob.getResult().getData().begin();
		std::vector<double>::iterator parameterIterator	= m_parent.m_parmKnob.getResult().getData().begin();
		std::vector<double>::iterator phaseIterator		= m_parent.m_phaseKnob.getResult().getData().begin();
		std::vector<double>::iterator amplitudeIterator	= m_parent.m_amplitudeKnob.getResult().getData().begin();
		std::vector<double>::iterator panningIterator	= m_parent.m_panningKnob.getResult().getData().begin();
		std::vector<double>::iterator uCentsIterator	= m_parent.m_unisonDetuneKnob.getResult().getData().begin();
		std::vector<double>::iterator uPanningIterator	= m_parent.m_unisonPanningKnob.getResult().getData().begin();
		std::vector<double>::iterator uPhaseIterator	= m_parent.m_unisonPhaseKnob.getResult().getData().begin();
		for(int channel = 0; channel < AudioBuffer::getChannels(); channel++)
		{
			for(int sample = 0; sample < AudioBuffer::getSize(); sample++)
			{
				finalValue = 0.0;
				basePan = (*panningIterator);
				unisonFactor = getUnisonFactor(voice, m_parent.m_unisonVoicesKnob.getValue());
				if(animatedPanning)
				{
					if(manyVoices)
					{
						pan = basePan + (unisonFactor * (* uPanningIterator));
						pan = (pan + 1) * 0.5;
					}
					else
					{
						pan = (basePan + 1) * 0.5;
					}
					//Sinusoidal panning
					if(channel == 0) //left
						panAmplitude = cos(pan * (M_PI / 2.0));
					else //right
						panAmplitude = sin(pan * (M_PI / 2.0));

					panAmplitude = std::min(1.0, panAmplitude / panMaxing); //TODO: remove or relpace with something faster.
				}
				else
				{
					panAmplitude = panAmplitudes[voice * AudioBuffer::getChannels() + channel];
				}

				if(manyVoices)
				{
					frequency = baseFrequency * voiceDetunes[voice];
				}
				else
				{
					frequency = baseFrequency;
				}


				detune = 1.0;
				if(animatedCoarseDetune)
				{
					detune *= OctavesKnob::detune(1.0, (* octaveIterator));
					detune *= SemitonesKnob::detune(1.0, (* semitoneIterator));
				}
				if(animatedCents)
				{
					detune *= CentsKnob::detune(1.0, (* centsIterator));
				}

				m_phases[voice][channel] += (frequency * detune) / m_sampleRate;
				if(m_phases[voice][channel] >= 1.0)
					m_phases[voice][channel] -= 2.0;
				phase = m_phases[voice][channel] + (* phaseIterator) + ((* uPhaseIterator) * unisonFactor);
				if(phase >= 1.0)
				{
					phase -= 2.0;
				}
				else if(phase <= -1.0)
				{
					phase += 2.0;
				}
				m_outputs[0].getData()[sample + (AudioBuffer::getSize() * channel)] += Waveform::calculate(m_parent.m_shape, phase, (* topIterator), (* bottomIterator), (* centerIterator), (* parameterIterator)) * (* amplitudeIterator) / voices * panAmplitude;
			}
			octaveIterator++;
			semitoneIterator++;
			centsIterator++;
			topIterator++;
			bottomIterator++;
			centerIterator++;
			parameterIterator++;
			phaseIterator++;
			amplitudeIterator++;
			panningIterator++;
			uCentsIterator++;
			uPanningIterator++;
			uPhaseIterator++;
		}
	}
}

} /* namespace AtomSynth */
