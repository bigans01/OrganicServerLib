#pragma once

#ifndef SJREQUESTCONTOURPLANLOCK_H
#define SJREQUESTCONTOURPLANLOCK_H

#include "ServerJobBase.h"

/*

Description: 

Intended to do the work required for "Phase 1" of a CPV2 contour plan running -- specifically, that means simply acquiring
the required contour plan lock so that the plan can get the signal to start. The main logic of this function should be based around the call to
getCurrentVerdict, which should issue a call to the ServerJobProxy::callAttempFlagActivation, and if the return value of that is true, simply
submit a job that records/logs the lock being acquired. The function SJRunCPV2::getCurrentVerdict can be used as a model to construct the verdict.

*/

class SJRequestContourPlanLock : public ServerJobBase
{
	public:
		void runJob(OrganicThread* in_threadToRunOn);
		std::string getJobName();
		void runPostCompleteTasks();
		ServerJobRunVerdict getCurrentVerdict();
	private:
};

#endif
