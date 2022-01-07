#include "stdafx.h"
#include "SJRunContourPlanFracturingAndMassDriving.h"

void SJRunContourPlanFracturingAndMassDriving::runJob(OrganicThread* in_threadToRunOn)
{
	currentJobState = ServerJobState::RUNNING;	// set as running before we submit to the thread.

	// fetch the string value from the start message, that has the contour plan.
	startMessage.open();
	std::string planName = startMessage.readString();
	in_threadToRunOn->submit(&ServerJobProxy::callServerRunContourPlanFracturingAndMassDriving, server, planName);	// run fracturing and mass driving
	in_threadToRunOn->submit(&ServerJobProxy::callServerJobSendUpdateMessageToJobManager, server, completionMessage);	// required: job completion message
}

std::string SJRunContourPlanFracturingAndMassDriving::getJobName()
{
	return "";
}

void SJRunContourPlanFracturingAndMassDriving::runPostCompleteTasks()
{

}

ServerJobRunVerdict SJRunContourPlanFracturingAndMassDriving::getCurrentVerdict()
{
	ServerJobRunVerdict returnVerdict(true, "TERRAIN");
	estimatedWorkLoad = 1.0f;
	return returnVerdict;
}