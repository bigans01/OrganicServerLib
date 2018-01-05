#include "stdafx.h"
#include "OSServer.h"
//#include "OSContourPlan.h"
//#include "shitTest.h"

OSServer::OSServer()
{
	OrganicSystem Organic(3, 3, 13, 1024, 768, 3);
	organicSystemPtr = &Organic;
}

OSServer::OSServer(int x)
{
	OrganicSystem Organic(3, 3, 13, 1024, 768, 3);
	organicSystemPtr = &Organic;
}

void OSServer::addContourPlan(string in_string, float in_x, float in_y, float in_z)
{
	OSContourPlan tempPlan(in_x, in_y, in_z);
	//contourPlanMap[in_string].planMode = 0;	// add a new plan			<<< ---- error is here (FIXED)
	contourPlanMap[in_string] = tempPlan;

}

void OSServer::executeContourPlan(string in_string)
{
	OSContourPlan* planPtr = &contourPlanMap[in_string];
	int numberOfTriangleStrips = planPtr->triangleStripMap.size();
	unordered_map<int, OSContouredTriangleStrip>::iterator stripMapIterator = planPtr->triangleStripMap.begin();
	unordered_map<int, OSContouredTriangleStrip>::iterator stripMapEnd = planPtr->triangleStripMap.end();

	// iterate through each strip
	for (stripMapIterator; stripMapIterator != stripMapEnd; stripMapIterator++)
	{
		//cout << "Triangle strip ID: " << stripMapIterator->first << endl;
		unordered_map<int, OSContouredTriangle>::iterator triangleMapIterator = stripMapIterator->second.triangleMap.begin();
		unordered_map<int, OSContouredTriangle>::iterator triangleMapEnd = stripMapIterator->second.triangleMap.end();
		// iterate through each triangle
		for (triangleMapIterator; triangleMapIterator != triangleMapEnd; triangleMapIterator++)
		{
			//cout << "Current triangle ID: " << triangleMapIterator->first << endl;
			OSContouredTriangle currentTriangle = triangleMapIterator->second;
			traceTriangleThroughBlueprints(currentTriangle);
		}
	}
}

void OSServer::traceTriangleThroughBlueprints(OSContouredTriangle in_Triangle)
{
	// firstly, determine the blueprint each point is in:
	for (int x = 0; x < 3; x++)
	{
		CursorPathTraceContainer x_container, y_container, z_container;
		x_container = organicSystemPtr->EnclaveCollections.GetCursorCoordTrace(in_Triangle.trianglePoints[x].x);
		y_container = organicSystemPtr->EnclaveCollections.GetCursorCoordTrace(in_Triangle.trianglePoints[x].y);
		z_container = organicSystemPtr->EnclaveCollections.GetCursorCoordTrace(in_Triangle.trianglePoints[x].z);
		EnclaveKeyDef::EnclaveKey blueprintKey;
		blueprintKey.x = x_container.CollectionCoord;
		blueprintKey.y = y_container.CollectionCoord;
		blueprintKey.z = z_container.CollectionCoord;
		int doesBPExist = checkIfBlueprintExists(blueprintKey);
		//y_container = EnclaveCollections.GetCursorCoordTrace(origin_point.y);
		//z_container = EnclaveCollections.GetCursorCoordTrace(origin_point.z);
	}
}

int OSServer::checkIfBlueprintExists(EnclaveKeyDef::EnclaveKey in_Key)
{
	std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>::iterator blueprintMapIterator;
	blueprintMapIterator = blueprintMap.find(in_Key);
	if (blueprintMapIterator != blueprintMap.end())
	{
		cout << "Blueprint exists at: " << in_Key.x << ", " << in_Key.y << ", " << in_Key.z << endl;
		return 1;
	}
	cout << "Blueprint does not exist at: " << in_Key.x << ", " << in_Key.y << ", " << in_Key.z << endl;
	return 0;
}

OSContourPlan* OSServer::getContourPlan(string in_string)
{
	return &contourPlanMap[in_string];		// return a pointer to the plan
}