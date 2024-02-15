#pragma once

#ifndef SJDETERMINEAFFECTEDBLUEPRINTS_H
#define SJDETERMINEAFFECTEDBLUEPRINTS_H

#include "ServerJobBase.h"

/*

Description:

Designed to call determineAffectedBlueprintsForCPV2 on the current plan that is being prepared for a run. this culminates in
the pillarCPV2Keys of the HotBlueprints member of OSServer being populated. 

Does no checks to see if the plan exists, or if the previous steps are done; those checks are outside the scope of this SJ. 

*/

class SJDetermineAffectedBlueprints : public ServerJobBase
{
	public:
		void runJob(OrganicThread* in_threadToRunOn);
		std::string getJobName();
		void runPostCompleteTasks();
		ServerJobRunVerdict getCurrentVerdict();
	private:
};


#endif
