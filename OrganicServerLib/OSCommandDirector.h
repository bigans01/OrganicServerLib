#pragma once

#ifndef OSCOMMANDDIRECTOR_H
#define OSCOMMANDDIRECTOR_H

#include "CommandContainerManager.h"
#include <mutex>

class OSServer;
class OSCommandDirector
{
public:
	CommandContainerManager OScommandListener;
	//std::mutex* assignedHeapMutex;
	OSServer* OSptr;
	void initialize(OSServer* in_organicServerPtr);
};

#endif
