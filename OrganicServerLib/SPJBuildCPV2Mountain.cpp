#include "stdafx.h"
#include "SPJBuildCPV2Mountain.h"

void SPJBuildCPV2Mountain::interpretMessage(Message in_message)
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
	phaseMap[phaseID]->jobMap[jobID]->currentJobState = ServerJobState::COMPLETE;	// set as complete, so that the call to ServerJobPhase::loadDonePhaseSubjobs
																					// can manage and remove this correctly. 
	phaseMap[phaseID]->currentJobsCompleted = 1;		// should just be 1.
}

void SPJBuildCPV2Mountain::initializeAndSetOptionalSPJMetadata(Message in_message)
{
	requiredThreadDesignation = "TERRAIN";	// must be run on the server's TERRAIN thread.
	mountainMetadataMessage = in_message;	// Need to create a new message, for the call to OSServer::prepCPMountain; 
											// extract the metadata from the message.  This will be sent to the SJPrepCPMountainRun job, via a call to setStartMessage.
	planName = in_message.readString();		// The contour plan name to use for the various SJs that need it.

	if (locality == MessageLocality::LOCAL)
	{
		std::cout << ":::: Inserting new local phases..." << std::endl;
		insertNewPhases(2);		// insert 2 new phases for this SPJ.
	}
	currentPhaseIter = phaseMap.begin();	// Required: set the current iterator to be the very first phase.
}

void SPJBuildCPV2Mountain::initializeCurrentPhase()
{
	currentPhaseIndex = currentPhaseIter->first;
	if (currentPhaseIndex == 0)
	{
		/*
		Summary: this phase has one job that will attempt to create backups of the estimated affected blueprints that a CPV2's construction will
		touch. It will then proceed with executing the plan. 
		
		The job should only run if ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN isn't already set; if it isn't set, the verdict
		should flag the ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN and ServerJobBlockingFlags::HALT_FUTURE_COLLECTION_MODIFICATIONS flags.

		The ServerJobBlockingFlags::HALT_FUTURE_COLLECTION_MODIFICATIONS will prevent any future terrain modification categorized jobs from being submitted,
		while letting the current ones run their course, therefore bringing the counter to 0.

		Otherwise, if ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN cannot be set, the CP will have to wait for this flag to be down before it starts its run.
		for this flag to be available (which would indicate another ContourPlan is running).

		*/
		std::cout << "(SPJBuildCPV2Mountain) Phase 0 - Prepping contoured mountain plan... " << std::endl;
		int currentJobMapKey = 0;

		Message completionMessage = buildCompletionMessageForSJ(currentPhaseIndex, currentJobMapKey);			// Required: completion message must be built.
		std::shared_ptr<ServerJobBase> job(new (SJRunCPV2));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey] = job;	// instantiation; program will crash if this isn't called.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setServerPtr(server);									// Set the required OSServer pointer in the job.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setStartMessage(std::move(mountainMetadataMessage));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setCompletionMessage(std::move(completionMessage));		// Move the built completion message into the job.
		phaseMap[currentPhaseIndex]->requiredJobsToBeCompleted = 1;
	}


	else if (currentPhaseIndex == 1)
	{
		/*

		Summary: send a message to the OrganicCore (client) to have it send it's current OGLM center collection key; this key is used by the Server
		to produce a list of blueprints that it will need to send back to the client.

		*/

		std::cout << "(SPJBuildCPV2Mountain) Phase 6 - Sending request to client to send OGLMRMC key..." << std::endl;
		int currentJobMapKey = 0;

		Message completionMessage = buildCompletionMessageForSJ(currentPhaseIndex, currentJobMapKey);
		std::shared_ptr<ServerJobBase> job(new (SJSendRequestForOGLMRMCBlueprints));
		Message enumedMessage;
		enumedMessage.messageType = MessageType::REQUEST_FROM_SERVER_SEND_BLUEPRINTS_FOR_OGLMBUFFERMANAGER;
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey] = job;	// instantiation.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setServerPtr(server);									// Set the required OSServer pointer in the job.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setStartMessage(std::move(enumedMessage));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setCompletionMessage(std::move(completionMessage));		// Move the built completion message into the job.
		phaseMap[currentPhaseIndex]->requiredJobsToBeCompleted = 1;
	}

}