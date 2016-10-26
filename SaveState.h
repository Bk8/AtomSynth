/*
 * SaveState.h
 *
 *  Created on: Oct 17, 2016
 *      Author: josh
 */

#ifndef SOURCE_SAVESTATE_H_
#define SOURCE_SAVESTATE_H_

#include <string>
#include <vector>

namespace AtomSynth
{

class SaveState
{
private:
	std::vector<double> m_values;
	std::vector<SaveState> m_states;
	int m_nextCounter;
public:
	SaveState();
	SaveState(std::string from);
	virtual ~SaveState();
	void addValue(double value) { m_values.push_back(value); }
	void addState(SaveState state) { m_states.push_back(state); }
	double getValue(int index) { return m_values[index]; }
	SaveState & getState(int index) { return m_states[index]; }
	std::vector<SaveState> & getStates() { return m_states; }
	SaveState & getNextState();
	std::string getString();
};

} /* namespace AtomSynth */

#endif /* SOURCE_SAVESTATE_H_ */
