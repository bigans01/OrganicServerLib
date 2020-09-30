#pragma once

#ifndef SERVERJOBMANAGER_H
#define SERVERJOBMANAGER_H

#include "ServerThreadDesignationMap.h"
#include <map>
#include <mutex>
#include "ServerPhasedJobBase.h"
#include "SPJRunSingleMountTest.h"
#include "ServerJobsIntContainer.h"
#include "ServerJobUpdateIntQueue.h"
#include "ServerJobMessageQueue.h"

// Job life cycle:
// 1. ServerJobManager reads Messages from its Pending message queue, and creates new phased or non-phased jobs.
// 2. The job is run on an available thread; at the end of the job, a job update is sent back to the ServerJobManager; there are no CVs or waits for the jobs to complete -- they are done when they are done, and they 
//    send a job update message to the ServerJobManager instance whenever something is complete (or fails). It should be noted, that this instance of ServerJobManager may receive
//    job updates from another server, for example in the event of a job being completed on another server. However, in this case, the job ID in the job update must already exist in this instance.
// 3. In the next tick, the server reads from the job updates and updates jobs appropriately, based off the appropriate ServerJobUpdateQueue(int, string) setc)

class OSServer;
class ServerJobManager
{
	public:
		OSServer* server;	// a pointer to the server instance that this ServerJobManager instance should be contained in.
		ServerThreadDesignationMap designations;

		void initialize(OSServer* in_serverPtr);
		void startCommandLine();

		// server job map
		//std::map<int, std::shared_ptr<ServerPhasedJobBase>> serverJobs;
		ServerJobsIntContainer intJobsContainer;

		// job insertion calls
		void insertPhasedJobRunSingleMountTest(Message in_message);		// the TRUE test function.

		// STEP 1: Read messages 
		ServerJobMessageQueue messageQueue;

		// STEP 2: insert jobs based into the appropraite containers, based on the message.
		void checkForMessages();

		// STEP 3: job update containers (each one should have built in mutex for lock_guarding); 
		ServerJobUpdateIntQueue updatesViaInt;
};

#endif
