#include "stdafx.h"
#include "BlueprintFillerRunner.h"

void BlueprintFillerRunner::initialize(PrimaryLineT1* in_lineRef, 
									ECBPolyPoint in_currentSegmentBegin, 
									ECBPolyPoint in_currentSegmentEnd, 
									EnclaveKeyDef::EnclaveKey in_currentSegmentBlueprintKey, 
									std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>* in_tracedBlueprintCountMapRef, 
									std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>* in_filledBlueprintMapRef, 
									OSContouredTriangle* in_osTriangleRef, 
									std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapRef,
									PointAdherenceOrder* in_adherenceOrderRef)
{
	// get the intercept point of the referenced line
	//ECBIntersectMeta initialIntersect = OrganicUtils::findClosestBlueprintIntersection(in_lineRef->beginPointRealXYZ, in_lineRef->endPointRealXYZ, in_lineRef->beginPointBlueprintKey, in_lineRef->endPointBlueprintKey);
	//ECBPolyPoint initialIntersectedPoint = initialIntersect.intersectedPoint;
	//ECBBorderLineList currentBorderLineList = IndependentUtils::determineBorderLines(in_lineRef->beginPointBlueprintKey);
	//ECBPPOrientationResults currentEndpointOrientationResults = IndependentUtils::GetBlueprintPointOrientation(initialIntersectedPoint, &currentBorderLineList);
	//BorderDataMap borderData;
	//EnclaveKeyDef::EnclaveKey moveVals = OrganicUtils::retrieveBorderDirection(currentEndpointOrientationResults, &borderData);

	//std::cout << "!!! Original line coordinates are: Begin (" << in_lineRef->beginPointRealXYZ.x << ", " << in_lineRef->beginPointRealXYZ.y << ", " << in_lineRef->beginPointRealXYZ.z << ") | End (" << in_lineRef->endPointRealXYZ.x << ", " << in_lineRef->endPointRealXYZ.y << ", " << in_lineRef->endPointRealXYZ.z << ") " << std::endl;
	//std::cout << "!!! New end point is: (" << initialIntersectedPoint.x << ", " << initialIntersectedPoint.y << ", " << initialIntersectedPoint.z << ") " << std::endl;
	//std::cout << "!!! New point move vals are: (" << moveVals.x << ", " << moveVals.y << ", " << moveVals.z << ") " << std::endl;
	//std::cout << "~~~~~~~~~~~~ Beginning initial fill " << std::endl;
	//std::cout << "Parent Line point A: " << in_lineRef->beginPointRealXYZ.x << ", " << in_lineRef->beginPointRealXYZ.y << ", " << in_lineRef->beginPointRealXYZ.z << std::endl;
	//std::cout << "Parent Line point B: " << in_lineRef->endPointRealXYZ.x << ", " << in_lineRef->endPointRealXYZ.y << ", " << in_lineRef->endPointRealXYZ.z << std::endl;
	//std::cout << "(|| SERVER) current segment begin (from constructor) is: " << in_currentSegmentBegin.x << ", " << in_currentSegmentBegin.y << ", " << in_currentSegmentBegin.z << std::endl;
	//std::cout << "(|| SERVER) current segment end   (from constructor) is: " << in_currentSegmentEnd.x << ", " << in_currentSegmentEnd.y << ", " << in_currentSegmentEnd.z << std::endl;

	tracedBlueprintCountMapRef = in_tracedBlueprintCountMapRef;
	filledBlueprintMapRef = in_filledBlueprintMapRef;
	contouredTrianglePtr = in_osTriangleRef;

	baseKey = in_currentSegmentBlueprintKey;
	adherenceOrderRef = in_adherenceOrderRef;

	blueprintMapRef = in_blueprintMapRef;
	fillerRunnerPrimaryLine = constructFillerPrimaryInitial(in_lineRef, in_currentSegmentBegin, in_currentSegmentEnd, in_currentSegmentBlueprintKey);	// assign the primary line
	primaryLineSlope = OrganicUtils::findSlope(fillerRunnerPrimaryLine.beginPointRealXYZ, fillerRunnerPrimaryLine.endPointRealXYZ);							
	normalizedPrimaryLineSlope = OrganicUtils::findNormalizedSlope(fillerRunnerPrimaryLine.beginPointRealXYZ, fillerRunnerPrimaryLine.endPointRealXYZ);	
	traverseToNewBlueprint();
}
bool BlueprintFillerRunner::checkIfRunComplete()
{
	return isRunComplete;
}
void BlueprintFillerRunner::traverseToNewBlueprint()
{
	// first, check the orientation of the endpoint, to see if it's a corner, and perform special logic if so.
	if (currentEndOrientation.otype == ECBPPOrientations::CORNER)
	{

	}

	iterateAndCheckedForTouchedBlueprint();
}

void BlueprintFillerRunner::iterateAndCheckedForTouchedBlueprint()
{
	
	//EnclaveKeyDef::EnclaveKey tempMoveVals = borderDataMapRef->cornerMap[currentBeginOrientation.osubtype].borderLimits;
	//currentBorderLineList = IndependentUtils::determineBorderLines(blueprintKey);	// get the blueprint borders for this iteration, in order to figure out the next iteration.

	/*
	if (currentEndOrientation.otype == ECBPPOrientations::FACE)
	{

	}
	else if (currentEndOrientation.otype == ECBPPOrientations::LINE)
	{

	}
	else if (currentEndOrientation.otype == ECBPPOrientations::CORNER)
	{

	}
	*/
	//std::cout << "###################### Beginning fill attempt. ######################### " << std::endl;
	//std::cout << "Point A of Line: " << fillerRunnerPrimaryLine.beginPointRealXYZ.x << ", " << fillerRunnerPrimaryLine.beginPointRealXYZ.y << ", " << fillerRunnerPrimaryLine.beginPointRealXYZ.z << std::endl;
	//std::cout << "Point B of Line: " << fillerRunnerPrimaryLine.endPointRealXYZ.x << ", " << fillerRunnerPrimaryLine.endPointRealXYZ.y << ", " << fillerRunnerPrimaryLine.endPointRealXYZ.z << std::endl;
	//std::cout << "Slope of primary line: " << primaryLineSlope.x << ", " << primaryLineSlope.y << ", " << primaryLineSlope.z << std::endl;
	//std::cout << "Beginning value of blueprint key: " << blueprintKey.x << ", " << blueprintKey.y << ", " << blueprintKey.z << std::endl;
	if (isRunValid == true) // only do this if the run is valid
	{
		currentBorderLineList = IndependentUtils::determineBorderLines(blueprintKey);
		ECBPPOrientationResults currentEndpointOrientationResults = IndependentUtils::GetBlueprintPointOrientation(fillerRunnerPrimaryLine.endPointRealXYZ, &currentBorderLineList);
		EnclaveKeyDef::EnclaveKey tempMoveVals = OrganicUtils::retrieveBorderDirection(currentEndpointOrientationResults, &borderData);
		ECBPolyPoint commonValsToFind;
		commonValsToFind.x = tempMoveVals.x;
		commonValsToFind.y = tempMoveVals.y;
		commonValsToFind.z = tempMoveVals.z;
		ECBPolyPoint resultantMoveVals = OrganicUtils::findCommonMoveValues(commonValsToFind, normalizedPrimaryLineSlope);
		//std::cout << "!! Move vals for this iteration are: << " << resultantMoveVals.x << ", " << resultantMoveVals.y << ", " << resultantMoveVals.z << std::endl;
		applyMovementToBlueprintKey(resultantMoveVals);					// shift the blueprint key we will go into


		if
			(
			(baseKey.x == -2)
				&&
				(baseKey.y == -3)
				&&
				(baseKey.z == 4)
				)
		{
			printTracedBlueprintsFromRef();
		}



		bool wasTraced = checkIfBlueprintWasTraced(blueprintKey);		// check if the new key was actually traced by the contoured triangle

		if (wasTraced == false)
		{
			while (checkIfRunComplete() == false)
			{

				//primaryLineSlope = OrganicUtils::findSlope(fillerRunnerPrimaryLine.beginPointRealXYZ, fillerRunnerPrimaryLine.endPointRealXYZ);
				//normalizedPrimaryLineSlope = OrganicUtils::findNormalizedSlope(fillerRunnerPrimaryLine.beginPointRealXYZ, fillerRunnerPrimaryLine.endPointRealXYZ);

				fillerRunnerPrimaryLine.beginPointRealXYZ = fillerRunnerPrimaryLine.endPointRealXYZ;	// load the endpoint into the begin point.
				ECBIntersectMeta intersectData = OrganicUtils::findClosestBlueprintIntersectionFromSlope(fillerRunnerPrimaryLine.beginPointRealXYZ, primaryLineSlope, blueprintKey); // get the intersect data
				
				//std::cout << "Primary line slope was: " << primaryLineSlope.x << ", " << primaryLineSlope.y << ", " << primaryLineSlope.z << std::endl;

				fillerRunnerPrimaryLine.endPointRealXYZ = intersectData.intersectedPoint;			// load the new point into the line.
				currentBorderLineList = IndependentUtils::determineBorderLines(blueprintKey);           // set the new border lines based on the new blueprint key value.



				//std::cout << "##!!! new value of beginPointRealXYZ: " << fillerRunnerPrimaryLine.beginPointRealXYZ.x << ", " << fillerRunnerPrimaryLine.beginPointRealXYZ.y << ", " << fillerRunnerPrimaryLine.beginPointRealXYZ.z << std::endl;
				//std::cout << "##!!! new value of endPointRealXYZ: " << fillerRunnerPrimaryLine.endPointRealXYZ.x << ", " << fillerRunnerPrimaryLine.endPointRealXYZ.y << ", " << fillerRunnerPrimaryLine.endPointRealXYZ.z << std::endl;
				//std::cout << "!!! new blueprint key value: " << blueprintKey.x << ", " << blueprintKey.y << ", " << blueprintKey.z << std::endl;

				// check if current blueprint was filled
				bool wasBlueprintFilled = checkIfBlueprintWasFilled(blueprintKey);
				if (wasBlueprintFilled == false)
				{
					// fill the blueprint
					//std::cout << "!! >>> Blueprint not found as filled, filling... (" << blueprintKey.x << ", " << blueprintKey.y << ", " << blueprintKey.z << ") " << std::endl;
					insertKeyAsFilledAndCreatePoly(blueprintKey);
					adherenceOrderRef->attemptAdherentInsertion(blueprintKey);

				}
				else if (wasBlueprintFilled == true)
				{
					// do nothing
					//std::cout << "!!! Blueprint was already filled.... (" << blueprintKey.x << ", " << blueprintKey.y << ", " << blueprintKey.z << ") " << std::endl;
				}

				// iterate to the next blueprint, and check if it was a traced blueprint. If it isn't a traced blueprint, continue the while loop.
				currentEndpointOrientationResults = IndependentUtils::GetBlueprintPointOrientation(fillerRunnerPrimaryLine.endPointRealXYZ, &currentBorderLineList);	// set new endpoint orientation
				tempMoveVals = OrganicUtils::retrieveBorderDirection(currentEndpointOrientationResults, &borderData);		// get the border direction
				commonValsToFind.x = tempMoveVals.x;		// set the values for the common vals to find.
				commonValsToFind.y = tempMoveVals.y;
				commonValsToFind.z = tempMoveVals.z;
				resultantMoveVals = OrganicUtils::findCommonMoveValues(commonValsToFind, normalizedPrimaryLineSlope);
				applyMovementToBlueprintKey(resultantMoveVals);					// shift the blueprint key we will go into (may not be used in next iteration
				wasTraced = checkIfBlueprintWasTraced(blueprintKey);
				if (wasTraced == true)
				{
					// std::cout << "!!! current blueprint was found as traced by the contour! >> (" << blueprintKey.x << ", " << blueprintKey.y << ", " << blueprintKey.z << std::endl;
				}

				//std::cout << "Continuing while... " << std::endl;
				//std::cout << "Continuing while, from base key: " << baseKey.x << ", " << baseKey.y << ", " << baseKey.z << std::endl;

			}
		}
	}

	//std::cout << "Traversing to next blueprint: " << blueprintKey.x << ", " << blueprintKey.y << ", " << blueprintKey.z << std::endl;
	/*
	if (wasTraced == true)
	{
		std::cout << "!!! current blueprint was found as traced by the contour! >> (" << blueprintKey.x << ", " << blueprintKey.y << ", " << blueprintKey.z << std::endl;
	}
	*/
}

void BlueprintFillerRunner::insertKeyAsFilledAndCreatePoly(EnclaveKeyDef::EnclaveKey in_blueprintKey)
{
	// put the value of the newly added poly into here (when its found)
	ECBPoly newPoly(contouredTrianglePtr->contouredPolyType);
	newPoly.materialID = contouredTrianglePtr->materialID;
	newPoly.emptyNormal = contouredTrianglePtr->contouredEmptyNormal;
	EnclaveCollectionBlueprint* blueprintPtr = &(*blueprintMapRef)[blueprintKey];

	int elementID = blueprintPtr->fetchNextECBPolyKeyID();						// will store the ID of the newly inserted polygon
	contouredTrianglePtr->forgedPolyRegistryRef->addToPolyset(blueprintKey, elementID);	// Add the new poly to the ForgedPolyRegistry
	ECBPolyLine newLine;
	fillECBPolyLineWithPrimary(&newLine);

	blueprintPtr->insertPolyWithKeyValue(elementID, newPoly);
	blueprintPtr->insertECBPolyLineIntoPoly(elementID, fillerRunnerPrimaryLine.IDofLine, newLine);


	(*filledBlueprintMapRef)[in_blueprintKey] = elementID;
}

void BlueprintFillerRunner::fillECBPolyLineWithPrimary(ECBPolyLine* in_polyLineRef)
{
	in_polyLineRef->pointA = fillerRunnerPrimaryLine.beginPointRealXYZ;
	in_polyLineRef->pointB = fillerRunnerPrimaryLine.endPointRealXYZ;
	in_polyLineRef->pointC = fillerRunnerPrimaryLine.thirdPointRealXYZ;
	in_polyLineRef->x_interceptSlope = fillerRunnerPrimaryLine.x_int;
	in_polyLineRef->y_interceptSlope = fillerRunnerPrimaryLine.y_int;
	in_polyLineRef->z_interceptSlope = fillerRunnerPrimaryLine.z_int;
}

void BlueprintFillerRunner::applyMovementToBlueprintKey(ECBPolyPoint in_moveVals)
{
	// OLD: 8/19/2020 and previous:
	/*
	if (in_moveVals.x != 0)
	{
		blueprintKey.x += in_moveVals.x;
	}
	if (in_moveVals.y != 0)
	{
		blueprintKey.y += in_moveVals.y;
	}
	if (in_moveVals.z != 0)
	{
		blueprintKey.z += in_moveVals.z;
	}
	*/

	// New, updated function: 8/20/2020;
	// for the fix caused by:
	//		constructMountainAtpoint(-32.12, 32.3, 32.73)


	EnclaveKeyDef::EnclaveKey blueprintKeyCopy = blueprintKey; // before modifying the blueprint key, get its copy.
	if (in_moveVals.x != 0)
	{
		EnclaveKeyDef::EnclaveKey x_complete = blueprintKeyCopy;
		x_complete.x += in_moveVals.x;
		checkIfBlueprintWasTraced(x_complete);
		blueprintKey.x += in_moveVals.x;
	}
	if (in_moveVals.y != 0)
	{
		EnclaveKeyDef::EnclaveKey y_complete = blueprintKeyCopy;
		y_complete.y += in_moveVals.y;
		checkIfBlueprintWasTraced(y_complete);
		blueprintKey.y += in_moveVals.y;
	}
	if (in_moveVals.z != 0)
	{
		EnclaveKeyDef::EnclaveKey z_complete = blueprintKeyCopy;
		z_complete.z += in_moveVals.z;
		checkIfBlueprintWasTraced(z_complete);
		blueprintKey.z += in_moveVals.z;
	}
}

bool BlueprintFillerRunner::checkIfBlueprintWasTraced(EnclaveKeyDef::EnclaveKey in_blueprintKey)
{
	//bool result = false;
	auto keySearchResult = tracedBlueprintCountMapRef->find(in_blueprintKey);
	//std::cout << "Checking if key " << in_blueprintKey.x << ", " << in_blueprintKey.y << ", " << in_blueprintKey.z << " was traced by the triangle..." << std::endl;f
	if (keySearchResult != tracedBlueprintCountMapRef->end())
	{
		//std::cout << ">>> Key was FOUND! " << std::endl;
		//result = true;
		isRunComplete = true;
	}
	else
	{
		//std::cout << ">>> Key was *NOT* FOUND! " << std::endl;
	}
	//return result;
	return isRunComplete;
}

void BlueprintFillerRunner::printTracedBlueprintsFromRef()
{
	auto tracedBegin = tracedBlueprintCountMapRef->begin();
	auto tracedEnd = tracedBlueprintCountMapRef->end();
	std::cout << "Printing trace blueprint keys from ref..." << std::endl;
	for (; tracedBegin != tracedEnd; tracedBegin++)
	{
		std::cout << tracedBegin->first.x << ", " << tracedBegin->first.y << ", " << tracedBegin->first.z << std::endl;
	}
}

bool BlueprintFillerRunner::checkIfBlueprintWasFilled(EnclaveKeyDef::EnclaveKey in_blueprintKey)
{
	bool isBlueprintFilled = false;
	auto keySearchResult = filledBlueprintMapRef->find(in_blueprintKey);
	//std::cout << "Checking if key was filled... (" << in_blueprintKey.x << ", " << in_blueprintKey.y << ", " << in_blueprintKey.z << ") " << std::endl;
	if (keySearchResult != filledBlueprintMapRef->end())
	{
		//std::cout << ">>> Blueprint was already filled..." << std::endl;
		isBlueprintFilled = true;
	}
	else
	{
		//std::cout << ">>> Blueprint WASN'T filled..." << std::endl;
	}
	return isBlueprintFilled;
}

PrimaryLineT1 BlueprintFillerRunner::constructFillerPrimaryInitial(PrimaryLineT1* in_lineRef, ECBPolyPoint in_currentSegmentBegin, ECBPolyPoint in_currentSegmentEnd, EnclaveKeyDef::EnclaveKey in_currentSegmentBlueprintKey)
{
	// set the initial slopes.
	initial_x_slope = in_lineRef->x_int;
	initial_y_slope = in_lineRef->y_int;
	initial_z_slope = in_lineRef->z_int;

	// get the intercept point of the referenced line 
	//ECBIntersectMeta initialIntersect = OrganicUtils::findClosestBlueprintIntersection(in_lineRef->beginPointRealXYZ, in_lineRef->endPointRealXYZ, in_lineRef->beginPointBlueprintKey, in_lineRef->endPointBlueprintKey);
	//ECBPolyPoint initialIntersectedPoint = initialIntersect.intersectedPoint;

	//std::cout << "!! ## Line segment begin: " << in_currentSegmentBegin.x << ", " << in_currentSegmentBegin.y << ", " << in_currentSegmentBegin.z << std::endl;
	int someVal = 3;
	//std::cout << "!! ## Line segment end: " << in_currentSegmentEnd.x << ", " << in_currentSegmentEnd.y << ", " << in_currentSegmentEnd.z << std::endl;

	ECBPolyPoint initialIntersectedPoint = in_currentSegmentEnd;
	//currentBorderLineList = IndependentUtils::determineBorderLines(in_lineRef->beginPointBlueprintKey);
	//std::cout << ">> Current blueprint key is: " << in_currentSegmentBlueprintKey.x << ", " << in_currentSegmentBlueprintKey.y << ", " << in_currentSegmentBlueprintKey.z << std::endl;
	currentBorderLineList = IndependentUtils::determineBorderLines(in_currentSegmentBlueprintKey);
	//std::cout << "Current blueprint key is: " << in_currentSegmentBlueprintKey.x << ", " << in_currentSegmentBlueprintKey.y << ", " << in_currentSegmentBlueprintKey.x << std::endl;
	ECBPPOrientationResults currentEndpointOrientationResults = IndependentUtils::GetBlueprintPointOrientation(initialIntersectedPoint, &currentBorderLineList);

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
	//std::cout << "(SERVER) ::Original primary line begin point: " << in_lineRef->beginPointRealXYZ.x << ", " << in_lineRef->beginPointRealXYZ.y << ", " << in_lineRef->beginPointRealXYZ.z << std::endl;
	//std::cout << "(SERVER) ::Original primary line end point: " << in_lineRef->endPointRealXYZ.x << ", " << in_lineRef->endPointRealXYZ.y << ", " << in_lineRef->endPointRealXYZ.z << std::endl;
	//std::cout << "(SERVER) ::Original primary line third point: " << in_lineRef->thirdPointRealXYZ.x << ", " << in_lineRef->thirdPointRealXYZ.y << ", " << in_lineRef->thirdPointRealXYZ.z << std::endl;
	//std::cout << "(SERVER) ::Original primary line intended faces: " << in_lineRef->intendedFaces.x << ", " << in_lineRef->intendedFaces.y << ", " << in_lineRef->intendedFaces.z << std::endl;
	//std::cout << "(SERVER) ::Initial move vals are: " << moveVals.x << ", " << moveVals.y << "," << moveVals.z << std::endl;
	//std::cout << "(SERVER) ::Orginal Primary X-int: " << in_lineRef->x_int.x << ", " << in_lineRef->x_int.y << ", " << in_lineRef->x_int.z << std::endl;
	//std::cout << "(SERVER) ::Orginal Primary Y-int: " << in_lineRef->y_int.x << ", " << in_lineRef->y_int.y << ", " << in_lineRef->y_int.z << std::endl;
	//std::cout << "(SERVER) ::Orginal Primary Z-int: " << in_lineRef->z_int.x << ", " << in_lineRef->z_int.y << ", " << in_lineRef->z_int.z << std::endl;

	//std::cout << "(SERVER) determining slope to use for getBlueprintTracingResult..." << std::endl;
	//std::cout << "(SERVER) current segment begin is    : " << in_currentSegmentBegin.x << ", " << in_currentSegmentBegin.y << ", " << in_currentSegmentBegin.z << std::endl;
	//std::cout << "(SERVER) current segment end  is: " << in_currentSegmentEnd.x << ", " << in_currentSegmentEnd.y << ", " << in_currentSegmentEnd.z << std::endl;

	// based on previous result, do some thing:
	ECBPolyPoint interceptToUse;
	EnclaveKeyDef::EnclaveKey moveValsToPass;
	// an intercept count of 1 will always mean a face is use
	if (tempStorage.numberOfIntercepts == 1)
	{
		//std::cout << ">>>>> Face encountered! " << std::endl;
		// an intercept count of 1 will always mean a face is used
		ECBPPOrientations slopeToUse = borderDataMapRef->faceInterceptMap[currentBeginOrientation.osubtype];
		interceptToUse = getSlopeToUse(slopeToUse);
		//std::cout << "|||| (face) Slope to use is: " << interceptToUse.x << ", " << interceptToUse.y << ", " << interceptToUse.z << std::endl;
	}

	// an intercept count of 2 means we need to get the appropriate slope from the line
	else if (tempStorage.numberOfIntercepts == 2)
	{
		//std::cout << ">>>>>> Line encountered! " << std::endl;
		// get the first face value
		ECBPPOrientations face1 = borderDataMapRef->faceInterceptMap[tempStorage.tempFaceList.faceList[0]];
		ECBPolyPoint intercept1 = getSlopeToUse(face1);
		//std::cout << "intercept 1 is : " << intercept1.x << ", " << intercept1.y << ", " << intercept1.z << std::endl;

		// get the second face value
		ECBPPOrientations face2 = borderDataMapRef->faceInterceptMap[tempStorage.tempFaceList.faceList[1]];
		ECBPolyPoint intercept2 = getSlopeToUse(face2);
		//std::cout << "intercept 2 is : " << intercept2.x << ", " << intercept2.y << ", " << intercept2.z << std::endl;
		interceptToUse = getInterceptToUseFromLine(intercept1, intercept2);
	}

	// an intercept count of 3 means we need to get the appropriate slope from the corner (in theory, there should only be 1 valid slope ever)
	else if (tempStorage.numberOfIntercepts == 3)
	{
		//std::cout << ">>>>>>>> Corner encountered! " << std::endl;
		moveValsToPass = borderDataMapRef->cornerMap[currentBeginOrientation.osubtype].borderLimits;
		ECBPolyPoint polyPointToPass;
		polyPointToPass.x = moveValsToPass.x;
		polyPointToPass.y = moveValsToPass.y;
		polyPointToPass.z = moveValsToPass.z;
		interceptToUse = getInterceptToUseFromCorner(polyPointToPass);
	}

	if
	(
		(interceptToUse.x == 0)
		&&
		(interceptToUse.y == 0)
		&&
		(interceptToUse.z == 0)
	)
	{
		//std::cout << "++++WARNING++++ slope is invalid!" << std::endl;
		isRunValid = false;
	}

	PrimaryLineT1 newLine;
	if (isRunValid == true)
	{
		//std::cout << "(SERVER) calling getBlueprintTracingResult..." << std::endl;
		PLTracingResult currentTraceResults = getBlueprintTracingResult(interceptToUse);
		currentEndPoint = currentTraceResults.resultingEndPoint;
		//std::cout << "::::::::::::::::::::::::::::::::Resulting endpoint is: " << currentEndPoint.x << ", " << currentEndPoint.y << ", " << currentEndPoint.z << ", " << std::endl;
		currentEndOrientation = currentTraceResults.resultingOrientation;

		EnclaveKeyDef::EnclaveKey newTestMoveVals = OrganicUtils::retrieveBorderDirection(currentEndOrientation, &borderData);
		//std::cout << "!!! Initial begin point is: " << currentBeginPoint.x << ", " << currentBeginPoint.y << ", " << currentBeginPoint.z << std::endl;
		//std::cout << "!!! Initial end point is: " << currentEndPoint.x << ", " << currentEndPoint.y << ", " << currentEndPoint.z << std::endl;
		//std::cout << "!!! New move vals are: (" << newTestMoveVals.x << ", " << newTestMoveVals.y << ", " << newTestMoveVals.z << ") " << std::endl;

		// setup variables for creation of new primary T2 line
		ECBBorderValues PT2blueprintLimits = OrganicUtils::getBlueprintLimits(blueprintKey);
		ECBPolyPointLocation PT2pointALocation = OrganicUtils::getPolyPointLocation(currentBeginPoint, PT2blueprintLimits);		// 
		ECBPolyPointLocation PT2pointBLocation = OrganicUtils::getPolyPointLocation(currentTraceResults.resultingEndPoint, PT2blueprintLimits);		// 
		
		newLine.IDofLine = in_lineRef->IDofLine;		// make the newly spawned line ID the same as the line it was spawned from
		newLine.perfectClampValue = in_lineRef->perfectClampValue;
		newLine.beginPointRealXYZ = currentBeginPoint;
		newLine.endPointRealXYZ = currentEndPoint;
		newLine.thirdPointRealXYZ = in_currentSegmentBegin;	// third point is equal to the begin point of the original primary line that this new line was spawned from.
		newLine.intendedFaces = OrganicUtils::determineIntendedFaces(newLine.beginPointRealXYZ, newLine.endPointRealXYZ, newLine.thirdPointRealXYZ);
		newLine.beginPointMeta = PT2pointALocation;
		newLine.endPointMeta = PT2pointBLocation;
		newLine.calibrate(newLine.thirdPointRealXYZ);
		newLine.applySlopesFromParentLine(in_lineRef->intendedFaces, in_lineRef->x_int, in_lineRef->y_int, in_lineRef->z_int);

		//std::cout << "Parent line intended faces are: " << in_lineRef->intendedFaces.x << ", " << in_lineRef->intendedFaces.y << ", " << in_lineRef->intendedFaces.z << std::endl;
		//std::cout << "+++ Parent Line slopes +++ " << std::endl;
		//std::cout << "X: " << in_lineRef->x_int.x << ", " << in_lineRef->x_int.y << ",  " << in_lineRef->x_int.z << std::endl;
		//std::cout << "Y: " << in_lineRef->y_int.x << ", " << in_lineRef->y_int.y << ",  " << in_lineRef->y_int.z << std::endl;
		//std::cout << "Z: " << in_lineRef->z_int.x << ", " << in_lineRef->z_int.y << ",  " << in_lineRef->z_int.z << std::endl;
		//std::cout << "New line intended faces are: " << newLine.intendedFaces.x << ", " << newLine.intendedFaces.y << ", " << newLine.intendedFaces.z << std::endl;
		//std::cout << "X: " << newLine.x_int.x << ", " << newLine.x_int.y << ",  " << newLine.x_int.z << std::endl;
		//std::cout << "Y: " << newLine.y_int.x << ", " << newLine.y_int.y << ",  " << newLine.y_int.z << std::endl;
		//std::cout << "Z: " << newLine.z_int.x << ", " << newLine.z_int.y << ",  " << newLine.z_int.z << std::endl;
	}
	return newLine;

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
	else
	{
		std::cout << "!!!!! WARNING: both slopes detected as invalid. " << std::endl;
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
	ECBPPOrientationResults orientationResults = IndependentUtils::GetBlueprintPointOrientation(resultEndpoint, &currentBorderLineList);
	//std::cout << "oh hello!! " << std::endl;
	BorderMDFaceList resultFaceList = IndependentUtils::getFaceList(orientationResults, &borderData);					// get resultant face list by looking up the map with the orientation
	//std::cout << "oh hello!!! " << std::endl;
	resultToReturn.resultingEndPoint = resultEndpoint;
	resultToReturn.resultingFaceList = resultFaceList;
	resultToReturn.resultingOrientation = orientationResults;
	return resultToReturn;
}