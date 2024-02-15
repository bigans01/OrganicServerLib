#pragma once

#ifndef SERVERJOBPROXY_H
#define SERVERJOBPROXY_H

#include "Message.h"
#include "ServerJobBlockingFlags.h"
#include <iostream>
#include "ServerJobRunCategory.h"

class OSServer;
class ServerJobProxy
{
	public:
		static void callServerJobPrepCPMountain(OSServer* in_serverRef, Message in_mountainMetaData);
		static void callServerRunContourPlanWorldTracing(OSServer* in_serverRef, std::string in_contourPlanName);
		static void callServerBuildContourPlanAffectedBlueprints(OSServer* in_serverRef, std::string in_contourPlanName);

		static void callServerRunContourPlanFracturingAndMassDriving(OSServer* in_serverRef, std::string in_contourPlanName);
		
		static void callServerRunCPV2(OSServer* in_serverRef, Message in_cpv2Metadata);
		static void callServerAddAndBuildCPV2MeshForSPJ(OSServer* in_serverRef, Message in_cpv2Metadata);	// Uses the referenced OSServer to add and initialize a plan. 
																											// Utilized by SJRequestContourPlanLock.
		
		static bool callServerAttemptCPV2KeyLoadIntoHotKeys(OSServer* in_serverRef);	// used in SJGenerateBackupsForCPV2::getCurrentVerdict(), to determine whether or not all plans of a CPV2 

		static void callServerRunMassDriversForCPV2(OSServer* in_serverRef, std::string in_contourPlanName);
																				// are available for locking, so that a CPV2 may run.

		static void callServerCleanupCPV2(OSServer* in_serverRef, std::string in_contourPlanName);

		static void callServerJobRunBigMountTest(OSServer* in_serverRef);
		static void callServerJobSendUpdateMessageToJobManager(OSServer* in_serverRef, Message in_message);
		static void callServerSendOutgoingMessageToMessageInterpreter(OSServer* in_serverRef, Message in_message);


		static void callServerGenerateBlueprintBackupsForCP(OSServer* in_serverRef, std::string in_contourPlanName);
		static void callServerGenerateBlueprintBackupsForCPV2SPJ(OSServer* in_serverRef);
		static void callServerCopyOverCPV2ECBPolys(OSServer* in_serverRef, std::string in_contourPlanName);

		static void callServerCheckContourPlanSuccess(OSServer* in_serverRef, std::string in_contourPlanName);

		static bool checkIfServerJobBlockingFlagExists(OSServer* in_serverRef, ServerJobBlockingFlags in_flagToCheck);
		static void activateServerJobBlockingFlag(OSServer* in_serverRef, ServerJobBlockingFlags in_flagToCheck);
		static void deactivateServerJobBlockingFlag(OSServer* in_serverRef, ServerJobBlockingFlags in_flagToCheck);
		static bool isJobCategoryAtZero(OSServer* in_serverRef, ServerJobRunCategory in_categoryToCheck);

		static bool callAttemptFlagActivation(OSServer* in_serverRef, ServerJobBlockingFlags in_flagToCheck);	// Attempts to lock a flag, and returns true if the lock is successful.
																												// Utilized by SJAddCPV2AndPrepareMeshPoints

		static void callDetermineAffectedBlueprintsForCPV2(OSServer* in_serverRef, std::string in_cpv2NameToDetermineFor);
};

#endif
