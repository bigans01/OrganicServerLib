#pragma once

#ifndef SJBUILDANDSENDWORLDDIRECTION_H
#define SJBUILDANDSENDWORLDDIRECTION_H

#include "ServerJobBase.h"

class SJBuildAndSendWorldDirection : public ServerJobBase
{
	public:
		void runPrechecks();
		void runJob(OrganicThread* in_threadToRunOn);
		std::string getJobName();
		void runPostCompleteTasks();
};

#endif