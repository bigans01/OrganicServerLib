#pragma once

#ifndef POINTADHERENCEORDER_H
#define POINTADHERENCEORDER_H

#include <vector>
#include <unordered_map>
#include "EnclaveKeyDef.h"

class PointAdherenceOrder
{
	public:
		void attemptAdherentInsertion(EnclaveKeyDef::EnclaveKey in_adherentKey);
		void printAdherentData();
		std::vector<EnclaveKeyDef::EnclaveKey> adherenceOrder;
		std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher> adherenceRelationships;
	private:
		int currentAdherentID = 0;
};

#endif
