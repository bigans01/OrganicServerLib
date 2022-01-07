#include "stdafx.h"
#include "SJBuildContourPlanAffectedBlueprints.h"

void SJBuildContourPlanAffectedBlueprints::runJob(OrganicThread* in_threadToRunOn)
{
	currentJobState = ServerJobState::RUNNING;	// set as running before we submit to the thread.

	// fetch the string value from the start message, that has the contour plan.
	startMessage.open();
	std::string planName = startMessage.readString();
	in_threadToRunOn->submit(&ServerJobProxy::callServerBuildContourPlanAffectedBlueprints, server, planName);	// build the contour plan's affected blueprints.
	in_threadToRunOn->submit(&ServerJobProxy::callServerJobSendUpdateMessageToJobManager, server, completionMessage);	// required: job completion message
}

std::string SJBuildContourPlanAffectedBlueprints::getJobName()
{
	return "";
}

void SJBuildContourPlanAffectedBlueprints::runPostCompleteTasks()
{

}

ServerJobRunVerdict SJBuildContourPlanAffectedBlueprints::getCurrentVerdict()
{
	ServerJobRunVerdict returnVerdict(true, "TERRAIN");
	estimatedWorkLoad = 1.0f;
	return returnVerdict;
}