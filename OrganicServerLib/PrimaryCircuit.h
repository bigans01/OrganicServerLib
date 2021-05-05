#pragma once

#ifndef PRIMARYCIRCUIT_H
#define PRIMARYCIRCUIT_H

#include "PrimaryCircuitSegment.h"
#include "ECBBorderLineList.h"
#include "EnclaveKeyDef.h"
#include "OrganicUtils.h"
#include "IndependentUtils.h"
#include "BorderDataMap.h"

class PrimaryCircuit
{
	public:
		void setBorderData(EnclaveKeyDef::EnclaveKey in_blueprintKey);

		PrimaryCircuitSegment primarySegments[3];
		ECBBorderLineList borderLineList;			
		EnclaveKeyDef::EnclaveKey blueprintKey;
		BorderDataMap borderDataMapInstance;
		int currentMaxIndex = 0;
		int currentSegmentCount = 0;
		int currentSegmentIndex = 0;

		void fillSegmentArrayMetaData(int in_faceMatchThreshold);
		void insertNewSegment(ECBPolyPoint in_beginPoint, ECBPolyPoint in_endPoint, int in_lineID);
		void calculateSegmentLinks(int in_faceMatchThreshold);
		bool checkIfSegmentIsOpenMulti(int in_lineID);
};

#endif
