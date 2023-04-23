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

/*

Description: The primary purpose of this class is to take what remains
of any OrganicTriangle considered to be "shattered" (shattered meaning
that at least one ORE that makes up part of the OrganicTriangle is going to
need to have its currentDependencyState set to a value of 
OREDependencyState::INDEPENDENT. This is done through the triangleTracker,
which stores all the metadata of this operation (the OrganicTriangleTracker
class comes from OrganicCoreLib, so don't get tempted to edit it here.)

When "shattered," it is entirely possible -- and common -- for the other OREs
that made up the OrganicTriangle to remain. Each of these OREs that end up remaining
need to be made into a new ECBPoly.

This class does all of this bookkeeping, so that the BPMassManagerV2 class can utilize it,
when it calls it's BPMassManagerV2::updatePersistentBlueprintPolys() function.

*/

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
					
		EnclaveKeyDef::EnclaveKey reformerBlueprintKey;	// stores the blueprint key associated with this instance of ECBPolyReformer.

		ECBPolyReformerType reformerType = ECBPolyReformerType::UNDEFINED;	// will be set by constructor

		OperableIntSet contouredMassShellECBPolyIDs;	// contains the IDs of all of the ECBPolys that the contour plan added to the
														// blueprint associated with this instance of ECBPolyReformer.

		EnclaveFractureResultsMap* contouredMassContentsRef = nullptr;	// a pointer to the EnclaveFractureResultsMap instance utilized by the contour plan.
																		// Set by constructor. 

		OperableIntSet persistentMassShellECBPolyIDs;	// contains the ids of all the ECBPoly intances that already existed in the associated
														// blueprint.

		EnclaveFractureResultsMap* persistentMassContentsRef = nullptr;	// a pointer to the presistent EnclaveFractureResultsMap. Set by consstructor.

		OrganicTriangleTracker triangleTracker;	// stores the actual bookkeeping for the shattering of OrganicTriangles.

	private:
		friend class BlueprintMassManager;
		friend class BPMassManagerV2;
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
		std::set<int> firstPassShatteredPolysSet;	// Keeps track of the IDs of any ECBPoly that was shattered in the first pass.
													// Used by the function BPMassManagerV2::updatePersistentBlueprintPolys().

		// members needed for second pass (persistent ECBPoyls compared to contoured mass)
		std::map<int, ShatteredECBPolys> secondPassResultingShatteredECBPolys;
		std::map<int, std::unordered_map<EnclaveKeyDef::EnclaveKey, ShatteredEnclaveTriangles, EnclaveKeyDef::KeyHasher>> secondPassResultingShatteredEnclaveTriangles;
		std::set<int> secondPassUnshatteredECBPolysToErase;
		std::unordered_set<EnclaveKeyDef::EnclaveKey, EnclaveKeyDef::KeyHasher> secondPassShatteredORESet;
		std::set<int> secondPassShatteredPolysSet;	// Keeps track of the IDs of any ECBPoly that was shattered in the second pass.
													// Used by the function BPMassManagerV2::updatePersistentBlueprintPolys().

		ShatteredResults produceShatteredResultsForORE(int* in_nextECBPolyIDTrackerRef, 
													   std::vector<EnclaveTriangle> in_enclaveTriangleVector, 
													   EnclaveKeyDef::EnclaveKey in_blueprintKey,
													   EnclaveKeyDef::EnclaveKey in_oreKey);

		// The below functions take in an argument of ShatteredResults, and create a new ECBPoly with an appropriate new ID
		// from that argument, and puts it into either the firstPassResultingShatteredECBPolys or
		// secondPassResultingShatteredECBPolys maps. These maps are needed later, during the call to
		// BPMassManagerV2::updatePersistentBlueprintPolys().
		void moveShatterResultsIntoFirstPassECBPolys(ShatteredResults in_shatterResults);
		void moveShatterResultsIntoSecondPassECBPolys(ShatteredResults in_shatterResults);

};

#endif
