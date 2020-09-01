#pragma once

#ifndef ADHESIVERUNNER_H
#define ADHESIVERUNNER_H

#include "EnclaveKeyDef.h"
#include <vector>
#include "AdherentDataList.h"
#include <unordered_map>
#include <EnclaveFractureResultsMap.h>


class AdhesiveRunner
{
	public:
		AdhesiveRunner(EnclaveKeyDef::EnclaveKey in_originalBlueprintKey,
			std::vector<AdherentDataList> in_adherentDataListVector,
			std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveFractureResultsMap,
			EnclaveKeyDef::KeyHasher>* in_enclaveFractureResultsMapMapRef) :
			originalBlueprintKey(in_originalBlueprintKey), adherentDataListVector(in_adherentDataListVector), enclaveFractureResultsMapMapRef(in_enclaveFractureResultsMapMapRef) 
		
		{
			performAdhesion();
		}

		EnclaveKeyDef::EnclaveKey originalBlueprintKey;
		std::vector<AdherentDataList> adherentDataListVector;
		std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveFractureResultsMap, EnclaveKeyDef::KeyHasher>* enclaveFractureResultsMapMapRef;

	private:
		void performAdhesion();

};

#endif
