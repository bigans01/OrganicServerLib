#include "stdafx.h"
#include <stdio.h>
#include "OSCommandDirector.h"
#include "OSServer.h"

void OSCommandDirector::initialize(OSServer* in_organicServerPtr, std::mutex& in_mutex)
{
	OSptr = in_organicServerPtr;
	assignedHeapMutex = &in_mutex;
}