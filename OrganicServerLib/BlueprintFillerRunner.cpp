#include "stdafx.h"
#include "BlueprintFillerRunner.h"

void BlueprintFillerRunner::initialize(PrimaryLineT1* in_lineRef, ECBPolyPoint in_currentSegmentBegin, ECBPolyPoint in_currentSegmentEnd, EnclaveKeyDef::EnclaveKey in_currentSegmentBlueprintKey)
{
	// get the intercept point of the referenced line
	//ECBIntersectMeta initialIntersect = OrganicUtils::findClosestBlueprintIntersection(in_lineRef->beginPointRealXYZ, in_lineRef->endPointRealXYZ, in_lineRef->beginPointBlueprintKey, in_lineRef->endPointBlueprintKey);
	//ECBPolyPoint initialIntersectedPoint = initialIntersect.intersectedPoint;
	//ECBBorderLineList currentBorderLineList = OrganicUtils::determineBorderLines(in_lineRef->beginPointBlueprintKey);
	//ECBPPOrientationResults currentEndpointOrientationResults = OrganicUtils::GetBlueprintPointOrientation(initialIntersectedPoint, &currentBorderLineList);
	//BorderDataMap borderData;
	//EnclaveKeyDef::EnclaveKey moveVals = OrganicUtils::retrieveBorderDirection(currentEndpointOrientationResults, &borderData);

	//std::cout << "!!! Original line coordinates are: Begin (" << in_lineRef->beginPointRealXYZ.x << ", " << in_lineRef->beginPointRealXYZ.y << ", " << in_lineRef->beginPointRealXYZ.z << ") | End (" << in_lineRef->endPointRealXYZ.x << ", " << in_lineRef->endPointRealXYZ.y << ", " << in_lineRef->endPointRealXYZ.z << ") " << std::endl;
	//std::cout << "!!! New end point is: (" << initialIntersectedPoint.x << ", " << initialIntersectedPoint.y << ", " << initialIntersectedPoint.z << ") " << std::endl;
	//std::cout << "!!! New point move vals are: (" << moveVals.x << ", " << moveVals.y << ", " << moveVals.z << ") " << std::endl;
	std::cout << "~~~~~~~~~~~~ Beginning initial fill " << std::endl;
	constructFillerPrimaryInitial(in_lineRef, in_currentSegmentBegin, in_currentSegmentEnd, in_currentSegmentBlueprintKey);

}
bool BlueprintFillerRunner::checkIfRunComplete()
{
	bool result = false;
	return result;
}
void BlueprintFillerRunner::traverseLineOnce()
{

}

void BlueprintFillerRunner::constructFillerPrimaryInitial(PrimaryLineT1* in_lineRef, ECBPolyPoint in_currentSegmentBegin, ECBPolyPoint in_currentSegmentEnd, EnclaveKeyDef::EnclaveKey in_currentSegmentBlueprintKey)
{
	// set the initial slopes.
	initial_x_slope = in_lineRef->x_int;
	initial_y_slope = in_lineRef->y_int;
	initial_z_slope = in_lineRef->z_int;

	// get the intercept point of the referenced line 
	//ECBIntersectMeta initialIntersect = OrganicUtils::findClosestBlueprintIntersection(in_lineRef->beginPointRealXYZ, in_lineRef->endPointRealXYZ, in_lineRef->beginPointBlueprintKey, in_lineRef->endPointBlueprintKey);
	//ECBPolyPoint initialIntersectedPoint = initialIntersect.intersectedPoint;

	std::cout << "!! ## Line segment begin: " << in_currentSegmentBegin.x << ", " << in_currentSegmentBegin.y << ", " << in_currentSegmentBegin.z << std::endl;
	int someVal = 3;
	std::cout << "!! ## Line segment end: " << in_currentSegmentEnd.x << ", " << in_currentSegmentEnd.y << ", " << in_currentSegmentEnd.z << std::endl;

	ECBPolyPoint initialIntersectedPoint = in_currentSegmentEnd;
	//currentBorderLineList = OrganicUtils::determineBorderLines(in_lineRef->beginPointBlueprintKey);
	std::cout << ">> Current blueprint key is: " << in_currentSegmentBlueprintKey.x << ", " << in_currentSegmentBlueprintKey.y << ", " << in_currentSegmentBlueprintKey.z << std::endl;
	currentBorderLineList = OrganicUtils::determineBorderLines(in_currentSegmentBlueprintKey);
	//std::cout << "Current blueprint key is: " << in_currentSegmentBlueprintKey.x << ", " << in_currentSegmentBlueprintKey.y << ", " << in_currentSegmentBlueprintKey.x << std::endl;
	ECBPPOrientationResults currentEndpointOrientationResults = OrganicUtils::GetBlueprintPointOrientation(initialIntersectedPoint, &currentBorderLineList);

	//std::cout << "!! Found inital orientation results..." << std::endl;

	// set the current blueprint key.
	//blueprintKey = in_lineRef->beginPointBlueprintKey;
	blueprintKey = in_currentSegmentBlueprintKey;

	// get the move vals
	//BorderDataMap borderData;
	BorderDataMap* borderDataMapRef = &borderData;		// clean this up later.
	moveVals = OrganicUtils::retrieveBorderDirection(currentEndpointOrientationResults, &borderData);

	currentBeginPoint = initialIntersectedPoint;	// the starting point of the new line, (POINT A) -- should be the intersected point.

	struct interceptStorage
	{
		BorderMDFaceList tempFaceList;
		int numberOfIntercepts = 0;
	};
	interceptStorage tempStorage;

	currentBeginOrientation = currentEndpointOrientationResults;
	if (currentBeginOrientation.otype == ECBPPOrientations::FACE)	// the hit is a face
	{
		tempStorage.tempFaceList.faceList[0] = currentBeginOrientation.osubtype;	// set the value to be the face
		tempStorage.numberOfIntercepts = 1;			// 1 intercept only
	}
	else if (currentBeginOrientation.otype == ECBPPOrientations::LINE)		// ...get the linked faces for the line
	{
		tempStorage.tempFaceList.faceList[0] = borderDataMapRef->lineMap[currentBeginOrientation.osubtype].linkedFaces[0];
		tempStorage.tempFaceList.faceList[1] = borderDataMapRef->lineMap[currentBeginOrientation.osubtype].linkedFaces[1];
		tempStorage.numberOfIntercepts = 2;
	}
	else if (currentBeginOrientation.otype == ECBPPOrientations::CORNER)	// ...get the linked faces for the corner
	{
		tempStorage.tempFaceList.faceList[0] = borderDataMapRef->cornerMap[currentBeginOrientation.osubtype].linkedFaces[0];
		tempStorage.tempFaceList.faceList[1] = borderDataMapRef->cornerMap[currentBeginOrientation.osubtype].linkedFaces[1];
		tempStorage.tempFaceList.faceList[2] = borderDataMapRef->cornerMap[currentBeginOrientation.osubtype].linkedFaces[2];
		tempStorage.numberOfIntercepts = 3;
	}

	// based on previous result, do some thing:
	ECBPolyPoint interceptToUse;
	EnclaveKeyDef::EnclaveKey moveValsToPass;
	// an intercept count of 1 will always mean a face is use
	if (tempStorage.numberOfIntercepts == 1)
	{
		std::cout << ">>>>> Face encountered! " << std::endl;
		// an intercept count of 1 will always mean a face is used
		ECBPPOrientations slopeToUse = borderDataMapRef->faceInterceptMap[currentBeginOrientation.osubtype];
		interceptToUse = getSlopeToUse(slopeToUse);
		std::cout << "|||| (face) Slope to use is: " << interceptToUse.x << ", " << interceptToUse.y << ", " << interceptToUse.z << std::endl;
	}

	// an intercept count of 2 means we need to get the appropriate slope from the line
	else if (tempStorage.numberOfIntercepts == 2)
	{
		std::cout << ">>>>>> Line encountered! " << std::endl;
		// get the first face value
		ECBPPOrientations face1 = borderDataMapRef->faceInterceptMap[tempStorage.tempFaceList.faceList[0]];
		ECBPolyPoint intercept1 = getSlopeToUse(face1);
		//std::cout << "intercept 1 is : " << intercept1.x << ", " << intercept1.y << ", " << intercept1.z << std::endl;

		// get the second face value
		ECBPPOrientations face2 = borderDataMapRef->faceInterceptMap[tempStorage.tempFaceList.faceList[1]];
		ECBPolyPoint intercept2 = getSlopeToUse(face2);
		interceptToUse = getInterceptToUseFromLine(intercept1, intercept2);
	}

	// an intercept count of 3 means we need to get the appropriate slope from the corner (in theory, there should only be 1 valid slope ever)
	else if (tempStorage.numberOfIntercepts == 3)
	{
		std::cout << ">>>>>>>> Corner encountered! " << std::endl;
		moveValsToPass = borderDataMapRef->cornerMap[currentBeginOrientation.osubtype].borderLimits;
		ECBPolyPoint polyPointToPass;
		polyPointToPass.x = moveValsToPass.x;
		polyPointToPass.y = moveValsToPass.y;
		polyPointToPass.z = moveValsToPass.z;
		interceptToUse = getInterceptToUseFromCorner(polyPointToPass);
	}

	PLTracingResult currentTraceResults = getBlueprintTracingResult(interceptToUse);
	currentEndPoint = currentTraceResults.resultingEndPoint;
	//std::cout << "::::::::::::::::::::::::::::::::Resulting endpoint is: " << currentEndPoint.x << ", " << currentEndPoint.y << ", " << currentEndPoint.z << ", " << std::endl;
	currentEndOrientation = currentTraceResults.resultingOrientation;

	EnclaveKeyDef::EnclaveKey newTestMoveVals = OrganicUtils::retrieveBorderDirection(currentEndOrientation, &borderData);
	std::cout << "!!! Initial begin point is: " << currentBeginPoint.x << ", " << currentBeginPoint.y << ", " << currentBeginPoint.z << std::endl;
	std::cout << "!!! Initial end point is: " << currentEndPoint.x << ", " << currentEndPoint.y << ", " << currentEndPoint.z << std::endl;
	std::cout << "!!! New move vals are: (" << newTestMoveVals.x << ", " << newTestMoveVals.y << ", " << newTestMoveVals.z << ") " << std::endl;
}

ECBPolyPoint BlueprintFillerRunner::getSlopeToUse(ECBPPOrientations in_interceptType)
{
	if (in_interceptType == ECBPPOrientations::X_INT)
	{
		//std::cout << "!!! Returning x-int..." << std::endl;
		return initial_x_slope;

	}
	else if (in_interceptType == ECBPPOrientations::Y_INT)
	{
		//std::cout << "!!! Returning y-int..." << std::endl;
		return initial_y_slope;
	}
	else if (in_interceptType == ECBPPOrientations::Z_INT)
	{
		//std::cout << "!!! Returning z-int..." << std::endl;
		return initial_z_slope;
	}
}

ECBPolyPoint BlueprintFillerRunner::getInterceptToUseFromLine(ECBPolyPoint in_intercept1, ECBPolyPoint in_intercept2)
{
	int first_intercept_invalid = 0;		// for storing if the first intercept value is valid selection, start at 0 (meaning its valid)
	int second_intercept_invalid = 0;		// for storing if the second intercept value is valid selection, start at 0 ""
	ECBPolyPoint normalized_first_intercept = OrganicUtils::findNormalizedPoint(in_intercept1);
	ECBPolyPoint normalized_second_intercept = OrganicUtils::findNormalizedPoint(in_intercept2);
	ECBPolyPoint returnPoint;				// the point to return
											// compare values for first intercept
											//std::cout << "move vals: " << moveVals.x << ", " << moveVals.y << ", " << moveVals.z << std::endl;
											//std::cout << "original first int: " << in_intercept1.x << ", " << in_intercept1.y << ", " << in_intercept1.z << std::endl;
											//std::cout << "original second int: " << in_intercept2.x << ", " << in_intercept2.y << ", " << in_intercept2.z << std::endl;
											//std::cout << "normalized first int: " << normalized_first_intercept.x << ", " << normalized_first_intercept.y << ", " << normalized_first_intercept.z << std::endl;
											//std::cout << "normalized second int: " << normalized_second_intercept.x << ", " << normalized_second_intercept.y << ", " << normalized_second_intercept.z << std::endl;
	if (
		(
		(moveVals.x == normalized_first_intercept.x)
			&&
			(moveVals.x != 0.0f)
			)
		||
		(
		(moveVals.y == normalized_first_intercept.y)
			&&
			(moveVals.y != 0.0f)
			)
		||
		(
		(moveVals.z == normalized_first_intercept.z)
			&&
			(moveVals.z != 0.0f)
			)
		)
	{
		//std::cout << "First intercept set to invalid..." << std::endl;
		first_intercept_invalid = 1;
	}

	// compare values for second intercept
	if (
		(
		(moveVals.x == normalized_second_intercept.x)
			&&
			(moveVals.x != 0.0f)
			)
		||
		(
		(moveVals.y == normalized_second_intercept.y)
			&&
			(moveVals.y != 0.0f)
			)
		||
		(
		(moveVals.z == normalized_second_intercept.z)
			&&
			(moveVals.z != 0.0f)
			)
		)
	{
		//std::cout << "Second intercept set to invalid..." << std::endl;
		second_intercept_invalid = 1;
	}

	// use the second intercept
	if ((first_intercept_invalid == 1) && (second_intercept_invalid == 0))
	{
		returnPoint = in_intercept2;
		//std::cout << "Second slope is valid, choosing: " << returnPoint.x << ", " << returnPoint.y << ", " << returnPoint.z << std::endl;
	}

	// use the first intercept
	else if ((second_intercept_invalid == 1) && (first_intercept_invalid == 0))
	{
		returnPoint = in_intercept1;
		//std::cout << "First slope is valid, choosing: " << returnPoint.x << ", " << returnPoint.y << ", " << returnPoint.z << std::endl;
	}

	// if both are valid, use the first one
	else if ((first_intercept_invalid == 0) && (second_intercept_invalid == 0))
	{
		returnPoint = in_intercept1;
		//std::cout << "Both slopes technically valid, returning first. " << std::endl;
	}
	return returnPoint;
}

ECBPolyPoint BlueprintFillerRunner::getInterceptToUseFromCorner(ECBPolyPoint in_moveValues)
{
	ECBPolyPoint returnPoint;
	int x_intercept_invalid = 0;	// for storing if the x intercept value is valid selection, start at 0 (meaning its valid)
	int y_intercept_invalid = 0;	// ""  y intercept
	int z_intercept_invalid = 0;	// ""  z intercept
	ECBPolyPoint normalized_x_intercept = OrganicUtils::findNormalizedPoint(initial_x_slope);
	ECBPolyPoint normalized_y_intercept = OrganicUtils::findNormalizedPoint(initial_y_slope);
	ECBPolyPoint normalized_z_intercept = OrganicUtils::findNormalizedPoint(initial_z_slope);

	// check for x intercept invalid
	if ((moveVals.y == normalized_x_intercept.y)
		||
		(moveVals.z == normalized_x_intercept.z)
		)
	{
		x_intercept_invalid = 1;
		//std::cout << "Corner calc: x int is invalid!!" << std::endl;
	}

	// check for y intercept invalid
	if ((moveVals.x == normalized_y_intercept.x)
		||
		(moveVals.z == normalized_y_intercept.z)
		)
	{
		y_intercept_invalid = 1;
		//std::cout << "Corner calc: y int is invalid!!" << std::endl;
	}

	// check for z intercept invalid
	if ((moveVals.x == normalized_z_intercept.x)
		||
		(moveVals.y == normalized_z_intercept.y)
		)
	{
		z_intercept_invalid = 1;
		//std::cout << "Corner calc: z int is invalid!!" << std::endl;
	}

	// return appropriate point
	if (x_intercept_invalid == 0)
	{
		//std::cout << "Returning valid x-slope!" << std::endl;
		returnPoint = initial_x_slope;
	}
	else if (y_intercept_invalid == 0)
	{
		//std::cout << "Returning valid y-slope!" << std::endl;
		returnPoint = initial_y_slope;
	}
	else if (z_intercept_invalid == 0)
	{
		//std::cout << "Returning valid z-slope!" << std::endl;
		returnPoint = initial_z_slope;
	}
	return returnPoint;
}

PLTracingResult BlueprintFillerRunner::getBlueprintTracingResult(ECBPolyPoint in_interceptToUse)
{
	PLTracingResult resultToReturn;
	ECBPolyPoint resultEndpoint = OrganicUtils::getBlueprintTracingEndpointForIsolatedPrimaryT2(currentBeginPoint, in_interceptToUse, &currentBorderLineList, blueprintKey);
	ECBPPOrientationResults orientationResults = OrganicUtils::GetBlueprintPointOrientation(resultEndpoint, &currentBorderLineList);
	//std::cout << "oh hello!! " << std::endl;
	BorderMDFaceList resultFaceList = OrganicUtils::getFaceList(orientationResults, &borderData);					// get resultant face list by looking up the map with the orientation
	//std::cout << "oh hello!!! " << std::endl;
	resultToReturn.resultingEndPoint = resultEndpoint;
	resultToReturn.resultingFaceList = resultFaceList;
	resultToReturn.resultingOrientation = orientationResults;
	return resultToReturn;
}