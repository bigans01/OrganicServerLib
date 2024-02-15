#pragma once

#ifndef SJGENERATEBACKUPSFORCPV2_H
#define SJGENERATEBACKUPSFORCPV2_H

#include "ServerJobBase.h"

/*

Description: 

This SJ uses the getCurrentVerdict() function to attempt a lock (i.e, flag as "Hot") all the blueprints that
a CPV2 needs to run, by calling attemptLoadOfCPV2KeysIntoHotKeys in the hotBPManager of the OSServer class. This will
effectively lock all blueprints that a CPV2 needs to run on, before running. 

Once this check passes, the runJob function will actually issue a call to do a backup of the blueprints.

*/

class SJGenerateBackupsForCPV2 : public ServerJobBase
{
	public:
		void runJob(OrganicThread* in_threadToRunOn);
		std::string getJobName();
		void runPostCompleteTasks();
		ServerJobRunVerdict getCurrentVerdict();
	private:
};

#endif
