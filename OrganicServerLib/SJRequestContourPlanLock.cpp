#include "stdafx.h"
#include "SJRequestContourPlanLock.h"

void SJRequestContourPlanLock::runJob(OrganicThread* in_threadToRunOn)
{
	currentJobState = ServerJobState::RUNNING;	// set as running before we submit to the thread.

	// fetch the string value from the start message, that has the contour plan.
	//startMessage.open();
	//std::string planName = startMessage.readString();

	// we shouldn't have to run any job here; just submit a message to indicate that we're done. Remember, 
	// this SJ is really intended to pass only when the verdict is called for it to be executed.

	//in_threadToRunOn->submit(&ServerJobProxy::callServerCheckContourPlanSuccess, server, planName);	// run fracturing and mass driving
	in_threadToRunOn->submit(&ServerJobProxy::callServerJobSendUpdateMessageToJobManager, server, completionMessage);	// required: job completion message
}

std::string SJRequestContourPlanLock::getJobName()
{
	return "";
}

void SJRequestContourPlanLock::runPostCompleteTasks()
{

}

ServerJobRunVerdict SJRequestContourPlanLock::getCurrentVerdict()
{
	ServerJobRunVerdict currentVerdict;
	bool flagAcquired = ServerJobProxy::callAttemptFlagActivation(server, ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN);
	if (flagAcquired)
	{
		currentVerdict.setTruthAndDesignatedString("TERRAIN");
	}

	return currentVerdict;
}