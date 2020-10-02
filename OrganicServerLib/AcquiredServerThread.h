#pragma once

#ifndef ACQUIREDSERVERTHREAD_H
#define ACQUIREDSERVERTHREAD_H

#include "OrganicThread.h"

class AcquiredServerThread
{
	public:
		AcquiredServerThread(int in_threadID, OrganicThread* in_threadPtr) : threadID(in_threadID), threadPtr(in_threadPtr) {};
		int threadID = 0;	// the ID of the acquired thread (if it was acquired). It should be equivalent to the map key of an existing ServerThreadWorkloadMonitor
		OrganicThread* threadPtr = NULL;
};

#endif
