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
#include "IndependentUtils.h"
#include "PrimaryLineT1.h"
#include "PrimaryLineT1Array.h"
#include "PolyRunDirection.h"
#include "EnclaveKeyPair.h"
#include "OSBlueprintTraversalController.h"
#include "BlueprintFillerRunner.h"
#include "BorderDataMap.h"
#include "ForgedPolyRegistry.h"
#include "PointAdherenceOrder.h"
#include "ECBMap.h"

class OSContouredTriangleRunner
{
	public:


		OSContouredTriangleRunner(OSContouredTriangle* in_trianglePtr,
			OSContourPlanDirections in_contourPlanDirections,
			ECBMap* in_serverECBMapRef,
			ForgedPolyRegistry* in_forgedPolyRegistryRef,
			PointAdherenceOrder* in_adherenceOrderRef
		) :
			contouredTrianglePtr(in_trianglePtr),
			planDirections(in_contourPlanDirections),
			serverECBMapRef(in_serverECBMapRef),
			runnerForgedPolyRegistryRef(in_forgedPolyRegistryRef),
			adherenceOrderRef(in_adherenceOrderRef)
		{}

		OSContouredTriangle* contouredTrianglePtr = nullptr;	// initialize to null; set in initializer list anyway
		OSContourPlanDirections planDirections;
		ECBMap* serverECBMapRef = nullptr;
		BorderDataMap borderDataMapInstance;
		ForgedPolyRegistry* runnerForgedPolyRegistryRef = nullptr;	// set on constructor initialization
		PointAdherenceOrder* adherenceOrderRef = nullptr;			// ""

		//PrimaryLineT1Array contourLineArray;

		void performRun();	// run the contoured triangle; perform filling
	private:
		//void checkForPerfectClamping();		// checks to see if the contoured triangle is perfectly clamped.
		//void calibrateTrianglePointKeys();	// adjusts the contoured triangle's point EnclaveKeys, in the event that any perfect clamping is detected.
		//void findTrueKeysForTriangleLinePoints(OSContouredTriangle* in_Triangle, TriangleLine in_Line, EnclaveKeyDef::EnclaveKey* in_KeyPtr, ECBBorderLineList in_borderLineList);
		void rayCastTrianglePoints();		// trace the points (lines) through the blueprints; inner filling of the ContouredTriangle isn't done here (this comes later)
		void printContourTrianglePointsDebug();
		void tracePointThroughBlueprints(int in_pointID);
		void fillBlueprintArea(PrimaryLineT1Array* in_contourLineArrayRef);
		void prepareContouredTriangleData(PolyRunDirection in_direction, PrimaryLineT1Array* in_contourLineArrayRef);
		void runContouredTriangleOriginalDirection();
		void runContouredTriangleReverseDirection();
		bool checkIfPointIsOnBlueprintBorder(ECBPolyPoint in_point, EnclaveKeyDef::EnclaveKey in_blueprintKey);
		//int isContouredTrianglePerfectlyClamped();
		void printTracingCounts();
		EnclaveKeyPair getBlueprintKeysForPrimaryLinePoints(int in_lineID);
};

#endif
