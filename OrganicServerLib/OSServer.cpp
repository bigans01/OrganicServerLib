#include "stdafx.h"
#include "OSServer.h"
//#include "OSContourPlan.h"
//#include "shitTest.h"

OSServer::OSServer()
{
	OrganicSystem Organic(3, 3, 13, 1024, 768, 3);
}

OSServer::OSServer(int x)
{
	OrganicSystem Organic(3, 3, 13, 1024, 768, 3);
}

void OSServer::addContourPlan(string in_string, float in_x, float in_y, float in_z)
{
	contourPlanMap[in_string].planMode = 0;	// add a new plan			<<< ---- error is here (FIXED)
}

OSContourPlan* OSServer::getContourPlan(string in_string)
{
	return &contourPlanMap[in_string];		// return a pointer to the plan
}