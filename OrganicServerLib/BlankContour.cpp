#include "stdafx.h"
#include "BlankContour.h"

void BlankContour::initialize(ECBPolyPoint in_startPoint, int in_numberOfLayers, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue)
{

}
void BlankContour::amplifyAllContourLinePoints()
{

}

void BlankContour::constructSingleContouredTriangle(ECBPolyPoint in_point0, ECBPolyPoint in_point1, ECBPolyPoint in_point2, ECBPolyPoint in_massReferencePoint, int in_triangleStripID, short in_materialID)
{
	planDirections.y_direction = -1;	// DEFAULT, can be changed later.
	OSContouredTriangle testTriangle(in_point0, in_point1, in_point2, in_materialID, in_massReferencePoint, &planPolyRegistry, ECBPolyType::FREE);
	int baseStripSize = triangleStripMap[in_triangleStripID].triangleMap.size();		// get the number of triangles in the base strip, should be 0
	//std::cout << "### Adding new triangle with ID " << baseStripSize << std::endl;
	triangleStripMap[in_triangleStripID].triangleMap[baseStripSize] = testTriangle;
	//std::cout << "### New size is: " << triangleStripMap[in_triangleStripID].triangleMap.size() << std::endl;
}

void BlankContour::runMassDrivers(OrganicClient* in_clientRef, std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapRef, EnclaveFractureResultsMap* in_fractureResultsMapRef)
{

	// Step 1) find each poly that is a SHELL_MASSDRIVER type, put it into the appropriate ForgedPolySet (mapped by blueprint key).
	OrganicTriangleTracker oreTracker;
	auto planPolyRegistryBegin = planPolyRegistry.polySetRegistry.begin();
	auto planPolyRegistryEnd = planPolyRegistry.polySetRegistry.end();
	for (planPolyRegistryBegin; planPolyRegistryBegin != planPolyRegistryEnd; planPolyRegistryBegin++)
	{
		EnclaveKeyDef::EnclaveKey blueprintKey = planPolyRegistryBegin->first;					// get the key of the blueprint to check.
		int foundGroupID = planPolyRegistryBegin->second.groupID;									// grab the group ID we'll be working with.
		std::cout << "Found poly set " << foundGroupID << "in key: (" << blueprintKey.x << ", " << blueprintKey.y << ", " << blueprintKey.z << std::endl;
		EnclaveCollectionBlueprint* blueprintToCheck = &(*in_blueprintMapRef)[blueprintKey];	// get a ref to the blueprint that exists SERVER side (not on the client), using the blueprintKey
		//std::map<int, ECBPoly>* polyMapRef = &blueprintToCheck->primaryPolygonMap;				// get a ref to the poly map inside the blueprint.
		auto forgedPolySetBegin = planPolyRegistryBegin->second.polySet.begin();
		auto forgedPolySetEnd = planPolyRegistryBegin->second.polySet.end();

		ForgedPolySet originalSet = planPolyRegistry.polySetRegistry[blueprintKey];	// get the original, unaltered set
		EnclaveFractureResultsMap tempMap;
		in_clientRef->OS->produceRawEnclavesForPolySetWithTracking(&tempMap, blueprintKey, blueprintToCheck, originalSet.polySet, &oreTracker);		// first, generate the OrganicRawEnclaves that would be produced by this set; keep track of each ORE that an individual OrganicTriangle touches (needed for SPoly post-collision check)
		in_clientRef->OS->spawnAndAppendEnclaveTriangleSkeletonsToBlueprint(blueprintKey, &tempMap, blueprintToCheck, &oreTracker);					// second, spawn the EnclaveTriangleSkeletonContainers for the current EnclaveFractureResultsMap; then append the results to the target blueprint to update.
	}
}

void BlankContour::buildContouredTriangles()
{

}

std::vector<OSContouredTriangle*> BlankContour::getProcessableContouredTriangles()
{
	std::vector<OSContouredTriangle*> returnRefVector;
	// push top strip contoured triangles into vector
	auto topTrianglesStripsBegin = triangleStripMap.begin();
	auto topTrianglesStripsEnd = triangleStripMap.end();
	for (; topTrianglesStripsBegin != topTrianglesStripsEnd; topTrianglesStripsBegin++)
	{
		auto currentTopTrianglesBegin = topTrianglesStripsBegin->second.triangleMap.begin();
		auto currentTopTrianglesEnd = topTrianglesStripsBegin->second.triangleMap.end();
		for (; currentTopTrianglesBegin != currentTopTrianglesEnd; currentTopTrianglesBegin++)
		{
			returnRefVector.push_back(&currentTopTrianglesBegin->second);
		}
	}
	return returnRefVector;
}