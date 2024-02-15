#include "stdafx.h"
#include "SJAppendCPV2TrianglesToServerBlueprints.h"

void SJAppendCPV2TrianglesToServerBlueprints::runJob(OrganicThread* in_threadToRunOn)
{
	currentJobState = ServerJobState::RUNNING;	// set as running before we submit to the thread.

	// This SJ requires that the plan name to copy the data from be provided; the startMessage of this SJ should be populated
	// with the plan name.
	startMessage.open();
	std::string planToAppendFrom = startMessage.readString();

	in_threadToRunOn->submit(&ServerJobProxy::callServerCopyOverCPV2ECBPolys, server, planToAppendFrom);	// run fracturing and mass driving
	in_threadToRunOn->submit(&ServerJobProxy::callServerJobSendUpdateMessageToJobManager, server, completionMessage);	// required: job completion message
}

std::string SJAppendCPV2TrianglesToServerBlueprints::getJobName()
{
	return "";
}

void SJAppendCPV2TrianglesToServerBlueprints::runPostCompleteTasks()
{

}

ServerJobRunVerdict SJAppendCPV2TrianglesToServerBlueprints::getCurrentVerdict()
{
	// There shouldn't be any special verdict required here, by the time this SJ is run;
	// If done in the correct order, the verdict of an executed SJGenerateBackupsForCPV2 should have already locked
	// the affected blueprints exclusively for the SPJ that this SJ object would be called in. 
	//
	// In other words, this should be able to run immediately, under the aforementioned circumstances.
	ServerJobRunVerdict returnVerdict(true, "TERRAIN");
	return returnVerdict;
}