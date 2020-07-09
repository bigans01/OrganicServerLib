#include "stdafx.h"
#include "PrimaryCircuitSegment.h"

void PrimaryCircuitSegment::determineInitialType()
{
	ECBPPOrientationResults currentSegmentEndOrientation = OrganicUtils::GetBlueprintPointOrientation(endPoint, borderLineListRef);
	ECBPPOrientationResults targetOrientation = OrganicUtils::GetBlueprintPointOrientation(targetPoint, borderLineListRef);
	BorderMDFaceList currentSegmentEndFaceList = OrganicUtils::getFaceList(currentSegmentEndOrientation, borderDataMapRef);
	BorderMDFaceList targetFaceList = OrganicUtils::getFaceList(targetOrientation, borderDataMapRef);

	// are the points the same
	if (OrganicUtils::checkIfPolyPointsMatch(endPoint, targetPoint) == 1)
	{
		type = CircuitSegmentType::CLOSED;	// it is CLOSED case
	}

	// if not, are they on the same face?
	else if (OrganicUtils::checkIfFaceListsMatch(currentSegmentEndFaceList, targetFaceList, faceMatchThreshold) == 1)	// they aren't the same point, but they exist on the same face
	{
		type = CircuitSegmentType::OPEN_SINGLE;	// it is an OPEN_SINGLE case
	}

	// the points are not the same, and they are not on the same face. More than one line will be produced.
	else
	{
		type = CircuitSegmentType::OPEN_MULTI;
	}
}