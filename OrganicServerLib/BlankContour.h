#pragma once

#ifndef BLANKCONTOUR_H
#define BLANKCONTOUR_H

#include "ContourBase.h"

class BlankContour : public ContourBase
{
	public:	
		// definitions for virtual functions
		void initialize(ECBPolyPoint in_startPoint, int in_numberOfLayers, float in_distanceBetweenLayers, float in_startRadius, float in_expansionValue);
		void amplifyAllContourLinePoints();
		void constructStripTriangles(int in_stripID, int in_materialID);
		void constructSingleContouredTriangle(ECBPolyPoint in_x, ECBPolyPoint in_y, ECBPolyPoint in_z, ECBPolyPoint in_massReferencePoint, int in_triangleStripID, short in_materialID);
		void runMassDrivers();
	private:
		
};

#endif
