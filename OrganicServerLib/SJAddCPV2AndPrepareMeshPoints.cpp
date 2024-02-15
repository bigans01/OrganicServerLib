#include "stdafx.h"
#include "SJAddCPV2AndPrepareMeshPoints.h"

void SJAddCPV2AndPrepareMeshPoints::runJob(OrganicThread* in_threadToRunOn)
{
	currentJobState = ServerJobState::RUNNING;	// set as running before we submit to the thread.

	// all of the meta data for the plan should be contained in the startMessage that is passed to this SJ, in the call to setStartMessage in this class.
	in_threadToRunOn->submit(&ServerJobProxy::callServerAddAndBuildCPV2MeshForSPJ, server, startMessage);	// run fracturing and mass driving
	in_threadToRunOn->submit(&ServerJobProxy::callServerJobSendUpdateMessageToJobManager, server, completionMessage);	// required: job completion message
}

std::string SJAddCPV2AndPrepareMeshPoints::getJobName()
{
	return "";
}

void SJAddCPV2AndPrepareMeshPoints::runPostCompleteTasks()
{

}

ServerJobRunVerdict SJAddCPV2AndPrepareMeshPoints::getCurrentVerdict()
{
	/*
	ServerJobRunVerdict currentVerdict;
	bool flagAcquired = ServerJobProxy::callAttemptFlagActivation(server, ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN);
	if (flagAcquired)
	{
		currentVerdict.setTruthAndDesignatedString("TERRAIN");
	}
	else
	{
		std::cout << "Activation failed!! " << std::endl;
	}
	*/

	ServerJobRunVerdict currentVerdict(true, "TERRAIN");

	return currentVerdict;
}