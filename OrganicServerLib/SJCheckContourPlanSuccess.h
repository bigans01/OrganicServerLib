#pragma once

#ifndef SJCHECKCONTOURPLANSUCCESS_H
#define SJCHECKCONTOURPLANSUCCESS_H

#include "ServerJobBase.h"

class SJCheckContourPlanSuccess : public ServerJobBase
{
	public:
		void runJob(OrganicThread* in_threadToRunOn);
		std::string getJobName();
		void runPostCompleteTasks();
		ServerJobRunVerdict getCurrentVerdict();
	private:
};

#endif
