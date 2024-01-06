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
	/*
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
	*/
	in_polyPtr->isPolyPerfectlyClamped = in_contouredTriangle->getPerfectClampValue();
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

bool OSServerUtils::analyzePolyValidityAndInsert(OSContouredTriangle* in_contouredTriangle, 
										 ECBPolyPoint in_segmentPointA,
										 ECBPolyPoint in_segmentPointB,
										 int in_lineID,
										 EnclaveKeyDef::EnclaveKey in_currentTraceKey, 
										 BorderDataMap* in_dataMapRef, 
										 EnclaveCollectionBlueprint* in_blueprintPtr,
										 ECBPoly* in_polyToInsertRef)
{
	bool wasInserted = false;

	int elementID = in_blueprintPtr->fetchNextECBPolyKeyID();				// get the element ID.
	// the below line to needs to be re-analyzed. (noted on 8/29/2021)
	//in_blueprintPtr->insertPolyWithKeyValue(elementID, *in_polyToInsertRef);	// always insert the poly first; if the poly becomes degenerate after adding the first line, we'll flag it.
	ECBPolyLine newPolyLine;
	fillLineMetaData(&newPolyLine, in_contouredTriangle, in_lineID, in_segmentPointA, in_segmentPointB);
	bool validityCheck = OrganicUtils::checkIfBlueprintLineIsValid(newPolyLine, in_dataMapRef, in_currentTraceKey, in_polyToInsertRef->isPolyPerfectlyClamped);		// run the validity check on the new candidate line
	if (validityCheck == true)
	{
		// check if a poly with this key already exists
		bool doesElementIDExist = in_blueprintPtr->doesECBPolyWithKeyAlreadyExist(elementID);
		if (doesElementIDExist == true)
		{
			std::cout << "(OSServerUtils) !!!! WARNING: element ID of " << elementID << " is already in use in blueprint "; in_currentTraceKey.printKey(); std::cout << std::endl;
		}

		in_contouredTriangle->addNewPrimarySegment(in_segmentPointA, in_segmentPointB, in_lineID, in_currentTraceKey);		// register the valid segment with the contoured triangle
		in_blueprintPtr->insertPolyWithKeyValue(elementID, *in_polyToInsertRef);												// insert the new poly data we were going to insert anyway
		in_blueprintPtr->insertECBPolyLineIntoPoly(elementID,in_lineID, newPolyLine);																	// insert the valid segment into the poly
		in_contouredTriangle->addPolygonPiece(in_currentTraceKey, elementID);																			// register the new poly with the contoured triangle, tied to the appropriate blueprint
		in_contouredTriangle->forgedPolyRegistryRef->addToPolyset(in_currentTraceKey, elementID);														// register the piece with the parent ContourBase-derived class (contour plan)

		wasInserted = true;
	}
	else if (validityCheck == false)
	{
		//std::cout << "!!!! Warning, validity check failed!! In key: (" << in_currentTraceKey.x << ", " << in_currentTraceKey.y << ", " << in_currentTraceKey.z << ") " << std::endl;
		//std::cout << "!!!! Element ID was: " << elementID << std::endl;
		//int someVal = 3;
		//std::cin >> someVal;

		// do logic for ROGUE polys here (perhaps, inserting into a ROGUE poly map?)
		
	}
	return wasInserted;
}

void OSServerUtils::writeBlueprintToDisk(std::string in_worldName, 
										EnclaveKeyDef::EnclaveKey in_blueprintKey,
										std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapRef)
{
	EnclaveCollectionBlueprint* blueprintRef = &(*in_blueprintMapRef)[in_blueprintKey];		// get a ref to the blueprint.
	BlueprintTransformRefs transformRefs(blueprintRef->getPolygonMapBeginIter(),
										blueprintRef->getPolygonMapEndIter(),
										blueprintRef->getPolygonMapSize(),
										&blueprintRef->fractureResults.fractureResultsContainerMap,
										&blueprintRef->polyGroupRangeMap);
	OSWinAdapter::writeBlueprintsToFile(in_worldName, in_blueprintKey, transformRefs);
}

 std::vector<AdherentDataList> OSServerUtils::findAdherableBlueprints(PointAdherenceOrder* in_pointAdherenceOrderRef, NeighboringBlueprints in_neighboringBlueprints)
 {

	 std::vector<AdherentDataList> returnVector;



	//std::cout << "----------Finding adherents of blueprint: " << in_neighboringBlueprints.originalKey.x << ", " << in_neighboringBlueprints.originalKey.y << ", " << in_neighboringBlueprints.originalKey.z << std::endl;

	 // order of current blueprint
	 int currentOrder = in_pointAdherenceOrderRef->adherenceRelationships[in_neighboringBlueprints.originalKey];

	 // check east blueprint
	 auto east = in_pointAdherenceOrderRef->adherenceRelationships.find(in_neighboringBlueprints.eastKey);
	 if (east != in_pointAdherenceOrderRef->adherenceRelationships.end())	// was the east found? OK, time to check its order.
	 {
		 int eastOrder = in_pointAdherenceOrderRef->adherenceRelationships[in_neighboringBlueprints.eastKey];
		 if (currentOrder > eastOrder)		// the blueprint we're checking must adhere to the east.
		 {
			 AdherentDataList adherentData(EuclideanDirection3D::POS_X, in_neighboringBlueprints.eastKey, eastOrder);
			 returnVector.push_back(adherentData);
			 //std::cout << "This blueprint adheres to the EAST blueprint! (" << in_neighboringBlueprints.eastKey.x << ", " << in_neighboringBlueprints.eastKey.y << ", " << in_neighboringBlueprints.eastKey.z << ") " << std::endl;
		 }
	 }

	 // check north blueprint
	 auto north = in_pointAdherenceOrderRef->adherenceRelationships.find(in_neighboringBlueprints.northKey);
	 if (north != in_pointAdherenceOrderRef->adherenceRelationships.end())
	 {
		 int northOrder = in_pointAdherenceOrderRef->adherenceRelationships[in_neighboringBlueprints.northKey];
		 if (currentOrder > northOrder)
		 {
			 AdherentDataList adherentData(EuclideanDirection3D::POS_Z, in_neighboringBlueprints.northKey, northOrder);
			 returnVector.push_back(adherentData);
			 //std::cout << "This blueprint adheres to the NORTH blueprint! (" << in_neighboringBlueprints.northKey.x << ", " << in_neighboringBlueprints.northKey.y << ", " << in_neighboringBlueprints.northKey.z << ") " << std::endl;
		 }
	 }

	 // check west blueprint
	 auto west = in_pointAdherenceOrderRef->adherenceRelationships.find(in_neighboringBlueprints.westKey);
	 if (west != in_pointAdherenceOrderRef->adherenceRelationships.end())
	 {
		 int westOrder = in_pointAdherenceOrderRef->adherenceRelationships[in_neighboringBlueprints.westKey];
		 if (currentOrder > westOrder)
		 {
			 AdherentDataList adherentData(EuclideanDirection3D::NEG_X, in_neighboringBlueprints.westKey, westOrder);
			 returnVector.push_back(adherentData);
			 //std::cout << "This blueprint adheres to the WEST blueprint! (" << in_neighboringBlueprints.westKey.x << ", " << in_neighboringBlueprints.westKey.y << ", " << in_neighboringBlueprints.westKey.z << ") " << std::endl;
		 }
	 }

	 // check south blueprint
	 auto south = in_pointAdherenceOrderRef->adherenceRelationships.find(in_neighboringBlueprints.southKey);
	 if (south != in_pointAdherenceOrderRef->adherenceRelationships.end())
	 {
		 int southOrder = in_pointAdherenceOrderRef->adherenceRelationships[in_neighboringBlueprints.southKey];
		 if (currentOrder > southOrder)
		 {
			 AdherentDataList adherentData(EuclideanDirection3D::NEG_Z, in_neighboringBlueprints.southKey, southOrder);
			 returnVector.push_back(adherentData);
			 //std::cout << "This blueprint adheres to the SOUTH blueprint! (" << in_neighboringBlueprints.southKey.x << ", " << in_neighboringBlueprints.southKey.y << ", " << in_neighboringBlueprints.southKey.z << ") " << std::endl;
		 }
	 }

	 // check above blueprint
	 auto above = in_pointAdherenceOrderRef->adherenceRelationships.find(in_neighboringBlueprints.aboveKey);
	 if (above != in_pointAdherenceOrderRef->adherenceRelationships.end())
	 {
		 int aboveOrder = in_pointAdherenceOrderRef->adherenceRelationships[in_neighboringBlueprints.aboveKey];
		 if (currentOrder > aboveOrder)
		 {
			 AdherentDataList adherentData(EuclideanDirection3D::POS_Y, in_neighboringBlueprints.aboveKey, aboveOrder);
			 returnVector.push_back(adherentData);
			 //std::cout << "This blueprint adheres to the ABOVE blueprint! (" << in_neighboringBlueprints.aboveKey.x << ", " << in_neighboringBlueprints.aboveKey.y << ", " << in_neighboringBlueprints.aboveKey.z << ") " << std::endl;
		 }
	 }

	 // check below blueprint
	 auto below = in_pointAdherenceOrderRef->adherenceRelationships.find(in_neighboringBlueprints.belowKey);
	 if (below != in_pointAdherenceOrderRef->adherenceRelationships.end())
	 {
		 int belowOrder = in_pointAdherenceOrderRef->adherenceRelationships[in_neighboringBlueprints.belowKey];
		 if (currentOrder > belowOrder)
		 {
			 AdherentDataList adherentData(EuclideanDirection3D::NEG_Y, in_neighboringBlueprints.belowKey, belowOrder);
			 returnVector.push_back(adherentData);
			 //std::cout << "This blueprint adheres to the BELOW blueprint! (" << in_neighboringBlueprints.belowKey.x << ", " << in_neighboringBlueprints.belowKey.y << ", " << in_neighboringBlueprints.belowKey.z << ") " << std::endl;
		 }
	 }

	 return returnVector;
 }