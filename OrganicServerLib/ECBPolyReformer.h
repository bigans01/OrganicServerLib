#pragma once

#ifndef ECBPOLYREFORMER_H
#define ECBPOLYREFORMER_H

#include "OrganicTriangleTracker.h"
#include "OperableIntSet.h"
#include "ECBPolyReformerType.h"
#include "EnclaveFractureResultsMap.h"
#include <unordered_map>
#include "EnclaveKeyDef.h"
#include "EnclaveCollectionBlueprint.h"
#include "ECBPolyPoint.h"
#include <map>
#include <unordered_set>

class ECBPolyReformer
{
	public:
		ECBPolyReformer() {};
		ECBPolyReformer(
			EnclaveKeyDef::EnclaveKey in_blueprintKey,
			EnclaveCollectionBlueprint* in_serverBlueprintRef,
			ECBPolyReformerType in_reformerType,
			OperableIntSet in_contouredMassShellECBPolyIDs,
			EnclaveFractureResultsMap* in_contouredMassContentsRef,
			OperableIntSet in_persistentMassShellECBPolyIDs,
			EnclaveFractureResultsMap* in_persistentMassContentsRef) :
		reformerBlueprintKey(in_blueprintKey),
		serverBlueprintRef(in_serverBlueprintRef),
		reformerType(in_reformerType),
		contouredMassShellECBPolyIDs(in_contouredMassShellECBPolyIDs),
		contouredMassContentsRef(in_contouredMassContentsRef),
		persistentMassShellECBPolyIDs(in_persistentMassShellECBPolyIDs),
		persistentMassContentsRef(in_persistentMassContentsRef)
		{};

		void processContouredPolysAgainstPersistentMass(int* in_nextECBPolyIDTrackerRef);
		void processPersistentPolysAgainstContouredMass(int* in_nextECBPolyIDTrackerRef);
					
		EnclaveKeyDef::EnclaveKey reformerBlueprintKey;
		ECBPolyReformerType reformerType = ECBPolyReformerType::UNDEFINED;	// will be set by constructor
		OperableIntSet contouredMassShellECBPolyIDs;
		EnclaveFractureResultsMap* contouredMassContentsRef = nullptr;
		OperableIntSet persistentMassShellECBPolyIDs;
		EnclaveFractureResultsMap* persistentMassContentsRef = nullptr;

		OrganicTriangleTracker triangleTracker;

	private:
		friend class BlueprintMassManager;
		struct ShatteredECBPolys
		{
			ShatteredECBPolys() {};
			void insertECBPoly(int in_ecbPolyID, ECBPoly in_ecbPoly)
			{
				shatteredEcbPolyMap[in_ecbPolyID] = in_ecbPoly;
			}
			void printPolyIDs()
			{
				auto shatteredEcbPolyMapBegin = shatteredEcbPolyMap.begin();
				auto shatteredEcbPolyMapEnd = shatteredEcbPolyMap.end();
				for (; shatteredEcbPolyMapBegin != shatteredEcbPolyMapEnd; shatteredEcbPolyMapBegin++)
				{
					std::cout << "ID: " << shatteredEcbPolyMapBegin->first << std::endl;
				}
			}
			std::map<int, ECBPoly> shatteredEcbPolyMap;
		};

		struct ShatteredEnclaveTriangles
		{
			ShatteredEnclaveTriangles() {};
			void insertTriangle(int in_triangleID, EnclaveTriangle in_enclaveTriangle)
			{
				shatteredTriangleMap[in_triangleID] = in_enclaveTriangle;
			}
			std::map<int, EnclaveTriangle> shatteredTriangleMap;
		};

		struct ShatteredResults
		{
			ShatteredResults() {};
			ShatteredECBPolys shatterResultECBPolys;
			ShatteredEnclaveTriangles shatteredResultEnclaveTriangles;
		};

		EnclaveCollectionBlueprint* serverBlueprintRef = nullptr;

		// members needed for first pass (contoured ECBPolys compared to persistent mass)
		std::map<int, ShatteredECBPolys> firstPassResultingShatteredECBPolys;
		std::map<int, std::unordered_map<EnclaveKeyDef::EnclaveKey, ShatteredEnclaveTriangles, EnclaveKeyDef::KeyHasher>> firstPassResultingShatteredEnclaveTriangles;
		std::set<int> firstPassUnshatteredECBPolysToErase;
		std::unordered_set<EnclaveKeyDef::EnclaveKey, EnclaveKeyDef::KeyHasher> firstPassShatteredORESet;

		// members needed for second pass (persistent ECBPoyls compared to contoured mass)
		std::map<int, ShatteredECBPolys> secondPassResultingShatteredECBPolys;
		std::map<int, std::unordered_map<EnclaveKeyDef::EnclaveKey, ShatteredEnclaveTriangles, EnclaveKeyDef::KeyHasher>> secondPassResultingShatteredEnclaveTriangles;
		std::set<int> secondPassUnshatteredECBPolysToErase;
		std::unordered_set<EnclaveKeyDef::EnclaveKey, EnclaveKeyDef::KeyHasher> secondPassShatteredORESet;

		ShatteredResults produceShatteredResultsForORE(int* in_nextECBPolyIDTrackerRef, 
													   std::vector<EnclaveTriangle> in_enclaveTriangleVector, 
													   ECBPolyPoint in_currentECBPolyMRP,
													   EnclaveKeyDef::EnclaveKey in_blueprintKey,
													   EnclaveKeyDef::EnclaveKey in_oreKey);

};

#endif
