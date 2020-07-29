#pragma once

#ifndef BLUEPRINTFILLERRUNNER_H
#define BLUEPRINTFILLERRUNNER_H

#include "OSContouredTriangle.h"
#include "PrimaryLineT1.h"
#include "OrganicUtils.h"
#include "ECBIntersectMeta.h"
#include "EnclaveKeyDef.h"
#include "ECBPPOrientationResults.h"
#include "PLTracingResult.h"
#include "ECBPoly.h"
#include "EnclaveCollectionBlueprint.h"

class BlueprintFillerRunner
{
	public:

		EnclaveKeyDef::EnclaveKey beginKey;		// the beginning key = the key that point A lies in
		EnclaveKeyDef::EnclaveKey currentKey;	// the current key value
		EnclaveKeyDef::EnclaveKey nextKeyAdd;	// how much will be added to currentKey in the next iteration
		EnclaveKeyDef::EnclaveKey endKey;		// the ending key = the key that point B lies in (will be done when this happens)

		ECBPolyPoint currentBeginPoint;				// equals point A of line
		ECBPolyPoint currentEndPoint;	// equals whatever the point is when this line hits an ECB border
		ECBPolyPoint endPoint;					// equals point B of line

		// point orientations
		ECBPPOrientationResults currentBeginOrientation;
		ECBPPOrientationResults currentEndOrientation;

		// move vals
		EnclaveKeyDef::EnclaveKey moveVals;

		// initial slopes
		ECBPolyPoint initial_x_slope;
		ECBPolyPoint initial_y_slope;
		ECBPolyPoint initial_z_slope;

		// border data map
		BorderDataMap borderData;

		// border line list
		ECBBorderLineList currentBorderLineList;

		// blueprint key
		EnclaveKeyDef::EnclaveKey blueprintKey;	// the current blueprint key for the runner.

		// primary line to use, and associated slope
		PrimaryLineT1 fillerRunnerPrimaryLine;
		ECBPolyPoint primaryLineSlope;				// the slope of the line
		ECBPolyPoint normalizedPrimaryLineSlope;	// the normalized slope of the line (for checking for blueprints that have been touched by the contoured triangle already)

		// ref to the traced blueprint map
		std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>* tracedBlueprintCountMapRef;

		// ref to the filled blueprint map
		std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>* filledBlueprintMapRef;

		// pointer to the OS contoured triangle
		OSContouredTriangle* contouredTrianglePtr = NULL;

		// blueprint map ref
		std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* blueprintMapRef;

		// run status flags
		bool isRunComplete = false;

		// is run valid flag
		bool isRunValid = true;		// assumes true (will be true 99.99% of the time)

		void initialize(PrimaryLineT1* in_lineRef, ECBPolyPoint in_currentSegmentBegin, ECBPolyPoint in_currentSegmentEnd, EnclaveKeyDef::EnclaveKey in_currentSegmentBlueprintKey, std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>* in_tracedBlueprintCountMapRef, std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher>* in_filledBlueprintMapRef, OSContouredTriangle* in_osTriangleRef, std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapRef);
		bool checkIfRunComplete();			// check if the run is complete
		void traverseToNewBlueprint();
		void iterateAndCheckedForTouchedBlueprint();
		void applyMovementToBlueprintKey(ECBPolyPoint in_moveVals);
		void insertKeyAsFilledAndCreatePoly(EnclaveKeyDef::EnclaveKey in_blueprintKey);
		bool checkIfBlueprintWasTraced(EnclaveKeyDef::EnclaveKey in_blueprintKey);
		bool checkIfBlueprintWasFilled(EnclaveKeyDef::EnclaveKey in_blueprintKey);
		void fillECBPolyLineWithPrimary(ECBPolyLine* in_polyLineRef);
		PrimaryLineT1 constructFillerPrimaryInitial(PrimaryLineT1* in_lineRef, ECBPolyPoint in_currentSegmentBegin, ECBPolyPoint in_currentSegmentEnd, EnclaveKeyDef::EnclaveKey in_currentSegmentBlueprintKey);	// construct, and return the newly spawned primary line that this runner instance will use.

		ECBPolyPoint getSlopeToUse(ECBPPOrientations in_interceptType);
		ECBPolyPoint getInterceptToUseFromLine(ECBPolyPoint in_intercept1, ECBPolyPoint in_intercept2);
		ECBPolyPoint getInterceptToUseFromCorner(ECBPolyPoint in_moveValues);
		PLTracingResult getBlueprintTracingResult(ECBPolyPoint in_interceptToUse);
};

#endif
