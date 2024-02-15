#include "stdafx.h"
#include "SJCleanupCPV2.h"

void SJCleanupCPV2::runJob(OrganicThread* in_threadToRunOn)
{
	currentJobState = ServerJobState::RUNNING;	// set as running before we submit to the thread.

	// This SJ will need to send the name of the plan the we will be cleaning up.
	startMessage.open();
	std::string planToCleanup = startMessage.readString();

	in_threadToRunOn->submit(&ServerJobProxy::callServerCleanupCPV2, server, planToCleanup);	// run fracturing and mass driving
	in_threadToRunOn->submit(&ServerJobProxy::callServerJobSendUpdateMessageToJobManager, server, completionMessage);	// required: job completion message
}

std::string SJCleanupCPV2::getJobName()
{
	return "";
}

void SJCleanupCPV2::runPostCompleteTasks()
{

}

ServerJobRunVerdict SJCleanupCPV2::getCurrentVerdict()
{
	// There shouldn't be any special verdict required here, by the time this SJ is run;
	// If done in the correct order, the verdict of an executed SJGenerateBackupsForCPV2 should have already locked
	// the affected blueprints exclusively for the SPJ that this SJ object would be called in. 
	//
	// In other words, this should be able to run immediately, under the aforementioned circumstances.
	ServerJobRunVerdict returnVerdict(true, "TERRAIN");
	return returnVerdict;
}