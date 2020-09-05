#pragma once

#ifndef ADHESIVERESULTS_H
#define ADHESIVERESULTS_H

#include "ECBPolyPoint.h"
#include <map>
#include <math.h>
#include "LocalizedPointsMetaData.h"
#include <iostream>


class AdhesiveResults
{
	public:
		AdhesiveResults(ECBPolyPoint in_point0, ECBPolyPoint in_point1, ECBPolyPoint in_point2);	// initializer
		struct PointAdhesiveMetaData
		{
			ECBPolyPoint originalPointValue;		// the original value, from the enclave triangle
			ECBPolyPoint currentMatchedPointValue;	// the current closest value
			bool adhesionAttempted = false;			// has an adhesion even been attempted? if this is true, we'll have to set the order below.
			bool touchedYet = false;				// when this gets set to true, it will set the distance to 1000;
			float currentDistance = 0.0f;			// if this is still 0 after comparisons are done, it's an exact match
			int order = 0;
		};
		PointAdhesiveMetaData pointData[3];

		bool enclaveTriangleReformRequired = false;	// indicates whether or not the results of this AdhesiveResults instance indicate that the EnclaveTriangle will need to be reformed
		void applyMetaDataMap(std::map<int, LocalizedPointsMetaData> in_metaDataMap);
		void runLocalizedPointMetaDataAgainstPoint(LocalizedPointsMetaData in_pointMetaData, int in_pointIndex);
		float calculateDistanceBetweenPoints(ECBPolyPoint in_pointA, ECBPolyPoint in_pointB);
		bool scanResults();
};

#endif
