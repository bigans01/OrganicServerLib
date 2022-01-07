#include "stdafx.h"
#include "SPJRunSingleMountTest.h"

void SPJRunSingleMountTest::interpretMessage(Message in_message)
{
	int someVal = 3;

	Message currentMessage = in_message;
	currentMessage.open();

	// SPJ-F7 (skip 2 ints, because first 2 ints are for the SPJ's location)
	currentMessage.readInt();
	currentMessage.readInt();
	
	

	int phaseID = currentMessage.readInt();
	int jobID = currentMessage.readInt();

	// mark individual jobs as complete, as needed
	phaseMap[phaseID]->jobMap[jobID]->currentJobState = ServerJobState::COMPLETE;
	phaseMap[phaseID]->currentJobsCompleted = 1;		// should just be 1.
}

void SPJRunSingleMountTest::initializeAndSetOptionalSPJMetadata(Message in_message)
{
	requiredThreadDesignation = "TERRAIN";	// must be run on the server's TERRAIN thread.
	mountainMetadataMessage = in_message;	// Need to create a new message, for the call to OSServer::constructSingleMountTestNoInput; 
											// extract the metadata from the message.  This will be sent to the SJRunSingleMountTest job, via a call to setStartMessage.
	planName = in_message.readString();		// The contour plan name to use for the various SJs that need it.

	if (locality == MessageLocality::LOCAL)
	{
		std::cout << ":::: Inserting new local phase..." << std::endl;
		std::shared_ptr<ServerJobPhase> phaseOne(new (ServerJobPhase));
		insertNewPhase(phaseOne);

		std::shared_ptr<ServerJobPhase> phaseTwo(new (ServerJobPhase));
		insertNewPhase(phaseTwo);
	
		std::shared_ptr<ServerJobPhase> phaseThree(new (ServerJobPhase));
		insertNewPhase(phaseThree);

		std::shared_ptr<ServerJobPhase> phaseFour(new (ServerJobPhase));
		insertNewPhase(phaseFour);
	
		std::shared_ptr<ServerJobPhase> phaseFive(new (ServerJobPhase));
		insertNewPhase(phaseFive);		
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

		
		// SPJ-F8 (insert the spj's layer, and layer-unique ID)
		completionMessage.insertInt(spjLayerID);
		completionMessage.insertInt(spjLayerSmartID);
		

		completionMessage.insertInt(currentPhaseIndex);								// insert the phase that this job is in
		completionMessage.insertInt(currentJobMapKey);								// insert the key value of the job, as it exists in jobMap


		std::shared_ptr<ServerJobBase> job(new (SJRunSingleMountTest));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey] = job;													// instantiation.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setServerPtr(server);									// Set the required OSServer pointer in the job.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setStartMessage(std::move(mountainMetadataMessage));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setCompletionMessage(std::move(completionMessage));		// Move the built completion message into the job.
		phaseMap[currentPhaseIndex]->requiredJobsToBeCompleted = 1;
	}

	
	else if (currentPhaseIndex == 1)
	{
		// Required: completion message must be built.
		int currentJobMapKey = 0;
		Message completionMessage;
		completionMessage.messageType = MessageType::SERVER_JOB_EVENT_UPDATE_INT;	// signal that this message will come back to a container of int'd values

		// SPJ-F8 (insert the spj's layer, and layer-unique ID)
		completionMessage.insertInt(spjLayerID);
		completionMessage.insertInt(spjLayerSmartID);

		completionMessage.insertInt(currentPhaseIndex);								// insert the phase that this job is in
		completionMessage.insertInt(currentJobMapKey);								// insert the key value of the job, as it exists in jobMap

		Message tracingMessage;
		tracingMessage.insertString(planName);
		std::shared_ptr<ServerJobBase> job(new (SJRunContourPlanWorldTracing));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey] = job;
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setServerPtr(server);
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setStartMessage(std::move(tracingMessage));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setCompletionMessage(std::move(completionMessage));		// Move the built completion message into the job.
		phaseMap[currentPhaseIndex]->requiredJobsToBeCompleted = 1;
	}
	
	
	else if (currentPhaseIndex == 2)
	{
		// Required: completion message must be built.
		int currentJobMapKey = 0;
		Message completionMessage;
		completionMessage.messageType = MessageType::SERVER_JOB_EVENT_UPDATE_INT;	// signal that this message will come back to a container of int'd values

		// SPJ-F8 (insert the spj's layer, and layer-unique ID)
		completionMessage.insertInt(spjLayerID);
		completionMessage.insertInt(spjLayerSmartID);

		completionMessage.insertInt(currentPhaseIndex);								// insert the phase that this job is in
		completionMessage.insertInt(currentJobMapKey);								// insert the key value of the job, as it exists in jobMap

		Message affectedBlueprintsMessage;
		affectedBlueprintsMessage.insertString(planName);
		std::shared_ptr<ServerJobBase> job(new (SJBuildContourPlanAffectedBlueprints));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey] = job;
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setServerPtr(server);									// Set the required OSServer pointer in the job.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setStartMessage(std::move(affectedBlueprintsMessage));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setCompletionMessage(std::move(completionMessage));		// Move the built completion message into the job.
		phaseMap[currentPhaseIndex]->requiredJobsToBeCompleted = 1;
	}
	
	
	else if (currentPhaseIndex == 3)
	{
		std::cout << "!!! Initializing phase 2. " << std::endl;
		// Required: completion message must be built.
		int currentJobMapKey = 0;
		Message completionMessage;
		completionMessage.messageType = MessageType::SERVER_JOB_EVENT_UPDATE_INT;	// signal that this message will come back to a container of int'd values

		// SPJ-F8 (insert the spj's layer, and layer-unique ID)
		completionMessage.insertInt(spjLayerID);
		completionMessage.insertInt(spjLayerSmartID);

		completionMessage.insertInt(currentPhaseIndex);								// insert the phase that this job is in
		completionMessage.insertInt(currentJobMapKey);								// insert the key value of the job, as it exists in jobMap

		Message fracturingAndMassDrivingMessage;
		fracturingAndMassDrivingMessage.insertString(planName);
		std::shared_ptr<ServerJobBase> job(new (SJRunContourPlanFracturingAndMassDriving));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey] = job;
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setServerPtr(server);									// Set the required OSServer pointer in the job.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setStartMessage(std::move(fracturingAndMassDrivingMessage));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setCompletionMessage(std::move(completionMessage));		// Move the built completion message into the job.
		phaseMap[currentPhaseIndex]->requiredJobsToBeCompleted = 1;

	}
	
	

	else if (currentPhaseIndex == 4)
	{
		std::cout << "!!! Initializing phase 3. " << std::endl;

		int currentJobMapKey = 0;
		Message completionMessage;
		completionMessage.messageType = MessageType::SERVER_JOB_EVENT_UPDATE_INT;	// signal that this message will come back to a container of int'd values

		// SPJ-F8 (insert the spj's layer, and layer-unique ID)
		completionMessage.insertInt(spjLayerID);
		completionMessage.insertInt(spjLayerSmartID);
		
		

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

