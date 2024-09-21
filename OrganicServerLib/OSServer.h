#pragma once
#ifndef OSSERVER_H
#define OSSERVER_H
#include <iostream>
#include "OrganicSystem.h"
#include "OSTrianglePoint.h"
#include "OSTriangleLineTraverser.h"
#include "OSTerrainFormation.h"
#include "OSCommandDirector.h"
#include "OrganicStemcellManager.h"
#include "OSContouredTriangleRunner.h"
#include "OSPDir.h"
#include "EnclaveCollectionBlueprint.h"
#include "OSContourPlanDirections.h"
#include "EnclaveKeyDef.h"
#include "ECBBorderLineList.h"
#include "ECBPoly.h"
#include "ECBPolyLine.h"
#include "ECBPolyPointTri.h"
#include "EnclaveKeyTri.h"
#include "OSWinAdapter.h"
#include <unordered_map>
#include <string>
#include <chrono>
#include <mutex>
#include "OrganicClient.h"
#include "ServerProperties.h"
#include "TriangleLine.h"
#include "PolyDebugLevel.h"
#include "ContourBase.h"
#include "ContouredMountain.h"
#include "BlankContour.h"
#include "PointAdherenceOrder.h"
#include "MessageCable.h"
#include "ServerMessageInterpreter.h"
#include "ServerJobManager.h"
#include "ContourPlanStateContainer.h"
#include "ECBMap.h"
#include "AdjacentFinder.h"
#include "HotBlueprints.h"

#include "ContouredPlanV2Base.h"
#include "CPV2Mountain.h"
#include "CPV2SingleTriangle.h"

// Below: temporary header files; just for testing BDM
#include "ReconstitutionManager.h"
#include "OSCMThreadMode.h"


class OSServer
{
	public:
		std::shared_ptr<OrganicSystem> organicSystemPtr;
		OrganicClient client;
		int serverRunMode = 0;				// will be set in constructor

		OSServer(int numberOfFactories, int T1_bufferCubeSize, int T2_bufferCubeSize, int windowWidth, int windowHeight, int serverMode, int serverSlaves);		// manual startup specification
		OSServer();							// will read from server properties file to start
		~OSServer();						// destructor; required for deletion of threads

		void runServer();					// runs the server, after the command line has been set up.
		void executeCommandLine();			// runs the command line
		int checkIfBlueprintExists(EnclaveKeyDef::EnclaveKey in_Key);									// returns 1 if blueprint exists (requires heap mutex)




		void sendAndRenderBlueprintToLocalOS(EnclaveKeyDef::EnclaveKey in_key);
		void sendAndRenderAllBlueprintsToLocalOS();											// transfers all processed blueprints to the local OS.
		void setWorldDirectionInClient(float in_directionX, float in_directionY, float in_directionZ);


		// blueprint testing functions for ContouredPlanV2Base
		void constructCPV2SingleTriangle();
		void runSingleMountainV2();

	private:
		ServerMessageInterpreter messageInterpreter;
		MessageCable serverMessages;
		ServerJobManager serverJobManager;
		OrganicStemcellManager OSCManager;
		OSCommandDirector OSdirector;
		HotBlueprints hotBPManager;		// stores the current blueprint keys that are determined as possibly being affected by the currently 
																// running ContourPlan. It should only be read, written, and analyzed 
																// in a controlled manner (via SJs)

		short isServerActive = 1;			// flag for determining server
		short numberOfSlaves = 0;			// number of slave threads
		int isCommandLineRunning = 1;
		int isCommandLineShutDown = 0;		// is the commandLine shutdown?
		std::string currentWorld;
		std::mutex serverReadWrite;			// the server's mutex for reading/writing into it's variables
		std::mutex commandLineRunningMutex;	// mutex for when the command line runs
		std::condition_variable commandLineCV;
		friend class OSTriangleLineTraverser;
		friend class ServerMessageInterpreter;
		friend class ServerJobManager;
		friend class ServerJobProxy;

		std::unordered_map<string, std::unique_ptr<ContourBase>> newContourMap;

		ECBMap serverBlueprints; // ECBMap which stores all server blueprints
		ECBMap cpRunBackupBlueprints;	// a temporary map that stores copies of blueprints that are about to be affected by a ContourPlan run; it is 
										// used to restore blueprints in the event that a ContourPlan run fails; regardless of success or failure of the CP run,
										// this always needs to be emptied at the end.

		ContourPlanStateContainer planStateContainer;

		ContourBase* getDerivedContourPlan(string in_string);





		// NEW Contour plan functions
		std::unordered_map<string, std::unique_ptr<ContouredPlanV2Base>> plansV2Map;
		void addPlanV2(std::string in_planName, 
						OSTerrainFormation in_Formation, 
						DoublePoint in_polyPoint, 
						int in_numberOfLayers, 
						float in_distanceBetweenLayers, 
						float in_startRadius, 
						float in_expansionValue);
		
		// For below: creates a new plan, and then initializes it via Message; 
		// the name of the plan will be the value of in_planNAme, and the in_formation value will determine
		// which plan gets used.
		void addAndInitializePlanFromMessage(std::string in_planName,
											OSTerrainFormation in_formation,
											Message in_planData);

		bool attemptCPV2KeyLoadIntoHotKeys();	// attempts to load the needed keys of a CPV2 that is about to execute, into the hotKeys, of the hotBPManager.
												// The attempt will fail if any of the CPV2 needed keys are already in the hotKeys (which indicates that another job/work is using them)										
												//
												// Utilized by the SJ, SJGenerateBackupsForCPV2, to determine a valid verdict that allows the SJ to continue.

												

		void runCPV2SPJ(Message in_metadataMessage);	// Does the following: 
														// 1.	initialze a plan with a call to addAndInitializePlanFromMessage (also in this class),
														// 2.	call amplifyContourPoints, insertMaterials, buildContouredTriangles on the plan
														// 3.	execute the plan, no-input style.

		void addAndBuildCPV2MeshForSPJ(Message in_metadataMessage);	// Does the following: 
																		// 1.	initialze a plan with a call to addAndInitializePlanFromMessage (also in this class),
																		// 2.	call amplifyContourPoints, insertMaterials, buildContouredTriangles on the plan
																		//
																		// Ultimately called by SJAddCPV2AndPrepareMeshPoints, via ServerJobProxy.

		void determineAffectedBlueprintsForCPV2(std::string in_cpv2PlanName);	// Determines what blueprints a CPV2 would potentially need to be used, in the HotBlueprints
																				// member, but does not actually move them to be "hot" (i.e, appendPillarKeysToHotkeys).
																				//
																				// Ultimately called by SJDetermineAffectedBlueprints, via ServerJobProxy.

		void runMassDriversForCPV2(std::string in_cpv2PlanName);	// run the mass drivers for a given CPV2; does no special checks to ensure all the data is there, it
																	// simply calls for the mass drivers to run; so use carefully!
																	//
																	// Ultimately called by SJRunMassDriversForCPV2, via ServerJobProxy.

		void cleanupCPV2(std::string in_planName);		// cleans up the hotBPManager, and deactivates blocking flags as necessary, after a CPV2 has been run via SPJ.
														// The plan name passed into this function is used to determine whether or not the CPV2 that was run was "successful",
														// by checking the CPV2 for that data; If successful, the backups made for the CPV2 may be erased. However, if not successful,
														// backups will probably need to be restored.
														//
														// Ultimately called by SJCleanupCPV2, via ServerJobProxy.

		ContouredPlanV2Base* getPlanV2Ref(std::string in_planNameToGet);
		void executePlanV2(std::string in_planNameToExecute);
		void copyOverCPV2ECBPolys(std::string in_planNameToExecute);
		void executePlanV2NoInput(std::string in_planNameToExecute);	// needs to be capped whenever a CPV2 must be run
																		// by a SPJ.




		void runPolyFracturer(EnclaveKeyDef::EnclaveKey in_key, PolyDebugLevel in_debugLevel);							// testing only (for now)
		void runPolyFracturerForAllBlueprints();

		void constructBlueprintFromFile(std::string in_worldName, EnclaveKeyDef::EnclaveKey in_blueprintKey);
		void checkClientMessages();
		void addDerivedContourPlan(std::string in_planName, OSTerrainFormation in_Formation, ECBPolyPoint in_polyPoint, int in_numberOfLayers, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue);
		void executeDerivedContourPlan(std::string in_string);

		// Generic ContourPlan run functions
		void executeDerivedContourPlanNoInput(std::string in_string);	// run the plan without waiting for input afterwards.

		void generateBlueprintBackups(std::string in_string);	// will generate backups for blueprints affected by a CP; the message should just cotn
		void generateBlueprintBackupsForCPV2SPJ();	// looks into the pillarCPV2Keys of hotBPManager, to determine blueprints to backup.

		//void generateBlueprintBackupsV2(std::string in_string);	// will generate backups for blueprints affected by a CP; the message should just cotn
		void runContourPlanWorldTracing(std::string in_string);
		void buildContourPlanAffectedBlueprints(std::string in_string);
		void runContourPlanFracturingAndMassDriving(std::string in_string);
		void checkContourPlanSuccess(std::string in_string);

		void traceTriangleThroughBlueprints(OSContouredTriangle* in_Triangle, OSContourPlanDirections in_Directions, PointAdherenceOrder* in_orderRef);		// constructs primary polygon lines for each line of the contoured triangle that the 
		void writeECBPolysToDisk(EnclaveKeyDef::EnclaveKey in_keys);
		void transferBlueprintToLocalOS(EnclaveKeyDef::EnclaveKey in_key);
		void analyzeECBPoly(ECBPoly* in_polyRef);
		void setCurrentWorld(std::string in_worldName);
		int runCommandLine(mutex& in_serverReadWrite, std::condition_variable& in_conditionVariable, int in_commandLineRunningStatus, int* is_commandLineShutDownStatus);		// may be deprecated eventually, replaced by runCommandLineV3. Deprecation validity tests began 9/29/2020.
		int runCommandLineV2(mutex& in_serverReadWrite, int in_commandLineRunningStatus, int* is_commandLineShutDownStatus);													// may be deprecated eventually, replaced by runCommandLineV3. Deprecation validity tests began 9/29/2020.
		void runCommandLineV3();
		int checkServerStatus(mutex& in_serverReadWrite);
		void setServerStatus(mutex& in_serverReadWrite, int in_valueToSet, int* in_commandLineStatus);
		void signalCommandLineShutdown(mutex& in_serverReadWrite, int in_valueToSet, int* in_clShutdownFlag);
		int getCommandLineShutdownValue(mutex& in_serverReadWrite);
		void signalServerShutdown(mutex& in_serverMutex);
		OSPDir getFormationDirections(OSTerrainFormation in_terrainFormation);

		// test run jobs for ServerJobManager
		void prepCPMountain(Message in_metadataMessage);	// preps the triangles and parameters used for a ContourPlan that builds a MOUNTAIN;
															// used when the client clicks button1 to run a contour plan. (Last update: 1/7/2022)
		void constructBigMountTestNoInput();
		void jobSendUpdateMessageToJobManager(Message in_message);
		void jobSendOutgoingMessageToInterpreter(Message in_message);

};

#endif