#pragma once

#ifndef SPJBUILDCPV2MOUNTAIN_H
#define SPJBUILDCPV2MOUNTAIN_H

/*

Summary: This SPJ is responsible for creating a contoured mass, server side, in the CPV2 style. It should
execute one CPV2 plan when it is called, not multiple.

As of 10/14/2023, the basic mechanics of this SPJ seem to work fine, but this implementation is not final
(need to work on the proper locking/unlocking of the ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN and 
ServerJobBlockingFlags::HALT_FUTURE_COLLECTION_MODIFICATION flags, which likely has to be done at SJ (server job) level)

--------------------------------------------------
NEW Proposed logic, 1/17/2024:
--------------------------------------------------

Phase 1 - issue a SJ <SJRequestContourPlanLock> that simply continues when a verdict is met; the verdict should return true if a call to
		  ServerJobProxy::checkIfServerJobBlockingFlagExists(server, ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN) returns false, 
		  --AND-- the verdict is able to acquire the flag through a call to 
		  ServerJobProxy::activateServerJobBlockingFlag(server, ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN).

		  NOTE: we will likely need some improvement on the acquisition of flags, where the attempted activation, and result of 
		  said activation, is done in a single function call.
			
Phase 2 - issue a job <name?> that will add the plan, initialize it, and then call amplifyAllContourLinePoints, insertMaterials, and buildContouredTriangles on the plan, 
		  all in that order.

		  NOTE: the addition of the plan will be hardcoded into this job for the time being, but this is not meant to be permanent.

Phase 3 - issue a job <name?> that determines the estimated blueprints to run, which can be done by issuing a call to getProcessableContouredTriangles(),
		  and using those results AND a reference to the OSServer's HotBlueprints member (hotBPManager), in a call to constructHotEstimatedAffectedBlueprints.
		  A new flag (ServerJobBlockingFlags::AFFECTED_BLUEPRINTS_ACQUIRED?) will need to be set as well.

		  At this point, the following should be set:

		  --server flag ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN is UP
		  --a list of all affected blueprints is acquired
		  --server flag ServerJobBlockingFlags::AFFECTED_BLUEPRINTS_ACQUIRED is UP

Phase 4	- issue a job <name?> that attempts to acquire the locks/conditions that are required to run the plan, but only if a specific condition is met: The verdict that allows the plan to run can only be true 
		  if all the potentially affected blueprints that the SPJ will lock at the same time, and they aren't locked by another job; When this verdict is acquired,
		  begin to make backups of the blueprints we intend to potentially affect; the hotBPManager, which should have the plan's intended blueprints to backup stored somewhere,
		  should be queried for a list of blueprints to perform backup operations on. Then, perform backups for every item in that list.

		  If the entire contour plan is successful, we won't need to rollback to the backup blueprints,
		  and we may erase them. NOTE: Blueprints that don't exist yet shouldnt bother to be copied.

Phase 5 - With the backups acquired, call getProcessableContouredTriangles to fetch the data needed to process WORLD-space FTriangle objects, and run produceEquivalentFTriangle on each of those entries.
		  The outputs of the WORLD-space FTriangles will be groups of BLUEPRINT-space fracturedTriangles, one group for each blueprint that the WORLD-space FTriangle was fractured into.

		  For each BLUEPRINT-space fracturedTriangles object, fetch a number of next-ECBPolyIDs equal to the number of items in the fracturedTriangles, then use
		  that to create an idSet. The idSet will be used in two passes:

		  1.) First pass -- Make a call to currentCTV2->allForgedPolysRegistryRef->addToPolyset for all elements in the idSet --AND-- make a call to 
		      currentCTV2->shellMassDriverRegistryRef->addToPolyset if the currentCTV2PolyType == ECBPolyType::SHELL_MASSDRIVER.

		  2.) Second pass -- Cycle through each element currentOutput.second.fracturedTriangles, create an ECBPoly from each element, and insert it into the current blueprint,
		      via currentBlueprintPtr->insertPolyWithKeyValue
			
		  

Phase 6	- With the CPV2's ECBPolys now added to the server/persistent blueprints, and successfully registered with the appropriate registries, mass driving may now
          begin.

Phase 7 - cleanup job <name?> that removes affected blueprints utilized by the plan, from the hotBPManager; release ServerJobBlockingFlags, etc.



*/

#include "ServerPhasedJobBase.h"
#include "SJRunCPV2.h"
#include "SJSendRequestForOGLMRMCBlueprints.h"

class SPJBuildCPV2Mountain : public ServerPhasedJobBase
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