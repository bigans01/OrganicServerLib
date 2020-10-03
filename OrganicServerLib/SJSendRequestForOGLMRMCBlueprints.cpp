#include "stdafx.h"
#include "SJSendRequestForOGLMRMCBlueprints.h"

void SJSendRequestForOGLMRMCBlueprints::runPrechecks()
{
	estimatedWorkLoad = 1.0f;
}

void SJSendRequestForOGLMRMCBlueprints::runJob(OrganicThread* in_threadToRunOn)
{
	currentJobState = ServerJobState::RUNNING;	// set as running before we submit to the thread.
	//in_threadToRunOn->submit(&ServerJobProxy::callServerJobRunSingleMountTest, server);									// main work
	//in_threadToRunOn->submit(&ServerJobProxy::callServerJobRunBigMountTest, server);									// main work
	in_threadToRunOn->submit(&ServerJobProxy::callServerJobSendRequestToSendOGLMCubeFromClient, server);									// main work
	in_threadToRunOn->submit(&ServerJobProxy::callServerJobSendUpdateMessageToJobManager, server, completionMessage);	// required: job completion message
}

std::string SJSendRequestForOGLMRMCBlueprints::getJobName()
{
	return "";
}
void SJSendRequestForOGLMRMCBlueprints::runPostCompleteTasks()
{

}