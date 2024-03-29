#pragma once

#ifndef SERVERJOBMANAGER_H
#define SERVERJOBMANAGER_H

#include "ServerThreadDesignationMap.h"
#include <map>
#include <mutex>
#include "ServerPhasedJobBase.h"

#include "SPJBuildCPMountain.h"
#include "SPJBuildCPV2Mountain.h"
#include "SPJAdvBuildCPV2Mountain.h"

#include "SPJSendWorldDirectionToClient.h"

#include "ServerJobsIntContainer.h"
#include "ServerPhasedJobHierarchy.h"

#include "ServerJobUpdateQueue.h"
#include "ServerJobMessageQueue.h"
#include "ServerJobProxy.h"
#include "ServerJobBlockingFlagsSet.h"
#include "ReadyJobSearch.h"
#include "ServerThreadWorkloadUpdate.h"
#include "OSTerrainFormation.h"
#include "ServerJobRunCounters.h"


// Job life cycle:
// 1. ServerJobManager reads Messages from its Pending message queue, and creates new phased or non-phased jobs.
// 2. Sub-Jobs, phases, and Jobs that were completed are removed.
// 3. Messages that generate new jobs are checked, and if the check passes a new job is created.
// 4. The job is run on an available thread; at the end of the job, a job update is sent back to the ServerJobManager; there are no CVs or waits for the jobs to complete -- they are done when they are done, and they 
//    send a job update message to the ServerJobManager instance whenever something is complete (or fails). It should be noted, that this instance of ServerJobManager may receive
//    job updates from another server, for example in the event of a job being completed on another server. However, in this case, the job ID in the job update must already exist in this instance.

class OSServer;
class ServerJobManager
{
	private:
		friend class OSServer;
		friend class ServerMessageInterpreter;
		friend class ServerJobProxy;			// needed for ServerJobProxy::callServerJobSendUpdateMessageToJobManager

		OSServer* server;	// a pointer to the server instance that this ServerJobManager instance should be contained in.
		ServerThreadDesignationMap designations;




		// STEP 1: Read messages 
		ServerJobMessageQueue jobRequestQueue;

		// server job map
		ServerPhasedJobHierarchy spjHierarchy;
		ServerJobBlockingFlagsSet jobBlockingFlags;

		// STEP 3: job update containers (each one should have built in mutex for lock_guarding); 
		ServerJobUpdateQueue updateMessages;

		// run counters, for jobs of a specific type.
		ServerJobRunCounters runCounters;

		void initialize(OSServer* in_serverPtr);
		void startCommandLine();
		void insertJobRequestMessage(Message in_message);

		// STEP 2: insert jobs based into the appropraite containers, based on the message.
		void checkForUpdateMessages();		// 1: check for messages that would be updates to existing jobs.
		void removeCompletedPhasedJobs();   // 2: remove jobs flagged as completed in the phases they are in (this should also call the runPostCompleteTasks for 
											//	  each instance of ServerJobBase that is about to erased; this should be done through a series of function calls that ultimately results in a call to
											//	  runPostCompleteTaks for each job that is being removed.); remove ServerPhasedJobs that have been completed
		void checkForMessages();			// 3: check the messages that would spawn new jobs.
		void runJobScan();					// 4: look for jobs to execute.

		// message handling calls
		void handleContourPlanRequest(Message in_message);
		void handleSetDirectionRequest(Message in_message);

		// blocking flag operations
		bool doesFlagExist(ServerJobBlockingFlags in_flagToCheck);	// checks if exactly one flag exists.
		void activateBlockingFlag(ServerJobBlockingFlags in_flagToActivate);
		void deactivateBlockingFlag(ServerJobBlockingFlags in_flagToDeactivate);

		// flag raising attempt functions
		bool getAttemptedFlagRaiseResult(ServerJobBlockingFlags in_flagToCheck);

		// job insertion calls
		void insertPhasedJobRunCPV2Test(Message in_message);		// the TRUE test function.
		void insertPhasedJobSetWorldDirection(Message in_message);

		// phase checking
		void checkCurrentJobPhaseSetup(std::shared_ptr<ServerPhasedJobBase>* in_phasePtr);

		ServerThreadWorkloadUpdate readUpdateDataFromMessage(Message* in_messageRef);
};

#endif
