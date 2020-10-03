#pragma once

#ifndef SJSENDREQUESTFOROGLMRMCBLUEPRINTS_H
#define SJSENDREQUESTFOROGLMRMCBLUEPRINTS_H

#include "ServerJobBase.h"

class SJSendRequestForOGLMRMCBlueprints : public ServerJobBase
{
public:
	void runPrechecks();
	void runJob(OrganicThread* in_threadToRunOn);
	std::string getJobName();
	void runPostCompleteTasks();
};

#endif 
