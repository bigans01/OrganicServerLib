#include "stdafx.h"
#include "SJRunSingleMountTest.h"
#include "OSServer.h"

void SJRunSingleMountTest::runJob(OrganicThread* in_threadToRunOn) 
{
	currentJobState = ServerJobState::RUNNING;	// set as running before we submit to the thread.
	in_threadToRunOn->submit(&ServerJobProxy::callServerJobRunSingleMountTest, server, startMessage);	// main work; the call needs an intance of OSServer + plan meta data.							// main work
	in_threadToRunOn->submit(&ServerJobProxy::callServerJobSendUpdateMessageToJobManager, server, completionMessage);	// required: job completion message
}

std::string SJRunSingleMountTest::getJobName() 
{
	return "";
}

void SJRunSingleMountTest::runPostCompleteTasks() 
{

}

ServerJobRunVerdict SJRunSingleMountTest::getCurrentVerdict()
{
	ServerJobRunVerdict currentVerdict(true, "TERRAIN");
	estimatedWorkLoad = 11.5f;		// hardcoded test.

	// Below line is Test code only, does nothing; (just making sure it works for now)
	bool doesFlagExist = ServerJobProxy::checkIfServerJobBlockingFlagExists(server, ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN);
	if (doesFlagExist == false)
	{
		// if no, acquire/activate the flag...

		// and then flag the blocking flag for HALT_FUTURE_COLLECTION_MODIFICATIONS; only one contour plan should be flagging this at a time.
		// A different "atomic" server job will need to release this flag, after the contour plan's affected blueprint list has been generated.
		// 
		// Steps would be (assuming ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN is acquired by this job):
		// 
		// 1. A placebo ServerJob checks if the HALT_FUTURE_COLLECTION_MODIFICATIONS flag is activated, acquires/activates it if not. This job will get submitted, but will probably do nothing (placebo).
		//    The HALT_FUTURE_COLLECTION_MODIFICATIONS will prevent other block jobs from being submitted; current block jobs that are queued to run will have to finish. 
		//
		// 2. In the next phase of the phased job, there will be a single job that waits for all existing collection modifications (i.e, block edits) to be at 0;
		//	  this can be done by checking some sort of counter in the system that decrements when a block modification (or similiar) is finished. When this value is 0,
		//	  this job (#2) can run on the next call to runJobScan() -- which will generate the contour plan's affected blueprint list. 
		//
		// 3. In the next phase, call a placebo job that releases the HALT_FUTURE_COLLECTION_MODIFICATIONS flag; any other block modification jobs may now run.
		//
		// 4. At the very end of the phased job, the last job should be one that releases the SERVER_RUNNING_CONTOUR_PLAN flag.

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