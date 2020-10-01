#pragma once

#ifndef CONTOURPLANSTATECONTAINER_H
#define CONTOURPLANSTATECONTAINER_H

#include <string>
#include <unordered_map>
#include <mutex>
#include "ContourPlanState.h"

class ContourPlanStateContainer
{
	public:
		bool checkIfStateExistsForPlan(std::string in_planName)
		{
			std::lock_guard<std::mutex> lock(mapGuard);
			auto findString = stateMap.find(in_planName);
			bool wasFound = false;	// assume false
			if (findString != stateMap.end())
			{
				wasFound = true;
			}
			return wasFound;
		}
		void insertNewState(std::string in_planName, ContourPlanState in_planState)
		{
			std::lock_guard<std::mutex> lock(mapGuard);
			stateMap[in_planName] = in_planState;
		}
	private:
		std::mutex mapGuard;
		std::unordered_map<std::string, ContourPlanState> stateMap;
};

#endif
