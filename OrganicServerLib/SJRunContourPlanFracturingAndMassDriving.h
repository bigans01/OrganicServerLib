#pragma once

#ifndef SJRUNCONTOURPLANTRACINGANDMASSDRIVING_H
#define SJRUNCONTOURPLANTRACINGANDMASSDRIVING_H

#include "ServerJobBase.h"

class SJRunContourPlanFracturingAndMassDriving : public ServerJobBase
{
	public:
		void runJob(OrganicThread* in_threadToRunOn);
		std::string getJobName();
		void runPostCompleteTasks();
		ServerJobRunVerdict getCurrentVerdict();
};


#endif