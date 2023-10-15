#pragma once

#ifndef SJRUNCPV2_H
#define SJRUNCPV2_H

#include "ServerJobBase.h"

/*

Summary: runs a single ContourPlanV2Base-derived plan; this also includes creating backups of the
to-be affected blueprints, in case something goes wrong (see this logic in the function ContouredPlanV2Base::copyOverForSPJ,
for further elaboration and understanding).

*/

class SJRunCPV2 : public ServerJobBase
{
	public:
		void runJob(OrganicThread* in_threadToRunOn);
		std::string getJobName();
		void runPostCompleteTasks();
		ServerJobRunVerdict getCurrentVerdict();
};

#endif
