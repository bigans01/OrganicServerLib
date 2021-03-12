#pragma once

#ifndef CONTOURBASE_H
#define CONTOURBASE_H

#include <chrono>
#include "ECBPolyPoint.h"
#include "OSContouredTriangleStrip.h"
#include "OSContourPlanDirections.h"
#include "ForgedPolyRegistry.h"
#include <map>
#include "OrganicClient.h"
#include "ECBPolyType.h"
#include "BlueprintTransformRefs.h"
#include "EnclaveFractureResultsMap.h"
#include "PointAdherenceOrder.h"


// base class for all future contour plans (will eventually replace old contour plan)
class ContourBase
{
public:

	// member variables each derived class should have
	unordered_map<int, OSContouredTriangleStrip> triangleStripMap;
	unordered_map<int, OSContouredTriangleStrip> bottomTriangleStripMap;
	OSContourPlanDirections planDirections;
	ForgedPolyRegistry planPolyRegistry;			// for any "shell" poly.
	ForgedPolyRegistry massDriverPolyRegistry;		// only for "shell" polys which will become a mass driver.
	PointAdherenceOrder adherenceData;

	// virtual functions
	virtual void initialize(ECBPolyPoint in_startPoint, int in_numberOfLayers, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue) = 0;
	virtual void amplifyAllContourLinePoints() = 0;
	virtual void constructStripTriangles(int in_stripID, int in_materialID) = 0;
	virtual void constructBottomStripTriangles(int in_stripID, int in_materialID) = 0;
	virtual void constructSingleContouredTriangle(ECBPolyPoint in_x, ECBPolyPoint in_y, ECBPolyPoint in_z, ECBPolyPoint in_massReferencePoint, int in_triangleStripID, short in_materialID, ECBPolyType in_type) = 0;
	virtual void constructSingleContouredTriangle(ECBPolyPoint in_x, ECBPolyPoint in_y, ECBPolyPoint in_z, ECBPolyPoint in_massReferencePoint, int in_triangleStripID, short in_materialID) = 0;
	virtual void runMassDrivers(OrganicClient* in_clientRef, std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapRef, EnclaveFractureResultsMap* in_fractureResultsMapRef) = 0;

	// inherited functions for derived classes
	void runPolyFracturerForAffectedBlueprints(OrganicClient* in_clientRef, std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapRef);
	void writeAffectedBlueprintsToDisk(std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapRef, std::string in_worldName);
	void updateAffectedBlueprints(OrganicClient* in_clientRef, std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapRef, EnclaveFractureResultsMap* in_fractureResultsMapRef);
	ECBPolyPoint roundContourPointToHundredths(ECBPolyPoint in_contourPoint);
};

#endif
