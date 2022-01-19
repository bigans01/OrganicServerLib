#include "stdafx.h"
#include "SJCheckContourPlanSuccess.h"

void SJCheckContourPlanSuccess::runJob(OrganicThread* in_threadToRunOn)
{
	currentJobState = ServerJobState::RUNNING;	// set as running before we submit to the thread.

// fetch the string value from the start message, that has the contour plan.
	startMessage.open();
	std::string planName = startMessage.readString();
	in_threadToRunOn->submit(&ServerJobProxy::callServerCheckContourPlanSuccess, server, planName);	// run fracturing and mass driving
	in_threadToRunOn->submit(&ServerJobProxy::callServerJobSendUpdateMessageToJobManager, server, completionMessage);	// required: job completion message
}

std::string SJCheckContourPlanSuccess::getJobName()
{
	return "";
}

void SJCheckContourPlanSuccess::runPostCompleteTasks()
{

}

ServerJobRunVerdict SJCheckContourPlanSuccess::getCurrentVerdict()
{
	ServerJobRunVerdict returnVerdict(true, "TERRAIN");
	estimatedWorkLoad = 1.0f;
	return returnVerdict;
}
