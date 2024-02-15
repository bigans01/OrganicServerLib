#include "stdafx.h"
#include "SJDetermineAffectedBlueprints.h"

void SJDetermineAffectedBlueprints::runJob(OrganicThread* in_threadToRunOn)
{
	currentJobState = ServerJobState::RUNNING;	// set as running before we submit to the thread.

	// fetch the string value from the start message, that has the contour plan's name; this is required for the call to 
	// callDetermineAffectedBlueprintsForCPV2 below.
	startMessage.open();
	std::string planName = startMessage.readString();

	in_threadToRunOn->submit(&ServerJobProxy::callDetermineAffectedBlueprintsForCPV2, server, planName);	// run fracturing and mass driving
	in_threadToRunOn->submit(&ServerJobProxy::callServerJobSendUpdateMessageToJobManager, server, completionMessage);	// required: job completion message
}

std::string SJDetermineAffectedBlueprints::getJobName()
{
	return "";
}

void SJDetermineAffectedBlueprints::runPostCompleteTasks()
{

}

ServerJobRunVerdict SJDetermineAffectedBlueprints::getCurrentVerdict()
{
	ServerJobRunVerdict returnVerdict(true, "TERRAIN");
	return returnVerdict;
}