#include "stdafx.h"
#include "SJRunSingleMountTest.h"
#include "OSServer.h"

void SJRunSingleMountTest::runPrechecks() 
{
	estimatedWorkLoad = 11.5f;		// hardcoded test.
}

void SJRunSingleMountTest::runJob(OrganicThread* in_threadToRunOn) 
{
	currentJobState = ServerJobState::RUNNING;	// set as running before we submit to the thread.
	//in_threadToRunOn->submit(&ServerJobProxy::callServerJobRunSingleMountTest, server);									// main work
	in_threadToRunOn->submit(&ServerJobProxy::callServerJobRunBigMountTest, server);									// main work
	in_threadToRunOn->submit(&ServerJobProxy::callServerJobSendUpdateMessageToJobManager, server, completionMessage);	// required: job completion message
}

std::string SJRunSingleMountTest::getJobName() 
{
	return "";
}

void SJRunSingleMountTest::runPostCompleteTasks() 
{

}
