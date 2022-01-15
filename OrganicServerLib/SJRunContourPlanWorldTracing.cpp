#include "stdafx.h"
#include "SJRunContourPlanWorldTracing.h"

void SJRunContourPlanWorldTracing::runJob(OrganicThread* in_threadToRunOn)
{
	currentJobState = ServerJobState::RUNNING;	// set as running before we submit to the thread.

	// fetch the string value from the start message, that has the contour plan.
	startMessage.open();
	std::string planName = startMessage.readString();
	in_threadToRunOn->submit(&ServerJobProxy::callServerRunContourPlanWorldTracing, server, planName);	// run contour plan world tracing 
	in_threadToRunOn->submit(&ServerJobProxy::callServerJobSendUpdateMessageToJobManager, server, completionMessage);	// required: job completion message
}

std::string SJRunContourPlanWorldTracing::getJobName()
{
	return "";
}

void SJRunContourPlanWorldTracing::runPostCompleteTasks()
{

}

ServerJobRunVerdict SJRunContourPlanWorldTracing::getCurrentVerdict()
{
	// Remember, the default verdict value is always false, until it is set.
	ServerJobRunVerdict currentVerdict;

	// This job will only run as long as there are no active terrain modifications.
	if (ServerJobProxy::isJobCategoryAtZero(server, ServerJobRunCategory::TERRAIN_MODIFICATION) == true)
	{
		std::cout << "!! No active terrain modifications found; ContourPlan will begin world tracing. " << std::endl;
		currentVerdict.setTruthAndDesignatedString("TERRAIN");
		estimatedWorkLoad = 1.0f;
	}
	else
	{
		return currentVerdict;
	}
	return currentVerdict;
}