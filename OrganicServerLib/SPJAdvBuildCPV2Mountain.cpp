#include "stdafx.h"
#include "SPJAdvBuildCPV2Mountain.h"

void SPJAdvBuildCPV2Mountain::interpretMessage(Message in_message)
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

void SPJAdvBuildCPV2Mountain::initializeAndSetOptionalSPJMetadata(Message in_message)
{
	requiredThreadDesignation = "TERRAIN";	// must be run on the server's TERRAIN thread.
	mountainMetadataMessage = in_message;	// Need to create a new message, for the call to OSServer::prepCPMountain; 
											// extract the metadata from the message.  This will be sent to the SJAddCPV2AndPrepareMeshPoints job, via a call to setStartMessage.
	planName = in_message.readString();		// The contour plan name to use for the various SJs that need it.

	if (locality == MessageLocality::LOCAL)
	{
		std::cout << ":::: Inserting new local phases..." << std::endl;
		insertNewPhases(8);		// insert 8 new phases for this SPJ (Phases 0 through 8)
	}
	currentPhaseIter = phaseMap.begin();	// Required: set the current iterator to be the very first phase.
}

void SPJAdvBuildCPV2Mountain::initializeCurrentPhase()
{
	currentPhaseIndex = currentPhaseIter->first;

	// ||||||||||||||||||||||||||||||||||||||||||||| PHASE 0
	if (currentPhaseIndex == 0)
	{
		/*
		Summary: do a simple request to run the contour plan, by calling SJRequestContourPlanLock; this 
		shouldn't require a start Message, as the runJob call on this SJ simply inserts a completion Message signalling it's done, 
		BUT only if the verdict is acquired.

		*/
		std::cout << "(SPJAdvBuildCPV2Mountain) Phase 0 - Requesting contour plan lock... " << std::endl;
		int currentJobMapKey = 0;

		Message completionMessage = buildCompletionMessageForSJ(currentPhaseIndex, currentJobMapKey);			// Required: completion message must be built.
		std::shared_ptr<ServerJobBase> job(new (SJRequestContourPlanLock));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey] = job;	// instantiation; program will crash if this isn't called.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setServerPtr(server);									// Set the required OSServer pointer in the job.
		//phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setStartMessage(std::move(mountainMetadataMessage));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setCompletionMessage(std::move(completionMessage));		// Move the built completion message into the job.
		phaseMap[currentPhaseIndex]->requiredJobsToBeCompleted = 1;
	}

	// ||||||||||||||||||||||||||||||||||||||||||||| PHASE 1
	else if (currentPhaseIndex == 1)
	{
		/*

		Summary: add the plan, initialize it, and build the mesh; do this with a call to SJAddCPV2AndPrepareMeshPoints.
		The startMessage of this job needs to be the Message value contained in mountainMetadataMessage.

		*/

		std::cout << "(SPJAdvBuildCPV2Mountain) Phase 1 - Adding plan, initializing, and setting up mesh..." << std::endl;
		int currentJobMapKey = 0;

		Message completionMessage = buildCompletionMessageForSJ(currentPhaseIndex, currentJobMapKey);

		std::shared_ptr<ServerJobBase> job(new (SJAddCPV2AndPrepareMeshPoints));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey] = job;	// instantiation.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setServerPtr(server);	// Set the required OSServer pointer in the job.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setStartMessage(std::move(mountainMetadataMessage));	// pass down the data for the mountain
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setCompletionMessage(std::move(completionMessage));		// Move the built completion message into the job.
		phaseMap[currentPhaseIndex]->requiredJobsToBeCompleted = 1;

	}


	// ||||||||||||||||||||||||||||||||||||||||||||| PHASE 2
	else if (currentPhaseIndex == 2)
	{
		/*

		Summary: determine the affected blueprints needed for this CPV2 to run; we will not lock these blueprints yet, but simply populate the pillarCPV2Keys
		of the OSServer's HotBlueprints member (hotBPManager)

		*/

		std::cout << "(SPJAdvBuildCPV2Mountain) Phase 2 - Determining affected blueprints..." << std::endl;
		int currentJobMapKey = 0;

		// Remember, this SJ requires that it gets the planName, so it knows where to check for the estimated blueprints this CPV2 would run.
		Message planToDetermineForMessage;
		planToDetermineForMessage.insertString(planName);
		Message completionMessage = buildCompletionMessageForSJ(currentPhaseIndex, currentJobMapKey);

		std::shared_ptr<ServerJobBase> job(new (SJDetermineAffectedBlueprints));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey] = job;	// instantiation.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setServerPtr(server);	// Set the required OSServer pointer in the job.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setStartMessage(std::move(planToDetermineForMessage));	// pass down the data for the mountain
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setCompletionMessage(std::move(completionMessage));		// Move the built completion message into the job.
		phaseMap[currentPhaseIndex]->requiredJobsToBeCompleted = 1;

	}

	// ||||||||||||||||||||||||||||||||||||||||||||| PHASE 3
	else if (currentPhaseIndex == 3)
	{
		/*

		Summary: attempt to flag the blueprint keys that the CPV2 needs to safely run as "hot", (effectively locking them from use by other jobs/work).
		Using a verdict to do this, and once locked, we can safely make backup copies of the blueprints.

		This job doesn't require a startMessage to be sent to it. The call to ServerJobProxy::callServerGenerateBlueprintBackupsForCPV2SPJ only
		requires a pointer to the OrganicServer.

		*/

		std::cout << "(SPJAdvBuildCPV2Mountain) Phase 3 - Attempting to lock required blueprints for CPV2 that is about to run, and then doing backups of those blueprints..." << std::endl;
		int currentJobMapKey = 0;
		Message completionMessage = buildCompletionMessageForSJ(currentPhaseIndex, currentJobMapKey);

		std::shared_ptr<ServerJobBase> job(new (SJGenerateBackupsForCPV2));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey] = job;	// instantiation.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setServerPtr(server);	// Set the required OSServer pointer in the job.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setCompletionMessage(std::move(completionMessage));		// Move the built completion message into the job.
		phaseMap[currentPhaseIndex]->requiredJobsToBeCompleted = 1;

	}

	// ||||||||||||||||||||||||||||||||||||||||||||| PHASE 4
	else if (currentPhaseIndex == 4)
	{
		/*

		Summary: Append the blueprint-level FTriangle objects the plan produced, over into the affected persistent blueprints.
		The startMessage of this SJ needs to contain the plan name to append the data from, into the relevant blueprints.

		*/

		std::cout << "(SPJAdvBuildCPV2Mountain) Phase 4 - Attempting to append blueprint-level FTriangle objects to affected blueprints..." << std::endl;
		int currentJobMapKey = 0;
		Message targetPlanMessage;
		targetPlanMessage.insertString(planName);

		Message completionMessage = buildCompletionMessageForSJ(currentPhaseIndex, currentJobMapKey);

		std::shared_ptr<ServerJobBase> job(new (SJAppendCPV2TrianglesToServerBlueprints));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey] = job;	// instantiation.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setServerPtr(server);	// Set the required OSServer pointer in the job.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setStartMessage(std::move(targetPlanMessage));	// pass down the data for the mountain
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setCompletionMessage(std::move(completionMessage));		// Move the built completion message into the job.
		phaseMap[currentPhaseIndex]->requiredJobsToBeCompleted = 1;


	}

	// ||||||||||||||||||||||||||||||||||||||||||||| PHASE 5
	else if (currentPhaseIndex == 5)
	{
		/*

		Summary: Run the mass drivers

		*/

		std::cout << "(SPJAdvBuildCPV2Mountain) Phase 5 - Attempting to run mass drivers..." << std::endl;
		int currentJobMapKey = 0;
		Message targetPlanMessage;
		targetPlanMessage.insertString(planName);

		Message completionMessage = buildCompletionMessageForSJ(currentPhaseIndex, currentJobMapKey);

		std::shared_ptr<ServerJobBase> job(new (SJRunMassDriversForCPV2));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey] = job;	// instantiation.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setServerPtr(server);	// Set the required OSServer pointer in the job.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setStartMessage(std::move(targetPlanMessage));	// pass down the data for the mountain
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setCompletionMessage(std::move(completionMessage));		// Move the built completion message into the job.
		phaseMap[currentPhaseIndex]->requiredJobsToBeCompleted = 1;
	}

	// ||||||||||||||||||||||||||||||||||||||||||||| PHASE 6
	else if (currentPhaseIndex == 6)
	{
		/*

		Summary: clean up the items the CPV2 claimed, such as removing the populated keys of the pillarCPV2Keys set from the hotKeys (in hotBPManager),
		and deactivating any blocking flags. This SJ will need to know the name of the plan to cleanup, so that it may check to see whether or not
		the run of the plan was considered successful.

		*/

		std::cout << "(SPJBuildCPV2Mountain) Phase 6 - Cleaning up CPV2..." << std::endl;
		int currentJobMapKey = 0;
		Message targetPlanMessage;
		targetPlanMessage.insertString(planName);

		Message completionMessage = buildCompletionMessageForSJ(currentPhaseIndex, currentJobMapKey);
		std::shared_ptr<ServerJobBase> job(new (SJCleanupCPV2));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey] = job;	// instantiation.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setServerPtr(server);	// Set the required OSServer pointer in the job.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setStartMessage(std::move(targetPlanMessage));	// pass down the data for the mountain
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setCompletionMessage(std::move(completionMessage));		// Move the built completion message into the job.
		phaseMap[currentPhaseIndex]->requiredJobsToBeCompleted = 1;
	}


	// ||||||||||||||||||||||||||||||||||||||||||||| PHASE 7
	else if (currentPhaseIndex == 7)
	{
		/*

		Summary: send a message to the OrganicCore (client) to have it send it's current OGLM center collection key; this key is used by the Server
		to produce a list of blueprints that it will need to send back to the client.

		*/

		std::cout << "(SPJBuildCPV2Mountain) Phase 7 - Sending request to client to send OGLMRMC key..." << std::endl;
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