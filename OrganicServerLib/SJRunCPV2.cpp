#include "stdafx.h"
#include "SJRunCPV2.h"

void SJRunCPV2::runJob(OrganicThread* in_threadToRunOn)
{
	currentJobState = ServerJobState::RUNNING;	// set as running before we submit to the thread.

	// fetch the string value from the start message, that has the contour plan;
	// we will also pass this same message down in the call to callServerRunCPV2 below.

	startMessage.open();
	std::string planName = startMessage.readString();
	in_threadToRunOn->submit(&ServerJobProxy::callServerRunCPV2, server, startMessage);	// run fracturing and mass driving
	in_threadToRunOn->submit(&ServerJobProxy::callServerJobSendUpdateMessageToJobManager, server, completionMessage);	// required: job completion message
}

std::string SJRunCPV2::getJobName()
{
	return "";
}

void SJRunCPV2::runPostCompleteTasks()
{
	// Once this job has attempted to run, be sure to deactivate the appropriate flags.
	ServerJobProxy::deactivateServerJobBlockingFlag(server, ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN);
	ServerJobProxy::deactivateServerJobBlockingFlag(server, ServerJobBlockingFlags::HALT_FUTURE_COLLECTION_MODIFICATIONS);
}

ServerJobRunVerdict SJRunCPV2::getCurrentVerdict()
{
	ServerJobRunVerdict currentVerdict;
	estimatedWorkLoad = 1.0f;

	// Below line is Test code only, does nothing; (just making sure it works for now)
	bool doesFlagExist = ServerJobProxy::checkIfServerJobBlockingFlagExists(server, ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN);
	if (doesFlagExist == false)
	{
		// if no, acquire/activate the ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN flag...this flag should be released by whatever server function is called by the
		// SJRunContourPlanFracturingAndMassDriving. 
		//
		// Also need to acquire the ServerJobBlockingFlags::HALT_FUTURE_COLLECTION_MODIFICATIONS flag;
		// this flag will be released by the function OSServer::buildContourPlanAffectedBlueprints, via the SJ SJBuildContourPlanAffectedBlueprints.
		ServerJobProxy::activateServerJobBlockingFlag(server, ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN);
		ServerJobProxy::activateServerJobBlockingFlag(server, ServerJobBlockingFlags::HALT_FUTURE_COLLECTION_MODIFICATIONS);

		currentVerdict.setTruthAndDesignatedString("TERRAIN");
	}


	return currentVerdict;
}
