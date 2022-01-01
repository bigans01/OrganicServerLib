#include "stdafx.h"
#include "ServerJobManager.h"
#include "OSServer.h"

void ServerJobManager::initialize(OSServer* in_serverPtr)
{
	server = in_serverPtr;
	intJobsContainer.server = server;
	designations.initialize(&server->OSCManager);
	designations.buildInitialUndesignatedPool();
	designations.designateCommandLineThread(0);							// thread 0 from the pool should be the command line thread.
	designations.designateTerrainThread(1);								// thread 1 from the pool should be the the terrain thread.
	// rest of threads come here...when developed appropriately in time.
}

void ServerJobManager::startCommandLine()
{
	OrganicThread* targetThread = designations.getCommandLineThread();

	//int commandLineRunning = server->isCommandLineRunning;
	//int* clShutdownStatus = &server->isCommandLineShutDown;
	//std::future<int> testFuture2 = OSCManager.stemcellMap[0].threadPtr->submit(&OSServer::runCommandLine, this, std::ref(serverReadWrite), std::ref(commandLineCV), std::ref(isCommandLineRunning), std::ref(clShutdownStatus));
	//targetThread->submit(&OSServer::runCommandLine, server, std::ref(server->serverReadWrite), std::ref(server->commandLineCV), std::ref(commandLineRunningRef), std::ref(clShutdownStatus));
	//targetThread->submit(&OSServer::runCommandLineV2, server, std::ref(server->serverReadWrite), std::ref(server->isCommandLineRunning), std::ref(clShutdownStatus));
	targetThread->submit(&OSServer::runCommandLineV3, server);
}

void ServerJobManager::insertJobRequestMessage(Message in_message)
{
	jobRequestQueue.insertMessage(std::move(in_message));
}

void ServerJobManager::insertPhasedJobRunSingleMountTest(Message in_message)		// the TRUE test function
{
	std::shared_ptr<ServerPhasedJobBase> job(new (SPJRunSingleMountTest));
	intJobsContainer.insertJob(1, &job, std::move(in_message));	// SPJ-F1
}

void ServerJobManager::insertPhasedJobSetWorldDirection(Message in_message)
{
	std::cout << "############# Inserted set direction job..." << std::endl;
	std::shared_ptr<ServerPhasedJobBase> job(new (SPJSendWorldDirectionToClient));
	Message directionMessage = in_message;
	job->insertStringedMessage("direction", directionMessage);

	intJobsContainer.insertJob(2, &job, std::move(in_message));	// SPJ-F1
	std::cout << ">> Finished inserting set direction job..." << std::endl;
}

void ServerJobManager::checkForUpdateMessages()
{
	while (!updateMessages.isEmpty())
	{
		Message* currentMessageRef = updateMessages.getMessageRefFromFront();
		currentMessageRef->open();

		// after opening the message, read the update data.
		ServerThreadWorkloadUpdate updateData = readUpdateDataFromMessage(currentMessageRef);
		std::cout << "---> Job update updateData workload to update is: " << updateData.workload << std::endl;

		switch (currentMessageRef->messageType)
		{
			// All update messages must have an appropriate MessageType, to direct it to the proper job container.
			// Allowed values are:
			//	--MessageType::SERVER_JOB_EVENT_UPDATE_INT 

			case MessageType::SERVER_JOB_EVENT_UPDATE_INT :		// if the server job is of this message type, it must be sent to jobs in the intJobsContainer.
			{
				int parentJobID = currentMessageRef->readInt();
				intJobsContainer.handleUpdateMessage(std::move(*currentMessageRef));	// SPJ-F4
				break;
			}
		}

		designations.decrementWorkload(updateData.threadID, updateData.workload);		// decrement the workload, based on what we read from the message.
		updateMessages.safePopQueue();													// pop the message
	}
}

ServerThreadWorkloadUpdate ServerJobManager::readUpdateDataFromMessage(Message* in_messageRef)
{
	// read the int
	auto targetThread = in_messageRef->intVector.rbegin();
	auto workload = in_messageRef->floatVector.rbegin();

	ServerThreadWorkloadUpdate update;
	update.threadID = *targetThread;
	update.workload = *workload;

	return update;
}

void ServerJobManager::checkForMessages()
{
	/* ######################### --Old switch style sample (without using std::move in a handling function) :
		case MessageType::REQUEST_FROM_CLIENT_RUN_CONTOUR_PLAN :
			{
				
				std::cout << "SERVER: JobManager found contour plan request. " << std::endl;
				std::string planName = currentMessageRef->readString();											// remember, every read increments the string.
				bool wasContouredPlanFound = server->planStateContainer.checkIfStateExistsForPlan(planName);	// check if the plan has a state; if it does, we won't run this plan (because of the rule: a single contour plan may only run once.)
				if (wasContouredPlanFound == false)																// it wasn't found as having a state, lets run it.
				{
					server->planStateContainer.insertNewState(planName, ContourPlanState::WAITING_TO_RUN);
					OrganicThread* targetThread = designations.getFirstAvailableThread();
					targetThread->submit(&ServerJobProxy::callServerJobRunSingleMountTest, server);
				}
				else if (wasContouredPlanFound == true)															// drop the job request.
				{
					std::cout << "SERVER: plan " << planName << "already has a state; will not insert job. " << std::endl;
				}
				
				handleContourPlanRequest(std::move(*currentMessageRef));

				break;
			}
	
	*/

	while (!jobRequestQueue.isEmpty())
	{
		Message* currentMessageRef = jobRequestQueue.getMessageRefFromFront();		// get a ref to the message
		currentMessageRef->open();												// open the message
		switch (currentMessageRef->messageType)
		{
			std::cout << "!!! Message found. " << std::endl;
			case MessageType::REQUEST_FROM_CLIENT_RUN_CONTOUR_PLAN : {  handleContourPlanRequest(std::move(*currentMessageRef));  break;  }
			case MessageType::REQUEST_FROM_SERVER_SET_WORLD_DIRECTION: { handleSetDirectionRequest(std::move(*currentMessageRef)); break; }

		}
		jobRequestQueue.safePopQueue();
	}
}

void ServerJobManager::runJobScan()
{
	/* 
	
	The running of jobs in the OrganicServer is different, than how they run in an OrganicCoreLib; 
	The OrganicServer job engine does not periodically "pause" for all threads, like the ticks in an OrganicSystem instance.

	Additionally, jobs to submit are done via round-robin style -- this means that each ServerPhasedJob-derived class
	gets an opportunity to submit exactly one job in per server tick, to the underlying threads, and only if that job to be submitted meets
	three conditions:

	1. The job is waiting to run
	2. The designated thread for the job exists
	3. The job meets the criteria to be run

	Every tick, each job in the tree is analyzed from the highest priority jobs (usually beginning at 0) to the lowest priority jobs.
	For example, terrain modification/alteration operations are going to be higher priority than an operation where a single block is needed to be 
	changed. I.e, if in OrganicServer tick 0 (T0), there is only a single block modification job (priority 1) and nothing else, that will definitely run. 
	However, if in OrganicServer T1, there is a terrain modification/alteration operation (priority 0) AND a single block modification (priority 1),
	Then the priority 0 will always run first and get any necessary locks, and likely preventing the single block modification from running in that tick.
	At it's end, the terrain modificaition/alteration operation will release its locks, and then that single block modification that might still exist in T2 will be
	allowed to run (if it needs to).

	This system allows us to only have to worry about the level of priority for each job type, and ensure that the order of checks and locks follows an
	appropriate hierarchy.

	Remember, as of 12/16/2021, the intJobsContainer is still very simple -- it's an std::map (int) of ServerJobs to run; so, one
	int, one job. This should be reworked early next year (January)
	
	*/

	// SPJ-F5 (OK, it seems...)
	auto jobList = intJobsContainer.produceRunnableJobs();
	auto intServerJobsBegin = jobList.begin();
	auto intServerJobsEnd = jobList.end();
	for (; intServerJobsBegin != intServerJobsEnd; intServerJobsBegin++)
	{
		//std::cout << "!!! Found job to run. " << std::endl;

		checkCurrentJobPhaseSetup(&(*intServerJobsBegin));
		ReadyJobSearch searchForAvailableJobInCurrentPhase = (*intServerJobsBegin)->findNextWaitingJob();		// don't run jobs that are already executing.

		// Check 1: we found a job to run.
		if (searchForAvailableJobInCurrentPhase.wasJobFound == true)
		{
			// Check 2: have the current target job do pre-checks to ensure it will run;
			// the call to isCurrentJobRunnable() will call the getCurrentVerdict() virtual function of the job,
			// to determine if that job should run, and additionally set its workload if needed. If the job can run, 
			// we will proceed. In addition to this, the return value of this function will only be true if the designated thread exists.
			if ((*intServerJobsBegin)->isCurrentJobRunnable(&designations) == true)
			{
				float calculatedWorkload = (*searchForAvailableJobInCurrentPhase.currentJobPtr)->estimatedWorkLoad;		// ..grab it (for readability purposes)

				// std::cout << "---> Acquired estimated workload is: " << calculatedWorkload << std::endl;

				//AcquiredServerThread acquiredThread = designations.getFirstAvailableThread();							// get the acquired thread data
				AcquiredServerThread acquiredThread = designations.fetchDesignatedThread((*intServerJobsBegin)->fetchThreadDesignation());							// get the acquired thread data
				designations.incrementWorkload(acquiredThread.threadID, calculatedWorkload);							// increment the workload for the monitor that's wrapped around the thread we are about to submit to

				(*searchForAvailableJobInCurrentPhase.currentJobPtr)->appendMatchedThreadAndWorkLoadToMessage(acquiredThread.threadID);		// append the monitor ID, and the job's current workload to the message.
				(*searchForAvailableJobInCurrentPhase.currentJobPtr)->runJob(acquiredThread.threadPtr);		// finally, run the job; and set 
																												// the ServerPhasedJob's status to ServerJobState::RUNNING
			}
		}
	}
	
	
}

void ServerJobManager::removeCompletedPhasedJobs()
{
	intJobsContainer.cleanupJobsInPhasedJobs();
	intJobsContainer.removeCompletedPhasedJobs();

	// SPJ-F2
	// SPJ-F3
}

void ServerJobManager::checkCurrentJobPhaseSetup(std::shared_ptr<ServerPhasedJobBase>* in_phasePtr)
{
	bool isInProgress = (*in_phasePtr)->checkIfCurrentPhaseIsInProgress();
	if (isInProgress == false)															// if the phase isn't in progress, start it
	{
		(*in_phasePtr)->initializeCurrentPhase();										// initialize FIRST, then...
	}
	(*in_phasePtr)->getCurrentPhaseState();												// ...calculate the current phase state
}

void ServerJobManager::handleContourPlanRequest(Message in_message)
{
	// ###################
	//
	// MESSAGE CHAIN: clientRequestsContourPlanRun(attempts to run the requested contour plan, 3 of 3)
	// PREVIOUS CALL/MESSAGE: ServerMessageInterpreter sent the message to this ServerJobManager.
	//

	// message should have already been opened before (test of std::move)
	std::cout << "SERVER: JobManager found contour plan request. " << std::endl;
	std::string planName = in_message.readString();											// remember, every read increments the string.
	bool wasContouredPlanFound = server->planStateContainer.checkIfStateExistsForPlan(planName);	// check if the plan has a state; if it does, we won't run this plan (because of the rule: a single contour plan may only run once.)
	if (wasContouredPlanFound == false)																// it wasn't found as having a state, lets run it.
	{
		insertPhasedJobRunSingleMountTest(std::move(in_message));									// move the message into the job.	
		server->planStateContainer.insertNewState(planName, ContourPlanState::WAITING_TO_RUN);		// insert the state
	}
	else if (wasContouredPlanFound == true)															// drop the job request.
	{
		std::cout << "SERVER: plan " << planName << "already has a state; will not insert job. " << std::endl;
	}
}

void ServerJobManager::handleSetDirectionRequest(Message in_message)
{
	//std::cout << "#(((((((((((((( handling set direction request... " << std::endl;

	// LOCAL message logic
	insertPhasedJobSetWorldDirection(in_message);

	// REMOTE message logic
}

bool ServerJobManager::doesFlagExist(ServerJobBlockingFlags in_flagToCheck)
{
	return jobBlockingFlags.checkIfFlagExists(in_flagToCheck);
}