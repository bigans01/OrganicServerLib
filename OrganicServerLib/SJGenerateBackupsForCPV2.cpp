#include "stdafx.h"
#include "SJGenerateBackupsForCPV2.h"

void SJGenerateBackupsForCPV2::runJob(OrganicThread* in_threadToRunOn)
{
	currentJobState = ServerJobState::RUNNING;	// set as running before we submit to the thread.

	// At this point in time, the below should only be called when the call to getCurrentVerdict() of this class returns true, 
	// which indicates that all of the blueprints needed by CPV2 have been loaded as "hot" (so that other jobs know not to work on them).
	in_threadToRunOn->submit(&ServerJobProxy::callServerGenerateBlueprintBackupsForCPV2SPJ, server);	// run fracturing and mass driving
	in_threadToRunOn->submit(&ServerJobProxy::callServerJobSendUpdateMessageToJobManager, server, completionMessage);	// required: job completion message
}

std::string SJGenerateBackupsForCPV2::getJobName()
{
	return "";
}

void SJGenerateBackupsForCPV2::runPostCompleteTasks()
{

}

ServerJobRunVerdict SJGenerateBackupsForCPV2::getCurrentVerdict()
{
	// The below call will attempt to lock all the blueprints that the CPV2 needs; they must all 
	// be locked in order to prevent other work/jobs from working on these claimed blueprints, in order to avoid
	// unwanted system behavior.
	ServerJobRunVerdict lockedVerdict;
	bool areCPV2BlueprintsLocked = ServerJobProxy::callServerAttemptCPV2KeyLoadIntoHotKeys(server);
	if (areCPV2BlueprintsLocked)
	{
		lockedVerdict.setTruthAndDesignatedString("TERRAIN");
	}

	return lockedVerdict;
}