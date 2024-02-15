#pragma once

#ifndef SPJADVBUILDCPV2MOUNTAIN_H
#define SPJADVBUILDCPV2MOUNTAIN_H

/*

Summary: This SPJ is responsible for creating a contoured mass, server side, in the CPV2 style. It should
execute one CPV2 plan when it is called, not multiple.

As of 10/14/2023, the basic mechanics of this SPJ seem to work fine, but this implementation is not final
(need to work on the proper locking/unlocking of the ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN and
ServerJobBlockingFlags::HALT_FUTURE_COLLECTION_MODIFICATION flags, which likely has to be done at SJ (server job) level)

--------------------------------------------------
NEW Proposed logic, 1/17/2024:
--------------------------------------------------

Phase 0 - issue a SJ <SJRequestContourPlanLock> that simply continues when a verdict is met; the verdict should return true if a call to
		  ServerJobProxy::checkIfServerJobBlockingFlagExists(server, ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN) returns false,
		  --AND-- the verdict is able to acquire the flag through a call to
		  ServerJobProxy::activateServerJobBlockingFlag(server, ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN).

		  NOTE: we will likely need some improvement on the acquisition of flags, where the attempted activation, and result of
		  said activation, is done in a single function call.

Phase 1 - issue a job <SJAddCPV2AndPrepareMeshPoints> that will add the plan, initialize it, and then call amplifyAllContourLinePoints, insertMaterials, and buildContouredTriangles on the plan,
		  all in that order; the verdict for this may immediately return, as it builds off of the verdict from Phase 0.


Phase 2 - issue a job <SJDetermineAffectedBlueprints> that determines the estimated blueprints to run, and populates the pillarCPV2Keys member in hotBlueprints.
		  This is done by the SJ calling ServerJobProxy::callDetermineAffectedBlueprintsForCPV2. At this point, the blueprints that can be potentially affected should of been determined,
		  but will not have been flagged as "hot." That is the responsibility of the next phase.

Phase 3	- issue a job <SJGenerateBackupsForCPV2> that attempts to acquire the locks/conditions that are required to run the plan, but only if a specific condition is met: The verdict that allows the plan to run can only be true
		  if all the potentially affected blueprints that the SPJ will lock at the same time, and they aren't locked by another job; When this verdict is acquired,
		  begin to make backups of the blueprints we intend to potentially affect (this can be done via call to OSServer::generateBlueprintBackupsForCPV2SPJ). For te main work of the SJ, the hotBPManager
		  should have the plan's intended blueprints to backup stored somewhere, before OSServer::generateBlueprintBackupsForCPV2SPJ is called (they should be in pillarCPV2Keys). 
		  OSServer::generateBlueprintBackupsForCPV2SPJ should perform backups for every key in the fetched set from the hotBPManager, and this is what this SJ ultimately calls,
		  via a call to ServerJobProxy::callServerGenerateBlueprintBackupsForCPV2SPJ.

		  If the entire contour plan is successful, we won't need to rollback to the backup blueprints,
		  and we may erase them. NOTE: Blueprints that don't exist yet shouldnt bother to be copied.

Phase 4 - With the backups acquired, call getProcessableContouredTriangles <SJAppendCPV2TrianglesToServerBlueprints> to fetch the data needed to process WORLD-space FTriangle objects, and run produceEquivalentFTriangle on each of those entries.
		  The outputs of the WORLD-space FTriangles will be groups of BLUEPRINT-space fracturedTriangles, one group for each blueprint that the WORLD-space FTriangle was fractured into.
		  This can be done by calling ServerJobProxy::callServerCopyOverCPV2ECBPolys, through the SJ, <insert SJ name here>

		  For each BLUEPRINT-space fracturedTriangles object, fetch a number of next-ECBPolyIDs equal to the number of items in the fracturedTriangles, then use
		  that to create an idSet. The idSet will be used in two passes:

		  1.) First pass -- Make a call to currentCTV2->allForgedPolysRegistryRef->addToPolyset for all elements in the idSet --AND-- make a call to
			  currentCTV2->shellMassDriverRegistryRef->addToPolyset if the currentCTV2PolyType == ECBPolyType::SHELL_MASSDRIVER.

		  2.) Second pass -- Cycle through each element currentOutput.second.fracturedTriangles, create an ECBPoly from each element, and insert it into the current blueprint,
			  via currentBlueprintPtr->insertPolyWithKeyValue

		  **NOTE: See ContouredPlanV2Base::copyOverForSPJ as a model for how this can be done.


Phase 5	- With the CPV2's ECBPolys now added to the server/persistent blueprints, and successfully registered with the appropriate registries, mass driving may now
		  begin. <SJRunMassDriversForCPV2>

Phase 6 - cleanup job <SJCleanupCPV2> that removes affected blueprints utilized by the plan, from the hotBPManager; 
		  release ServerJobBlockingFlags (i.e., ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN).

		  This SJ should also clear out the backups if they no longer need to be used, -OR- somehow utilize the backups for a data rollback, in the event the CPV2 met failure criteria (i.e, not successful)

Phase 7 - send a Message to relevant OrganicSystem instances, to incicate that a CPV2 has completed, and to have them send requests 
		  (use SJSendRequestForOGLMRMCBlueprints for this)



*/


#include "ServerPhasedJobBase.h"
#include "SJRequestContourPlanLock.h"
#include "SJAddCPV2AndPrepareMeshPoints.h"
#include "SJDetermineAffectedBlueprints.h"
#include "SJGenerateBackupsForCPV2.h"
#include "SJAppendCPV2TrianglesToServerBlueprints.h"
#include "SJRunMassDriversForCPV2.h"
#include "SJCleanupCPV2.h"

#include "SJSendRequestForOGLMRMCBlueprints.h"

class SPJAdvBuildCPV2Mountain : public ServerPhasedJobBase
{
	public:
		void interpretMessage(Message in_message);
		void initializeAndSetOptionalSPJMetadata(Message in_message);	// initialization steps are always dependent on whether or not the message locality is LOCAL or REMOTE.
		void initializeCurrentPhase();
	private:
		Message mountainMetadataMessage;
		std::string planName = "";
};


#endif