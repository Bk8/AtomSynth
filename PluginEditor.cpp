/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include <iostream>

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "AudioBuffer.h"
#include "Colours.h"
#include "Output.h"
#include "SaveState.h"
#include "Synth.h"

//==============================================================================
AtomSynthAudioProcessorEditor::AtomSynthAudioProcessorEditor (AtomSynthAudioProcessor& p)
    : AudioProcessorEditor (&p),
	  processor (p),
	  m_currentAtom(nullptr),
	  m_px(0), m_py(0),
	  m_xOffset(0), m_yOffset(0),
	  m_currentTab(0),
	  m_dragStatus(DragStatus::MOVING),
	  m_addingAtom(true)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 600);
    processor.m_synth.setAtomsReloaded(true);
}

AtomSynthAudioProcessorEditor::~AtomSynthAudioProcessorEditor()
{
}

void AtomSynthAudioProcessorEditor::sliderValueChanged(Slider * slider)
{
	processor.m_frequency = slider->getValue();
}

#define ATOM_WIDTH MAX_INPUTS * 16 + 4
#define ATOM_HEIGHT 80
#define TAB_WIDTH 16
#define TAB_HEIGHT 20

void AtomSynthAudioProcessorEditor::mouseDown(const MouseEvent & event)
{
	if(m_addingAtom)
	{
		int index = 0;
		index = event.getPosition().getY() / (ATOM_HEIGHT + 3);
		index *= 2;
		if(event.getPosition().getX() > 600)
			index++;
		if(index < processor.m_synth.getPreviews().size())
		{
			AtomSynth::AtomController * controller = processor.m_synth.getPreviews()[index].m_createFunction();
			controller->getGui()->setParent(this);
			controller->setPosition(m_xOffset, m_yOffset);
			processor.m_synth.addAtom(controller);
		}
		m_addingAtom = false;
		repaint();
	}
	else
	{
		if((event.getPosition().getX() > 765) && (event.getPosition().getY() > 565))
		{
			m_addingAtom = true;
			repaint();
			return;
		}
		else if((event.getPosition().getX() > 731) && (event.getPosition().getY() > 565))
		{
			std::cout << processor.m_synth.saveSaveState().getString() << std::endl;
		}

		if(processor.m_synth.getAtomsReloaded())
		{
			for(AtomSynth::AtomController * controller : processor.m_synth.getAtoms())
				controller->getGui()->setParent(this);
			processor.m_synth.setAtomsReloaded(false);
		}

		m_px = event.x;
		m_py = event.y;
		m_dragStatus = DragStatus::MOVING;
		for(AtomSynth::AtomController * controller : processor.m_synth.getAtoms())
		{
			int x = event.x - 400 - controller->getX() + m_xOffset, y = event.y - controller->getY() + m_yOffset;
			if((x >= 0) && (y >= 0) &&
			   (x <= ATOM_WIDTH) &&
			   (y <= ATOM_HEIGHT))
			{
				if(m_currentAtom != nullptr)
					m_currentAtom->getGui()->hide();
				m_currentAtom = controller;
				m_currentAtom->getGui()->show();
				x -= 3;
				x /= TAB_WIDTH;
				//Input pins
				if((y <= TAB_HEIGHT) && (x < m_currentAtom->getNumInputs()))
				{
					//If the input pin is not used, do a normal input to output link
					if(m_currentAtom->getInput(x).first == nullptr)
					{
						m_dragStatus = DragStatus::IN_TO_OUT;
						m_currentTab = x;
					}
					//Otherwise, setup an output to input link with the previous link's parent as the root
					else
					{
						m_dragStatus = DragStatus::OUT_TO_IN;
						m_currentTab = m_currentAtom->getInput(x).second;
						AtomSynth::AtomController * previous = m_currentAtom;
						m_currentAtom = m_currentAtom->getInput(x).first;
						previous->unlinkInput(x);
						previous->getGui()->hide();
						m_currentAtom->getGui()->show();
					}
				}
				else if((y >= (ATOM_HEIGHT - TAB_HEIGHT)) && (MAX_OUTPUTS - x - 1 < m_currentAtom->getNumOutputs()))
				{
					m_dragStatus = DragStatus::OUT_TO_IN;
					m_currentTab = MAX_OUTPUTS - x - 1;
					if(m_currentTab >= m_currentAtom->getNumOutputs())
						m_dragStatus = DragStatus::MOVING;
				}
				return;
			}
		}
		if(m_currentAtom != nullptr)
			m_currentAtom->getGui()->hide();
		m_currentAtom = nullptr;
	}
}

void AtomSynthAudioProcessorEditor::mouseDrag(const MouseEvent & event)
{
	int dx = event.x - m_px, dy = event.y - m_py;
	m_px += dx; m_py += dy;

	if(m_dragStatus == DragStatus::MOVING)
	{
		if(m_currentAtom == nullptr)
		{
			m_xOffset -= dx;
			m_yOffset -= dy;
		}
		else
		{
			m_currentAtom->setPosition(m_currentAtom->getX() + dx, m_currentAtom->getY() + dy);
		}
	}
	repaint(400, 0, 400, 600);
}

void AtomSynthAudioProcessorEditor::mouseUp(const MouseEvent & event)
{
	AtomSynth::AtomController * dropped = nullptr;
	int tab = 0;
	DragStatus dragStatus = m_dragStatus;
	m_dragStatus = DragStatus::MOVING;
	if((dragStatus == DragStatus::IN_TO_OUT) || (dragStatus == DragStatus::OUT_TO_IN))
	{
		repaint(400, 0, 400, 600);
		for(AtomSynth::AtomController * controller : processor.m_synth.getAtoms())
		{
			int x = event.x - 400 - controller->getX() + m_xOffset, y = event.y - controller->getY() + m_yOffset;
			if((x >= 0) && (y >= 0) &&
			   (x <= ATOM_WIDTH) &&
			   (y <= ATOM_HEIGHT))
			{
				if(((dragStatus == DragStatus::IN_TO_OUT) && (y >= ATOM_HEIGHT - TAB_HEIGHT)) ||
				   ((dragStatus == DragStatus::OUT_TO_IN) && (y <= TAB_HEIGHT)))
				{
					dropped = controller;
					tab = (x - 3) / TAB_WIDTH;
					break;
				}
			}
		}

		if((dropped == nullptr) || (dropped == m_currentAtom))
			return;

		if(dragStatus == DragStatus::IN_TO_OUT)
		{
			tab = MAX_OUTPUTS - tab - 1;
			if(tab < dropped->getNumOutputs())
				m_currentAtom->linkInput(m_currentTab, dropped, tab);
		}
		else
		{
			dropped->linkInput(tab, m_currentAtom, m_currentTab);
		}
		processor.m_synth.updateExecutionOrder();
	}
}

const double hues[10] = { 0.0f, 0.3f/ 3.6f, 0.6f/ 3.6f, 0.9f/ 3.6f, 1.2f/ 3.6f, 1.8f/ 3.6f, 2.1f/ 3.6f, 2.4f/ 3.6f, 2.7f/ 3.6f, 3.0f/ 3.6f };
void setColor(Graphics & g, int i)
{
	switch(i)
	{
	case 0:
		g.setColour(Colour(0.0f, 0.0f, 0.95f, 1.0f));
		break;
	case 1:
		g.setColour(Colour(0.0f, 0.0f, 0.3f, 1.0f));
		break;
	default:
		g.setColour(Colour(hues[i - 2], 0.8f, 1.0f, 1.0f));
		break;
	}
}

//==============================================================================
void AtomSynthAudioProcessorEditor::paint (Graphics& g)
{
	g.fillAll(ATOM_CANVAS);

	if(m_addingAtom)
	{
		int x = 3, y = 3;
		for(AtomSynth::AtomControllerPreview & preview : processor.m_synth.getPreviews())
		{
			g.setColour(MID_LAYER);
			g.fillRoundedRectangle(x + 400, y, ATOM_WIDTH, ATOM_HEIGHT, 4.0f);
			for(int i = 0; i < preview.m_inputs; i++)
			{
				setColor(g, i);
				g.fillRoundedRectangle(x + 403 + i * 16, y, TAB_WIDTH - 2, TAB_HEIGHT, 4.0f);
				g.fillRect(x + 403 + i * 16, y, 14, 4);
			}
			for(int i = 0; i < preview.m_outputs; i++)
			{
				setColor(g, i);
				g.fillRoundedRectangle(x + 403 + (MAX_OUTPUTS - i - 1) * 16, y + ATOM_HEIGHT - 20, TAB_WIDTH - 2, TAB_HEIGHT, 4.0f);
				g.fillRect(x + 403 + (MAX_OUTPUTS - i - 1) * 16, y + ATOM_HEIGHT - 4, TAB_WIDTH - 2, 4);
			}
			g.setColour(ATOM_TEXT_BG);
			g.fillRoundedRectangle(x + 404, y + (ATOM_HEIGHT - 28) / 2, ATOM_WIDTH - 8, 28, 4.0f);
			g.setColour(ATOM_BG);
			g.setFont(22.0f);
			g.drawText(preview.m_name, x + 404, y, ATOM_WIDTH - 8, ATOM_HEIGHT, Justification::centred);
			x += 199;
			if(x > 400)
			{
				x = 3;
				y += ATOM_HEIGHT + 3;
			}
		}
	}
	else
	{
		for(AtomSynth::AtomController * controller : processor.m_synth.getAtoms())
		{
			int x = controller->getX() + 400 - m_xOffset, y = controller->getY() - m_yOffset;
			for(int i = 0; i < MAX_INPUTS; i++)
			{
				setColor(g, i);
				if(i < controller->getNumInputs())
				{
					AtomSynth::AtomController * input = controller->getInput(i).first;
					if(input != nullptr)
					{
						g.drawLine(x + 11 + i * 16, y - 3, input->getX() + ((MAX_OUTPUTS - controller->getInput(i).second - 1) * 16 + 11) - m_xOffset + 400, input->getY() + ATOM_HEIGHT - m_yOffset + 3, 6.0f); //Yuck.
						g.fillEllipse(x + 3 + i * 16, y - 7, TAB_WIDTH - 2, TAB_WIDTH - 2);
						g.fillEllipse(input->getX() + ((MAX_OUTPUTS - controller->getInput(i).second - 1) * 16 + 3) - m_xOffset + 400, input->getY() + ATOM_HEIGHT - m_yOffset - 6, TAB_WIDTH - 2, TAB_WIDTH - 2);
					}
				}
			}
		}

		for(AtomSynth::AtomController * controller : processor.m_synth.getAtoms())
		{
			int x = controller->getX() + 400 - m_xOffset, y = controller->getY() - m_yOffset;
			g.setColour(ATOM_BG);
			g.fillRoundedRectangle(x, y, ATOM_WIDTH, ATOM_HEIGHT, 4.0f);
			g.setColour(ATOM_TEXT_BG);
			g.fillRoundedRectangle(x + 4, y + (ATOM_HEIGHT - 28) / 2, ATOM_WIDTH - 8, 28, 4.0f);
			g.setColour(ATOM_BG);
			g.setFont(22.0f);
			g.drawText(controller->getName(), x + 4, y, ATOM_WIDTH - 8, ATOM_HEIGHT, Justification::centred);
		}

		for(AtomSynth::AtomController * controller : processor.m_synth.getAtoms())
		{
			int x = controller->getX() + 400 - m_xOffset, y = controller->getY() - m_yOffset;
			for(int i = 0; i < MAX_INPUTS; i++)
			{
				if(i < controller->getNumInputs())
				{
					setColor(g, i);
					g.fillRoundedRectangle(x + 3 + i * 16, y, TAB_WIDTH - 2, TAB_HEIGHT, 4.0f);
					g.fillRect(x + 3 + i * 16, y, 14, 4);
				}

				if((MAX_OUTPUTS - i - 1) < controller->getNumOutputs())
				{
					setColor(g, MAX_OUTPUTS - i - 1);
					g.fillRoundedRectangle(x + 3 + i * 16, y + ATOM_HEIGHT - 20, TAB_WIDTH - 2, TAB_HEIGHT, 4.0f);
					g.fillRect(x + 3 + i * 16, y + ATOM_HEIGHT - 4, TAB_WIDTH - 2, 4);
				}
			}
		}

		g.setColour(ATOM_BG);
		g.fillRect(400, 564, 400, 36);
		g.setColour(BACK_LAYER);
		g.fillRect(766, 566, 32, 32);
		g.setColour(FORE_LAYER);
		g.fillRect(768, 578, 28, 8);
		g.setColour(FORE_LAYER);
		g.fillRect(778, 568, 8, 28);

		g.setColour(BACK_LAYER);
		g.fillRect(732, 566, 32, 32);
		g.setColour(FORE_LAYER);
		g.fillRect(734, 588, 28, 8);
		g.setColour(FORE_LAYER);
		g.fillRect(744, 568, 8, 28);
	}

    g.setColour(ATOM_BG);
    if(m_dragStatus == DragStatus::IN_TO_OUT)
    	g.drawLine(m_currentAtom->getX() + 400 + 3 + (TAB_WIDTH / 2) - m_xOffset + (m_currentTab * TAB_WIDTH), m_currentAtom->getY() - m_yOffset, m_px, m_py, 1.0f);
    else if(m_dragStatus == DragStatus::OUT_TO_IN)
    	g.drawLine(m_currentAtom->getX() + 400 + ATOM_WIDTH - (TAB_WIDTH / 2) - 3 - m_xOffset - (m_currentTab * TAB_WIDTH), m_currentAtom->getY() - m_yOffset + ATOM_HEIGHT, m_px, m_py, 1.0f);
    g.fillRect(0, 0, 400, 600);

    if(m_currentAtom != nullptr)
    {
    	g.setColour(ATOM_TEXT_BG);
    	g.fillRoundedRectangle(4, 4, ATOM_WIDTH - 8, 28, 4.0f);
    	g.setColour(ATOM_BG);
    	g.setFont(22.0f);
    	g.drawText(m_currentAtom->getName(), 4, 4, ATOM_WIDTH - 8, 28, Justification::centred);
    }
}

void AtomSynthAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void AtomSynthAudioProcessorEditor::updateScreen()
{
	repaint();
}
