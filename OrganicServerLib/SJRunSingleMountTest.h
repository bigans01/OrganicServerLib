#pragma once

#ifndef SJRUNSINGLEMOUNTTEST_H
#define SJRUNSINGLEMOUNTTEST_H

#include "ServerJobBase.h"

class SJRunSingleMountTest : public ServerJobBase
{
	public:
		void runPrechecks();
		void runJob(OrganicThread* in_threadToRunOn);
		std::string getJobName();
		void runPostCompleteTasks();
};

#endif
