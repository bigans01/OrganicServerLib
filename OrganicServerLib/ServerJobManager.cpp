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
	//std::shared_ptr<ServerPhasedJobBase> job(new (SPJRunSingleMountTest));
	intJobsContainer.insertJob(&job, std::move(in_message));
}

void ServerJobManager::insertPhasedJobSetWorldDirection(Message in_message)
{
	//std::cout << "############# Inserted set direction job..." << std::endl;
	std::shared_ptr<ServerPhasedJobBase> job(new (SPJSendWorldDirectionToClient));
	Message directionMessage = in_message;
	job->insertStringedMessage("direction", directionMessage);
	intJobsContainer.insertJob(&job, std::move(in_message));
}

void ServerJobManager::checkForUpdateMessages()
{
	while (!updateMessages.isEmpty())
	{
		Message* currentMessageRef = updateMessages.getMessageRefFromFront();
		currentMessageRef->open();

		// after opening the message, read the update data.
		ServerThreadWorkloadUpdate updateData = readUpdateDataFromMessage(currentMessageRef);
		//std::cout << "---> Job update updateData workload to update is: " << updateData.workload << std::endl;

		switch (currentMessageRef->messageType)
		{
			case MessageType::SERVER_JOB_EVENT_UPDATE_INT :
			{
				int parentJobID = currentMessageRef->readInt();
				intJobsContainer.serverJobs[parentJobID]->interpretMessage(std::move(*currentMessageRef));
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
			//std::cout << "!!! Message found. " << std::endl;
			case MessageType::REQUEST_FROM_CLIENT_RUN_CONTOUR_PLAN : {  handleContourPlanRequest(std::move(*currentMessageRef));  break;  }
			case MessageType::REQUEST_FROM_SERVER_SET_WORLD_DIRECTION: { handleSetDirectionRequest(std::move(*currentMessageRef)); break; }

		}
		jobRequestQueue.safePopQueue();
	}
}

void ServerJobManager::runJobScan()
{
	if (!intJobsContainer.serverJobs.empty())
	{
		auto intServerJobsBegin = intJobsContainer.serverJobs.begin();
		auto intServerJobsEnd = intJobsContainer.serverJobs.end();
		for (; intServerJobsBegin != intServerJobsEnd; intServerJobsBegin++)
		{
			//std::cout << "!!! Found job to run. " << std::endl;

			checkCurrentJobPhaseSetup(&intServerJobsBegin->second);
			ReadyJobSearch searchForAvailableJobInCurrentPhase = intServerJobsBegin->second->findNextWaitingJob();		// don't run jobs that are already executing.
			if 
			(
				(searchForAvailableJobInCurrentPhase.wasJobFound == true)
				&&
				(designations.doesDesignatedThreadExist(intServerJobsBegin->second->fetchThreadDesignation()) == true)	// a job can't be submitted unless it's designated thread exists
			)
			{
				(*searchForAvailableJobInCurrentPhase.currentJobPtr)->runPrechecks();									// calculate the work load.
				float calculatedWorkload = (*searchForAvailableJobInCurrentPhase.currentJobPtr)->estimatedWorkLoad;		// ..grab it (for readability purposes)

				// std::cout << "---> Acquired estimated workload is: " << calculatedWorkload << std::endl;

				//AcquiredServerThread acquiredThread = designations.getFirstAvailableThread();							// get the acquired thread data
				AcquiredServerThread acquiredThread = designations.fetchDesignatedThread(intServerJobsBegin->second->fetchThreadDesignation());							// get the acquired thread data
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