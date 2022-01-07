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
	ServerJobRunVerdict returnVerdict(true, "TERRAIN");
	estimatedWorkLoad = 1.0f;
	return returnVerdict;
}