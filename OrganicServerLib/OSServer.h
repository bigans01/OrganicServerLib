#pragma once
#ifndef OSSERVER_H
#define OSSERVER_H
#include <iostream>
#include "OrganicSystem.h"
#include "OSContourPlan.h"
#include <unordered_map>
#include <string>



class OSServer
{
public:
	
	OSServer();
	OSServer(int x);

	void addContourPlan(string in_string, float in_x, float in_y, float in_z);
	OSContourPlan* getContourPlan(string in_string);
	std::unordered_map<string, OSContourPlan> contourPlanMap;
	
};

#endif