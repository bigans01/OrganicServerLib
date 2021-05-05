#include "stdafx.h"
#include "PrimaryCircuit.h"

void PrimaryCircuit::setBorderData(EnclaveKeyDef::EnclaveKey in_blueprintKey)
{
	blueprintKey = in_blueprintKey;
	borderLineList = IndependentUtils::determineBorderLines(blueprintKey);	// set up the blueprint border line list
	//borderDataMapRef = in_borderDataMapRef;
}

void PrimaryCircuit::insertNewSegment(ECBPolyPoint in_beginPoint, ECBPolyPoint in_endPoint, int in_lineID)
{
	PrimaryCircuitSegment newSegment;
	newSegment.beginPoint = in_beginPoint;
	newSegment.endPoint = in_endPoint;
	newSegment.lineID = in_lineID;
	newSegment.borderLineListRef = &borderLineList;
	newSegment.borderDataMapRef = &borderDataMapInstance;
	primarySegments[currentSegmentIndex++] = newSegment;
	currentSegmentCount++;
	currentMaxIndex++;
}

void PrimaryCircuit::fillSegmentArrayMetaData(int in_faceMatchThreshold)
{
	currentMaxIndex--;
	calculateSegmentLinks(in_faceMatchThreshold);

}

void PrimaryCircuit::calculateSegmentLinks(int in_faceMatchThreshold)
{

	// first, set the next link chain values in each PrimaryCircuitSegment, along with the corresponding targetPoint
	for (int x = 0; x < currentSegmentCount; x++)
	{
		if (x != currentMaxIndex)
		{
			primarySegments[x].indexOfNextSegment = x + 1;
			ECBPolyPoint targetPointToAdd = primarySegments[x + 1].beginPoint;
			primarySegments[x].targetPoint = targetPointToAdd;
		}
		else if (x == currentMaxIndex)
		{
			primarySegments[x].indexOfNextSegment = 0;
			ECBPolyPoint targetPointToAdd = primarySegments[0].beginPoint;
			primarySegments[x].targetPoint = targetPointToAdd;
		}
	}

	// next, calculate the initial values for the PrimaryCircuitSegment (default value is NOVAL, but always changes)
	for (int x = 0; x < currentSegmentCount; x++)
	{
		primarySegments[x].faceMatchThreshold = in_faceMatchThreshold;
		primarySegments[x].determineInitialType();
	}
}

bool PrimaryCircuit::checkIfSegmentIsOpenMulti(int in_lineID)
{
	bool result = false;
	for (int x = 0; x < currentSegmentCount; x++)
	{
		if (primarySegments[x].lineID == in_lineID)
		{
			if (primarySegments[x].type == CircuitSegmentType::OPEN_MULTI)
			{
				result = true;
			}
		}
	}
	return result;
}