#include "stdafx.h"
#include "CalibratableBlueprintKeyPair.h"

void CalibratableBlueprintKeyPair::initialize(EnclaveKeyDef::EnclaveKey in_startKey, EnclaveKeyDef::EnclaveKey in_endKey, ECBPolyPoint in_pointA, ECBPolyPoint in_pointB, ECBPolyPoint in_pointC)
{
	startKey = in_startKey;
	endKey = in_endKey;
	pointA = in_pointA;
	pointB = in_pointB;
	pointC = in_pointC;
}

void CalibratableBlueprintKeyPair::calibrate()
{
	// find the normalized slopes we will be using.
	ECBPolyPoint normalizedSlopeA = OrganicUtils::findNormalizedSlope(pointA, pointB);
	ECBPolyPoint normalizedSlopeB = OrganicUtils::findNormalizedSlope(pointB, pointA);

	EnclaveKeyDef::EnclaveKey startKeyCopy = startKey;
	EnclaveKeyDef::EnclaveKey endKeyCopy = endKey;

	std::cout << "Start key is: " << startKey.x << ", " << startKey.y << ", " << startKey.z << std::endl;
	std::cout << "End key is: " << endKey.x << ", " << endKey.y << ", " << endKey.z << std::endl;
	isCalibratable(&startKey.x, normalizedSlopeA.x, pointA.x, pointB.x, pointC.x, startKeyCopy.x, endKeyCopy.x);
	isCalibratable(&startKey.y, normalizedSlopeA.y, pointA.y, pointB.y, pointC.y, startKeyCopy.y, endKeyCopy.y);
	isCalibratable(&startKey.z, normalizedSlopeA.z, pointA.z, pointB.z, pointC.z, startKeyCopy.z, endKeyCopy.z);


	//isCalibratable(&endKey.x, normalizedSlopeB.x, pointA.x, pointB.x, pointC.x);
	//isCalibratable(&endKey.y, normalizedSlopeB.y, pointA.y, pointB.y, pointC.y);
	//isCalibratable(&endKey.z, normalizedSlopeB.z, pointA.z, pointB.z, pointC.z);

	isCalibratable(&endKey.x, normalizedSlopeB.x, pointB.x, pointA.x, pointC.x, startKeyCopy.x, endKeyCopy.x);
	isCalibratable(&endKey.y, normalizedSlopeB.y, pointB.y, pointA.y, pointC.y, startKeyCopy.y, endKeyCopy.y);
	isCalibratable(&endKey.z, normalizedSlopeB.z, pointB.z, pointA.z, pointC.z, startKeyCopy.z, endKeyCopy.z);

	std::cout << "(NEW) Start key is: " << startKey.x << ", " << startKey.y << ", " << startKey.z << std::endl;
	std::cout << "(NEW) End key is: " << endKey.x << ", " << endKey.y << ", " << endKey.z << std::endl;
}

void CalibratableBlueprintKeyPair::isCalibratable(int* in_blueprintLocationPtr, float in_normalizedSlopeValue, float in_pointAxyz, float in_pointBxyz, float in_pointCxyz, float in_keyAfloat, float in_keyBfloat)
{
	float xyzModResult = float(fmod(in_pointAxyz, 32));
	if 
	(
		(xyzModResult == 0)						// must be on a border point
		&& 
		in_normalizedSlopeValue == -1.0f		// must be negative direction
		&&
		(in_keyAfloat != in_keyBfloat)			// don't bother calibrating if the points were originally in the same blueprint.
	)	// potential blueprint key calibration, if this is 0
	{
		*in_blueprintLocationPtr -= 1;
	}

	if 
	(
		(xyzModResult == 0)
		&& 
		in_normalizedSlopeValue == 0.0f
		//&&
		//(in_keyAfloat != in_keyBfloat)
	)
	{
		float direction = OrganicUtils::findNormalizedDirection(in_pointAxyz, in_pointCxyz);
		if (direction == -1.0f)
		{
			*in_blueprintLocationPtr -= 1;
		}
	}
}

EnclaveKeyPair CalibratableBlueprintKeyPair::getBeginAndEndKeys()
{
	EnclaveKeyPair returnPair;
	returnPair.keyA = startKey;
	returnPair.keyB = endKey;
	return returnPair;
}
