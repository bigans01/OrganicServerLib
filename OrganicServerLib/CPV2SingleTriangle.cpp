#include "stdafx.h"
#include "CPV2SingleTriangle.h"

void CPV2SingleTriangle::initialize(DoublePoint in_startPoint,			// Step 1: initializer function that must be called once the derived class is instantiated.
	int in_numberOfLayers,
	float in_distanceBetweenLayers,
	float in_startRadius,
	float in_expansionValue)
{

}

void CPV2SingleTriangle::amplifyAllContourLinePoints()
{

}

void CPV2SingleTriangle::buildContouredTriangles()
{
	ContouredTriangleV2 newTriangle(DoublePoint(0.0f, 0.0f, 16.0f),
									DoublePoint(16.0f, 0.0f, 16.0f),
									DoublePoint(16.0f, 0.0f, 0.0f),
									TriangleMaterial::GRASS,
									DoublePoint(0.0f, -16.0f, 0.0f),
									&allPolysRegistry,
									&shellMassDriverRegistry,
									ECBPolyType::FREE);
	spawnedTriangle = newTriangle;
}

std::vector<ContouredTriangleV2*> CPV2SingleTriangle::getProcessableContouredTriangles()
{
	std::vector<ContouredTriangleV2*> returnVector;
	returnVector.push_back(&spawnedTriangle);
	return returnVector;
}

void CPV2SingleTriangle::runMassDriversV2(OrganicClient* in_clientRef,
	ECBMap* in_ecbMapRef,
	EnclaveFractureResultsMap* in_fractureResultsMapRef)
{
	OrganicTriangleTracker oreTracker;
	for (auto& currentForgedPolySetIter : allPolysRegistry.polySetRegistry)
	{
		EnclaveKeyDef::EnclaveKey blueprintKey = currentForgedPolySetIter.first;
		EnclaveCollectionBlueprint* currentServerBlueprintRef = in_ecbMapRef->getBlueprintRef(blueprintKey);

		ForgedPolySet originalSet = allPolysRegistry.polySetRegistry[blueprintKey];	// get the original, unaltered set
		ECBMap tempECBMap;

		// We don't have a BlueprintMassManager / BPMassManager here, so we'll have to put the OREs into a temp map.
		ContouredPlanUtils::produceRawEnclavesForTempMap(&tempECBMap, blueprintKey, currentServerBlueprintRef, originalSet.polySet, &oreTracker);

		// Now, load the EnclaveTriangleSkeletonContainers from the tempMap into the persisent mass (in_ecbMapRef)
		ContouredPlanUtils::appendContourPlanEnclaveTriangleSkeletons(blueprintKey, &tempECBMap, in_ecbMapRef, &oreTracker);
	}
}