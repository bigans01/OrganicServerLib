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
#include "UniquePointVector.h"


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
		struct AdherentData	// struct for this class only (needs better description)
		{
			EuclideanDirection3D adherentDirection;			// direction
			EnclaveKeyDef::EnclaveKey adherentBlueprintKey;	// blueprint key of the adherent
		};

		struct DiscoveredORELocation	// stores the location of a discovered, valid ORE in another blueprint
		{
			//OrganicRawEnclave* orginalRawEnclaveRef = NULL;
			EuclideanDirection3D direction;
			EnclaveFractureResultsMap* enclaveFractureResultsMapRef;
			EnclaveKeyDef::EnclaveKey neighboringBlueprintKey;		// key of the neighboring blueprint the ORE was found in 
			EnclaveKeyDef::EnclaveKey keyInNeighboringBlueprint;	// the key of the ORE in the neighboring blueprint
		};

		std::map<int, AdherentData> adherentDataMap;	// stores what directions we will be scanning against
		std::unordered_map<EuclideanDirection3D, AdherentDataList> directionLookup;		// a lookup to check against when we performing the point calibration in each OrganicRawEnclave; used so that we may look up the order later.
		std::unordered_map<EnclaveKeyDef::EnclaveKey, OrganicRawEnclaveAdhesives, EnclaveKeyDef::KeyHasher> adhesiveRawEnclavesMap;

		void performAdhesion();
		void scanForOrganicRawEnclavesViaDirection(EuclideanDirection3D in_euclideanDirection);
		void insertAdhesiveData(EnclaveKeyDef::EnclaveKey in_adhesiveEnclaveKey, OrganicRawEnclave* in_organicRawEnclaveRef, EuclideanDirection3D in_euclideanDirection);
		void runPointAdhesions();
		EnclaveKeyDef::EnclaveKey findBorderingOrganicRawEnclaveToCompareAgainst(EuclideanDirection3D in_euclideanDirection, EnclaveKeyDef::EnclaveKey in_currentOrganicRawEnclaveKey);
		void applyAdhesions(EnclaveKeyDef::EnclaveKey in_originalRawEnclaveKey, OrganicRawEnclave* in_originalOrganicRawEnclaveRef, std::map<int, DiscoveredORELocation> in_oreLocations);
		std::vector<ECBPolyPoint> acquireLocalizedPointsFromDiscoveredORELocation(DiscoveredORELocation in_discoveredORELocation);

};

#endif
