#pragma once

#ifndef SJPrepCPMountainRun_H
#define SJPrepCPMountainRun_H

#include "ServerJobBase.h"

class SJPrepCPMountainRun : public ServerJobBase
{
	public:
		void runJob(OrganicThread* in_threadToRunOn);
		std::string getJobName();
		void runPostCompleteTasks();
		ServerJobRunVerdict getCurrentVerdict();
};

#endif
