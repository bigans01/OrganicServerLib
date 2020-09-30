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
	while (!messageQueue.isEmpty())
	{
		Message* currentMessageRef = messageQueue.getMessageRefFromFront();		// get a ref to the message
		currentMessageRef->open();												// open the message
		switch (currentMessageRef->messageType)
		{
			case MessageType::REQUEST_FROM_CLIENT_RUN_CONTOUR_PLAN :
			{
				std::cout << "SERVER: JobManager found contour plan request. " << std::endl;
				OrganicThread* targetThread = designations.getFirstAvailableThread();
				targetThread->submit(&ServerJobProxy::callServerJobRunSingleMountTest, server);
				break;
			}
		}
		messageQueue.safePopQueue();
	}
}