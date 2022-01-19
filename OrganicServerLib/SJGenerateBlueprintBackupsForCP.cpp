#include "stdafx.h"
#include "SJGenerateBlueprintBackupsForCP.h"

void SJGenerateBlueprintBackupsForCP::runJob(OrganicThread* in_threadToRunOn)
{
	currentJobState = ServerJobState::RUNNING;	// set as running before we submit to the thread.

	// fetch the string value from the start message, that has the contour plan.
	startMessage.open();
	std::string planName = startMessage.readString();
	in_threadToRunOn->submit(&ServerJobProxy::callServerGenerateBlueprintBackupsForCP, server, planName);	// run fracturing and mass driving
	in_threadToRunOn->submit(&ServerJobProxy::callServerJobSendUpdateMessageToJobManager, server, completionMessage);	// required: job completion message
}

std::string SJGenerateBlueprintBackupsForCP::getJobName()
{
	return "";
}

void SJGenerateBlueprintBackupsForCP::runPostCompleteTasks()
{

}
ServerJobRunVerdict SJGenerateBlueprintBackupsForCP::getCurrentVerdict()
{
	ServerJobRunVerdict returnVerdict(true, "TERRAIN");
	estimatedWorkLoad = 1.0f;
	return returnVerdict;
}