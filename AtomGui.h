/*
 * AtomGui.h
 *
 *  Created on: Aug 15, 2016
 *      Author: josh
 */

#ifndef SOURCE_ATOMGUI_H_
#define SOURCE_ATOMGUI_H_

#include "../JuceLibraryCode/JuceHeader.h"

#define ATOM_GUI_X 400
#define ATOM_GUI_Y 600

namespace AtomSynth
{

class AtomKnob;

enum SliderStatus { SLIDER_NOT_SLIDING, SLIDER_MOVE, SLIDER_MIN, SLIDER_MAX };

class KnobFooter : public Component
{
private:
	AtomKnob * m_currentKnob;
	Point<int> m_previousLocation;
	SliderStatus m_sliderStatus;
	int m_index;
public:
	KnobFooter();
	virtual ~KnobFooter();
	void paint(Graphics & g);
	void mouseDown(const MouseEvent & event);
	void mouseDrag(const MouseEvent & event);
	void setActiveKnob(AtomKnob * knob);
};

class AtomGui
{
private:
	Component m_root;
	KnobFooter m_footer;
	bool m_shown;
protected:
	void setShown(bool shown);
public:
	AtomGui();
	virtual ~AtomGui();
	void addComponent(Component * toAdd);
	void show() { setShown(true); }
	void hide() { setShown(false); }
	void repaintFooter() { m_footer.repaint(); }
	void setParent(Component * parent);
	void setActiveKnob(AtomKnob * knob) { m_footer.setActiveKnob(knob); }
	Component * getRoot();
	bool getShown() { return m_shown; }
};

}

#endif /* SOURCE_ATOMGUI_H_ */
