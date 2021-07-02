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
#include "OSTriangleMaterialLookup.h"
#include <vector>
#include "OrganicTriangleTracker.h"
#include "OperableIntSet.h"


// base class for all future contour plans (will eventually replace old contour plan)
class ContourBase
{
public:
	// template function for inserting preferred materials
	template<typename FirstMaterial, typename ...RemainingMaterials> void insertMaterials(FirstMaterial && first, RemainingMaterials && ...remaining)
	{
		insertPreferredMaterial(OSTriangleMaterialLookup::findMaterialID(std::forward<FirstMaterial>(first)));
		insertMaterials(std::forward<RemainingMaterials>(remaining)...);
	}
	void insertMaterials() {};

	// member variables each derived class should have
	std::unordered_map<int, OSContouredTriangleStrip> triangleStripMap;
	std::unordered_map<int, OSContouredTriangleStrip> bottomTriangleStripMap;
	OSContourPlanDirections planDirections;
	ForgedPolyRegistry planPolyRegistry;			// for any "shell" poly.
	ForgedPolyRegistry massDriverPolyRegistry;		// only for "shell" polys which will become a mass driver.
	PointAdherenceOrder adherenceData;
	std::map<int, int> preferredMaterialLookup;		// a lookup map for specific materials that a Contour plan expects; each contour plan may look at this map
													// for materials to use, how they look it up is up to each plan.

	// virtual functions
	virtual void initialize(ECBPolyPoint in_startPoint, int in_numberOfLayers, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue) = 0;
	virtual void amplifyAllContourLinePoints() = 0;
	virtual void constructSingleContouredTriangle(ECBPolyPoint in_x, ECBPolyPoint in_y, ECBPolyPoint in_z, ECBPolyPoint in_massReferencePoint, int in_triangleStripID, short in_materialID) = 0;
	virtual void runMassDrivers(OrganicClient* in_clientRef, std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapRef, EnclaveFractureResultsMap* in_fractureResultsMapRef) = 0;
	virtual void buildContouredTriangles() = 0;
	virtual std::vector<OSContouredTriangle*> getProcessableContouredTriangles() = 0;

	// inherited functions for derived classes
	void runPolyFracturerForAffectedBlueprints(OrganicClient* in_clientRef, std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapRef);
	void writeAffectedBlueprintsToDisk(std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapRef, std::string in_worldName);
	void updateAffectedBlueprints(OrganicClient* in_clientRef, std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapRef, EnclaveFractureResultsMap* in_fractureResultsMapRef);
	ECBPolyPoint roundContourPointToHundredths(ECBPolyPoint in_contourPoint);
	void insertPreferredMaterial(int in_materialID);
	int getPreferredMaterialAtIndex(int in_indexToLookup);
};

#endif
