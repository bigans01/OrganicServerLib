#include "stdafx.h"
#include <iostream>
#include "OSTriangleLineTraverser.h"
#include "ECBIntersectMeta.h"
#include "OSServer.h"

OSTriangleLineTraverser::OSTriangleLineTraverser(OSContouredTriangle* in_TrianglePtr, int in_lineID, OSServer* in_serverPtr)
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
	serverPtr = in_serverPtr;
	//ECBBorderLineList borderLineList;
	//borderLineList = OrganicUtils::determineBorderLines(beginKey);		// get the border line list for the first point
	//std::cout << "-->>>> begiging OS Triangle Line Traverser..." << std::endl;
	ECBIntersectMeta resultantIntersect = OrganicUtils::findClosestBlueprintIntersection(beginPoint, endPoint, beginKey, endKey);	// do the initial set up; beginKey will be replaced by currentKey in later function calls
	nextKeyAdd = resultantIntersect.incrementingKey;					// the next key add will be a result from previous function call
	currentIterationEndpoint = resultantIntersect.intersectedPoint;		// set the incrementing point
	//std::cout << "--Initial resultant intersect: " << resultantIntersect.intersectedPoint.x << ", " << resultantIntersect.intersectedPoint.y << ", " << resultantIntersect.intersectedPoint.z << std::endl;
	std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>::iterator polyMapIter = in_TrianglePtr->polygonPieceMap.find(beginKey);	// check to see if the polygon exists already in the contoured triangle

	/**/
	if (polyMapIter != in_TrianglePtr->polygonPieceMap.end())	// polygon was already found
	{
		//cout << "|||| Polygon was found!! " << endl;
		int polygonIDinBlueprint = polyMapIter->second;						// get the corresponding int value from the triangle's registered blueprint polygon map
		EnclaveCollectionBlueprint* blueprintPtr = &in_serverPtr->blueprintMap[beginKey];	// get a pointer to the blueprint (for code readability only)
		ECBPolyLine newPolyLine;												// create a new poly line

		OSServer::fillLineMetaData(&newPolyLine, in_TrianglePtr, lineID, resultantIntersect.originPoint, resultantIntersect.intersectedPoint);
		//cout << "|||||||||||||>>>>>>>>>>> X slope values: " << newPolyLine.x_interceptSlope.x << ", " << newPolyLine.x_interceptSlope.y << ", " << newPolyLine.x_interceptSlope.z << std::endl;
		//cout << ".....Resultant origin: " << resultantIntersect.originPoint.x << ", " << resultantIntersect.originPoint.y << ", " << resultantIntersect.originPoint.z << endl;
		//cout << ".....Resultant intersection: " << resultantIntersect.intersectedPoint.x << ", " << resultantIntersect.intersectedPoint.y << ", " << resultantIntersect.intersectedPoint.z << ", " << endl;
		//cout << ":::::blueprint ID: " << beginKey.x << ", " << beginKey.y << ", " << beginKey.z << endl;
		//cout << ":::::elementID: " << polygonIDinBlueprint << endl;
		//cout << ":::::lineID: " << lineID << endl;
		blueprintPtr->primaryPolygonMap[polygonIDinBlueprint].lineMap[in_lineID] = newPolyLine;

	}
	else  // polygon wasn't found, it needs to be created
	{
		//cout << "|||| Polygon was NOT found!! " << endl;
		ECBPoly newPoly;
		newPoly.materialID = in_TrianglePtr->materialID;
		OSServer::fillPolyWithClampResult(&newPoly, in_TrianglePtr);
		EnclaveCollectionBlueprint* blueprintPtr = &in_serverPtr->blueprintMap[beginKey];
		
		int elementID = blueprintPtr->primaryPolygonMap.size();						// will store the ID of the newly inserted polygon
		blueprintPtr->primaryPolygonMap[elementID] = newPoly;							// insert a new polygon; the ID will be equalto the size
		ECBPolyLine newPolyLine;												// create a new poly line
		OSServer::fillLineMetaData(&newPolyLine, in_TrianglePtr, lineID, resultantIntersect.originPoint, resultantIntersect.intersectedPoint);
		//cout << "|||||||||||||>>>>>>>>>>> X slope values: " << newPolyLine.x_interceptSlope.x << ", " << newPolyLine.x_interceptSlope.y << ", " << newPolyLine.x_interceptSlope.z << std::endl;
		//cout << ".....Resultant origin: " << resultantIntersect.originPoint.x << ", " << resultantIntersect.originPoint.y << ", " << resultantIntersect.originPoint.z << endl;
		//cout << ".....Resultant intersection: " << resultantIntersect.intersectedPoint.x << ", " << resultantIntersect.intersectedPoint.y << ", " << resultantIntersect.intersectedPoint.z << ", " << endl;
		//cout << ":::::blueprint ID: " << beginKey.x << ", " << beginKey.y << ", " << beginKey.z << endl;
		//cout << ":::::elementID: " << elementID << endl;
		//cout << ":::::lineID: " << lineID << endl;
		blueprintPtr->primaryPolygonMap[elementID].lineMap[in_lineID] = newPolyLine;	// add the line to the newly created polygon
		in_TrianglePtr->addPolygonPiece(beginKey, elementID);					// add the polygon piece to the triangle

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
	//cout << "current iteration endpoint (before findClosestIntersection call) is: " << currentIterationEndpoint.x << ", " << currentIterationEndpoint.y << ", " << currentIterationEndpoint.z << endl;
	ECBIntersectMeta resultantIntersect = OrganicUtils::findClosestBlueprintIntersection(currentIterationEndpoint, endPoint, *currentKeyPtr, endKey);
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
		EnclaveCollectionBlueprint* blueprintPtr = &serverPtr->blueprintMap[currentKey];	// get a pointer to the blueprint (for code readability only)
		ECBPolyLine newPolyLine;												// create a new poly line
		OSServer::fillLineMetaData(&newPolyLine, in_TrianglePtr, lineID, resultantIntersect.originPoint, resultantIntersect.intersectedPoint);
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
		OSServer::fillPolyWithClampResult(&newPoly, in_TrianglePtr);
		EnclaveCollectionBlueprint* blueprintPtr = &serverPtr->blueprintMap[currentKey];
		int elementID = blueprintPtr->primaryPolygonMap.size();						// will store the ID of the newly inserted polygon
		blueprintPtr->primaryPolygonMap[elementID] = newPoly;							// insert a new polygon; the ID will be equalto the size
		ECBPolyLine newPolyLine;												// create a new poly line
		OSServer::fillLineMetaData(&newPolyLine, in_TrianglePtr, lineID, resultantIntersect.originPoint, resultantIntersect.intersectedPoint);
		//cout << "|||||||||||||>>>>>>>>>>> X slope values: " << newPolyLine.x_interceptSlope.x << ", " << newPolyLine.x_interceptSlope.y << ", " << newPolyLine.x_interceptSlope.z << std::endl;
		//cout << ".....Resultant origin: " << resultantIntersect.originPoint.x << ", " << resultantIntersect.originPoint.y << ", " << resultantIntersect.originPoint.z << endl;
		//cout << ".....Resultant intersection: " << resultantIntersect.intersectedPoint.x << ", " << resultantIntersect.intersectedPoint.y << ", " << resultantIntersect.intersectedPoint.z << ", " << endl;
		//cout << ":::::blueprint ID: " << currentKey.x << ", " << currentKey.y << ", " << currentKey.z << endl;
		//cout << ":::::elementID: " << elementID << endl;
		//cout << ":::::lineID: " << lineID << endl;
		blueprintPtr->primaryPolygonMap[elementID].lineMap[lineID] = newPolyLine;	// add the line to the newly created polygon
		in_TrianglePtr->addPolygonPiece(currentKey, elementID);					// add the polygon piece to the triangle

	}
	currentIterationEndpoint = resultantIntersect.intersectedPoint;
	//cout << "~~~~~~~~~~~~~" << endl;

}

