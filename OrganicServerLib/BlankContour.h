#pragma once

#ifndef BLANKCONTOUR_H
#define BLANKCONTOUR_H

#include "ContourBase.h"
#include "ECBPolyType.h"
#include "ECBMap.h"

class BlankContour : public ContourBase
{
	public:	
		// definitions for virtual functions
		void initialize(ECBPolyPoint in_startPoint, int in_numberOfLayers, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue);
		void amplifyAllContourLinePoints();
		void constructSingleContouredTriangle(ECBPolyPoint in_point0, ECBPolyPoint in_point1, ECBPolyPoint in_point2, ECBPolyPoint in_massReferencePoint, int in_triangleStripID, short in_materialID);
		void runMassDrivers(OrganicClient* in_clientRef, ECBMap* in_blueprintMapRef, EnclaveFractureResultsMap* in_fractureResultsMapRef);
		void buildContouredTriangles();
		std::vector<OSContouredTriangle*> getProcessableContouredTriangles();
	private:
		
};

#endif
