#pragma once

#ifndef SJRUNMASSDRIVERSFORCPV2_H
#define SJRUNMASSDRIVERSFORCPV2_H

#include "ServerJobBase.h"

/*

Description: 

Runs the mass driver function for a specified CPV2. The name of the CPV2 is contained within the startMessage.

*/

class SJRunMassDriversForCPV2 : public ServerJobBase
{
	public:
		void runJob(OrganicThread* in_threadToRunOn);
		std::string getJobName();
		void runPostCompleteTasks();
		ServerJobRunVerdict getCurrentVerdict();
	private:

};

#endif
