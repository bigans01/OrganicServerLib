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
	OSCManager.initialize(1, serverSlaves);			// signal for server mode, 2 threads
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
	OSCManager.initialize(1, serverProperties.serverSlaves);			// signal for server mode, 2 threads
	serverJobManager.initialize(this);
	OSdirector.initialize(this);
	messageInterpreter.initialize(this, &serverMessages);
}

OSServer::~OSServer()
{
	std::cout << "!!! OSServer destructor call BEGIN...." << std::endl;
	OSCManager.despawnAllCells();
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
		
		plansV2Map[in_planName].reset(new CPV2Mountain());
		plansV2Map[in_planName].get()->initialize(in_polyPoint, in_numberOfLayers, in_distanceBetweenLayers, in_startRadius, in_expansionValue);		// call the initialization function (common to all ContourBase derivatives)
	}
}

void OSServer::constructSingleOrganicTest()
{
	std::cout << "||||||| constructing single poly for OrganicTriangle testing (version 1)...." << std::endl;

	ECBPolyPoint startPoint, mrPoint;
	startPoint.x = -85.0f;
	startPoint.y = 80.0f;
	startPoint.z = 90.0f;
	addDerivedContourPlan("plan", OSTerrainFormation::NOVAL, startPoint, 1, 0, 0, 0);
	ContourBase* planRef = getDerivedContourPlan("plan");

	ECBPolyPoint testPoint_0;
	ECBPolyPoint testPoint_1;
	ECBPolyPoint testPoint_2;
	ECBPolyPoint testPoint_3;

	/*
	testPoint_0.x = 48.0f;
	testPoint_0.y = 0.0f;		// try: 2.2, 2.2, 2.5, 2.6 (9/16/2018); 2.2 = needs mending; 2.4 = axis searching length too short
	testPoint_0.z = 48.2f;

	testPoint_1.x = 64.0f;
	testPoint_1.y = 0.0f;
	testPoint_1.z = 48.2f;

	testPoint_2.x = 63.5f;
	testPoint_2.y = 15.5f;
	testPoint_2.z = 48.2f;
	
	testPoint_3.x = 48.0f;
	testPoint_3.y = 15.5f;
	testPoint_3.z = 48.2f;
	*/
	
	// !!!!!!!!!!!!!!!! OK, WITH TWO TRIANGLES	(7/14/2020)

	
	testPoint_0.x = 0.0f;
	testPoint_0.y = 0.0f;		// try: 2.2, 2.2, 2.5, 2.6 (9/16/2018); 2.2 = needs mending; 2.4 = axis searching length too short
	testPoint_0.z = 0.0f;

	testPoint_1.x = 32.0f;
	testPoint_1.y = 0.0f;
	testPoint_1.z = 32.0f;

	testPoint_2.x = 32.0f;
	testPoint_2.y = 32.0f;
	testPoint_2.z = 32.0f;

	testPoint_3.x = 0.0f;
	testPoint_3.y = 32.0f;
	testPoint_3.z = 0.0f;

	// !!!!!!!!!!!!!!!! OK, WITH TWO TRIANGLES	(7/14/2020)

	/*
	testPoint_0.x = 0.0f;
	testPoint_0.y = 0.0f;		// try: 2.2, 2.2, 2.5, 2.6 (9/16/2018); 2.2 = needs mending; 2.4 = axis searching length too short
	testPoint_0.z = 0.0f;

	testPoint_1.x = 64.0f;
	testPoint_1.y = 0.0f;
	testPoint_1.z = 64.0f;

	testPoint_2.x = 64.0f;
	testPoint_2.y = 64.0f;
	testPoint_2.z = 64.0f;

	testPoint_3.x = 0.0f;
	testPoint_3.y = 64.0f;
	testPoint_3.z = 0.0f;
	*/
	
	

	
	// !!!!!!!!!!!!!!!! OK, WITH TWO TRIANGLES	(7/14/2020)
	/*
	testPoint_0.x = 0.0f;
	testPoint_0.y = 0.0f;
	testPoint_0.z = 0.0f;

	testPoint_1.x = 32.0f;
	testPoint_1.y = 0.0f;
	testPoint_1.z = 0.0f;

	testPoint_2.x = 32.0f;
	testPoint_2.y = 32.0f;
	testPoint_2.z = 32.0f;

	testPoint_3.x = 0.0f;
	testPoint_3.y = 32.0f;
	testPoint_3.z = 32.0f;
	
	
	// |||||||||||||||| Needs testing refinement. (7/14/2020)
	
	testPoint_0.x = 32.0f;
	testPoint_0.y = 32.0f;		// try: 2.2, 2.2, 2.5, 2.6 (9/16/2018); 2.2 = needs mending; 2.4 = axis searching length too short
	testPoint_0.z = 32.0f;

	testPoint_1.x = 64.0f;
	testPoint_1.y = 32.0f;
	testPoint_1.z = 64.0f;

	testPoint_2.x = 64.0f;
	testPoint_2.y = 64.0f;
	testPoint_2.z = 64.0f;

	testPoint_3.x = 32.0f;
	testPoint_3.y = 64.0f;
	testPoint_3.z = 32.0f;
	*/


	// needs fixing/work; related to the border values used when tracing.
	/*
	testPoint_0.x = 64.0f;
	testPoint_0.y = 64.0f;		// try: 2.2, 2.2, 2.5, 2.6 (9/16/2018); 2.2 = needs mending; 2.4 = axis searching length too short
	testPoint_0.z = 64.0f;

	testPoint_1.x = 96.0f;
	testPoint_1.y = 32.0f;
	testPoint_1.z = 96.0f;

	testPoint_2.x = 96.0f;
	testPoint_2.y = 94.0f;
	testPoint_2.z = 96.0f;
	*/
	
	planRef->constructSingleContouredTriangle(testPoint_0, testPoint_1, testPoint_2, mrPoint, 0, TriangleMaterial::GRASS);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	planRef->constructSingleContouredTriangle(testPoint_0, testPoint_2, testPoint_3, mrPoint, 0, TriangleMaterial::GRASS);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	executeDerivedContourPlan("plan");

}

void OSServer::constructOrganicRawTest()
{
	std::cout << "!!! Running RAW test! " << std::endl;

	ECBPolyPoint startPoint, mrPoint;
	startPoint.x = -85.0f;
	startPoint.y = 80.0f;
	startPoint.z = 90.0f;
	addDerivedContourPlan("plan", OSTerrainFormation::NOVAL, startPoint, 1, 0, 0, 0);
	ContourBase* planRef = getDerivedContourPlan("plan");

	ECBPolyPoint testPoint_0;
	ECBPolyPoint testPoint_1;
	ECBPolyPoint testPoint_2;
	ECBPolyPoint testPoint_3;

	/*
	testPoint_0.x = 16.0f;
	testPoint_0.y = 0.0f;		// try: 2.2, 2.2, 2.5, 2.6 (9/16/2018); 2.2 = needs mending; 2.4 = axis searching length too short
	testPoint_0.z = 16.0f;

	testPoint_1.x = 20.0f;
	testPoint_1.y = 0.0f;
	testPoint_1.z = 16.0f;

	testPoint_2.x = 20.0f;
	testPoint_2.y = 4.0f;
	testPoint_2.z = 16.0f;
	*/

	// a journey into the bizarre below (7/18/2020)
	
	
	testPoint_0.x = 54.0f;
	testPoint_0.y = 16.0f;
	testPoint_0.z = 49.0f;

	testPoint_1.x = 50.0f;
	testPoint_1.y = 16.0f;
	testPoint_1.z = 49.0f;

	testPoint_2.x = 50.0f;
	testPoint_2.y = 12.0f;
	testPoint_2.z = 49.0f;

	testPoint_3.x = 54.0f;
	testPoint_3.y = 12.0f;
	testPoint_3.z = 49.0f;
	
	
	
	/*
	testPoint_0.x = 49.0f;
	testPoint_0.y = 13.0f;
	testPoint_0.z = 49.0f;

	testPoint_1.x = 48.0f;
	testPoint_1.y = 13.0f;
	testPoint_1.z = 49.0f;

	testPoint_2.x = 48.0f;
	testPoint_2.y = 12.0f;
	testPoint_2.z = 49.0f;

	testPoint_3.x = 50.0f;
	testPoint_3.y = 12.0f;
	testPoint_3.z = 49.0f;
	*/

	/*
	testPoint_0.x = 50.0f;
	testPoint_0.y = 16.0f;
	testPoint_0.z = 49.2f;

	testPoint_1.x = 48.0f;
	testPoint_1.y = 16.0f;
	testPoint_1.z = 49.2f;

	testPoint_2.x = 48.0f;
	testPoint_2.y = 12.0f;
	testPoint_2.z = 49.2f;

	testPoint_3.x = 50.0f;
	testPoint_3.y = 12.0f;
	testPoint_3.z = 49.2f;
	*/
	

	/*
	testPoint_0.x = 52.0f;
	testPoint_0.y = 16.0f;
	testPoint_0.z = 49.2f;

	testPoint_1.x = 48.0f;
	testPoint_1.y = 16.0f;
	testPoint_1.z = 49.2f;

	testPoint_2.x = 48.0f;
	testPoint_2.y = 12.0f;
	testPoint_2.z = 49.2f;

	testPoint_3.x = 54.0f;
	testPoint_3.y = 12.0f;
	testPoint_3.z = 49.2f;
	*/

	planRef->constructSingleContouredTriangle(testPoint_0, testPoint_1, testPoint_2, mrPoint, 0, TriangleMaterial::GRASS);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	//planRef->constructSingleContouredTriangle(testPoint_0, testPoint_2, testPoint_3, mrPoint, 0, 2);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	executeDerivedContourPlan("plan");
}

void OSServer::constructBlueprintFillTest()
{

	std::cout << "||||||| Constructing blueprint fill test (version 1)...." << std::endl;
	ECBPolyPoint startPoint, mrPoint;
	startPoint.x = 0.0f;
	startPoint.y = 20.0f;
	startPoint.z = 0.0f;
	addDerivedContourPlan("plan", OSTerrainFormation::NOVAL, startPoint, 1, 0, 0, 0);
	ContourBase* planRef = getDerivedContourPlan("plan");

	ECBPolyPoint testPoint_0;
	ECBPolyPoint testPoint_1;
	ECBPolyPoint testPoint_2;

	testPoint_0.x = 16.0f;
	testPoint_0.y = -8.0f;		// try: 2.2, 2.2, 2.5, 2.6 (9/16/2018); 2.2 = needs mending; 2.4 = axis searching length too short
	testPoint_0.z = 4.5f;

	testPoint_1.x = -4.0f;
	testPoint_1.y = -8.0f;
	testPoint_1.z = -16.0f;

	testPoint_2.x = -4.0f;
	testPoint_2.y = 16.0f;
	testPoint_2.z = 4.5f;

	//testPoint_0.x = 16.0f;
	//testPoint_0.y = 16.0f;		// try: 2.2, 2.2, 2.5, 2.6 (9/16/2018); 2.2 = needs mending; 2.4 = axis searching length too short
	//testPoint_0.z = 4.5f;

	//testPoint_1.x = 0.0f;
	//testPoint_1.y = 16.0f;
	//testPoint_1.z = 4.5f;

	//testPoint_2.x = 8.0f;
	//testPoint_2.y = 12.0f;
	//testPoint_2.z = -4.5f;

	planRef->constructSingleContouredTriangle(testPoint_0, testPoint_1, testPoint_2, mrPoint, 0, TriangleMaterial::GRASS);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	executeDerivedContourPlan("plan");
}


void OSServer::constructSingleDebug()
{
	std::cout << "!!! SINGLE debug test. " << std::endl;

	ECBPolyPoint startPoint, mrPoint;
	startPoint.x = -85.0f;
	startPoint.y = 80.0f;
	startPoint.z = 90.0f;
	addDerivedContourPlan("plan", OSTerrainFormation::NOVAL, startPoint, 1, 0, 0, 0);
	ContourBase* planRef = getDerivedContourPlan("plan");

	ECBPolyPoint testPoint_0;
	ECBPolyPoint testPoint_1;
	ECBPolyPoint testPoint_2;
	ECBPolyPoint testPoint_3;

	/*
	testPoint_0.x = 0.0f;
	testPoint_0.y = 0.0f;
	testPoint_0.z = 0.0f;

	testPoint_1.x = 32.0f;
	testPoint_1.y = 0.0f;
	testPoint_1.z = 0.0f;

	testPoint_2.x = 0.00f;
	testPoint_2.y = 0.0f;
	testPoint_2.z = 32.00f;
	*/

	
	/*
	testPoint_0.x = 32.0f;
	testPoint_0.y = 32.0f;
	testPoint_0.z = 32.0f;

	testPoint_1.x = 41.0f;
	testPoint_1.y = 32.0f;
	testPoint_1.z = 32.0f;

	testPoint_2.x = 32.00f;
	testPoint_2.y = 32.0f;
	testPoint_2.z = 41.00f;
	*/

	// for strange behavior detected  on 3/11/2021
	testPoint_0.x = 1;
	testPoint_0.y = 3.57;
	testPoint_0.z = 4;

	testPoint_1.x = 1.75;
	testPoint_1.y = 3.57;
	testPoint_1.z = 1.19;

	testPoint_2.x = 0.8f;
	testPoint_2.y = 3.57;
	testPoint_2.z = 0;
	

	// for reddit question
	/*
	testPoint_0.x = 2.0f;
	testPoint_0.y = 0.0f;
	testPoint_0.z = 0.0f;

	testPoint_1.x = 0.0f;
	testPoint_1.y = 2.0f;
	testPoint_1.z = 0.0f;

	testPoint_2.x = 0.0f;
	testPoint_2.y = 0.0f;
	testPoint_2.z = 2.0f;
	*/

	// caused bug on 8/20/2020, at ~32000.12, 32000.3, 32000.73.
	// fails when edgeThreshold = .0001 (8/21/2020)

	/*
	testPoint_0.x = 0;
	testPoint_0.y = 1.61;
	testPoint_0.z = 3.06;

	testPoint_1.x = 4;
	testPoint_1.y = 1.54;
	testPoint_1.z = 4;

	testPoint_2.x = 4;
	testPoint_2.y = 0.8;
	testPoint_2.z = 2.98;
	*/

	// fails when edgeThreshold = .00015 (8/21/2020)
	
	/*
	testPoint_0.x = 3.42;
	testPoint_0.y = 0.59;
	testPoint_0.z = 0;

	testPoint_1.x = 0.88;
	testPoint_1.y = 4;
	testPoint_1.z = 4;

	testPoint_2.x = 0;
	testPoint_2.y = 4;
	testPoint_2.z = 2.83;
	*/

	/*
	testPoint_0.x = 0.0f;
	testPoint_0.y = 3.79f;
	testPoint_0.z = 4;


	testPoint_1.x = 0;
	testPoint_1.y = 2.21;
	testPoint_1.z = 0;

	testPoint_2.x = 4;
	testPoint_2.y = 1.15;
	testPoint_2.z = 4;
	*/

	/*
	testPoint_0.x = 64.0f;
	testPoint_0.y = 64.0f;
	testPoint_0.z = 64.0f;

	testPoint_1.x = 73.0f;
	testPoint_1.y = 64.0f;
	testPoint_1.z = 64.0f;

	testPoint_2.x = 64.00f;
	testPoint_2.y = 64.0f;
	testPoint_2.z = 73.00f;
	*/

	planRef->constructSingleContouredTriangle(testPoint_0, testPoint_1, testPoint_2, mrPoint, 0, TriangleMaterial::GRASS);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	//planRef->constructSingleContouredTriangle(testPoint_0, testPoint_2, testPoint_3, mrPoint, 0, 2);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	executeDerivedContourPlan("plan");
}

void OSServer::constructMountainAtPoint(float in_summitX, float in_summitY, float in_summitZ, int in_numberOfLayers)
{
	std::cout << "||||||| Constructing Mountain at point, with " << in_numberOfLayers << " layers. " << std::endl;
	ECBPolyPoint mountainSummit;
	mountainSummit.x = in_summitX;
	mountainSummit.y = in_summitY;
	mountainSummit.z = in_summitZ;
	int numberOfLayers = in_numberOfLayers;
	addDerivedContourPlan("mountain", OSTerrainFormation::MOUNTAIN, mountainSummit, numberOfLayers, 6.81, 9, 9);	// create the points in all contour lines
	ContourBase* planRef = getDerivedContourPlan("mountain");
	planRef->amplifyAllContourLinePoints();						// amplify the points in all contour lines
	planRef->insertMaterials(TriangleMaterial::GRASS, TriangleMaterial::DIRT); // set materials for mountain
	planRef->buildContouredTriangles();
	std::cout << "!!!!!!!!! --------------> Number of strips that will be executed is: " << planRef->triangleStripMap.size() << std::endl;
	executeDerivedContourPlan("mountain");
}

void OSServer::constructSingleMountTest()
{
	ECBPolyPoint summit1;
	int numberOfLayers = 3;

	// first mountain
	summit1.x = 48;
	summit1.y = 16;
	summit1.z = 16;
	addDerivedContourPlan("summit1", OSTerrainFormation::MOUNTAIN, summit1, numberOfLayers, 12.81, 13.22, 9);	// create the points in all contour lines
	ContourBase* summit1Ref = getDerivedContourPlan("summit1");
	summit1Ref->amplifyAllContourLinePoints();						// amplify the points in all contour lines
	summit1Ref->insertMaterials(TriangleMaterial::GRASS, TriangleMaterial::DIRT); // set materials for mountain
	summit1Ref->buildContouredTriangles();
	std::cout << "!!!!!!!!! --------------> top strips: " << summit1Ref->triangleStripMap.size() << std::endl;
	std::cout << "!!!!!!!!! --------------> bottom strips: " << summit1Ref->bottomTriangleStripMap.size() << std::endl;

	executeDerivedContourPlan("summit1");
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
	
	DoublePoint summit3(80.0, 16.0, 25.0);
	addPlanV2("summit3", OSTerrainFormation::MOUNTAIN, summit3, 3, 4.81, 16.82, 7);
	auto summit3Ref = getPlanV2Ref("summit3");
	summit3Ref->amplifyAllContourLinePoints();
	summit3Ref->insertMaterials(TriangleMaterial::GRASS, TriangleMaterial::DIRT);
	summit3Ref->buildContouredTriangles();
	executePlanV2("summit3");
	

	int doneValue = 3;
	std::cout << "Plan V2 test done. Enter key to continue (or just end). " << std::endl;
	std::cin >> doneValue;
	
}



void OSServer::constructMultiMountTestWithElevator()
{
	ECBPolyPoint summit1, summit2, summit3;
	int numberOfLayers = 3;

	EnclaveKeyDef::EnclaveKey serverBlueprintKey(0, 0, 0);
	EnclaveKeyDef::EnclaveKey serverBlueprintOREKey(2, 2, 4);

	AdjacentFinder testFinder(&serverBlueprints);

	
	
	// first mountain (the BIG GRASS mountain)
	summit1.x = 28;
	summit1.y = 16;
	summit1.z = 16;
	addDerivedContourPlan("summit1", OSTerrainFormation::MOUNTAIN, summit1, 5, 12.81, 31.82, 9);	// create the points in all contour lines
	ContourBase* summit1Ref = getDerivedContourPlan("summit1");
	summit1Ref->amplifyAllContourLinePoints();						// amplify the points in all contour lines
	summit1Ref->insertMaterials(TriangleMaterial::GRASS, TriangleMaterial::DIRT);
	summit1Ref->buildContouredTriangles();
	executeDerivedContourPlan("summit1");
	
	// second mountain (the DIRT mountain)	
	summit2.x = 3.33;	// 3.43 = crash? (4/3/2021) --> fixed on 4/5/2021, improved on 4/7/2021, reviewed on 4/8/2021 for smoothness (i.e., removal of hangnails
					// 3.36 = caused a PARTIAL_BOUND to be constructed as a NON_BOUND, due to s/t threshold incorrectly being < 0.000f when it should be < -0.001f, in
					// FusionCandidateProducer::determineRayRelationShipToTriangle (OrganicGLWinLib).
	summit2.y = 16;
	summit2.z = 16;
	addDerivedContourPlan("summit2", OSTerrainFormation::MOUNTAIN, summit2, numberOfLayers, 6.81, 9, 9);	// create the points in all contour lines
	ContourBase* summit2Ref = getDerivedContourPlan("summit2");
	summit2Ref->amplifyAllContourLinePoints();
	summit2Ref->buildContouredTriangles();
	executeDerivedContourPlan("summit2");
	

	
	// third mountain -- diagonal to other two
	summit3.x = 14.00f;
	summit3.y = 16;
	summit3.z = 28;
	addDerivedContourPlan("summit3", OSTerrainFormation::MOUNTAIN, summit3, numberOfLayers, 6.81, 9, 9);
	ContourBase* summit3Ref = getDerivedContourPlan("summit3");
	summit3Ref->amplifyAllContourLinePoints();
	summit3Ref->insertMaterials(TriangleMaterial::GRASS, TriangleMaterial::DIRT);
	summit3Ref->buildContouredTriangles();
	executeDerivedContourPlan("summit3");
	

	std::cout << "*************************** Done with run of constructMultiMountTestWithElevator(), enter a number to continue. " << std::endl;
	int compVal = 3;
	std::cin >> compVal;
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
	addDerivedContourPlan(planName, OSTerrainFormation::MOUNTAIN, summit1, numberOfLayers, distBetweenLayers, startRadius, expansionValue);	// create the points in all contour lines

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



void OSServer::constructMissingFillBlueprint3()
{
	/*
	std::cout << "||||||| constructing 'Missing fill' for blueprints (version 3)...." << std::endl;
	ECBPolyPoint mountainSummit;
	// 2, 10, 2 = error (1/15/2019)
	mountainSummit.y = 7;
	mountainSummit.x = 2;
	mountainSummit.z = 2;

	mountainSummit.y = 7.73;		// error fixed. see notes for roundNearestBlockLineOrCorner on 1/19/2019
	mountainSummit.x = 1002.45;
	mountainSummit.z = 2.61;

	int numberOfLayers = 35;		// current is 17 (max at 35, no issues) // Fatal error at layer 14 when going 1000+x
	addDerivedContourPlan("mountain", OSTerrainFormation::MOUNTAIN, mountainSummit, numberOfLayers, 6.81, 9, 9);	// create the points in all contour lines
	ContourBase* planRef = getDerivedContourPlan("mountain");
	planRef->amplifyAllContourLinePoints();						// amplify the points in all contour lines
	for (int x = 29; x < 30; x++)
	{
		planRef->constructStripTriangles(x, 2);	// construct an individual layer
	}
	std::cout << "!!!!!!!!! --------------> Number of strips that will be executed is: " << planRef->triangleStripMap.size() << std::endl;
	executeDerivedContourPlan("mountain");
	*/

	std::cout << "||||||| constructing blueprints (version 2)...." << std::endl;
	ECBPolyPoint startPoint;
	startPoint.x = -85.0f;
	startPoint.y = -80.0f;
	startPoint.z = 90.0f;
	addDerivedContourPlan("plan", OSTerrainFormation::NOVAL, startPoint, 1, 0, 0, 0);
	ContourBase* planRef = getDerivedContourPlan("plan");

	ECBPolyPoint otherTriangle_0, otherTriangle_1, otherTriangle_2;
	otherTriangle_0.x = 867.89f;
	otherTriangle_0.y = -189.76f;
	otherTriangle_0.z = -221.03f;

	otherTriangle_1.x = 855.4f;
	otherTriangle_1.y = -196.57f;
	otherTriangle_1.z = -223.83f;

	otherTriangle_2.x = 867.45f;
	otherTriangle_2.y = -196.57f;
	otherTriangle_2.z = -231.22f;


	ECBPolyPoint mrPoint;
	planRef->constructSingleContouredTriangle(otherTriangle_0, otherTriangle_1, otherTriangle_2, mrPoint, 0, TriangleMaterial::GRASS);	// this call may need some work; will add a new triangle to the specified strip (fourth argument)
	executeDerivedContourPlan("plan");
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
	auto currentPlanAffectedBegin = currentPlanAffectedBlueprints.producedKeys.begin();
	auto currentPlanAffectedEnd = currentPlanAffectedBlueprints.producedKeys.end();
	for (; currentPlanAffectedBegin != currentPlanAffectedEnd; currentPlanAffectedBegin++)
	{
		auto processedMatch = affectedBlueprints.find(*currentPlanAffectedBegin);
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

void OSServer::checkContourPlanSuccess(std::string in_string)
{
	// backup blueprint map must always be cleared at the end of the run
	cpRunBackupBlueprints.clearAllBlueprints();
}

void OSServer::buildContourPlanAffectedBlueprints(std::string in_string)
{
	ContourBase* planPtr = newContourMap[in_string].get();
	auto keysBegin = planPtr->adherenceData.adherenceOrder.begin();
	auto keysEnd = planPtr->adherenceData.adherenceOrder.end();

	// add each key into the affected blueprints.
	for (; keysBegin != keysEnd; keysBegin++)
	{
		currentPlanAffectedBlueprints.insertKeyIntoPillar(*keysBegin);
	}

	// build the affected set
	currentPlanAffectedBlueprints.produceKeys();

	std::cout << "!! Total number of potentially affected blueprint keys for the current ContourPlan is: " << currentPlanAffectedBlueprints.getProducedKeySize() << std::endl;


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
	//organicSystemPtr->SetupFutureCollectionForFullBlueprintRun(in_key);
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
		//organicSystemPtr->SetupFutureCollectionForFullBlueprintRun(currentKey);
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

void OSServer::testFunction()
{
	// testingonly
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

	std::cout << ":: (TEST FUNCTION) Hard TEST: currentTotalTriangles for debugging ORE: " << currentTotalTriangles << "; state is: " << state << "; appendedState is: " << appendedState << "; number of blocks: " << currentNumberOfBlocks << "; eraseCounter: " << currentEraseCount << std::endl;
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

			if (currentTickCounter == 0)
			{
				std::cout << "(BEFORE CLIENT MESSAGE CHECK) Hard TEST: currentTotalTriangles for debugging ORE: " << currentTotalTriangles << "; state is: " << state << "; appendedState is: " << appendedState << "; number of blocks: " << currentNumberOfBlocks << "; eraseCounter: " << currentEraseCount << std::endl;
			}

			checkClientMessages();							// Does 3 things: 
															// 1. Move messages coming in from the local client (the underlying OrganicClient on this host), into the messageInterpreter.
															// 2. the messageInterpreter (ServerMessageInterpreter) reads the messsages from local/remote clients, creates the jobs.
															// 3. outgoing messages meant to be sent to the local/remote clients are transferred from the messageInterpreter to their appropriate
															//    MessageCable
													

			serverJobManager.checkForUpdateMessages();		// 2.1: check for messages that would be updates -- or even logical changes to a ServerPhasedJobBase-derived class itself, 
															// such as inserting new ServerJobBase-derived classes  (aka, "atomic" server jobs) into the current job phase -- 
															// to existing jobs, from the previous tick

			serverJobManager.removeCompletedPhasedJobs();	// 2.2: with the updates applied, check to see if there are any jobs to remove.
			serverJobManager.checkForMessages();			// 2.3: check the messages that would spawn new jobs.
			serverJobManager.runJobScan();					// 2.4: look for jobs to execute.
			//serverJobManager.removeCompletedPhasedJobs();

			//serverJobManager.checkForMessages();	// have the job manager check for messages to process.
			//std::cout << ">>>> Running organic tick. " << std::endl;
			organicSystemPtr->runOrganicTick();		// run core loop

			currentTotalTriangles = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].getTotalTriangles();
			currentLodState = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].getLodState();
			currentNumberOfBlocks = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].blockMap.size();
			currentEraseCount = organicSystemPtr->getBlueprintPtr(blockReformBlueprintKey.x, blockReformBlueprintKey.y, blockReformBlueprintKey.z)->fractureResults.fractureResultsContainerMap[blockReformOREKey].eraseCounter;
			if (currentTickCounter == 0)
			{
				std::cout << "(AFTER CLIENT MESSAGE CHECK) Hard TEST: currentTotalTriangles for debugging ORE: " << currentTotalTriangles << "; state is: " << state << "; appendedState is: " << appendedState << "; number of blocks: " << currentNumberOfBlocks << "; eraseCounter: " << currentEraseCount << std::endl;
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
		OSCManager.stemcellMap[1].signalStop();		// halt the test thread
	}
	else if (in_valueToSet == 11)
	{
		//threadController.signalStart();
		OSCManager.stemcellMap[1].signalStart();	// resume the test thread
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