#include "stdafx.h"
#include "OSTriangleLineTraverserBare.h"


void OSTriangleLineTraverserBare::initialize(PrimaryLineT1* in_lineRef)
{
	beginKey = in_lineRef->beginPointBlueprintKey;
	currentKey = in_lineRef->beginPointBlueprintKey;
	endKey = in_lineRef->endPointBlueprintKey;

	beginPoint = in_lineRef->beginPointRealXYZ;
	endPoint = in_lineRef->endPointRealXYZ;

	ECBIntersectMeta resultantIntersect = OrganicUtils::findClosestBlueprintIntersection(beginPoint, endPoint, beginKey, endKey);	// do the initial set up; beginKey will be replaced by currentKey in later function calls
	nextKeyAdd = resultantIntersect.incrementingKey;					// the next key add will be a result from previous function call
	currentIterationEndpoint = resultantIntersect.intersectedPoint;		// set the incrementing point

}

bool OSTriangleLineTraverserBare::checkIfRunComplete()
{
	bool result = false;
	if
	(
		(currentKey.x == endKey.x)
		&&
		(currentKey.y == endKey.y)
		&&
		(currentKey.z == endKey.z)
	)
	{
		result = true;
		isRunComplete = 1;
	}
	return result;
}

void OSTriangleLineTraverserBare::traverseLineOnce()
{
	//cout << "currentKey Value: " << currentKey.x << ", " << currentKey.y << ", " << currentKey.z << endl;	// test output only
	//cout << "nextKeyAdd Value: " << nextKeyAdd.x << ", " << nextKeyAdd.y << ", " << nextKeyAdd.z << endl;
	EnclaveKeyDef::EnclaveKey* currentKeyPtr = &currentKey;													// get a pointer to the current key
	*currentKeyPtr = OrganicUtils::addEnclaveKeys(*currentKeyPtr, nextKeyAdd);								// add the nextKeyAdd to currentKey
	ECBIntersectMeta resultantIntersect = OrganicUtils::findClosestBlueprintIntersection(currentIterationEndpoint, endPoint, *currentKeyPtr, endKey);
	//std::cout << "--Resultant intersect at traverseLineOnce: " << resultantIntersect.intersectedPoint.x << ", " << resultantIntersect.intersectedPoint.y << ", " << resultantIntersect.intersectedPoint.z << std::endl;
	nextKeyAdd = resultantIntersect.incrementingKey;
	currentIterationEndpoint = resultantIntersect.intersectedPoint;
}