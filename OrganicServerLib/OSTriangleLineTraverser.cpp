#include "stdafx.h"
#include <iostream>
#include "OSTriangleLineTraverser.h"
#include "ECBIntersectMeta.h"
#include "OSServer.h"

OSTriangleLineTraverser::OSTriangleLineTraverser(OSContouredTriangle* in_TrianglePtr, int in_lineID, std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapPtr)
//OSTriangleLineTraverser::OSTriangleLineTraverser(OSContouredTriangle* in_TrianglePtr, int in_lineID, OSServer* in_serverPtr)
{
	if (in_lineID < 2)
	{
		beginKey = in_TrianglePtr->pointKeys[in_lineID];		// set begin key
		currentKey = in_TrianglePtr->pointKeys[in_lineID];		// current key is same as begin key
		endKey = in_TrianglePtr->pointKeys[in_lineID + 1];		// set end key

		beginPoint = in_TrianglePtr->triangleLines[in_lineID].pointA;	// set begin and end points
		endPoint = in_TrianglePtr->triangleLines[in_lineID].pointB;
		lineID = in_lineID;
	}
	else if (in_lineID == 2)
	{
		beginKey = in_TrianglePtr->pointKeys[2];
		currentKey = in_TrianglePtr->pointKeys[2];		// current key is same as begin key
		endKey = in_TrianglePtr->pointKeys[0];

		beginPoint = in_TrianglePtr->triangleLines[2].pointA;	// set begin and end points
		endPoint = in_TrianglePtr->triangleLines[2].pointB;
		lineID = 2;
	}
	//serverPtr = in_serverPtr;
	contouredTriangleRef = in_TrianglePtr;
	blueprintMapRef = in_blueprintMapPtr;
	//ECBBorderLineList borderLineList;
	//borderLineList = OrganicUtils::determineBorderLines(beginKey);		// get the border line list for the first point
	std::cout << "-->>>> OSTriangleLineTraverser constructor, blueprint intersection call..." << std::endl;
	std::cout << "T 0: " << in_TrianglePtr->trianglePoints[0].x << ", " << in_TrianglePtr->trianglePoints[0].y << ", " << in_TrianglePtr->trianglePoints[0].z << std::endl;
	std::cout << "T 1: " << in_TrianglePtr->trianglePoints[1].x << ", " << in_TrianglePtr->trianglePoints[1].y << ", " << in_TrianglePtr->trianglePoints[1].z << std::endl;
	std::cout << "T 2: " << in_TrianglePtr->trianglePoints[2].x << ", " << in_TrianglePtr->trianglePoints[2].y << ", " << in_TrianglePtr->trianglePoints[2].z << std::endl;

	std::cout << "Line ID is: " << lineID << std::endl;
	std::cout << "Begin Key is: " << beginKey.x << ", " << beginKey.y << ", " << beginKey.z << std::endl;
	std::cout << "End Key is: " << endKey.x << ", " << endKey.y << ", " << endKey.z << std::endl;
	std::cout << "############# OS Triangle Line traverser, constructor, findClosestBlueprintIntersection call " << std::endl;
	ECBIntersectMeta resultantIntersect = OrganicUtils::findClosestBlueprintIntersection(beginPoint, endPoint, beginKey, endKey);	// do the initial set up; beginKey will be replaced by currentKey in later function calls
	nextKeyAdd = resultantIntersect.incrementingKey;					// the next key add will be a result from previous function call
	currentIterationEndpoint = resultantIntersect.intersectedPoint;		// set the incrementing point
	//std::cout << "--Initial resultant intersect: " << resultantIntersect.intersectedPoint.x << ", " << resultantIntersect.intersectedPoint.y << ", " << resultantIntersect.intersectedPoint.z << std::endl;
	std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>::iterator polyMapIter = in_TrianglePtr->polygonPieceMap.find(beginKey);	// check to see if the polygon exists already in the contoured triangle
	int debugIncremental = 0;
	if
	(
		(beginKey.x == 7)
		&&
		(beginKey.y == -6)
		&&
		(beginKey.z == -1)
	)
	{
		//std::cout << "SERVER: incrementing key to analyze FOUND in CASE 2 " << std::endl;
		//debugIncremental = 1;
		//std::cout << "Triangle's points are: " << std::endl;
		//std::cout << "T 0: " << in_TrianglePtr->trianglePoints[0].x << ", " << in_TrianglePtr->trianglePoints[0].y << ", " << in_TrianglePtr->trianglePoints[0].z << std::endl;
		//std::cout << "T 1: " << in_TrianglePtr->trianglePoints[1].x << ", " << in_TrianglePtr->trianglePoints[1].y << ", " << in_TrianglePtr->trianglePoints[1].z << std::endl;
		//std::cout << "T 2: " << in_TrianglePtr->trianglePoints[2].x << ", " << in_TrianglePtr->trianglePoints[2].y << ", " << in_TrianglePtr->trianglePoints[2].z << std::endl;
	}

	/**/
	if (polyMapIter != in_TrianglePtr->polygonPieceMap.end())	// polygon was already found
	{
		//cout << "|||| Polygon was found!! " << endl;
		int polygonIDinBlueprint = polyMapIter->second;						// get the corresponding int value from the triangle's registered blueprint polygon map

		//EnclaveCollectionBlueprint* blueprintPtr = &in_serverPtr->blueprintMap[beginKey];	// get a pointer to the blueprint (for code readability only)
		//&blueprintMapRef->find(incrementingKey)->second;
		//EnclaveCollectionBlueprint* blueprintPtr = &blueprintMapRef->find(beginKey)->second;
		EnclaveCollectionBlueprint* blueprintPtr = &(*blueprintMapRef)[beginKey];
		in_TrianglePtr->insertTracedBlueprint(beginKey);

		ECBPolyLine newPolyLine;												// create a new poly line
		//EnclaveCollectionBlueprint* testPtr = &(*blueprintMapRef)[beginKey];

		OSServerUtils::fillLineMetaData(&newPolyLine, in_TrianglePtr, lineID, resultantIntersect.originPoint, resultantIntersect.intersectedPoint);
		in_TrianglePtr->addNewPrimarySegment(resultantIntersect.originPoint, resultantIntersect.intersectedPoint, lineID, beginKey);
		//cout << "|||||||||||||>>>>>>>>>>> X slope values: " << newPolyLine.x_interceptSlope.x << ", " << newPolyLine.x_interceptSlope.y << ", " << newPolyLine.x_interceptSlope.z << std::endl;
		//cout << ".....Resultant origin: " << resultantIntersect.originPoint.x << ", " << resultantIntersect.originPoint.y << ", " << resultantIntersect.originPoint.z << endl;
		//cout << ".....Resultant intersection: " << resultantIntersect.intersectedPoint.x << ", " << resultantIntersect.intersectedPoint.y << ", " << resultantIntersect.intersectedPoint.z << ", " << endl;
		//cout << ":::::blueprint ID: " << beginKey.x << ", " << beginKey.y << ", " << beginKey.z << endl;
		//cout << ":::::elementID: " << polygonIDinBlueprint << endl;
		//cout << ":::::lineID: " << lineID << endl;
		if (debugIncremental == 1)
		{
			std::cout << "Poly line points: " << std::endl;
			std::cout << "0: " << newPolyLine.pointA.x << ", " << newPolyLine.pointA.y << ", " << newPolyLine.pointA.z << std::endl;
			std::cout << "1: " << newPolyLine.pointB.x << ", " << newPolyLine.pointB.y << ", " << newPolyLine.pointB.z << std::endl;
			std::cout << "2: " << newPolyLine.pointC.x << ", " << newPolyLine.pointC.y << ", " << newPolyLine.pointC.z << std::endl;
			std::cout << "----Slopes: " << std::endl;
			std::cout << "X: " << newPolyLine.x_interceptSlope.x << ", " << newPolyLine.x_interceptSlope.y << ", " << newPolyLine.x_interceptSlope.z << std::endl;
			std::cout << "Y: " << newPolyLine.y_interceptSlope.x << ", " << newPolyLine.y_interceptSlope.y << ", " << newPolyLine.y_interceptSlope.z << std::endl;
			std::cout << "Z: " << newPolyLine.z_interceptSlope.x << ", " << newPolyLine.z_interceptSlope.y << ", " << newPolyLine.z_interceptSlope.z << std::endl;
		}
		blueprintPtr->primaryPolygonMap[polygonIDinBlueprint].lineMap[in_lineID] = newPolyLine;

	}
	else  // polygon wasn't found, it needs to be created
	{
		//cout << "|||| Polygon was NOT found!! " << endl;
		ECBPoly newPoly;
		newPoly.materialID = in_TrianglePtr->materialID;
		newPoly.emptyNormal = in_TrianglePtr->contouredEmptyNormal;
		OSServerUtils::fillPolyWithClampResult(&newPoly, in_TrianglePtr);

		//EnclaveCollectionBlueprint* blueprintPtr = &in_serverPtr->blueprintMap[beginKey];
		//EnclaveCollectionBlueprint* blueprintPtr = &blueprintMapRef->find(beginKey)->second;
		EnclaveCollectionBlueprint* blueprintPtr = &(*blueprintMapRef)[beginKey];
		in_TrianglePtr->insertTracedBlueprint(beginKey);
		
		int elementID = blueprintPtr->primaryPolygonMap.size();						// will store the ID of the newly inserted polygon
		blueprintPtr->primaryPolygonMap[elementID] = newPoly;							// insert a new polygon; the ID will be equalto the size
		ECBPolyLine newPolyLine;												// create a new poly line
		OSServerUtils::fillLineMetaData(&newPolyLine, in_TrianglePtr, lineID, resultantIntersect.originPoint, resultantIntersect.intersectedPoint);
		in_TrianglePtr->addNewPrimarySegment(resultantIntersect.originPoint, resultantIntersect.intersectedPoint, lineID, beginKey);
		//cout << "|||||||||||||>>>>>>>>>>> X slope values: " << newPolyLine.x_interceptSlope.x << ", " << newPolyLine.x_interceptSlope.y << ", " << newPolyLine.x_interceptSlope.z << std::endl;
		//cout << ".....Resultant origin: " << resultantIntersect.originPoint.x << ", " << resultantIntersect.originPoint.y << ", " << resultantIntersect.originPoint.z << endl;
		//cout << ".....Resultant intersection: " << resultantIntersect.intersectedPoint.x << ", " << resultantIntersect.intersectedPoint.y << ", " << resultantIntersect.intersectedPoint.z << ", " << endl;
		//cout << ":::::blueprint ID: " << beginKey.x << ", " << beginKey.y << ", " << beginKey.z << endl;
		//cout << ":::::elementID: " << elementID << endl;
		//cout << ":::::lineID: " << lineID << endl;
		if (debugIncremental == 1)
		{
			std::cout << "Poly line points: " << std::endl;
			std::cout << "0: " << newPolyLine.pointA.x << ", " << newPolyLine.pointA.y << ", " << newPolyLine.pointA.z << std::endl;
			std::cout << "1: " << newPolyLine.pointB.x << ", " << newPolyLine.pointB.y << ", " << newPolyLine.pointB.z << std::endl;
			std::cout << "2: " << newPolyLine.pointC.x << ", " << newPolyLine.pointC.y << ", " << newPolyLine.pointC.z << std::endl;
			std::cout << "----Slopes: " << std::endl;
			std::cout << "X: " << newPolyLine.x_interceptSlope.x << ", " << newPolyLine.x_interceptSlope.y << ", " << newPolyLine.x_interceptSlope.z << std::endl;
			std::cout << "Y: " << newPolyLine.y_interceptSlope.x << ", " << newPolyLine.y_interceptSlope.y << ", " << newPolyLine.y_interceptSlope.z << std::endl;
			std::cout << "Z: " << newPolyLine.z_interceptSlope.x << ", " << newPolyLine.z_interceptSlope.y << ", " << newPolyLine.z_interceptSlope.z << std::endl;
		}
		blueprintPtr->primaryPolygonMap[elementID].lineMap[in_lineID] = newPolyLine;	// add the line to the newly created polygon
		in_TrianglePtr->addPolygonPiece(beginKey, elementID);					// add the polygon piece to the triangle
		in_TrianglePtr->forgedPolyRegistryRef->addToPolyset(beginKey, elementID); // Add the new poly to the ForgedPolyRegistry

	}

	//cout << "~~~~~~~~~~~~~" << endl;

}

void OSTriangleLineTraverser::traverseLineOnce(OSContouredTriangle* in_TrianglePtr)
{
	//cout << "currentKey Value: " << currentKey.x << ", " << currentKey.y << ", " << currentKey.z << endl;	// test output only
	//cout << "nextKeyAdd Value: " << nextKeyAdd.x << ", " << nextKeyAdd.y << ", " << nextKeyAdd.z << endl;
	EnclaveKeyDef::EnclaveKey* currentKeyPtr = &currentKey;													// get a pointer to the current key
	*currentKeyPtr = OrganicUtils::addEnclaveKeys(*currentKeyPtr, nextKeyAdd);								// add the nextKeyAdd to currentKey

	//cout << "NEW currentKey Value (AKA current blueprint to get): " << currentKey.x << ", " << currentKey.y << ", " << currentKey.z << endl;
	std::cout << "----> current iteration endpoint (before findClosestIntersection call) is: " << currentIterationEndpoint.x << ", " << currentIterationEndpoint.y << ", " << currentIterationEndpoint.z << endl;
	std::cout << "---->  traverseLineOnce intersection call " << std::endl;
	//ECBIntersectMeta resultantIntersect = OrganicUtils::findClosestBlueprintIntersection(currentIterationEndpoint, endPoint, *currentKeyPtr, endKey);
	ECBIntersectMeta resultantIntersect = OrganicUtils::findClosestBlueprintIntersection(currentIterationEndpoint, endPoint, *currentKeyPtr, endKey, in_TrianglePtr->centroid, in_TrianglePtr->trianglePoints[0], in_TrianglePtr->trianglePoints[1], in_TrianglePtr->trianglePoints[2]);
	//std::cout << "--Resultant intersect at traverseLineOnce: " << resultantIntersect.intersectedPoint.x << ", " << resultantIntersect.intersectedPoint.y << ", " << resultantIntersect.intersectedPoint.z << std::endl;
	nextKeyAdd = resultantIntersect.incrementingKey;
	// 1. Do check for the currentIterationEndpoint; it must exist on an axis somewhere.
	//cout << "current iteration endpoint is: " << currentIterationEndpoint.x << ", " << currentIterationEndpoint.y << ", " << currentIterationEndpoint.z << endl;
	std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>::iterator polyMapIter = in_TrianglePtr->polygonPieceMap.find(*currentKeyPtr);	// check to see if the polygon exists already in the contoured triangle (using the currentKeyPtr)
																																								/**/
	if (polyMapIter != in_TrianglePtr->polygonPieceMap.end())	// polygon was already found
	{
		//cout << "|||| Polygon was found!! " << endl;
		int polygonIDinBlueprint = polyMapIter->second;						// get the corresponding int value from the triangle's registered blueprint polygon map

		//EnclaveCollectionBlueprint* blueprintPtr = &blueprintMapRef->find(currentKey)->second;	// get a pointer to the blueprint (for code readability only)
		EnclaveCollectionBlueprint* blueprintPtr = &(*blueprintMapRef)[currentKey];
		//if (!(currentKey == endKey))	// don't update the count if it is the last one.
		//{
			in_TrianglePtr->insertTracedBlueprint(currentKey);
		//}
		//EnclaveCollectionBlueprint* blueprintPtr = &serverPtr->blueprintMap[currentKey];

		ECBPolyLine newPolyLine;												// create a new poly line
		OSServerUtils::fillLineMetaData(&newPolyLine, in_TrianglePtr, lineID, resultantIntersect.originPoint, resultantIntersect.intersectedPoint);
		in_TrianglePtr->addNewPrimarySegment(resultantIntersect.originPoint, resultantIntersect.intersectedPoint, lineID, currentKey);
		//cout << "|||||||||||||>>>>>>>>>>> X slope values: " << newPolyLine.x_interceptSlope.x << ", " << newPolyLine.x_interceptSlope.y << ", " << newPolyLine.x_interceptSlope.z << std::endl;
		//cout << ".....Resultant origin: " << resultantIntersect.originPoint.x << ", " << resultantIntersect.originPoint.y << ", " << resultantIntersect.originPoint.z << endl;
		//cout << ".....Resultant intersection: " << resultantIntersect.intersectedPoint.x << ", " << resultantIntersect.intersectedPoint.y << ", " << resultantIntersect.intersectedPoint.z << ", " << endl;
		//cout << ":::::blueprint ID: " << currentKey.x << ", " << currentKey.y << ", " << currentKey.z << endl;
		//cout << ":::::elementID: " << polygonIDinBlueprint << endl;
		//cout << ":::::lineID: " << lineID << endl;
		blueprintPtr->primaryPolygonMap[polygonIDinBlueprint].lineMap[lineID] = newPolyLine;

	}
	else  // polygon wasn't found, it needs to be created
	{
		//cout << "|||| Polygon was NOT found!! " << endl;
		//cout << ">>>>> Line data: " << endl;
		//cout << " origin Point: " << resultantIntersect.originPoint.x << ", " << resultantIntersect.originPoint.y << ", " << resultantIntersect.originPoint.z << ", " << std::endl;
		//cout << " intersected Point: " << resultantIntersect.intersectedPoint.x << ", " << resultantIntersect.intersectedPoint.y << ", " << resultantIntersect.intersectedPoint.z << ", " << std::endl;
		ECBPoly newPoly;
		newPoly.materialID = in_TrianglePtr->materialID;
		newPoly.emptyNormal = in_TrianglePtr->contouredEmptyNormal;
		OSServerUtils::fillPolyWithClampResult(&newPoly, in_TrianglePtr);

		//EnclaveCollectionBlueprint* blueprintPtr = &blueprintMapRef->find(currentKey)->second;
		EnclaveCollectionBlueprint* blueprintPtr = &(*blueprintMapRef)[currentKey];
		//if (!(currentKey == endKey))	// don't update the count if it is the last one.
		//{
			in_TrianglePtr->insertTracedBlueprint(currentKey);
		//}
		//EnclaveCollectionBlueprint* blueprintPtr = &serverPtr->blueprintMap[currentKey];

		int elementID = blueprintPtr->primaryPolygonMap.size();						// will store the ID of the newly inserted polygon
		blueprintPtr->primaryPolygonMap[elementID] = newPoly;							// insert a new polygon; the ID will be equalto the size
		ECBPolyLine newPolyLine;												// create a new poly line
		OSServerUtils::fillLineMetaData(&newPolyLine, in_TrianglePtr, lineID, resultantIntersect.originPoint, resultantIntersect.intersectedPoint);
		in_TrianglePtr->addNewPrimarySegment(resultantIntersect.originPoint, resultantIntersect.intersectedPoint, lineID, currentKey);
		//cout << "|||||||||||||>>>>>>>>>>> X slope values: " << newPolyLine.x_interceptSlope.x << ", " << newPolyLine.x_interceptSlope.y << ", " << newPolyLine.x_interceptSlope.z << std::endl;
		//cout << ".....Resultant origin: " << resultantIntersect.originPoint.x << ", " << resultantIntersect.originPoint.y << ", " << resultantIntersect.originPoint.z << endl;
		//cout << ".....Resultant intersection: " << resultantIntersect.intersectedPoint.x << ", " << resultantIntersect.intersectedPoint.y << ", " << resultantIntersect.intersectedPoint.z << ", " << endl;
		//cout << ":::::blueprint ID: " << currentKey.x << ", " << currentKey.y << ", " << currentKey.z << endl;
		//cout << ":::::elementID: " << elementID << endl;
		//cout << ":::::lineID: " << lineID << endl;
		blueprintPtr->primaryPolygonMap[elementID].lineMap[lineID] = newPolyLine;	// add the line to the newly created polygon
		in_TrianglePtr->addPolygonPiece(currentKey, elementID);					// add the polygon piece to the triangle
		in_TrianglePtr->forgedPolyRegistryRef->addToPolyset(currentKey, elementID); // Add the new poly to the ForgedPolyRegistry

	}
	currentIterationEndpoint = resultantIntersect.intersectedPoint;
	//cout << "~~~~~~~~~~~~~" << endl;

}

