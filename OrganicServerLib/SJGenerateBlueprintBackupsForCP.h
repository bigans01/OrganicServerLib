#pragma once

#ifndef SJGENERATEBLUEPRINTBACKUPSFORCP_H
#define SJGENERATEBLUEPRINTBACKUPSFORCP_H

#include "ServerJobBase.h"

class SJGenerateBlueprintBackupsForCP : public ServerJobBase
{
	public:
		void runJob(OrganicThread* in_threadToRunOn);
		std::string getJobName();
		void runPostCompleteTasks();
		ServerJobRunVerdict getCurrentVerdict();
	private:
};

#endif
