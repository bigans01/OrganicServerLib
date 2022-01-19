#include "stdafx.h"
#include "SPJBuildCPMountain.h"

void SPJBuildCPMountain::interpretMessage(Message in_message)
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

void SPJBuildCPMountain::initializeAndSetOptionalSPJMetadata(Message in_message)
{
	requiredThreadDesignation = "TERRAIN";	// must be run on the server's TERRAIN thread.
	mountainMetadataMessage = in_message;	// Need to create a new message, for the call to OSServer::prepCPMountain; 
											// extract the metadata from the message.  This will be sent to the SJPrepCPMountainRun job, via a call to setStartMessage.
	planName = in_message.readString();		// The contour plan name to use for the various SJs that need it.

	if (locality == MessageLocality::LOCAL)
	{
		std::cout << ":::: Inserting new local phases..." << std::endl;
		insertNewPhases(7);		// insert 7 new phases for this SPJ.
	}
	currentPhaseIter = phaseMap.begin();	// Required: set the current iterator to be the very first phase.
}

void SPJBuildCPMountain::initializeCurrentPhase()
{
	currentPhaseIndex = currentPhaseIter->first;
	if (currentPhaseIndex == 0)
	{
		/*
		Summary: this phase has one job will prep the ContouredMontain's run (no blueprints will be affected, we are just setting up the OSContouredTriangles 
		that constitute the plan). The job should only run if ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN isn't already set; if it isn't set, the verdict 
		should flag the ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN and ServerJobBlockingFlags::HALT_FUTURE_COLLECTION_MODIFICATIONS flags. 

		The ServerJobBlockingFlags::HALT_FUTURE_COLLECTION_MODIFICATIONS will prevent any future terrain modification categorized jobs from being submitted, 
		while letting the current ones run their course, therefore bringing the counter to 0.
		
		Otherwise, if ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN cannot be set, the CP will have to wait for this flag to be down before it starts its run.
		for this flag to be available (which would indicate another ContourPlan is running).

		*/
		std::cout << "(SPJBuildCPMountain) Phase 0 - Prepping ContouredMountain plan... " << std::endl;
		int currentJobMapKey = 0;

		Message completionMessage = buildCompletionMessageForSJ(currentPhaseIndex, currentJobMapKey);			// Required: completion message must be built.
		std::shared_ptr<ServerJobBase> job(new (SJPrepCPMountainRun));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey] = job;	// instantiation; program will crash if this isn't called.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setServerPtr(server);									// Set the required OSServer pointer in the job.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setStartMessage(std::move(mountainMetadataMessage));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setCompletionMessage(std::move(completionMessage));		// Move the built completion message into the job.
		phaseMap[currentPhaseIndex]->requiredJobsToBeCompleted = 1;
	}

	
	else if (currentPhaseIndex == 1)
	{
		/*
		Summary: this phase will trace the contour plan through the actual blueprints. 

		This job should only run when the category ServerJobRunCategory::TERRAIN_MODIFICATION has a counter of 0. The counter should eventually go to 0, by allowing
		existing modification jobs to finish, but preventing new ones from starting (via the raising of the flag ServerJobBlockingFlags::HALT_FUTURE_COLLECTION_MODIFICATIONS from
		the previous SJ run). 

		As of 1/7/2022, the modification mechanism and placebo job haven't been developed.

		*/
		std::cout << "(SPJBuildCPMountain) Phase 1 - Tracing Mountain OSContouredTriangles through blueprints... " << std::endl;
		int currentJobMapKey = 0;

		Message completionMessage = buildCompletionMessageForSJ(currentPhaseIndex, currentJobMapKey);			// Required: completion message must be built.
		Message tracingMessage;
		tracingMessage.insertString(planName);
		std::shared_ptr<ServerJobBase> job(new (SJRunContourPlanWorldTracing));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey] = job;	// instantiation; program will crash if this isn't called.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setServerPtr(server);
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setStartMessage(std::move(tracingMessage));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setCompletionMessage(std::move(completionMessage));		// Move the built completion message into the job.
		phaseMap[currentPhaseIndex]->requiredJobsToBeCompleted = 1;
	}

	else if (currentPhaseIndex == 2)
	{
		/*

		Summary: this phase will build the Mountain's possible affected blueprints, which can only be done after the ContourPlan has been traced through world space,
		and subsequently inserted into the appropriate blueprints. The "possible affected blueprints" is a set that must be checked against when any terrain modification
		needs to be done; if the blueprint that the terrain modification is done in is found in the set, then that SJ must not run. At the same time, the set can only be built
		reliably without any race conditions or undefined behavior caused by other threads, if and only when no other current terrain modifications are taking place -- which is why the ServerJobBlockingFlags::HALT_FUTURE_COLLECTION_MODIFICATIONS
		was created.

		The SJ in this phase will need to release the ServerJobBlockingFlags::HALT_FUTURE_COLLECTION_MODIFICATIONS flag set by the yet-to-be-defined SJ that activates that flag,
		at the end of the OrganicServer function called by the SJ. This will allow other terrain modifications to at at least attempt their run on another thread, as long
		as the blueprint those jobs affect isn't in the set. The ContourPlan can then simultaneously run another thread, with the safety of knowing that none of the blueprints
		it is planning to affect will be altered while it is running.

		*/

		std::cout << "(SPJBuildCPMountain) Phase 2 - Building possible affected blueprints... " << std::endl;
		int currentJobMapKey = 0;

		Message completionMessage = buildCompletionMessageForSJ(currentPhaseIndex, currentJobMapKey);			// Required: completion message must be built.
		Message affectedBlueprintsMessage;
		affectedBlueprintsMessage.insertString(planName);
		std::shared_ptr<ServerJobBase> job(new (SJBuildContourPlanAffectedBlueprints));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey] = job;	// instantiation; program will crash if this isn't called.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setServerPtr(server);									// Set the required OSServer pointer in the job.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setStartMessage(std::move(affectedBlueprintsMessage));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setCompletionMessage(std::move(completionMessage));		// Move the built completion message into the job.
		phaseMap[currentPhaseIndex]->requiredJobsToBeCompleted = 1;
	}
	
	
	else if (currentPhaseIndex == 3)
	{
		/*

		Summary: this phase analyzes all blueprints that the CP added polys to, to get the IDs of the polys that it added into a OperableIntSet, and then creates a copy of
		each blueprint affected (the backups could be put into memory or written to disk, depending on conditions). The fetched OperableIntSet will the be used
		to delete the ECBPolys having IDs in that set, from the copied blueprint; the result will then be the blueprint in it's original form prior to the CP running.

		This should generate a safe backup, as only ECBPolys should have been added to each blueprint (but no fracturing done)

		*/

		std::cout << "(SPJBuildCPMountain) Phase 3 - Generating backups of CP affected blueprints..." << std::endl;
		int currentJobMapKey = 0;

		Message completionMessage = buildCompletionMessageForSJ(currentPhaseIndex, currentJobMapKey);			// Required: completion message must be built.
		Message affectedBlueprintsMessage;
		affectedBlueprintsMessage.insertString(planName);
		std::shared_ptr<ServerJobBase> job(new (SJGenerateBlueprintBackupsForCP));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey] = job;	// instantiation; program will crash if this isn't called.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setServerPtr(server);									// Set the required OSServer pointer in the job.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setStartMessage(std::move(affectedBlueprintsMessage));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setCompletionMessage(std::move(completionMessage));		// Move the built completion message into the job.
		phaseMap[currentPhaseIndex]->requiredJobsToBeCompleted = 1;
	}
	
	
	else if (currentPhaseIndex == 4)
	{
		/*

		Summary: this is the final phase, and arguably the longest. It will perform the required fracturing of ECBPolys and then run the mass driving for the ContourPlan.
		The ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN will need to be released at the end of OrganicServer function that is called by the SJ.

		With the release of this flag, both the ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN and ServerJobBlockingFlags::HALT_FUTURE_COLLECTION_MODIFICATIONS flags
		should be down, and another ContourPlan can now run.
		
		*/
		std::cout << "(SPJBuildCPMountain) Phase 4 - Fracturing ECBPolys and executing mass driving..." << std::endl;
		int currentJobMapKey = 0;

		Message completionMessage = buildCompletionMessageForSJ(currentPhaseIndex, currentJobMapKey);			// Required: completion message must be built.
		Message fracturingAndMassDrivingMessage;
		fracturingAndMassDrivingMessage.insertString(planName);
		std::shared_ptr<ServerJobBase> job(new (SJRunContourPlanFracturingAndMassDriving));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey] = job;	// instantiation; program will crash if this isn't called.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setServerPtr(server);									// Set the required OSServer pointer in the job.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setStartMessage(std::move(fracturingAndMassDrivingMessage));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setCompletionMessage(std::move(completionMessage));		// Move the built completion message into the job.
		phaseMap[currentPhaseIndex]->requiredJobsToBeCompleted = 1;

	}

	else if (currentPhaseIndex == 5)
	{
		std::cout << "(SPJBuildCPMountain) Phase 5 - Checking ContourPlan's success..." << std::endl;
		int currentJobMapKey = 0;

		Message completionMessage = buildCompletionMessageForSJ(currentPhaseIndex, currentJobMapKey);			// Required: completion message must be built.
		Message affectedBlueprintsMessage;
		affectedBlueprintsMessage.insertString(planName);
		std::shared_ptr<ServerJobBase> job(new (SJCheckContourPlanSuccess));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey] = job;	// instantiation; program will crash if this isn't called.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setServerPtr(server);									// Set the required OSServer pointer in the job.
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setStartMessage(std::move(affectedBlueprintsMessage));
		phaseMap[currentPhaseIndex]->jobMap[currentJobMapKey]->setCompletionMessage(std::move(completionMessage));		// Move the built completion message into the job.
		phaseMap[currentPhaseIndex]->requiredJobsToBeCompleted = 1;
	}
	
	

	else if (currentPhaseIndex == 6)
	{
		/*

		Summary: send a message to the OrganicCore (client) to have it send it's current OGLM center collection key; this key is used by the Server
		to produce a list of blueprints that it will need to send back to the client.

		*/

		std::cout << "(SPJBuildCPMountain) Phase 6 - Sending request to client to send OGLMRMC key..." << std::endl;
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

