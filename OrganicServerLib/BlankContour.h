#pragma once

#ifndef BLANKCONTOUR_H
#define BLANKCONTOUR_H

#include "ContourBase.h"
#include "ECBPolyType.h"

class BlankContour : public ContourBase
{
	public:	
		// definitions for virtual functions
		void initialize(ECBPolyPoint in_startPoint, int in_numberOfLayers, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue);
		void amplifyAllContourLinePoints();
		void constructStripTriangles(int in_stripID, int in_materialID);
		void constructBottomStripTriangles(int in_stripID, int in_materialID);
		void constructSingleContouredTriangle(ECBPolyPoint in_point0, ECBPolyPoint in_point1, ECBPolyPoint in_point2, ECBPolyPoint in_massReferencePoint, int in_triangleStripID, short in_materialID, ECBPolyType in_polyType);
		void constructSingleContouredTriangle(ECBPolyPoint in_point0, ECBPolyPoint in_point1, ECBPolyPoint in_point2, ECBPolyPoint in_massReferencePoint, int in_triangleStripID, short in_materialID);
		void runMassDrivers(OrganicClient* in_clientRef, std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveCollectionBlueprint, EnclaveKeyDef::KeyHasher>* in_blueprintMapRef);
	private:
		
};

#endif
