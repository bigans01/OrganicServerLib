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

	// mark individual jobs as complete, as needed
	phaseMap[phaseID]->jobMap[jobID]->currentJobState = ServerJobState::COMPLETE;
	phaseMap[phaseID]->currentJobsCompleted = 1;		// should just be 1.
}

void SPJRunSingleMountTest::initialize()
{
	if (locality == MessageLocality::LOCAL)
	{
		std::cout << ":::: Inserting new local phase..." << std::endl;
		std::shared_ptr<ServerJobPhase> phaseOne(new (ServerJobPhase));
		insertNewPhase(phaseOne);

		std::shared_ptr<ServerJobPhase> phaseTwo(new (ServerJobPhase));
		insertNewPhase(phaseTwo);
	}

	currentPhaseIter = phaseMap.begin();	// Required: set the current iterator to be the very first phase.
}

void SPJRunSingleMountTest::initializeCurrentPhase()
{
	currentPhaseIndex = currentPhaseIter->first;
	if (currentPhaseIndex == 0)
	{
		std::cout << "!!! Initializing phase 0. " << std::endl;

		// Required: completion message must be built.
		int currentJobMapKey = 0;
		Message completionMessage;
		completionMessage.messageType = MessageType::SERVER_JOB_EVENT_UPDATE_INT;	// signal that this message will come back to a container of int'd values
		completionMessage.insertInt(parentContainerIntKey);
		completionMessage.insertInt(currentPhaseIndex);								// insert the phase that this job is in
		completionMessage.insertInt(currentJobMapKey);								// insert the key value of the job, as it exists in jobMap


		std::shared_ptr<ServerJobBase> job(new (SJRunSingleMountTest));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey] = job;													// instantiation.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setServerPtr(server);									// Set the required OSServer pointer in the job.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setCompletionMessage(std::move(completionMessage));		// Move the built completion message into the job.
		phaseMap[currentPhaseIndex]->requiredJobsToBeCompleted = 1;
	}
	else if (currentPhaseIndex == 1)
	{
		std::cout << "!!! Initializing phase 1. " << std::endl;

		int currentJobMapKey = 0;
		Message completionMessage;
		completionMessage.messageType = MessageType::SERVER_JOB_EVENT_UPDATE_INT;	// signal that this message will come back to a container of int'd values
		completionMessage.insertInt(parentContainerIntKey);
		completionMessage.insertInt(currentPhaseIndex);								// insert the phase that this job is in
		completionMessage.insertInt(currentJobMapKey);								// insert the key value of the job, as it exists in jobMap

		std::shared_ptr<ServerJobBase> job(new (SJSendRequestForOGLMRMCBlueprints));
		Message enumedMessage;
		enumedMessage.messageType = MessageType::REQUEST_FROM_SERVER_SEND_BLUEPRINTS_FOR_OGLMBUFFERMANAGER;
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey] = job;													// instantiation.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setServerPtr(server);									// Set the required OSServer pointer in the job.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setStartMessage(std::move(enumedMessage));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setCompletionMessage(std::move(completionMessage));		// Move the built completion message into the job.
		phaseMap[currentPhaseIndex]->requiredJobsToBeCompleted = 1;
	}

}

