#include "stdafx.h"
#include "OSServerUtils.h"

void OSServerUtils::fillPolyWithClampResult(ECBPoly* in_polyPtr, OSContouredTriangle* in_contouredTriangle)
{
	//std::cout << "#######!#############!######### Attempting clamp fill! " << std::endl;
	/*
	if (in_contouredTriangle->perfect_clamp_x == 1
		||
		in_contouredTriangle->perfect_clamp_y == 1
		||
		in_contouredTriangle->perfect_clamp_z == 1)
	{
		in_polyPtr->isPolyPerfectlyClamped = 1;
		std::cout << "Perfect clamp detected! " << std::endl;
	}
	*/
	if (in_contouredTriangle->perfect_clamp_x == 1)
	{
		//std::cout << "Perfectly clamped to X!" << std::endl;
		in_polyPtr->isPolyPerfectlyClamped = 1;
	}
	else if (in_contouredTriangle->perfect_clamp_y == 1)
	{
		in_polyPtr->isPolyPerfectlyClamped = 2;
	}
	else if (in_contouredTriangle->perfect_clamp_z == 1)
	{
		in_polyPtr->isPolyPerfectlyClamped = 3;
	}
}
void OSServerUtils::fillLineMetaData(ECBPolyLine* in_LinePtr, OSContouredTriangle* in_Triangle, int in_pointID)
{
	//in_LinePtr->lineID = in_pointID;
	in_LinePtr->pointA = in_Triangle->triangleLines[in_pointID].pointA;							// set the new line to the pointed-to point A
	in_LinePtr->pointB = in_Triangle->triangleLines[in_pointID].pointB;							// set the new line to the pointed-to point B
	if (in_pointID == 0)																		// set the third point in the line (this will only be used when the line of point A and B is perfectly clamped to an axis)
	{
		in_LinePtr->pointC = in_Triangle->trianglePoints[2];
	}
	else if (in_pointID == 1)
	{
		in_LinePtr->pointC = in_Triangle->trianglePoints[0];
	}
	else if (in_pointID == 2)
	{
		in_LinePtr->pointC = in_Triangle->trianglePoints[1];
	}
	in_LinePtr->x_interceptSlope = in_Triangle->triangleLines[in_pointID].x_interceptSlope;		// assign x-intercept slope values
	in_LinePtr->y_interceptSlope = in_Triangle->triangleLines[in_pointID].y_interceptSlope;		// "" y 
	in_LinePtr->z_interceptSlope = in_Triangle->triangleLines[in_pointID].z_interceptSlope;		// "" z
}
void OSServerUtils::fillLineMetaData(ECBPolyLine* in_LinePtr, OSContouredTriangle* in_Triangle, int in_pointID, ECBPolyPoint in_beginPoint, ECBPolyPoint in_endPoint)
{
	//in_LinePtr->lineID = in_pointID;
	in_LinePtr->pointA = in_beginPoint;							// set the new line to the pointed-to point A
	in_LinePtr->pointB = in_endPoint;							// set the new line to the pointed-to point B
	if (in_pointID == 0)																		// set the third point in the line (this will only be used when the line of point A and B is perfectly clamped to an axis)
	{
		in_LinePtr->pointC = in_Triangle->trianglePoints[2];
	}
	else if (in_pointID == 1)
	{
		in_LinePtr->pointC = in_Triangle->trianglePoints[0];
	}
	else if (in_pointID == 2)
	{
		in_LinePtr->pointC = in_Triangle->trianglePoints[1];
	}
	in_LinePtr->x_interceptSlope = in_Triangle->triangleLines[in_pointID].x_interceptSlope;		// assign x-intercept slope values
	in_LinePtr->y_interceptSlope = in_Triangle->triangleLines[in_pointID].y_interceptSlope;		// "" y 
	in_LinePtr->z_interceptSlope = in_Triangle->triangleLines[in_pointID].z_interceptSlope;		// "" z
}

void OSServerUtils::analyzePolyValidityAndInsert(OSContouredTriangle* in_contouredTriangle, 
										 ECBPolyPoint in_segmentPointA,
										 ECBPolyPoint in_segmentPointB,
										 int in_lineID,
										 EnclaveKeyDef::EnclaveKey in_currentTraceKey, 
										 BorderDataMap* in_dataMapRef, 
										 EnclaveCollectionBlueprint* in_blueprintPtr,
										 ECBPoly* in_polyToInsertRef)
{
	int elementID = in_blueprintPtr->primaryPolygonMap.size();				// get the element ID.
	//in_blueprintPtr->primaryPolygonMap[elementID] = *in_polyToInsertRef;	// always insert the poly first; if the poly becomes degenerate after adding the first line, we'll flag it.
	ECBPolyLine newPolyLine;
	fillLineMetaData(&newPolyLine, in_contouredTriangle, in_lineID, in_segmentPointA, in_segmentPointB);
	bool validityCheck = OrganicUtils::checkIfBlueprintLineIsValid(newPolyLine, in_dataMapRef, in_currentTraceKey, in_polyToInsertRef->isPolyPerfectlyClamped);		// run the validity check on the new candidate line
	if (validityCheck == true)
	{
		in_contouredTriangle->addNewPrimarySegment(in_segmentPointA, in_segmentPointB, in_lineID, in_currentTraceKey);		// register the valid segment with the contoured triangle
		in_blueprintPtr->primaryPolygonMap[elementID] = *in_polyToInsertRef;												// insert the new poly data we were going to insert anyway
		in_blueprintPtr->primaryPolygonMap[elementID].lineMap[in_lineID] = newPolyLine;																	// insert the valid segment into the poly
		in_contouredTriangle->addPolygonPiece(in_currentTraceKey, elementID);																			// register the new poly with the contoured triangle, tied to the appropriate blueprint
		in_contouredTriangle->forgedPolyRegistryRef->addToPolyset(in_currentTraceKey, elementID);														// register the piece with the parent ContourBase-derived class (contour plan)
	}
	else if (validityCheck == false)
	{
		//std::cout << "!!!! Warning, validity check failed!! In key: (" << in_currentTraceKey.x << ", " << in_currentTraceKey.y << ", " << in_currentTraceKey.z << ") " << std::endl;
		//std::cout << "!!!! Element ID was: " << elementID << std::endl;
		//int someVal = 3;
		//std::cin >> someVal;

		// do logic for ROGUE polys here (perhaps, inserting into a ROGUE poly map?)
		
	}

}

void OSServerUtils::writeBlueprintToDisk(std::string in_worldName, 
										EnclaveKeyDef::EnclaveKey in_blueprintKey,
										std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapRef)
{
	EnclaveCollectionBlueprint* blueprintRef = &(*in_blueprintMapRef)[in_blueprintKey];		// get a ref to the blueprint.
	BlueprintTransformRefs transformRefs(&blueprintRef->primaryPolygonMap, 
										 &blueprintRef->fractureResults.fractureResultsContainerMap,
										 &blueprintRef->polyGroupRangeMap);
	OSWinAdapter::writeBlueprintsToFile(in_worldName, in_blueprintKey, transformRefs);
}