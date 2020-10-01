#include "stdafx.h"
#include "ServerJobManager.h"
#include "OSServer.h"

void ServerJobManager::initialize(OSServer* in_serverPtr)
{
	server = in_serverPtr;

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
			case MessageType::REQUEST_FROM_CLIENT_RUN_CONTOUR_PLAN : {  handleContourPlanRequest(std::move(*currentMessageRef));  break;  }
		}
		messageQueue.safePopQueue();
	}
}

void ServerJobManager::handleContourPlanRequest(Message in_message)
{
	// message should have already been opened before (test of std::move)
	std::cout << "SERVER: JobManager found contour plan request. " << std::endl;
	std::string planName = in_message.readString();											// remember, every read increments the string.
	bool wasContouredPlanFound = server->planStateContainer.checkIfStateExistsForPlan(planName);	// check if the plan has a state; if it does, we won't run this plan (because of the rule: a single contour plan may only run once.)
	if (wasContouredPlanFound == false)																// it wasn't found as having a state, lets run it.
	{
		insertPhasedJobRunSingleMountTest(std::move(in_message));									// move the message into the job.	
		server->planStateContainer.insertNewState(planName, ContourPlanState::WAITING_TO_RUN);		// insert the state
		OrganicThread* targetThread = designations.getFirstAvailableThread();						// temporary: run in thread.
		targetThread->submit(&ServerJobProxy::callServerJobRunSingleMountTest, server);				// ""
	}
	else if (wasContouredPlanFound == true)															// drop the job request.
	{
		std::cout << "SERVER: plan " << planName << "already has a state; will not insert job. " << std::endl;
	}
}