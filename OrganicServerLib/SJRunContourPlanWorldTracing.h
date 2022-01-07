#pragma once

#ifndef SJRUNCONTOURPLANWORLDTRACING_H
#define SJRUNCONTOURPLANWORLDTRACING_H

#include "ServerJobBase.h"

class SJRunContourPlanWorldTracing : public ServerJobBase
{
	public:
		void runJob(OrganicThread* in_threadToRunOn);
		std::string getJobName();
		void runPostCompleteTasks();
		ServerJobRunVerdict getCurrentVerdict();
};

#endif
