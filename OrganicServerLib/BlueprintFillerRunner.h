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

		void initialize(PrimaryLineT1* in_lineRef);
		bool checkIfRunComplete();			// check if the run is complete
		void traverseLineOnce();
		void constructFillerPrimaryInitial(PrimaryLineT1* in_lineRef);

		ECBPolyPoint getSlopeToUse(ECBPPOrientations in_interceptType);
		ECBPolyPoint getInterceptToUseFromLine(ECBPolyPoint in_intercept1, ECBPolyPoint in_intercept2);
		ECBPolyPoint getInterceptToUseFromCorner(ECBPolyPoint in_moveValues);
		PLTracingResult getBlueprintTracingResult(ECBPolyPoint in_interceptToUse);
};

#endif
