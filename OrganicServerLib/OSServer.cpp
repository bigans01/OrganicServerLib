#include "stdafx.h"
#include "OSServer.h"
#include "OrganicUtils.h"
//#include "OSContourPlan.h"
//#include "shitTest.h"

OSServer::OSServer(int numberOfFactories, int T1_bufferCubeSize, int T2_bufferCubeSize, int windowWidth, int windowHeight, int serverMode, int serverSlaves)
{
	client.createOS(numberOfFactories, T1_bufferCubeSize, T2_bufferCubeSize, windowWidth, windowHeight, serverMode);		// create the client's OS
	organicSystemPtr = client.OS;																							// assign organicSystemPtr to the client's OS
	numberOfSlaves = serverSlaves;		// set number of server slaves
	serverRunMode = serverMode;		// set the run mode (0, 1, 2, 3) etc
	OSWinAdapter::checkServerFolders();		// ensure that the world folder is created
	setCurrentWorld("test");	// test world folder
	OSCManager.initializeOSC(1, serverSlaves, organicSystemPtr->getMasterFactory());			// signal for server mode, 2 threads
	serverJobManager.initialize(this);
	OSdirector.initialize(this);
	messageInterpreter.initialize(this, &serverMessages);
}

OSServer::OSServer()
{
	ServerProperties serverProperties = OSWinAdapter::readServerProperties();
	std::cout << "++++++Performing startup via pre-defined server properties++++++" << std::endl;
	std::cout << "Factories: " << serverProperties.factoryCount << std::endl;
	std::cout << "T1 size: " << serverProperties.T1_bufferCubeSize << std::endl;
	std::cout << "T2 size: " << serverProperties.T2_bufferCubeSize << std::endl;
	std::cout << "OpenGL width: " << serverProperties.windowWidth << std::endl;
	std::cout << "OpenGL height: " << serverProperties.windowHeight << std::endl;
	std::cout << "Run mode: " << serverProperties.runMode << std::endl;
	std::cout << "Server slaves: " << serverProperties.serverSlaves << std::endl;
	client.createOS(serverProperties.factoryCount, serverProperties.T1_bufferCubeSize, serverProperties.T2_bufferCubeSize, serverProperties.windowWidth, serverProperties.windowHeight, serverProperties.runMode);		// create the client's OS
	organicSystemPtr = client.OS;																							// assign organicSystemPtr to the client's OS
	numberOfSlaves = serverProperties.serverSlaves;		// set number of server slaves
	serverRunMode = serverProperties.runMode;		// set the run mode (0, 1, 2, 3) etc
	OSWinAdapter::checkServerFolders();		// ensure that the world folder is created
	setCurrentWorld("test");	// test world folder
	OSCManager.initializeOSC(1, serverProperties.serverSlaves, organicSystemPtr->getMasterFactory());			// signal for server mode, 2 threads
	serverJobManager.initialize(this);
	OSdirector.initialize(this);
	messageInterpreter.initialize(this, &serverMessages);
}

OSServer::~OSServer()
{
	std::cout << "!!! OSServer destructor call BEGIN...." << std::endl;
	//OSCManager.despawnAllCells();
	std::cout << "!!! OSServer destructor call END...." << std::endl;
}

void OSServer::addDerivedContourPlan(string in_planName, OSTerrainFormation in_Formation, ECBPolyPoint in_startPoint, int in_numberOfLayers, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue)
{
	std::cout << "Adding new, DERIVED contour plan. " << std::endl;
	if (in_Formation == OSTerrainFormation::MOUNTAIN)	// add a mountain
	{
		newContourMap[in_planName].reset(new ContouredMountain());		// for new testing only (11/15/2019)
		newContourMap[in_planName].get()->initialize(in_startPoint, in_numberOfLayers, in_distanceBetweenLayers, in_startRadius, in_expansionValue);		// call the initialization function (common to all ContourBase derivatives)
	}
	if (in_Formation == OSTerrainFormation::NOVAL)		// add a blank plan (must add own triangles)
	{
		newContourMap[in_planName].reset(new BlankContour());
	}
}

void OSServer::addPlanV2(std::string in_planName,
	OSTerrainFormation in_Formation,
	DoublePoint in_polyPoint,
	int in_numberOfLayers,
	float in_distanceBetweenLayers,
	float in_startRadius,
	float in_expansionValue)
{
	std::cout << "Adding new, plan V2..." << std::endl;
	if (in_Formation == OSTerrainFormation::MOUNTAIN)
	{
		std::cout << "!! Attempting add of CPV2Mountain..." << std::endl;
		std::cout << "!! Number of layers: " << in_numberOfLayers << std::endl;

		plansV2Map[in_planName].reset(new CPV2Mountain());
		plansV2Map[in_planName].get()->initialize(in_polyPoint, in_numberOfLayers, in_distanceBetweenLayers, in_startRadius, in_expansionValue);		// call the initialization function (common to all ContourBase derivatives)
	}

	if (in_Formation == OSTerrainFormation::NOVAL)
	{
		plansV2Map[in_planName].reset(new CPV2SingleTriangle());
	}
}


void OSServer::runCPV2SPJ(Message in_metadataMessage)
{
	// Open the Message
	in_metadataMessage.open();


	// (CPV2C-1) First int: Read the OSTerrainFormation value, and pop it off (1 int)
	OSTerrainFormation selectedFormation = OSTerrainFormation(in_metadataMessage.readInt());
	in_metadataMessage.removeIntsFromFrontAndResetIter(1);

	// First string: the plan name, needed to add the plan, and pop it off
	std::string planName = in_metadataMessage.readString();
	in_metadataMessage.removeStringsFromFrontAndResetIter(1);

	// Construct the passdown Message, which is the original value of in_metadataMessage, MINUS the OSTerrainFormation enum and the planName string.
	Message passDownMessage = in_metadataMessage;

	// add the plan, initializizing it via Message. 
	addAndInitializePlanFromMessage(planName, selectedFormation, passDownMessage);

	// once the plan is added and initialized with its relevant data, amplify the points, insert materials, and build the contoured triangles.
	auto summit2Ref = getPlanV2Ref(planName);
	summit2Ref->amplifyAllContourLinePoints();
	summit2Ref->insertMaterials(TriangleMaterial::GRASS, TriangleMaterial::DIRT);
	summit2Ref->buildContouredTriangles();

	// Now, execute the plan with no inputs.
	executePlanV2NoInput(planName);
}

void OSServer::addAndBuildCPV2MeshForSPJ(Message in_metadataMessage)
{
	// Open the Message
	in_metadataMessage.open();

	std::cout << "OSServer::addAndBuildCPV2MeshForSPJ: beginning. " << std::endl;


	// (CPV2C-1) First int: Read the OSTerrainFormation value, and pop it off (1 int)
	OSTerrainFormation selectedFormation = OSTerrainFormation(in_metadataMessage.readInt());
	in_metadataMessage.removeIntsFromFrontAndResetIter(1);

	// First string: the plan name, needed to add the plan, and pop it off
	std::string planName = in_metadataMessage.readString();
	in_metadataMessage.removeStringsFromFrontAndResetIter(1);

	// Construct the passdown Message, which is the original value of in_metadataMessage, MINUS the OSTerrainFormation enum and the planName string.
	Message passDownMessage = in_metadataMessage;

	// add the plan, initializizing it via Message. 
	addAndInitializePlanFromMessage(planName, selectedFormation, passDownMessage);

	// once the plan is added and initialized with its relevant data, amplify the points, insert materials, and build the contoured triangles.
	auto summit2Ref = getPlanV2Ref(planName);
	summit2Ref->amplifyAllContourLinePoints();
	summit2Ref->insertMaterials(TriangleMaterial::GRASS, TriangleMaterial::DIRT);
	summit2Ref->buildContouredTriangles();

	std::cout << "OSServer::addAndBuildCPV2MeshForSPJ: ending. " << std::endl;
}

void OSServer::determineAffectedBlueprintsForCPV2(std::string in_cpv2PlanName)
{
	// WARNING: Remember, this function DOES not check if the plan already exists.
	auto targetPlan = getPlanV2Ref(in_cpv2PlanName);
	auto trianglesToProcess = targetPlan->getProcessableContouredTriangles();
	targetPlan->determineEstimatedAffectedBlueprints(trianglesToProcess, &hotBPManager);
}

void OSServer::runMassDriversForCPV2(std::string in_cpv2PlanName)
{
	auto targetPlan = getPlanV2Ref(in_cpv2PlanName);
	EnclaveFractureResultsMap tempMap;
	targetPlan->runMassDriversV2(&client, &serverBlueprints, &tempMap);
}

void OSServer::cleanupCPV2(std::string in_planName)
{
	// Part 1: If the CPV2 ran successfully, clear out the backups;
	//		   Use the passed-in string parameter to check the plan to see if it was considered "successful"
	//		
	//	If the plan was NOT successful, we will probably need to use backups to restore the blueprints that were
	//	affected by the CPV2 run.
	auto cpv2Ref = getPlanV2Ref(in_planName);
	if (cpv2Ref->wasPlanSuccessful())
	{
		cpRunBackupBlueprints.clearAllBlueprints();
	}

	// Part 2: unload the pillar keys
	hotBPManager.unloadCPV2KeysFromHotkeys();

	// Part 3: release the necessary flags, that were raised.
	serverJobManager.deactivateBlockingFlag(ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN);
}

void OSServer::addAndInitializePlanFromMessage(std::string in_planName,
	OSTerrainFormation in_formation,
	Message in_planData)
{
	std::cout << "Adding new plan, initialized from Message..." << std::endl;
	switch (in_formation)
	{
		case OSTerrainFormation::MOUNTAIN:
		{
			plansV2Map[in_planName].reset(new CPV2Mountain());
			break;
		}
	}

	// once the plan object has been set up, send it the plan data Message. Remember, it is up to the CPV2 to interpret this Message!
	plansV2Map[in_planName].get()->initializeFromMessage(in_planData);
}

void OSServer::constructCPV2SingleTriangle()
{
	DoublePoint naPoint;
	addPlanV2("singleTriangle", OSTerrainFormation::NOVAL, naPoint, 0, 0, 0, 0);
	auto singleTriangleRef = getPlanV2Ref("singleTriangle");

	singleTriangleRef->insertMaterials(TriangleMaterial::GRASS, TriangleMaterial::DIRT);
	singleTriangleRef->buildContouredTriangles();	// build the single contoured triangle, so that we may process it
	executePlanV2("singleTriangle");
}

void OSServer::runSingleMountainV2()
{
	int numberOfLayers = 5;	// (Noted on (1/31/2023) Switch to 1,2,3 separately when testing "OPTION 2" EnclaveTriangle production logic (see OrganicPolyOperationsLib)


	// second mountain

	//summit2.x = 3.33;	// 3.43 = crash? (4/3/2021) --> fixed on 4/5/2021, improved on 4/7/2021, reviewed on 4/8/2021 for smoothness (i.e., removal of hangnails
					// 3.36 = caused a PARTIAL_BOUND to be constructed as a NON_BOUND, due to s/t threshold incorrectly being < 0.000f when it should be < -0.001f, in
					// FusionCandidateProducer::determineRayRelationShipToTriangle (OrganicGLWinLib).
	//summit2.y = 16;
	//summit2.z = 16;

	DoublePoint summit2(3.33, 16, 16);
	addPlanV2("summit2", OSTerrainFormation::MOUNTAIN, summit2, numberOfLayers, 6.81, 9, 9);	// create the points in all contour lines
	auto summit2Ref = getPlanV2Ref("summit2");
	summit2Ref->amplifyAllContourLinePoints();
	summit2Ref->insertMaterials(TriangleMaterial::GRASS, TriangleMaterial::DIRT);
	summit2Ref->buildContouredTriangles();
	executePlanV2("summit2");


	
	DoublePoint summit1(28.0, 16.0, 16.0);
	addPlanV2("summit1", OSTerrainFormation::MOUNTAIN, summit1, numberOfLayers, 12.81, 31.82, 9);
	auto currentPlanRef = getPlanV2Ref("summit1");
	currentPlanRef->amplifyAllContourLinePoints();
	currentPlanRef->insertMaterials(TriangleMaterial::GRASS, TriangleMaterial::DIRT);
	currentPlanRef->buildContouredTriangles();
	executePlanV2("summit1");
	
	DoublePoint summit3(40.0, 16.0, 25.0);
	addPlanV2("summit3", OSTerrainFormation::MOUNTAIN, summit3, 3, 4.81, 16.82, 7);
	auto summit3Ref = getPlanV2Ref("summit3");
	summit3Ref->amplifyAllContourLinePoints();
	summit3Ref->insertMaterials(TriangleMaterial::GRASS, TriangleMaterial::DIRT);
	summit3Ref->buildContouredTriangles();
	executePlanV2("summit3");
	
	/*
	EnclaveKeyDef::EnclaveKey debugBPKey(0, 0, 0);
	EnclaveKeyDef::EnclaveKey debugOREKey(3, 1, 6);
	auto debug2BP = serverBlueprints.getBlueprintRef(debugBPKey);
	int debugBlockCount = debug2BP->fractureResults.fractureResultsContainerMap[debugOREKey].getTotalUnexposedBlocks();
	std::cout << "+++++++++++ DEBUG: total unexposed block count is: " << debugBlockCount << std::endl;
	int debugCountWait = 3;
	debug2BP->fractureResults.fractureResultsContainerMap[debugOREKey].printContainerStats();
	std::cin >> debugCountWait;
	*/
	
	// --------------- Experimental, optional: print the BDM message for the skeleton super group manager of an ORE.

	//EnclaveKeyDef::EnclaveKey testBPKey(-1, 0, 0);
	//EnclaveKeyDef::EnclaveKey testOREKey(7, 1, 7);

	// Below: an ORE that doesn't exist, shouldn't do anything when printBDMForORESkeletonSGM is called.
	//EnclaveKeyDef::EnclaveKey testBPKey(7, 3, 6);
	//EnclaveKeyDef::EnclaveKey testOREKey(0, 3, 0);

	EnclaveKeyDef::EnclaveKey testBPKey(-1, -2, 1);
	EnclaveKeyDef::EnclaveKey testOREKey(7, 3, 6);

	auto testBPRef = serverBlueprints.getBlueprintRef(testBPKey);
	testBPRef->printBDMForORESkeletonSGM(testBPKey, testOREKey);
	

	// Experimental: BDM logic tests.
	//std::cout << "++++ Starting basic BDM diagnostic tests. " << std::endl;
	int bdmWait = 3;
	EnclaveKeyDef::EnclaveKey bdmBlueprintTestKey(0, -1, 1);

	EnclaveKeyDef::EnclaveKey oreRTBlueprintKey(-1, -1, 1);
	EnclaveKeyDef::EnclaveKey oreRTKey(7, 5, 4);

	std::cout << "++++++++ Starting analysis of ORE oRTHandler stats..." << std::endl;
	std::cout << "Key being analyzed is: "; oreRTKey.printKey(); std::cout << std::endl;
	serverBlueprints.getOrganicRawEnclaveRef(oreRTBlueprintKey, oreRTKey)->testORTHblocks();
	int oreRTwait = 3;
	std::cout << "++++++++ Done with oRTHandler stats...enter number to continue. " << std::endl;
	std::cin >> oreRTwait;
	
	//EnclaveKeyDef::EnclaveKey bdmORETestKey(1, 5, 2);
	//serverBlueprints.getOrganicRawEnclaveRef(bdmBlueprintTestKey, bdmORETestKey)->printMapData();

	MessageContainer fetchedBDMData = serverBlueprints.getBlueprintRef(bdmBlueprintTestKey)->convertBlueprintTOBDMFormat(bdmBlueprintTestKey);
	ReconstitutionManager testRecon;
	testRecon.insertMessageContainerForProcessing(fetchedBDMData);
	//testRecon.executeContainerProcessing();
	//testRecon.printReconstitutedBlueprintStats(bdmBlueprintTestKey);


	/*
	// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	// BEGIN TEST CASE 1: replace an already-existing ORE in a LOD_BLOCK state with a reconstituted version of that ORE
	// the following ORE should be an LOD_BLOCK.
	testRecon.runOREReconstitutionSpecific(bdmBlueprintTestKey, EnclaveKeyDef::EnclaveKey(2, 4, 5));
	OrganicRawEnclave fetchedORE = testRecon.fetchReconstitutedORE(bdmBlueprintTestKey, EnclaveKeyDef::EnclaveKey(2, 4, 5));
	std::cout << "-----> printing fetched ORE data: " << std::endl;
	fetchedORE.printTrianglesPerBlock();

	// Below: test if the fetchedORE (LOD_BLOCK) works correctly, when it's inserted into serverBlueprints.
	auto testBPRef = serverBlueprints.getBlueprintRef(bdmBlueprintTestKey);

	bool doesOREExist= serverBlueprints.checkIfBlueprintContainsSpecificOre(bdmBlueprintTestKey, EnclaveKeyDef::EnclaveKey(2, 4, 5));
	if (doesOREExist)
	{
		std::cout << "!!! NOTICE: target ORE to replace with reconstituted ORE already exists. " << std::endl;
	}

	testBPRef->fractureResults.fractureResultsContainerMap[EnclaveKeyDef::EnclaveKey(2, 4, 5)] = fetchedORE;

	// END TEST CASE 1
	// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||





	// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	// BEGIN TEST CASE 2: Setting up and replacing an LOD_FULL ORE with a reconstituted one.
	EnclaveKeyDef::EnclaveKey fullKey(3, 3, 3);
	testRecon.runOREReconstitutionSpecific(bdmBlueprintTestKey, fullKey);
	OrganicRawEnclave fetchedFullORE = testRecon.fetchReconstitutedORE(bdmBlueprintTestKey, fullKey);
	auto testBPRefForFullORE = serverBlueprints.getBlueprintRef(bdmBlueprintTestKey);
	std::cout << "----> moving fetched FULL ORE into associated blueprint..." << std::endl;
	testBPRefForFullORE->fractureResults.fractureResultsContainerMap[fullKey] = fetchedFullORE;
	// END TEST CASE 2:
	// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



	
	// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	// BEGIN TEST CASE 3: Setting up a LOD_ENCLAVE_SMATTER ORE (not doing anything with it yet)
	std::cout << "Printing container stats for the target LOD_ENCLAVE_SMATTER ORE..." << std::endl;
	EnclaveKeyDef::EnclaveKey smatterKey(4, 2, 7); 
	serverBlueprints.getOrganicRawEnclaveRef(bdmBlueprintTestKey, smatterKey)->printContainerStats();
	std::cout << "Done printing stats for the target LOD_ENCLAVE_SMATTER ORE." << std::endl;

	std::cout << "Running reconstitution of the SMATTER ORE..." << std::endl;
	testRecon.runOREReconstitutionSpecific(bdmBlueprintTestKey, smatterKey);
	OrganicRawEnclave fetchedSMatterORE = testRecon.fetchReconstitutedORE(bdmBlueprintTestKey, smatterKey);
	auto testBPRefForSMatterORE = serverBlueprints.getBlueprintRef(bdmBlueprintTestKey);
	testBPRefForSMatterORE->fractureResults.fractureResultsContainerMap[smatterKey] = fetchedSMatterORE;

	// END TEST CASE 3
	// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	*/

	
	// BEGIN TEST CASE 4: Full reconstitution. Don't use with other test cases.
	// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	
	std::string originalHash = serverBlueprints.getBlueprintRef(bdmBlueprintTestKey)->getBlueprintHash();
	std::cout << "=============== hash of initial blueprint (non-constitution): " << originalHash << std::endl;
	int originalCheck = 3;
	std::cin >> originalCheck;


	//testRecon.attemptFullReconstitution(bdmBlueprintTestKey);
	testRecon.processMessageContainersAndCheckForReconstitutables();	// remember, this processes all Messsage containers, AND attempts to reconstitute any blueprint, 
																		// whenever it is called.


	EnclaveCollectionBlueprint fetchedBlueprint = testRecon.fetchReconstitutedBlueprint(bdmBlueprintTestKey);
	std::cout << "!!! Size of datain fetched blueprint: " << fetchedBlueprint.fractureResults.fractureResultsContainerMap.size() << std::endl;

	std::string reconstructedHash = fetchedBlueprint.getBlueprintHash();
	std::cout << "============== hash of reconstituted blueprint: " << reconstructedHash << std::endl;
	int reconstitutedCheck = 4;
	std::cin >> reconstitutedCheck;

	if (originalHash == reconstructedHash)
	{
		std::cout << "Hash match detected!" << std::endl;
	}
	std::cout << "original: " << originalHash << std::endl;
	std::cout << "reconstituted: " << reconstructedHash << std::endl;
	

	/*

	// NOTE: Don't add blueprints via reconstitution, until the OT handler transition is complete in OrganicIndependents library).
	// This is because BDM data doesn't take into account the oreOTHandler in OrganicRawEnclave yet (so oreRTHandler data doesnt ever get produced)
	// This will result in missing data in the OREs.

	auto copyStart = std::chrono::high_resolution_clock::now();
	//serverBlueprints.addBlueprintViaCopy(bdmBlueprintTestKey, fetchedBlueprint);
	//serverBlueprints.addBlueprintViaRef(bdmBlueprintTestKey, &fetchedBlueprint);
	auto copyEnd = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> copyElapsed = copyEnd - copyStart;

	

	std::cout << "********** Copy of reconstituted blueprint into server blueprints time: " << copyElapsed.count() << std::endl;
	*/
	
	//testRecon.printReconstitutedBlueprintStats(bdmBlueprintTestKey);
	// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	// END TEST CASE 4
	

	std::cout << "++++ DONE with basic BDM diagnostic tests. Enter integer to continue. " << std::endl;
	std::cin >> bdmWait;

	int doneValue = 3;
	std::cout << "Plan V2 test done. Enter key to continue (or just end). " << std::endl;
	std::cin >> doneValue;
	
}
void OSServer::prepCPMountain(Message in_metadataMessage)
{
	in_metadataMessage.open();
	std::string planName = in_metadataMessage.readString();
	int mountainLayers = in_metadataMessage.readInt();
	float mountainLocationX = in_metadataMessage.readFloat();
	float mountainLocationY = in_metadataMessage.readFloat();
	float mountainLocationZ = in_metadataMessage.readFloat();
	float distBetweenLayers = in_metadataMessage.readFloat();
	float startRadius = in_metadataMessage.readFloat();
	float expansionValue = in_metadataMessage.readFloat();

	// Part 1: Set up the plan, give it's necessary parmaters, etc.
	ECBPolyPoint summit1(mountainLocationX, mountainLocationY, mountainLocationZ);
	int numberOfLayers = mountainLayers;
	//addDerivedContourPlan(planName, OSTerrainFormation::MOUNTAIN, summit1, numberOfLayers, distBetweenLayers, startRadius, expansionValue);	// create the points in all contour lines
	addPlanV2(planName, OSTerrainFormation::MOUNTAIN, summit1, numberOfLayers, distBetweenLayers, startRadius, expansionValue);	// create the points in all contour lines

	// Part 2: run point amplification, insert materials, and build the actual triangles. Note that at this point, no actual blueprints are affected;
	// we are just constructing the triangles of the plan.
	ContourBase* summit1Ref = getDerivedContourPlan(planName);
	summit1Ref->amplifyAllContourLinePoints();						// amplify the points in all contour lines
	summit1Ref->insertMaterials(TriangleMaterial::GRASS, TriangleMaterial::DIRT); // set materials for mountain
	summit1Ref->buildContouredTriangles();
	std::cout << "!!!!!!!!! --------------> top strips: " << summit1Ref->triangleStripMap.size() << std::endl;
	std::cout << "!!!!!!!!! --------------> bottom strips: " << summit1Ref->bottomTriangleStripMap.size() << std::endl;

	//runContourPlanWorldTracing(planName);				// Part 2: trace contoured triangles. This should only be called once there are no terrain-modifying jobs running; 
														// As a result, this job must wait for all those to finish, which should be done with 
														// the HALT_FUTURE_COLLECTION_MODIFICATIONS blocking flag being set (this will prevent any future block/modification jobs
														// from running until this flag is no longer set, as long as those jobs check for that flag NOT being set as a pre-requisite to running)

	//buildContourPlanAffectedBlueprints(planName);		// Part 3: build affected blueprints.
	//runContourPlanFracturingAndMassDriving(planName);	// Part 4: run fracturing and mass driving.
}
void OSServer::runContourPlanWorldTracing(std::string in_string)
{
	OSWinAdapter::clearWorldFolder(currentWorld);

	std::cout << "SERVER: Executing derived contour plan. " << std::endl;
	ContourBase* planPtr = newContourMap[in_string].get();

	auto traceStart = std::chrono::high_resolution_clock::now();

	// 1. ) Execute all processable OSContouredTriangles in the plan.
	auto processableList = planPtr->getProcessableContouredTriangles();
	auto processableListBegin = processableList.begin();
	auto processableListEnd = processableList.end();
	for (; processableListBegin != processableListEnd; processableListBegin++)
	{
		traceTriangleThroughBlueprints(*processableListBegin, planPtr->planDirections, &planPtr->adherenceData);
	}

	auto traceEnd = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> traceElapsed = traceEnd - traceStart;

	std::cout << "********** Finished tracing this contoured plan's OSContoured triangles; time was: " << traceElapsed.count() << std::endl;

	std::cout << "######### Plan execution complete; " << std::endl;
}

void OSServer::constructBigMountTestNoInput()
{
	std::cout << "||||||| constructing blueprints (version 3)...." << std::endl;
	ECBPolyPoint mountainSummit;
	// 2, 10, 2 = error (1/15/2019)
	mountainSummit.y = 7;
	mountainSummit.x = 2;
	mountainSummit.z = 2;

	mountainSummit.y = 0;		// error fixed. see notes for roundNearestBlockLineOrCorner on 1/19/2019
	mountainSummit.x = 23.59;
	mountainSummit.z = 0;

	// 10 is stable
	// 6.81 causes anomaly at peak

	// Tested values for layer y difference	: 6.81 (PASS, fixed by F-001)
	//										: 3.81 (PASS, fixed by F-002)
	//										: 2.81 (PASS)
	//										: 1.81 (PASS)
	//										: 0.81 (PASS)

	int numberOfLayers = 35;		// current is 17 (max at 35, no issues) // Fatal error at layer 14 when going 1000+x
	addDerivedContourPlan("mountain", OSTerrainFormation::MOUNTAIN, mountainSummit, numberOfLayers, 6.81, 9, 9);	// create the points in all contour lines
	ContourBase* planRef = getDerivedContourPlan("mountain");
	planRef->amplifyAllContourLinePoints();						// amplify the points in all contour lines
	planRef->insertMaterials(TriangleMaterial::GRASS, TriangleMaterial::DIRT);
	planRef->buildContouredTriangles();

	std::cout << "!!!!!!!!! --------------> Number of strips that will be executed is: " << planRef->triangleStripMap.size() << std::endl;
	executeDerivedContourPlanNoInput("mountain");
}

void OSServer::executeDerivedContourPlan(string in_string)
{
	OSWinAdapter::clearWorldFolder(currentWorld);

	std::cout << "SERVER: Executing derived contour plan. " << std::endl;
	ContourBase* planPtr = newContourMap[in_string].get();

	// 1. ) Execute all processable OSContouredTriangles in the plan.
	auto processableList = planPtr->getProcessableContouredTriangles();

	std::cout << "!!!! Size of processable triangles: " << processableList.size() << std::endl;

	auto traceStart = std::chrono::high_resolution_clock::now();

	auto processableListBegin = processableList.begin();
	auto processableListEnd = processableList.end();
	for (; processableListBegin != processableListEnd; processableListBegin++)
	{
		traceTriangleThroughBlueprints(*processableListBegin, planPtr->planDirections, &planPtr->adherenceData);
	}

	auto traceEnd = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> traceElapsed = traceEnd - traceStart;

	std::cout << "********** Finished tracing this contoured plan's OSContoured triangles; time was: " << traceElapsed.count() << std::endl;

	
	std::cout << "######### Plan execution complete; " << std::endl;

	// 2.) perform fracturing for affected blueprints.
	planPtr->runPolyFracturerForAffectedBlueprints(&client, &serverBlueprints);

	// 3.) run the mass driver for the plan. (if the plan allows for it)
	EnclaveFractureResultsMap tempMap;
	planPtr->runMassDrivers(&client, &serverBlueprints, &tempMap);

	// 4.) update the affected blueprints (aka, update the OrganicRawEnclaves), after all work has been done
	//planPtr->updateAffectedBlueprints(&client, &blueprintMap, &tempMap);

	// 5.) write updated blueprints to disk
	planPtr->writeAffectedBlueprintsToDisk(&serverBlueprints, currentWorld);

	std::cout << "Write to disk complete; press key to transfer blueprints to local OrganicSystem. " << std::endl;
	int someVal = 3;
	std::cin >> someVal;
}

void OSServer::executePlanV2(std::string in_planNameToExecute)
{
	OSWinAdapter::clearWorldFolder(currentWorld);
	auto planV2Ptr = getPlanV2Ref(in_planNameToExecute);
	auto processableCTV2s = planV2Ptr->getProcessableContouredTriangles();

	std::cout << "Size of processableCTV2s: " << processableCTV2s.size() << std::endl;

	planV2Ptr->copyOverProducedECBPolys(processableCTV2s, &serverBlueprints);

	// 3.) run the mass driver for the plan. (if the plan allows for it)
	EnclaveFractureResultsMap tempMap;
	planV2Ptr->runMassDriversV2(&client, &serverBlueprints, &tempMap);

	std::cout << ">>>>>>>>>>>>> Done with test of MassDriving, for executePlanV2. Enter number to continue. " << std::endl;
	int continueVal = 3;
	std::cin >> continueVal;
}

void OSServer::copyOverCPV2ECBPolys(std::string in_planNameToExecute)
{
	auto planV2Ptr = getPlanV2Ref(in_planNameToExecute);
	auto processableCTV2s = planV2Ptr->getProcessableContouredTriangles();
	planV2Ptr->copyOverProducedECBPolys(processableCTV2s, &serverBlueprints);
}

void OSServer::executePlanV2NoInput(std::string in_planNameToExecute)
{
	OSWinAdapter::clearWorldFolder(currentWorld);
	auto planV2Ptr = getPlanV2Ref(in_planNameToExecute);
	auto processableCTV2s = planV2Ptr->getProcessableContouredTriangles();

	std::cout << "Size of processableCTV2s: " << processableCTV2s.size() << std::endl;

	// 1.) Make backup copies of the pre-determined blueprints that we think will be affected, then copy the data
	// from the FTriangle fracturing operation directly into the server blueprints.
	planV2Ptr->copyOverForSPJ(processableCTV2s, &serverBlueprints, &cpRunBackupBlueprints, &hotBPManager);

	// 2.) run the mass driver for the plan. (if the plan allows for it)
	std::cout << "Done copying over data. " << std::endl;


	EnclaveFractureResultsMap tempMap;
	planV2Ptr->runMassDriversV2(&client, &serverBlueprints, &tempMap);

	// 3. ) If everything was successful from the plan running, we can now clear out the backup blueprints.
	//	The criteria for what makes a run successful is still TBD, but this would be the point in time to do it.
	

	std::cout << ">>>>>>>>>>>>> Done with test of MassDriving, for executePlanV2. Enter number to continue. " << std::endl;
	//int continueVal = 3;
	//std::cin >> continueVal;
}

void OSServer::executeDerivedContourPlanNoInput(std::string in_string)
{
	OSWinAdapter::clearWorldFolder(currentWorld);

	std::cout << "SERVER: Executing derived contour plan. " << std::endl;
	ContourBase* planPtr = newContourMap[in_string].get();

	auto traceStart = std::chrono::high_resolution_clock::now();

	// 1. ) Execute all processable OSContouredTriangles in the plan.
	auto processableList = planPtr->getProcessableContouredTriangles();
	auto processableListBegin = processableList.begin();
	auto processableListEnd = processableList.end();
	for (; processableListBegin != processableListEnd; processableListBegin++)
	{
		traceTriangleThroughBlueprints(*processableListBegin, planPtr->planDirections, &planPtr->adherenceData);
	}

	auto traceEnd = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> traceElapsed = traceEnd - traceStart;

	std::cout << "********** Finished tracing this contoured plan's OSContoured triangles; time was: " << traceElapsed.count() << std::endl;

	std::cout << "######### Plan execution complete; " << std::endl;

	// 2.) perform fracturing for affected blueprints.
	planPtr->runPolyFracturerForAffectedBlueprints(&client, &serverBlueprints);

	// 3.) run the mass driver for the plan. (if the plan allows for it)
	EnclaveFractureResultsMap tempMap;
	planPtr->runMassDrivers(&client, &serverBlueprints, &tempMap);

	// 4.) update the affected blueprints (aka, update the OrganicRawEnclaves), after all work has been done
	//planPtr->updateAffectedBlueprints(&client, &blueprintMap, &tempMap);

	// 5.) write updated blueprints to disk
	planPtr->writeAffectedBlueprintsToDisk(&serverBlueprints, currentWorld);

	std::cout << "SERVER: completed contour plan run." << std::endl;
}


void OSServer::generateBlueprintBackups(std::string in_planName)
{
	ContourBase* planPtr = newContourMap[in_planName].get();

	// Part 1: ---------------
	// cycle through each affected blueprint that had generated ECBPolys; as we cycle through, take the EnclaveKey 
	// and put it into an unordered set.
	std::unordered_set<EnclaveKeyDef::EnclaveKey, EnclaveKeyDef::KeyHasher> affectedBlueprints;
	auto processableListBegin = planPtr->planPolyRegistry.polySetRegistry.begin();
	auto processableListEnd = planPtr->planPolyRegistry.polySetRegistry.end();
	for (; processableListBegin != processableListEnd; processableListBegin++)
	{
		affectedBlueprints.insert(processableListBegin->first);
		OperableIntSet tentativePolySet = processableListBegin->second.polySet;	// construct an OperableIntSet from the ForgedPolySet's std::set<int>.
																				// this set represents all the new ECBPoly IDs that were added to the blueprints
																				// during the tracing run; we need to subtract the newly added ones from the existing 
																				// blueprints to get the original value before the tracing began 
		EnclaveKeyDef::EnclaveKey currentBlueprintKey = processableListBegin->first;

		// first, copy the blueprint into the backupECBMap; do this by calling addBlueprintViaCopy
		cpRunBackupBlueprints.addBlueprintViaCopy(currentBlueprintKey, *serverBlueprints.getBlueprintRef(currentBlueprintKey));

		// second, get a ref to that newly added blueprint.
		auto currentBackupBlueprintRef = cpRunBackupBlueprints.getBlueprintRef(currentBlueprintKey);
		
		// third, subtract the added polys from the backup
		auto tentativesBegin = tentativePolySet.begin();
		auto tentativesEnd = tentativePolySet.end();
		for (; tentativesBegin != tentativesEnd; tentativesBegin++)
		{
			currentBackupBlueprintRef->deletePoly(*tentativesBegin);
		}

		// write to disk? (optional)
	}

	// Part 2: ---------------
	// the affected blueprints should already have been built; cycle through each CP affected blueprint key, and only insert that blueprint
	// if it wasn't already in the polySetRegistry we analyzed above. A 1:1 to copy should occur for these, but obviously only if there is
	// an existing blueprint with that key.

	for (auto& currentPlanAffectedBegin : hotBPManager.fetchHotKeys())
	{

		auto processedMatch = affectedBlueprints.find(currentPlanAffectedBegin);
		if (processedMatch == affectedBlueprints.end())	// if the key wasn't found in the above poly registry (from "Part 1" above), then that key wasn't affected
														// but still needs to be copied -- but only if it exists.
		{
			bool doesBlueprintExist = serverBlueprints.checkIfBlueprintExists(*processedMatch);
			if (doesBlueprintExist == true)
			{
				cpRunBackupBlueprints.addBlueprintViaCopy(*processedMatch, *serverBlueprints.getBlueprintRef(*processedMatch));
			}
		}
	}
}

void OSServer::generateBlueprintBackupsForCPV2SPJ()
{
	auto fetchedPillarKeys = hotBPManager.fetchPillarKeys();
	for (auto& currentKey : fetchedPillarKeys)
	{
		// Only bother doing backups, if a copy of the blueprint already exists. 
		if (serverBlueprints.checkIfBlueprintExists(currentKey))
		{
			cpRunBackupBlueprints.addBlueprintViaCopy(currentKey, *serverBlueprints.getBlueprintRef(currentKey));
		}
	}
}

void OSServer::checkContourPlanSuccess(std::string in_string)
{
	// backup blueprint map must always be cleared at the end of the run
	cpRunBackupBlueprints.clearAllBlueprints();
}

bool OSServer::attemptCPV2KeyLoadIntoHotKeys()
{
	return hotBPManager.attemptLoadOfCPV2KeysIntoHotKeys();
}

void OSServer::buildContourPlanAffectedBlueprints(std::string in_string)
{
	ContourBase* planPtr = newContourMap[in_string].get();
	auto keysBegin = planPtr->adherenceData.adherenceOrder.begin();
	auto keysEnd = planPtr->adherenceData.adherenceOrder.end();

	// add each key into the affected blueprints.
	for (; keysBegin != keysEnd; keysBegin++)
	{
		hotBPManager.insertKeyIntoPillar(*keysBegin);
	}

	// build the affected set
	hotBPManager.buildRequiredCPV2Keys();
	hotBPManager.appendPillarKeysToHotkeys();

	std::cout << "!! Total number of potentially affected blueprint keys for the current ContourPlan is: " << hotBPManager.gethotKeysize() << std::endl;


	// Remember, when everything is done, the ServerJobBlockingFlags::HALT_FUTURE_COLLECTION_MODIFICATIONS flag needs to be unset.
	serverJobManager.deactivateBlockingFlag(ServerJobBlockingFlags::HALT_FUTURE_COLLECTION_MODIFICATIONS);
}

void OSServer::runContourPlanFracturingAndMassDriving(std::string in_string)
{
	ContourBase* planPtr = newContourMap[in_string].get();

	// 2.) perform fracturing for affected blueprints.
	planPtr->runPolyFracturerForAffectedBlueprints(&client, &serverBlueprints);

	// 3.) run the mass driver for the plan. (if the plan allows for it)
	EnclaveFractureResultsMap tempMap;
	planPtr->runMassDrivers(&client, &serverBlueprints, &tempMap);

	// 4.) update the affected blueprints (aka, update the OrganicRawEnclaves), after all work has been done
	//planPtr->updateAffectedBlueprints(&client, &blueprintMap, &tempMap);

	// 5.) write updated blueprints to disk
	planPtr->writeAffectedBlueprintsToDisk(&serverBlueprints, currentWorld);

	// 6.) deactive the blocking flag.
	serverJobManager.deactivateBlockingFlag(ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN);

	std::cout << "SERVER: completed contour plan run." << std::endl;
}


void OSServer::transferBlueprintToLocalOS(EnclaveKeyDef::EnclaveKey in_key)
{
	organicSystemPtr->addBlueprint(in_key, serverBlueprints.getBlueprintRef(in_key), 0);		// assign constructed blueprint to organic system
}

void OSServer::runPolyFracturer(EnclaveKeyDef::EnclaveKey in_key, PolyDebugLevel in_debugLevel)
{
	auto bpBegin = serverBlueprints.getBlueprintBeginIter();
	auto bpEnd = serverBlueprints.getBlueprintEndIter();
	for (bpBegin; bpBegin != bpEnd; bpBegin++)
	{
		std::cout << "Running fracturing for: " << bpBegin->first.x << ", " << bpBegin->first.y << ", " << bpBegin->first.z << std::endl;
	}

	client.fracturePolysInBlueprint(in_key, serverBlueprints.getBlueprintRef(in_key), PolyFractureMode::INITIAL_FILL, in_debugLevel);		// we can set any debug level we want to here.
}

void OSServer::runPolyFracturerForAllBlueprints()
{
	auto bpBegin = serverBlueprints.getBlueprintBeginIter();
	auto bpEnd = serverBlueprints.getBlueprintEndIter();
	for (bpBegin; bpBegin != bpEnd; bpBegin++)
	{
		std::cout << "Running fracturing for: " << bpBegin->first.x << ", " << bpBegin->first.y << ", " << bpBegin->first.z << std::endl;
		client.fracturePolysInBlueprint(bpBegin->first, &bpBegin->second, PolyFractureMode::INITIAL_FILL, PolyDebugLevel::NONE);
	}
}

void OSServer::constructBlueprintFromFile(std::string in_worldName, EnclaveKeyDef::EnclaveKey in_blueprintKey)
{
	EnclaveCollectionBlueprint* blueprintRef = serverBlueprints.getBlueprintRef(in_blueprintKey); // fetch a pointer to the blueprint
	BlueprintTransformRefs	transformRefs(blueprintRef->getPolygonMapBeginIter(),
										blueprintRef->getPolygonMapEndIter(),
										blueprintRef->getPolygonMapSize(),
										&blueprintRef->fractureResults.fractureResultsContainerMap,
										&blueprintRef->polyGroupRangeMap);
	BlueprintReadData fetchedData = OSWinAdapter::readBlueprintPolysFromFile(in_worldName, in_blueprintKey, transformRefs);
}

void OSServer::sendAndRenderBlueprintToLocalOS(EnclaveKeyDef::EnclaveKey in_key)
{
	transferBlueprintToLocalOS(in_key);

	//std::cout  << "----> Sending blueprint to OrganicSystem: " << in_key.x << ", " << in_key.y << ", " << in_key.z << std::endl;
	organicSystemPtr->addKeyToRenderList(in_key);
}



void OSServer::sendAndRenderAllBlueprintsToLocalOS()
{
	auto organicstart = std::chrono::high_resolution_clock::now();


	auto blueprintBegin = serverBlueprints.getBlueprintBeginIter();
	auto blueprintEnd = serverBlueprints.getBlueprintEndIter();
	for (blueprintBegin; blueprintBegin != blueprintEnd; blueprintBegin++)
	{
		EnclaveKeyDef::EnclaveKey currentKey = blueprintBegin->first;

		//std::cout << "----> Sending blueprint to OrganicSystem: " << currentKey.x << ", " << currentKey.y << ", " << currentKey.z << std::endl;

		//std::cout << "Poly groupRange size: " << blueprintBegin->second.polyGroupRangeMap.size() << std::endl;
		//std::cout << "Poly map size: " << blueprintBegin->second.fetchNextECBPolyKeyID() << std::endl;
		//std::cout << "Raw enclave size: " << blueprintBegin->second.fractureResults.fractureResultsContainerMap.size() << std::endl;
		//std::cout << "Skeleton container size: " << blueprintBegin->second.fractureResults.ske.size() << std::endl;

		transferBlueprintToLocalOS(currentKey);
		organicSystemPtr->addKeyToRenderList(currentKey);
	}

	auto organicend = std::chrono::high_resolution_clock::now();		// optional, for performance testing only	
	std::chrono::duration<double> organicelapsed = organicend - organicstart;

	std::cout << "Finished transfering blueprints to local OS. Time: " << organicelapsed.count() << " | Press key to continue. " << std::endl;
	int someVal;
	std::cin >> someVal;
}

void OSServer::jobSendUpdateMessageToJobManager(Message in_message)
{
	serverJobManager.updateMessages.insertUpdate(in_message);
}

void OSServer::jobSendOutgoingMessageToInterpreter(Message in_message)
{
	messageInterpreter.messageCableRef->insertOutgoingMessage(std::move(in_message));
}


void OSServer::setWorldDirectionInClient(float in_directionX, float in_directionY, float in_directionZ)
{
	ECBPolyPoint direction(in_directionX, in_directionY, in_directionZ);
	Message newMessage;
	newMessage.messageType = MessageType::REQUEST_FROM_SERVER_SET_WORLD_DIRECTION;
	newMessage.insertPoint(direction);
	// need to insert the client type (localhost, or remotehost) into the message...
	serverJobManager.insertJobRequestMessage(std::move(newMessage));
}

void OSServer::traceTriangleThroughBlueprints(OSContouredTriangle* in_Triangle, OSContourPlanDirections in_Directions, PointAdherenceOrder* in_orderRef)
{
	OSContouredTriangleRunner runner(in_Triangle, in_Directions, &serverBlueprints, in_Triangle->forgedPolyRegistryRef, in_orderRef);
	runner.performRun();
}

void OSServer::writeECBPolysToDisk(EnclaveKeyDef::EnclaveKey in_keys)
{
	EnclaveCollectionBlueprint* blueprintRef = serverBlueprints.getBlueprintRef(in_keys);	// get a pointer to the blueprint
	std::map<int, ECBPoly>::iterator currentPrimaryPoly = blueprintRef->getPolygonMapBeginIter();	// beginning of the poly map
	std::map<int, ECBPoly>::iterator primaryPolyEnd = blueprintRef->getPolygonMapEndIter();		// ending of the poly map
	for (currentPrimaryPoly; currentPrimaryPoly != primaryPolyEnd; ++currentPrimaryPoly)
	{

	}
}

void OSServer::analyzeECBPoly(ECBPoly* in_polyRef)
{

}

void OSServer::setCurrentWorld(std::string in_worldName)
{
	currentWorld = in_worldName;
	std::string worldRootFolderString = "world";
	OSWinAdapter::ensureFolderExists(worldRootFolderString);		// make sure the world root folder exists.
	std::string worldFolderString = worldRootFolderString + "/" + in_worldName;
	OSWinAdapter::ensureFolderExists(worldFolderString);			// make sure the folder for this specific world exists.
}

OSPDir OSServer::getFormationDirections(OSTerrainFormation in_terrainFormation)
{
	OSPDir returnDir;
	if (in_terrainFormation == OSTerrainFormation::MOUNTAIN)
	{
		returnDir = OSPDir::BELOW;
		std::cout << "Mountain detected, will OSPDir will be BELOW" << std::endl;
	}
	else
	{
		returnDir = OSPDir::NOVAL;
	}
	return returnDir;
}

void OSServer::runServer()
{
	if (serverRunMode == 0)				// (prototype only, uncertain of use; still needs to be tested as of 2/17/2021) )
	{
		while (getCommandLineShutdownValue(std::ref(serverReadWrite)) == 0)	// run until the command line is shutdown
		{
			checkClientMessages();							// the server should still listen for remote client's messages, and handle them accordingly, even though OpenGL isn't active.
		}
	}
	else if (serverRunMode == 1)		// server runs actively alongside an instance of an OrganicSystem
	{
		int currentTickCounter = 0;
		while (getCommandLineShutdownValue(std::ref(serverReadWrite)) == 0) // ""
		{
			//std::cout << "Looping...." << std::endl;

			// testingonly
			EnclaveKeyDef::EnclaveKey blockReformBlueprintKey(0, -1, 0);
			EnclaveKeyDef::EnclaveKey blockReformOREKey(4, 6, 4);
			EnclaveKeyDef::EnclaveKey blockKey(2, 3, 2);
			int currentTotalTriangles = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].getTotalTriangles();
			auto currentLodState = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].getLodState();
			auto currentAppendedState = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].getAppendedState();
			int currentNumberOfBlocks = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].blockMap.size();
			//int currentEraseCount = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].eraseCounter;
			std::string state = "";
			std::string appendedState = "";
			if (currentLodState == ORELodState::LOD_BLOCK)
			{
				state = "LOD_BLOCK";
			}

			if (currentAppendedState == OREAppendedState::NONE)
			{
				appendedState = "NONE";
			}

			if (currentTickCounter == 0)
			{
				std::cout << "(BEFORE CLIENT MESSAGE CHECK) Hard TEST: currentTotalTriangles for debugging ORE: " << currentTotalTriangles << "; state is: " << state << "; appendedState is: " << appendedState << "; number of blocks: " << currentNumberOfBlocks << std::endl;
			}

			checkClientMessages();							// Does 3 things: 
															// 1. Move messages coming in from the local client (the underlying OrganicClient on this host), into the messageInterpreter.
															// 2. the messageInterpreter (ServerMessageInterpreter) reads the messsages from local/remote clients, creates the jobs.
															// 3. outgoing messages meant to be sent to the local/remote clients are transferred from the messageInterpreter to their appropriate
															//    MessageCable
													

			serverJobManager.checkForUpdateMessages();		// 2.1: check for messages that would be updates -- or even logical changes to a ServerPhasedJobBase-derived class itself, 
															// such as inserting new ServerJobBase-derived classes  (aka, "atomic" server jobs) into the current job phase -- 
															// to existing jobs, from the previous tick

			serverJobManager.removeCompletedPhasedJobs();	// 2.2: with the updates applied, check to see if there are any jobs to remove. Post-processing will need to be called 
															// (ServerJobBase::runPostCompleteTasks) on any ServreJobBase-derived classes before they are erased.

			serverJobManager.checkForMessages();			// 2.3: check the messages that would spawn new jobs.
			serverJobManager.runJobScan();					// 2.4: look for jobs to execute.
			//serverJobManager.removeCompletedPhasedJobs();

			//serverJobManager.checkForMessages();	// have the job manager check for messages to process.
			//std::cout << ">>>> Running organic tick start. " << std::endl;

			organicSystemPtr->runOrganicTick();		// run core loop

			currentTotalTriangles = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].getTotalTriangles();
			currentLodState = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].getLodState();
			currentNumberOfBlocks = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].blockMap.size();
			//currentEraseCount = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].eraseCounter;
			if (currentTickCounter == 0)
			{
				std::cout << "(AFTER CLIENT MESSAGE CHECK) Hard TEST: currentTotalTriangles for debugging ORE: " << currentTotalTriangles << "; state is: " << state << "; appendedState is: " << appendedState << "; number of blocks: " << currentNumberOfBlocks << std::endl;
				std::cout << ">>>>>>>>>> calling debug test a 2nd time..." << std::endl;
	
			}
			

			// testingonly
			/*
			EnclaveKeyDef::EnclaveKey blockReformBlueprintKey(0, -1, 0);
			EnclaveKeyDef::EnclaveKey blockReformOREKey(4, 6, 4);
			EnclaveKeyDef::EnclaveKey blockKey(2, 3, 2);
			int currentTotalTriangles = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].getTotalTriangles();
			auto currentLodState = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].getLodState();
			auto currentAppendedState = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].getAppendedState();
			int currentNumberOfBlocks = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].blockMap.size();
			int currentEraseCount = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].eraseCounter;
			std::string state = "";
			std::string appendedState = "";
			if (currentLodState == ORELodState::LOD_BLOCK)
			{
				state = "LOD_BLOCK";
			}

			if (currentAppendedState == OREAppendedState::NONE)
			{
				appendedState = "NONE";
			}

			if (currentTickCounter < 50)
			{

				std::cout << "Hard TEST: currentTotalTriangles for debugging ORE: " << currentTotalTriangles << "; state is: " << state << "; appendedState is: " << appendedState << "; number of blocks: " << currentNumberOfBlocks << "; eraseCounter: " << currentEraseCount << std::endl;
			}
			*/
			currentTickCounter++;
			//std::cout << ">>>> Organic tick done. " << std::endl;
		}
		organicSystemPtr->glCleanup();
	}
}

void OSServer::executeCommandLine()
{
	serverJobManager.startCommandLine();
}

int OSServer::runCommandLine(mutex& in_serverReadWrite, std::condition_variable& in_conditionVariable, int in_commandLineRunningStatus, int* is_commandLineShutDownStatus)
{
	std::cout << ">> Job submission begin..." << std::endl;
	int* isCommandLineRunningPtr = &in_commandLineRunningStatus;
	while (*isCommandLineRunningPtr == 1)
	{
		std::cout << "Waiting for command...." << std::endl;
		int input_value;
		std::cout << "> ";
		std::cin >> input_value;
		setServerStatus(std::ref(in_serverReadWrite), input_value, std::ref(isCommandLineRunningPtr));
	}
	signalServerShutdown(std::ref(in_serverReadWrite));
	signalCommandLineShutdown(std::ref(in_serverReadWrite), 1, std::ref(is_commandLineShutDownStatus));
	int returnVal = 5;
	return returnVal;
}

int OSServer::runCommandLineV2(mutex& in_serverReadWrite, int in_commandLineRunningStatus, int* is_commandLineShutDownStatus)
{
	std::cout << ">> Job submission begin..." << std::endl;
	int* isCommandLineRunningPtr = &in_commandLineRunningStatus;
	while (*isCommandLineRunningPtr == 1)
	{
		std::cout << "Waiting for command...." << std::endl;
		int input_value;
		std::cout << "> ";
		std::cin >> input_value;
		setServerStatus(std::ref(in_serverReadWrite), input_value, std::ref(isCommandLineRunningPtr));
	}
	signalServerShutdown(std::ref(in_serverReadWrite));
	signalCommandLineShutdown(std::ref(in_serverReadWrite), 1, std::ref(is_commandLineShutDownStatus));
	int returnVal = 5;
	return returnVal;
}

void OSServer::runCommandLineV3()
{
	std::cout << ">> Job submission begin..." << std::endl;
	while (isCommandLineRunning == 1)
	{
		std::cout << "Waiting for command...." << std::endl;
		int input_value;
		std::cout << "> ";
		std::cin >> input_value;
		setServerStatus(serverReadWrite, input_value, &isCommandLineRunning);
	}
	signalServerShutdown(serverReadWrite);
	signalCommandLineShutdown(serverReadWrite, 1, &isCommandLineShutDown);
}

int OSServer::checkServerStatus(mutex& in_serverReadWrite)
{
	std::lock_guard<std::mutex> lock(std::ref(serverReadWrite));
	return isServerActive;
}


void OSServer::setServerStatus(mutex& in_serverReadWrite, int in_valueToSet, int* in_commandLineStatus)
{
	std::lock_guard<std::mutex> lock(in_serverReadWrite);
	if (in_valueToSet == 0)
	{
		//isServerActive = in_valueToSet;
		std::cout << "shutdown flag found! " << std::endl;
		*in_commandLineStatus = 0;
	}
	else if (in_valueToSet == 10)	// "stop" the test thread"
	{
		//threadController.signalStop();
		//OSCManager.freeCellMap[1].signalStop();		// halt the test thread
	}
	else if (in_valueToSet == 11)
	{
		//threadController.signalStart();
		//OSCManager.freeCellMap[1].signalStart();	// resume the test thread
	}
	else if (in_valueToSet == 12)
	{
		std::cout << "Attempting delete....>>" << std::endl;
		//.deleteThread();
		std::cout << "Delete succeeded..." << std::endl;
	}

	else if (in_valueToSet == 5)
	{
		std::cout << "Toggling targeting..." << std::endl;
		organicSystemPtr->director.toggleTargeting();
	}
}

void OSServer::signalCommandLineShutdown(mutex& in_serverReadWrite, int in_valueToSet, int* in_clShutdownFlag)
{
	std::lock_guard<std::mutex> lock(in_serverReadWrite);
	std::cout << "(1) shutdown flag call complete" << std::endl;
	//*in_clShutdownFlag = 1;
	isCommandLineShutDown = 1;
	std::cout << "(2) shutdown flag call complete" << std::endl;
}

int OSServer::getCommandLineShutdownValue(mutex& in_serverReadWrite)
{
	std::lock_guard<std::mutex> lock(in_serverReadWrite);
	return isCommandLineShutDown;
}

void OSServer::signalServerShutdown(mutex& in_serverMutex)
{
	std::lock_guard<std::mutex> lock(std::ref(in_serverMutex));
	isServerActive = 0;
}

int OSServer::checkIfBlueprintExists(EnclaveKeyDef::EnclaveKey in_Key)
{
	std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>::iterator blueprintMapIterator;
	blueprintMapIterator = serverBlueprints.getSpecificBlueprintIter(in_Key);
	if (blueprintMapIterator != serverBlueprints.getBlueprintEndIter())
	{
		//cout << "Blueprint exists at: " << in_Key.x << ", " << in_Key.y << ", " << in_Key.z << endl;
		return 1;
	}
	//cout << "Blueprint does not exist at: " << in_Key.x << ", " << in_Key.y << ", " << in_Key.z << endl;
	return 0;
}

ContourBase* OSServer::getDerivedContourPlan(string in_string)
{
	return newContourMap[in_string].get();	// get a reference to the smart pointer
}

ContouredPlanV2Base* OSServer::getPlanV2Ref(std::string in_planNameToGet)
{
	return plansV2Map[in_planNameToGet].get();
}

void OSServer::checkClientMessages()
{
	client.transferRequestMessages(&serverMessages);							// retrieve incoming messages from the client (via std::move)
	messageInterpreter.interpretIncomingMessagesFromClient();					// interpret (and optionally, process) the messages from local and/or remote clients
	messageInterpreter.interpretOutgoingMessagesToClient();						// send outgoing messages to local and/or remote clients
}