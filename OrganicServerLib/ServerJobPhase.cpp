#include "stdafx.h"
#include "ServerJobPhase.h"

JobPhaseState ServerJobPhase::calculateAndReturnPhaseState()
{
	//std::cout << "#### Current phase stats: " << std::endl;
	//std::cout << "currentJobsCompleted: " << currentJobsCompleted << std::endl;
	//std::cout << "requiredJobsToBeCompleted: " << requiredJobsToBeCompleted << std::endl;

	if (currentJobsCompleted != requiredJobsToBeCompleted)
	{
		phaseState = JobPhaseState::IN_PROGRESS;
	}
	else if (currentJobsCompleted == requiredJobsToBeCompleted)		// if the total jobs remaining equals the currentJobsInProgress, it means they have all been pulled from the map, and there are none left to do.
	{
		//std::cout << "+++++Job phase flagged as FINISHED! " << std::endl;
		phaseState = JobPhaseState::FINISHED;
	}
	return phaseState;
}

void ServerJobPhase::removeDonePhaseSubJobs()
{
	loadDonePhaseSubjobs();
	eraseDonePhaseSubJobs();
}

void ServerJobPhase::loadDonePhaseSubjobs()
{
	auto jobsBegin = jobMap.begin();
	auto jobsEnd = jobMap.end();
	for (jobsBegin; jobsBegin != jobsEnd; jobsBegin++)
	{
		if (jobsBegin->second->currentJobState == ServerJobState::COMPLETE)		// if its complete, push it back into the completed vector.
		{
			//std::cout << "######### Found job to remove. " << std::endl;
			completedVector.push_back(jobsBegin->first);	// push back the element
		}
	}
}

void ServerJobPhase::eraseDonePhaseSubJobs()
{
	//std::cout << "!!! Attempting erase... " << std::endl;
	auto vectorBegin = completedVector.begin();
	auto vectorEnd = completedVector.end();
	for (vectorBegin; vectorBegin != vectorEnd; vectorBegin++)
	{
		//int someVal = *vectorBegin;
		//std::cout << "!!! Erasing job... " << std::endl;
		jobMap.erase(*vectorBegin);		// erase the element.

		//std::cout << "!!! Job map size is now: " << jobMap.size() << std::endl;
	}

	// clear the vector when done
	completedVector.clear();
}