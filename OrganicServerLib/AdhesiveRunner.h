#pragma once

#ifndef ADHESIVERUNNER_H
#define ADHESIVERUNNER_H

#include "EnclaveKeyDef.h"
#include <vector>
#include "AdherentDataList.h"
#include <map>
#include <unordered_map>
#include <EnclaveFractureResultsMap.h>
#include "OrganicRawEnclave.h"
#include "OrganicRawEnclaveAdhesives.h"


class AdhesiveRunner
{
	public:
		AdhesiveRunner(EnclaveKeyDef::EnclaveKey in_originalBlueprintKey,
			std::vector<AdherentDataList> in_adherentDataListVector,
			std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveFractureResultsMap, EnclaveKeyDef::KeyHasher>* in_enclaveFractureResultsMapMapRef) :
			originalBlueprintKey(in_originalBlueprintKey), adherentDataListVector(in_adherentDataListVector), enclaveFractureResultsMapMapRef(in_enclaveFractureResultsMapMapRef) 
		
		{
			//std::cout << "~~~~~~~~~~~!! Map ref count is: " << enclaveFractureResultsMapMapRef->size() << std::endl;
			performAdhesion();
		}

		EnclaveKeyDef::EnclaveKey originalBlueprintKey;
		std::vector<AdherentDataList> adherentDataListVector;
		std::unordered_map<EnclaveKeyDef::EnclaveKey, EnclaveFractureResultsMap, EnclaveKeyDef::KeyHasher>* enclaveFractureResultsMapMapRef;

	private:
		struct AdherentData	// struct for this class only
		{
			EuclideanDirection3D adherentDirection;			// direction
			EnclaveKeyDef::EnclaveKey adherentBlueprintKey;	// blueprint key of the adherent
		};
		std::map<int, AdherentData> adherentDataMap;	
		std::unordered_map<EnclaveKeyDef::EnclaveKey, OrganicRawEnclaveAdhesives, EnclaveKeyDef::KeyHasher> adhesiveRawEnclavesMap;

		void performAdhesion();
		void scanForOrganicRawEnclavesViaDirection(EuclideanDirection3D in_euclideanDirection);
		void insertAdhesiveData(EnclaveKeyDef::EnclaveKey in_adhesiveEnclaveKey, OrganicRawEnclave* in_organicRawEnclaveRef, EuclideanDirection3D in_euclideanDirection);

};

#endif
