/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include <vector>


//==============================================================================
/**
*/
namespace AtomSynth
{
	class AtomGui;
}

enum class DragStatus { MOVING, IN_TO_OUT, OUT_TO_IN };

class AtomSynthAudioProcessorEditor : public AudioProcessorEditor
{
public:
    AtomSynthAudioProcessorEditor (AtomSynthAudioProcessor&);
    ~AtomSynthAudioProcessorEditor();

    void sliderValueChanged(Slider * slider);
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    void mouseDown(const MouseEvent & event);
    void mouseDrag(const MouseEvent & event);
    void mouseUp(const MouseEvent & event);

    void updateScreen();

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AtomSynth::AtomController * m_currentAtom;
    AtomSynthAudioProcessor& processor;
    int m_px, m_py, m_xOffset, m_yOffset, m_currentTab;
    DragStatus m_dragStatus;
    bool m_addingAtom;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AtomSynthAudioProcessorEditor)
};

#endif  // PLUGINEDITOR_H_INCLUDED
