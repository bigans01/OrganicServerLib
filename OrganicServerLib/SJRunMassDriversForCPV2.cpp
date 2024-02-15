#include "stdafx.h"
#include "SJRunMassDriversForCPV2.h"

void SJRunMassDriversForCPV2::runJob(OrganicThread* in_threadToRunOn)
{
	currentJobState = ServerJobState::RUNNING;	// set as running before we submit to the thread.

	// This SJ requires that the plan name to copy the data from be provided; the startMessage of this SJ should be populated
	// with the plan name.
	startMessage.open();
	std::string planToAppendFrom = startMessage.readString();

	in_threadToRunOn->submit(&ServerJobProxy::callServerRunMassDriversForCPV2, server, planToAppendFrom);	// run fracturing and mass driving
	in_threadToRunOn->submit(&ServerJobProxy::callServerJobSendUpdateMessageToJobManager, server, completionMessage);	// required: job completion message
}

std::string SJRunMassDriversForCPV2::getJobName()
{
	return "";
}

void SJRunMassDriversForCPV2::runPostCompleteTasks()
{

}

ServerJobRunVerdict SJRunMassDriversForCPV2::getCurrentVerdict()
{
	// There shouldn't be any special verdict required here, by the time this SJ is run;
	// If done in the correct order, the verdict of an executed SJGenerateBackupsForCPV2 should have already locked
	// the affected blueprints exclusively for the SPJ that this SJ object would be called in. 
	//
	// In other words, this should be able to run immediately, under the aforementioned circumstances.
	ServerJobRunVerdict returnVerdict(true, "TERRAIN");
	return returnVerdict;
}