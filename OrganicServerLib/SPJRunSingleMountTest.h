#pragma once

#ifndef SPJRUNSINGLEMOUNTTEST_H
#define SPJRUNSINGLEMOUNTTEST_H

#include "ServerPhasedJobBase.h"

class SPJRunSingleMountTest : public ServerPhasedJobBase
{
	public:
		void initialize();	// initialization steps are always dependent on whether or not the message locality is LOCAL or REMOTE.
		void initializeCurrentPhase();
};

#endif
