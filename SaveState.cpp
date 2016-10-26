/*
 * SaveState.cpp
 *
 *  Created on: Oct 17, 2016
 *      Author: josh
 */

#include <iostream>

#include "SaveState.h"

namespace AtomSynth
{

SaveState::SaveState()
	: m_nextCounter(-1)
{
	// TODO Auto-generated constructor stub
}

SaveState::SaveState(std::string from)
	: m_nextCounter(-1)
{
	bool isNumber = true;
	double number = 0.0;
	int brackets = 0;
	std::string buffer = "";
	SaveState state;
	for(char & c : from)
	{
		switch(c)
		{
		case ':':
			if(brackets == 1)
			{
				m_values.push_back(std::stod(buffer));
				buffer = "";
			}
			else
			{
				buffer += c;
			}
			break;
		case '[':
			if(brackets == 0)
			{
				buffer = "";
			}
			else
			{
				buffer += '[';
			}
			brackets++;
			break;
		case ']':
			buffer += ']';
			brackets--;
			if(brackets == 1)
			{
				m_states.push_back(SaveState(buffer));
				buffer = "";
			}
			break;
		default:
			buffer += c;
		}
	}
}

SaveState::~SaveState()
{
	// TODO Auto-generated destructor stub
}

SaveState & SaveState::getNextState()
{
	m_nextCounter++;
	if(m_nextCounter > m_states.size())
	{
		m_nextCounter = 0;
	}
	return m_states[m_nextCounter];
}

std::string SaveState::getString()
{
	std::string output = "[";
	for(double & value : m_values)
	{
		output += std::to_string(value);
		output += ":";
	}

	for(SaveState & state : m_states)
	{
		output += state.getString();
	}
	output += ']';
	return output;
}

} /* namespace AtomSynth */
