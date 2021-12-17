#include "stdafx.h"
#include "SJSendRequestForOGLMRMCBlueprints.h"

void SJSendRequestForOGLMRMCBlueprints::runJob(OrganicThread* in_threadToRunOn)
{
	currentJobState = ServerJobState::RUNNING;	// set as running before we submit to the thread.							// main work
	//in_threadToRunOn->submit(&ServerJobProxy::callServerJobSendRequestToSendOGLMCubeFromClient, server);									// main work
	in_threadToRunOn->submit(&ServerJobProxy::callServerSendOutgoingMessageToMessageInterpreter, server, startMessage);		// send a message that has MessageType::REQUEST_FROM_SERVER_SEND_BLUEPRINTS_FOR_OGLMBUFFERMANAGER
	in_threadToRunOn->submit(&ServerJobProxy::callServerJobSendUpdateMessageToJobManager, server, completionMessage);	// required: job completion message
}

std::string SJSendRequestForOGLMRMCBlueprints::getJobName()
{
	return "";
}
void SJSendRequestForOGLMRMCBlueprints::runPostCompleteTasks()
{

}

ServerJobRunVerdict SJSendRequestForOGLMRMCBlueprints::getCurrentVerdict()
{
	ServerJobRunVerdict returnVerdict(true, "TERRAIN");
	estimatedWorkLoad = 1.0f;
	return returnVerdict;
}