#include "stdafx.h"
#include "SPJSendWorldDirectionToClient.h"

void SPJSendWorldDirectionToClient::interpretMessage(Message in_message)
{
	//std::cout << ":::: Interpreting message for set world direction..." << std::endl;

	Message currentMessage = in_message;
	currentMessage.open();
	currentMessage.readInt();
	int phaseID = currentMessage.readInt();
	int jobID = currentMessage.readInt();

	// mark individual jobs as complete, as needed
	phaseMap[phaseID]->jobMap[jobID]->currentJobState = ServerJobState::COMPLETE;
	phaseMap[phaseID]->currentJobsCompleted = 1;		// should just be 1.
}

void SPJSendWorldDirectionToClient::initialize()	// initialization steps are always dependent on whether or not the message locality is LOCAL or REMOTE.
{
	//std::cout << "!!!! ####### Attempting initialization..." << std::endl;
	if (locality == MessageLocality::LOCAL)
	{
		//std::cout << "!!! Initialization local phases..." << std::endl;

		std::shared_ptr<ServerJobPhase> phaseOne(new (ServerJobPhase));
		insertNewPhase(phaseOne);
	}
	currentPhaseIter = phaseMap.begin();	// Required: set the current iterator to be the very first phase.
}
void SPJSendWorldDirectionToClient::initializeCurrentPhase()
{
	//std::cout << "### Initialize current phase for setting world direction (1)... " << std::endl;
	currentPhaseIndex = currentPhaseIter->first;
	if (currentPhaseIndex == 0)
	{
		// std::cout << "### Initialize current phase for setting world direction (2)... " << std::endl;

		// Required: completion message must be built.
		int currentJobMapKey = 0;
		Message completionMessage;
		completionMessage.messageType = MessageType::SERVER_JOB_EVENT_UPDATE_INT;	// signal that this message will come back to a container of int'd values
		completionMessage.insertInt(parentContainerIntKey);
		completionMessage.insertInt(currentPhaseIndex);								// insert the phase that this job is in
		completionMessage.insertInt(currentJobMapKey);								// insert the key value of the job, as it exists in jobMap

		std::shared_ptr<ServerJobBase> job(new (SJBuildAndSendWorldDirection));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey] = job;													// instantiation.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setServerPtr(server);									// Set the required OSServer pointer in the job.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setStartMessage(stringedMessageMap["direction"]); // set the begin message; required to set the world direction.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setCompletionMessage(std::move(completionMessage));		// Move the built completion message into the job.
		phaseMap[currentPhaseIndex]->requiredJobsToBeCompleted = 1;
	}
}