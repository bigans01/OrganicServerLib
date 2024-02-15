#pragma once

#ifndef SJADDCPV2ANDPREPAREMESHPOINTS_H
#define SJADDCPV2ANDPREPAREMESHPOINTS_H

#include "ServerJobBase.h"

/*

Description: adds a CPV2 plan, initializes it, and then calls amplifyAllContourLinePoints, insertMaterials, and buildContouredTriangles on the plan, in that order.
This is done by using the function, callServerAddAndBuildCPV2MeshForSPJ.

*/

class SJAddCPV2AndPrepareMeshPoints : public ServerJobBase
{
	public:
		void runJob(OrganicThread* in_threadToRunOn);
		std::string getJobName();
		void runPostCompleteTasks();
		ServerJobRunVerdict getCurrentVerdict();
	private:
};

#endif