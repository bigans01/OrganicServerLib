#pragma once

#ifndef CONTOUREDPLANUTILS_H
#define CONTOUREDPLANUTILS_H

#include "EnclaveFractureResultsMap.h"
#include "EnclaveCollectionBlueprint.h"
#include "OrganicTriangleTracker.h"
#include "ECBMap.h"
#include "OrganicUtils.h"
#include "IndependentUtils.h"
#include "OrganicSystemUtils.h"
#include "ORELightweightCollider.h"

/*

Description:

This utility class is meant to provide useful functions for various ContourPlan-style classes, to encourage
code re-use. Operations such as ORE collision, or ORE tracking, should always be controlled/handled by this utility class.

*/

class ContouredPlanUtils
{
	public:
		// Below: this funtion will append EnclaveTriangle skeletons into a new ORE, or combine them via a 
		// collider-style class when an existing ORE is encountered in the persistent mass.
		static void appendContourPlanEnclaveTriangleSkeletons(EnclaveKeyDef::EnclaveKey in_blueprintKey,
															ECBMap* in_contourPlanGeneratedMap,
															ECBMap* in_serverECBMap,
															OrganicTriangleTracker* in_organicTriangleTrackerRef);

		// Below: this function acquires tracked OREs from a given set of ECBPolys that exist in the specified blueprint.
		// It is required by ContourPlans, and must be called appropriately on persistent and contoured mass bodies, in order
		// to dissolve ECBPoly triangles, if any such case needs to be handled.
		static void calculateTrackedOREsForAddedContourPolys(EnclaveKeyDef::EnclaveKey in_blueprintKey,
															EnclaveCollectionBlueprint* in_blueprintToCheck,
															std::set<int> in_set,
															std::mutex* in_mutexRef,
															OrganicTriangleTracker* in_organicTriangleTrackerRef);

		// Below: this function is used primarily by BlankContour, which doesn't use BlueprintMassManager / BPMassManagerV2.
		// It will store the produced raw OREs into a temporary map for later use. See it's usage in BlankContour::runMassDrivers.
		static void produceRawEnclavesForTempMap(ECBMap* in_tempECBMapRef,
															EnclaveKeyDef::EnclaveKey in_blueprintKey,
															EnclaveCollectionBlueprint* in_blueprintToCheck,
															std::set<int> in_set,
															OrganicTriangleTracker* in_organicTriangleTrackerRef);
};

#endif
