#include "stdafx.h"
#include "SJPrepCPMountainRun.h"
#include "OSServer.h"

void SJPrepCPMountainRun::runJob(OrganicThread* in_threadToRunOn) 
{
	currentJobState = ServerJobState::RUNNING;	// set as running before we submit to the thread.
	in_threadToRunOn->submit(&ServerJobProxy::callServerJobPrepCPMountain, server, startMessage);	// main work; the call needs an intance of OSServer + plan meta data.							// main work
	in_threadToRunOn->submit(&ServerJobProxy::callServerJobSendUpdateMessageToJobManager, server, completionMessage);	// required: job completion message
}

std::string SJPrepCPMountainRun::getJobName() 
{
	return "";
}

void SJPrepCPMountainRun::runPostCompleteTasks() 
{

}

ServerJobRunVerdict SJPrepCPMountainRun::getCurrentVerdict()
{
	ServerJobRunVerdict currentVerdict;	// remember, the verdict should always starts as "false" 
	estimatedWorkLoad = 11.5f;		// hardcoded test.

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

	// the flag is already up from another running contour job that hasn't completed yet; so the contorued job isn't ready to run.
	/*

	Don't uncomment this; simply move it over to the new job that does the initial flagging, when you come back in January.
	else if (doesFlagExist == true)
	{
		ServerJobRunVerdict badVerdict(false, "TERRAIN");
		currentVerdict = badVerdict;
	}
	*/

	return currentVerdict;
}