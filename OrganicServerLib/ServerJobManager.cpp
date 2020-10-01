#include "stdafx.h"
#include "ServerJobManager.h"
#include "OSServer.h"

void ServerJobManager::initialize(OSServer* in_serverPtr)
{
	server = in_serverPtr;

	intJobsContainer.server = server;

	designations.initialize(&server->OSCManager);
	//designations.buildInitialUndesignatedPool(&server->OSCManager);
	designations.buildInitialUndesignatedPool();
	designations.designateCommandLineThread(0);							// thread 0 from the pool should be the command line thread.
}

void ServerJobManager::startCommandLine()
{
	OrganicThread* targetThread = designations.getCommandLineThread();

	//int commandLineRunning = server->isCommandLineRunning;
	int* clShutdownStatus = &server->isCommandLineShutDown;
	//std::future<int> testFuture2 = OSCManager.stemcellMap[0].threadPtr->submit(&OSServer::runCommandLine, this, std::ref(serverReadWrite), std::ref(commandLineCV), std::ref(isCommandLineRunning), std::ref(clShutdownStatus));
	//targetThread->submit(&OSServer::runCommandLine, server, std::ref(server->serverReadWrite), std::ref(server->commandLineCV), std::ref(commandLineRunningRef), std::ref(clShutdownStatus));
	//targetThread->submit(&OSServer::runCommandLineV2, server, std::ref(server->serverReadWrite), std::ref(server->isCommandLineRunning), std::ref(clShutdownStatus));
	targetThread->submit(&OSServer::runCommandLineV3, server);
}

void ServerJobManager::insertPhasedJobRunSingleMountTest(Message in_message)		// the TRUE test function
{
	std::shared_ptr<ServerPhasedJobBase> job(new (SPJRunSingleMountTest));
	intJobsContainer.insertJob(&job, in_message);
}

void ServerJobManager::checkForUpdateMessages()
{
	while (!updateMessages.isEmpty())
	{
		//std::cout << " !! Found update message...continue? " << std::endl;

		//int someVal = 3;
		//std::cin >> someVal;

		Message* currentMessageRef = updateMessages.getMessageRefFromFront();
		Message tempCopy = *currentMessageRef;
		currentMessageRef->open();
		switch (currentMessageRef->messageType)
		{
			case MessageType::SERVER_JOB_EVENT_UPDATE_INT :
			{
				int parentJobID = currentMessageRef->readInt();
				int phaseID = currentMessageRef->readInt();
				int jobID = currentMessageRef->readInt();

				/*
				std::cout << "Parent job ID is: " << parentJobID << std::endl;
				int someVal = 3;

				auto findParentJobID = intJobsContainer.serverJobs.find(parentJobID);
				if (findParentJobID == intJobsContainer.serverJobs.end())
				{
					std::cout << "!!! Warning, parent job not found! " << std::endl;
				}

				std::cin >> someVal;
				*/

				//intJobsContainer.serverJobs[parentJobID]->interpretMessage(std::move(*currentMessageRef));
				//intJobsContainer.serverJobs[parentJobID]->interpretMessage(*currentMessageRef);
				intJobsContainer.serverJobs[parentJobID]->interpretMessage(tempCopy);
				break;
			}
		}
		updateMessages.safePopQueue();
	}
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

	while (!messageQueue.isEmpty())
	{
		Message* currentMessageRef = messageQueue.getMessageRefFromFront();		// get a ref to the message
		currentMessageRef->open();												// open the message
		switch (currentMessageRef->messageType)
		{
			//case MessageType::REQUEST_FROM_CLIENT_RUN_CONTOUR_PLAN : {  handleContourPlanRequest(std::move(*currentMessageRef));  break;  }
		case MessageType::REQUEST_FROM_CLIENT_RUN_CONTOUR_PLAN: {  handleContourPlanRequest(*currentMessageRef);  break;  }
		}
		messageQueue.safePopQueue();
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
			//std::cout << "!!! ----------- Found existing job " << std::endl;
			checkCurrentJobPhaseSetup(&intServerJobsBegin->second);
			//intServerJobsBegin->second->getCurrentPhaseState();
			// get the phase state again, to see if any jobs were added.
			ReadyJobSearch searchForAvailableJobInCurrentPhase = intServerJobsBegin->second->findNextWaitingJob();
			if (searchForAvailableJobInCurrentPhase.wasJobFound == true)
			{
				OrganicThread* targetThread = designations.getFirstAvailableThread();						// temporary: run in thread.
				(*searchForAvailableJobInCurrentPhase.currentJobPtr)->runJob(targetThread);
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
	if (isInProgress == false)		// if the phase isn't in progress, start it
	{
		(*in_phasePtr)->initializeCurrentPhase();										// initialize FIRST, then...
	}
	(*in_phasePtr)->getCurrentPhaseState();												// ...calculate the current phase state
}

void ServerJobManager::handleContourPlanRequest(Message in_message)
{
	// message should have already been opened before (test of std::move)
	std::cout << "SERVER: JobManager found contour plan request. " << std::endl;
	std::string planName = in_message.readString();											// remember, every read increments the string.
	bool wasContouredPlanFound = server->planStateContainer.checkIfStateExistsForPlan(planName);	// check if the plan has a state; if it does, we won't run this plan (because of the rule: a single contour plan may only run once.)
	if (wasContouredPlanFound == false)																// it wasn't found as having a state, lets run it.
	{
		//insertPhasedJobRunSingleMountTest(std::move(in_message));									// move the message into the job.	
		insertPhasedJobRunSingleMountTest(in_message);									// move the message into the job.	
		server->planStateContainer.insertNewState(planName, ContourPlanState::WAITING_TO_RUN);		// insert the state
		//OrganicThread* targetThread = designations.getFirstAvailableThread();						// temporary: run in thread.
		//targetThread->submit(&ServerJobProxy::callServerJobRunSingleMountTest, server);				// ""
	}
	else if (wasContouredPlanFound == true)															// drop the job request.
	{
		std::cout << "SERVER: plan " << planName << "already has a state; will not insert job. " << std::endl;
	}
}