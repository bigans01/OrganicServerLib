#pragma once

#ifndef SJAPPENDCPV2TRIANGLESTOSERVERBLUEPRINTS_H
#define SJAPPENDCPV2TRIANGLESTOSERVERBLUEPRINTS_H

#include "ServerJobBase.h"

/*

Description: 

This SJ copies over the per-blueprint FTriangle instances that the plan generated for each blueprint that it affected.
This is done by calling ServerJobProxy::callServerCopyOverCPV2ECBPolys.

*/

class SJAppendCPV2TrianglesToServerBlueprints : public ServerJobBase
{
	public:
		void runJob(OrganicThread* in_threadToRunOn);
		std::string getJobName();
		void runPostCompleteTasks();
		ServerJobRunVerdict getCurrentVerdict();
	private:
};

#endif