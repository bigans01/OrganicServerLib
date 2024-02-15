#pragma once

#ifndef SJCLEANUPCPV2_H
#define SJCLEANUPCPV2_H

#include "ServerJobBase.h"

/*

Description: 

meant to cleanup a CPV2 SPJ after it's been run

*/

class SJCleanupCPV2 : public ServerJobBase
{
	public:
		void runJob(OrganicThread* in_threadToRunOn);
		std::string getJobName();
		void runPostCompleteTasks();
		ServerJobRunVerdict getCurrentVerdict();
	private:
};

#endif