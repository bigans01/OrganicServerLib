#pragma once

#ifndef SERVERTHREADWORKLOADMONITOR_H
#define SERVERTHREADWORKLOADMONITOR_H

#include "OrganicThread.h"

class ServerThreadWorkloadMonitor
{
	public:
		ServerThreadWorkloadMonitor() {};
		ServerThreadWorkloadMonitor(OrganicThread* in_organicThread) : threadPtr(in_organicThread) {};
		void incrementWorkLoad(float in_workload) { currentWorkload += in_workload; };
		void decrementWorkLoad(float in_workload) { currentWorkload -= in_workload; };
		OrganicThread* getThreadPtr() { return threadPtr; };
	private:
		float currentWorkload = 0.0f;	
		OrganicThread* threadPtr = NULL;
};

#endif
