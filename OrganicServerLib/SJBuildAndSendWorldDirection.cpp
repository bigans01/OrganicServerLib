#include "stdafx.h"
#include "SJBuildAndSendWorldDirection.h"

void SJBuildAndSendWorldDirection::runPrechecks()
{

}
void SJBuildAndSendWorldDirection::runJob(OrganicThread* in_threadToRunOn)
{
	std::cout << "#### Running job for updating world direction..." << std::endl;

	currentJobState = ServerJobState::RUNNING;	// set as running before we submit to the thread.
	in_threadToRunOn->submit(&ServerJobProxy::callServerJobSendSetWorldDirectionToClient, server, startMessage);
	in_threadToRunOn->submit(&ServerJobProxy::callServerJobSendUpdateMessageToJobManager, server, completionMessage);	// required: job completion message
}

std::string SJBuildAndSendWorldDirection::getJobName()
{
	return "";
}

void SJBuildAndSendWorldDirection::runPostCompleteTasks()
{

}