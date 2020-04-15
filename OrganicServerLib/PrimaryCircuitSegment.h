#pragma once

#ifndef PRIMARYCIRCUITSEGMENT_H
#define PRIMARYCIRCUITSEGMENT_H

#include "ECBPolyPoint.h"
#include "SCircuitSegmentType.h"
#include "ECBBorderLineList.h"
#include "OrganicUtils.h"
#include "BorderDataMap.h"

class PrimaryCircuitSegment
{
	public:
		ECBPolyPoint beginPoint;
		ECBPolyPoint endPoint;
		int lineID = 0;					// the line ID of the segment
		int indexOfNextSegment = 0;	// will be filled with appropriate value
		int faceMatchThreshold = 1;	// for face list matching; default is 1
		ECBPolyPoint targetPoint;	// the target point to compare to; will be the start point of the next segment in the circuit
		ECBBorderLineList* borderLineListRef;	// a ref to the border line list to help determine the points
		BorderDataMap* borderDataMapRef;
		SCircuitSegmentType type = SCircuitSegmentType::NOVAL;

		void determineInitialType();
};

#endif
