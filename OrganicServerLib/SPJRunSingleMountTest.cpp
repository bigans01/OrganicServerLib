#include "stdafx.h"
#include "SPJRunSingleMountTest.h"

void SPJRunSingleMountTest::interpretMessage(Message in_message)
{
	int someVal = 3;

	Message currentMessage = in_message;
	currentMessage.open();
	currentMessage.readInt();
	int phaseID = currentMessage.readInt();
	int jobID = currentMessage.readInt();


	//std::cout << ">>> Interpret mesage OK, press key to continue. (1) " << std::endl;
	//std::cin >> someVal;

	phaseMap[phaseID]->jobMap[jobID]->currentJobState = ServerJobState::COMPLETE;

	//std::cout << ">>> Interpret mesage OK, press key to continue. (2) " << std::endl;
	//std::cin >> someVal;


	phaseMap[phaseID]->currentJobsCompleted = 1;		// should just be 1.

	
	//std::cout << ">>> Interpret mesage OK, press key to continue. (3) " << std::endl;
	//std::cin >> someVal;

	//int phaseID = currentMessage.readInt();
	//int jobID = currentMessage.readInt();
	//phaseMap[0]->jobMap[0]->currentJobState = ServerJobState::COMPLETE;
	//phaseMap[0]->currentJobsCompleted = 1;		// should just be 1.
}

void SPJRunSingleMountTest::initialize()
{
	if (locality == MessageLocality::LOCAL)
	{
		std::cout << ":::: Inserting new local phase..." << std::endl;
		std::shared_ptr<ServerJobPhase> phaseOne(new (ServerJobPhase));
		insertNewPhase(phaseOne);

	}

	currentPhaseIter = phaseMap.begin();	// set the current iterator to be the very first phase.
}

void SPJRunSingleMountTest::initializeCurrentPhase()
{
	if (currentPhaseIndex == 0)
	{
		std::cout << "!!! Initializing phase 0. " << std::endl;

		int currentJobMapKey = 0;
		Message completionMessage;
		completionMessage.messageType = MessageType::SERVER_JOB_EVENT_UPDATE_INT;	// signal that this message will come back to a container of int'd values
		completionMessage.insertInt(parentContainerIntKey);
		completionMessage.insertInt(currentPhaseIndex);								// insert the phase that this job is in
		completionMessage.insertInt(currentJobMapKey);								// insert the key value of the job, as it exists in jobMap


		std::shared_ptr<ServerJobBase> job(new (SJRunSingleMountTest));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey] = job;
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setServerPtr(server);
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setCompletionMessage(completionMessage);
		phaseMap[currentPhaseIndex]->requiredJobsToBeCompleted = 1;
	}
}

