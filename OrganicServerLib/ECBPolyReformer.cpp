#include "stdafx.h"
#include "ECBPolyReformer.h"

void ECBPolyReformer::produceFirstPassECBPolys(int* in_nextECBPolyIDTrackerRef)
{
	// First pass: build new ECBPolys from dissolved ECBPolys, for the ECBPolys that are in the contour plan.
	auto contouredSetBegin = contouredMassShellECBPolyIDs.intSet.begin();
	auto contouredSetEnd = contouredMassShellECBPolyIDs.intSet.end();
	for (; contouredSetBegin != contouredSetEnd; contouredSetBegin++)
	{
		auto wasCurrentECBPolyIDErased = reformerTracker.dissolvedTriangleErasedOREs.find(*contouredSetBegin);	// attempt to find an entry indicating some OREs were erased was found.
		if (wasCurrentECBPolyIDErased != reformerTracker.dissolvedTriangleErasedOREs.end())	// it was found
		{
			std::cout << "!! Value of next ECBPoly, before dissolve: " << *in_nextECBPolyIDTrackerRef << std::endl;
			std::cout << ".Contoured Set ECBPoly with ID " << *contouredSetBegin << " was dissolved, stats are: " << std::endl;
			std::unordered_set<EnclaveKeyDef::EnclaveKey, EnclaveKeyDef::KeyHasher> processableKeys;


			// get the remaining OREs; figure out which ones should remain. Should remain meaning that the ORE the new ECBPoly would go into isn't full.
			// For the first pass, that means to compare each of the remaining OREs to the persistentMassContentsRef.
			auto remainingOresBegin = reformerTracker.dissolvedTriangleRemainingOREs[*contouredSetBegin].begin();
			auto remainingOresEnd = reformerTracker.dissolvedTriangleRemainingOREs[*contouredSetBegin].end();
			for (; remainingOresBegin != remainingOresEnd; remainingOresBegin++)
			{
				if 
				(
					//(persistentMassContentsRef->checkIfSpecificOREExists(*remainingOresBegin) == true)	// the ORE was found
					//&&
					(persistentMassContentsRef->fractureResultsContainerMap[*remainingOresBegin].checkIfFull() == false)	// and the ORE to append to wasn't full
				)
				{
					processableKeys.insert(*remainingOresBegin);
				}
			}

			/*
			std::cout << ":: Printing erased OREs: " << std::endl;
			auto printErasedOREsBegin = reformerTracker.dissolvedTriangleErasedOREs[*contouredSetBegin].begin();
			auto printErasedOREsEnd = reformerTracker.dissolvedTriangleErasedOREs[*contouredSetBegin].end();
			for (; printErasedOREsBegin != printErasedOREsEnd; printErasedOREsBegin++)
			{
				std::cout << "(" << printErasedOREsBegin->x << ", " << printErasedOREsBegin->y << ", " << printErasedOREsBegin->z << ") " << std::endl;
			}
			*/

			// the processableKeys equate to the list of OREs that will receive a ECBPoly that is spawned from dissolving the parent ECBPOly;
			//std::cout << ":: Printing processable OREs: " << std::endl;
			std::unordered_map<EnclaveKeyDef::EnclaveKey, int, EnclaveKeyDef::KeyHasher> outputSet;
			auto processablePrintBegin = processableKeys.begin();
			auto processablePrintEnd = processableKeys.end();
			for (; processablePrintBegin != processablePrintEnd; processablePrintBegin++)
			{
				std::cout << "(" << processablePrintBegin->x << ", " << processablePrintBegin->y << ", " << processablePrintBegin->z << ") " << std::endl;
				OrganicRawEnclave* currentProcessableKeyedORERef = &contouredMassContentsRef->fractureResultsContainerMap[*processablePrintBegin];
				auto fetchedEnclaveTriangleVector = currentProcessableKeyedORERef->retriveAllEnclaveTrianglesForSupergroup(*contouredSetBegin);

				std::cout << "Skeleton Supergroups size of current ORE: " << currentProcessableKeyedORERef->skeletonSGM.triangleSkeletonSupergroups.size() << std::endl;
				std::cout << "etcSgm Supergroups size of current ORE: " << currentProcessableKeyedORERef->etcSGM.enclaveTriangleSupergroups.size() << std::endl;
				std::cout << "organicTriangle Supergroups size of current ORE: " << currentProcessableKeyedORERef->organicTriangleSecondarySGM.secondarySupergroups.size() << std::endl;

				std::cout << "Size of fetched triangles for poly with ID: " << *contouredSetBegin << " is " << fetchedEnclaveTriangleVector.size() << std::endl;
				for (int x = 0; x < fetchedEnclaveTriangleVector.size(); x++)
				{
					(*in_nextECBPolyIDTrackerRef)++;
				}

				//auto currentECBPolyIDInOreFinder = currentProcessableKeyedORERef->skeletonSGM.triangleSkeletonSupergroups.find(*contouredSetBegin);
				//if (currentECBPolyIDInOreFinder != currentProcessableKeyedORERef->skeletonSGM.triangleSkeletonSupergroups.end())

				auto currentECBPolyIDInOreFinder = currentProcessableKeyedORERef->organicTriangleSecondarySGM.secondarySupergroups.find(*contouredSetBegin);
				if (currentECBPolyIDInOreFinder != currentProcessableKeyedORERef->organicTriangleSecondarySGM.secondarySupergroups.end())
				{
					std::cout << "!! EnclaveTriangleSkeletonSupergroup with an ID matching " << *contouredSetBegin << " was found!" << std::endl;
				}
				//outputSet[*processablePrintBegin] = (*in_nextECBPolyIDTrackerRef)++;
			}
			firstPassOutPutIDs[*contouredSetBegin] = outputSet;
			std::cout << "!! Value of next ECBPoly ID will be: " << *in_nextECBPolyIDTrackerRef << std::endl;

			int waitVal = 3;
			std::cin >> waitVal;
		}
	}
}