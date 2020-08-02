#pragma once

#ifndef CALIBRATABLEBLUEPRINTKEYPAIR_H
#define CALIBRATABLEBLUEPRINTKEYPAIR_H

#include "EnclaveKeyDef.h"
#include "ECBPolyPoint.h"
#include "OrganicUtils.h"
#include "EnclaveKeyPair.h"

class CalibratableBlueprintKeyPair
{
	public:
		EnclaveKeyDef::EnclaveKey startKey;
		EnclaveKeyDef::EnclaveKey endKey;
		ECBPolyPoint pointA;
		ECBPolyPoint pointB;
		ECBPolyPoint pointC;

		void initialize(EnclaveKeyDef::EnclaveKey in_startKey, EnclaveKeyDef::EnclaveKey in_endKey, ECBPolyPoint in_pointA, ECBPolyPoint in_pointB, ECBPolyPoint in_pointC);
		void calibrate();
		EnclaveKeyPair getBeginAndEndKeys();
	private:
		void isCalibratable(int* in_blueprintLocationPtr, float in_normalizedSlopeValue, float in_pointAxyz, float in_pointBxyz, float in_pointCxyz, float in_keyAfloat, float in_keyBfloat);

};

#endif
