#pragma once

#ifndef OSCONTOUREDTRIANGLERUNNER_H
#define OSCONTOUREDTRIANGLERUNNER_H

#include <iostream>
#include "OSContouredTriangle.h"
#include "OSContourPlanDirections.h"
#include "OSTriangleLineTraverser.h"
#include "EnclaveCollectionBlueprint.h"
#include "EnclaveKeyDef.h"
#include "OrganicUtils.h"

class OSContouredTriangleRunner
{
	public:
		OSContouredTriangleRunner(OSContouredTriangle* in_trianglePtr, 
			OSContourPlanDirections in_contourPlanDirections, 
			std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapRef) 
			: contouredTrianglePtr(in_trianglePtr), planDirections(in_contourPlanDirections), blueprintMapRef(in_blueprintMapRef) {}

		OSContouredTriangle* contouredTrianglePtr = NULL;	// initialize to null; set in initializer list anyway
		OSContourPlanDirections planDirections;
		std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* blueprintMapRef = NULL;

		void performRun();	// run the contoured triangle; perform filling
	private:
		void checkForPerfectClamping();		// checks to see if the contoured triangle is perfectly clamped.
		void calibrateTrianglePointKeys();	// adjusts the contoured triangle's point EnclaveKeys, in the event that any perfect clamping is detected.
		void findTrueKeysForTriangleLinePoints(OSContouredTriangle* in_Triangle, TriangleLine in_Line, EnclaveKeyDef::EnclaveKey* in_KeyPtr, ECBBorderLineList in_borderLineList);
		void rayCastTrianglePoints();		// trace the points (lines) through the blueprints; inner filling of the ContouredTriangle isn't done here (this comes later)
		void tracePointThroughBlueprints(int in_pointID);
		void fillLineMetaData(ECBPolyLine* in_LinePtr, int in_pointID);
		void fillPolyWithClampResult(ECBPoly* in_polyPtr, OSContouredTriangle* in_contouredTriangle);
};

#endif
