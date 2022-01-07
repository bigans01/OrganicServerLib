#pragma once

#ifndef SJBUILDCONTOURPLANAFFECTEDBLUEPRINTS_H
#define SJBUILDCONTOURPLANAFFECTEDBLUEPRINTS_H

#include "ServerJobBase.h"

class SJBuildContourPlanAffectedBlueprints : public ServerJobBase
{
	public:
		void runJob(OrganicThread* in_threadToRunOn);
		std::string getJobName();
		void runPostCompleteTasks();
		ServerJobRunVerdict getCurrentVerdict();
};

#endif