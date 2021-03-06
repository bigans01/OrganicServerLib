#include "stdafx.h"
#include "ServerPhasedJobBase.h"
#include "OSServer.h"

void ServerPhasedJobBase::initializeBaseClass(OSServer* in_serverPtr, MessageLocality in_locality, ServerJobContainerType in_parentContainerType)
{
	server = in_serverPtr;
	locality = in_locality;
	parentContainerType = in_parentContainerType;
}

void ServerPhasedJobBase::insertParentContainerIntKey(int in_parentContainerKey)
{
	parentContainerIntKey = in_parentContainerKey;
}

void ServerPhasedJobBase::insertParentContainerStringKey(std::string in_parentContainerStringKey)
{
	parentContainerStringKey = in_parentContainerStringKey;
}

void ServerPhasedJobBase::insertStringedMessage(std::string in_stringName, Message in_message)
{
	stringedMessageMap[in_stringName] = in_message;
}

JobPhaseState ServerPhasedJobBase::getCurrentPhaseState()
{
	return currentPhaseIter->second->calculateAndReturnPhaseState();
}

void ServerPhasedJobBase::removeCompletedPhaseSubJobs()
{
	currentPhaseIter->second->removeDonePhaseSubJobs();	// remove the completed jobs.
	auto phaseEnd = phaseMap.rbegin();					// get the last phase in the map.
	int foundFlag = 0;
	currentPhaseIter->second->calculateAndReturnPhaseState();
	if (currentPhaseIter->second->phaseState == JobPhaseState::FINISHED)		// if the phase is done, do this.
	{
		if (currentPhaseIter->first != phaseEnd->first)		// as long as we aren't on the last phase, iterate.
		{
			std::cout << "##------ Found Flag 1 triggered! " << std::endl;
			foundFlag = 1;
		}
		else if (currentPhaseIter->first == phaseEnd->first)	// we are on the last phase, time to end the job.
		{
			std::cout << "### Found flag 2 triggered! " << std::endl;
			foundFlag = 2;
		}
	}

	if (foundFlag == 1)
	{
		currentPhaseIter++;	// iterate if found.
		//std::cout << "!! Next phase found, iterating. " << std::endl;
		//currentPhaseIter->second->
	}
	else if (foundFlag == 2)	// we didn't find another phase, so it's time to end this job.
	{
		jobState = ServerJobState::COMPLETE;
	}

}

void ServerPhasedJobBase::insertNewPhase(std::shared_ptr<ServerJobPhase> in_phasePtr)
{
	int currentMapIndexToUse = int(phaseMap.size());	// get the current index to use; for example, if the map is empty, this would be 0.
	phaseMap[currentMapIndexToUse] = in_phasePtr;
}

bool ServerPhasedJobBase::checkIfCurrentPhaseIsInProgress()
{
	bool isInProgress = false;
	if (currentPhaseIter->second->phaseState == JobPhaseState::IN_PROGRESS)
	{
		isInProgress = true;
	}
	return isInProgress;
}

ReadyJobSearch ServerPhasedJobBase::findNextWaitingJob()
{
	ReadyJobSearch jobSearchResult;

	auto currentPhaseJobMapBegin = currentPhaseIter->second->jobMap.begin();
	auto currentPhaseJobMapEnd = currentPhaseIter->second->jobMap.end();
	for (; currentPhaseJobMapBegin != currentPhaseJobMapEnd; currentPhaseJobMapBegin++)
	{
		if (currentPhaseJobMapBegin->second->currentJobState == ServerJobState::WAITING_TO_EXECUTE)
		{
			jobSearchResult.wasJobFound = true;
			jobSearchResult.currentJobPtr = &currentPhaseJobMapBegin->second;
		}
	}

	return jobSearchResult;
}