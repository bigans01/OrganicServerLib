#include "stdafx.h"
#include "ECBPolyReformer.h"

void ECBPolyReformer::processContouredPolysAgainstPersistentMass(int* in_nextECBPolyIDTrackerRef)
{
	// First pass: build new ECBPolys from dissolved ECBPolys, for the ECBPolys that are in the contour plan.
	//
	// In other words, compare the ECBPolys the contoured plan added to this blueprint, to the already-existing persistent mass.
	auto currentContouredSetPolyIDIter = contouredMassShellECBPolyIDs.intSet.begin();
	auto contouredSetEnd = contouredMassShellECBPolyIDs.intSet.end();
	for (; currentContouredSetPolyIDIter != contouredSetEnd; currentContouredSetPolyIDIter++)
	{
		auto wasCurrentECBPolyIDErased = triangleTracker.shatteredTriangleErasedOREs.find(*currentContouredSetPolyIDIter);	// attempt to find an entry indicating some OREs were erased was found.
		if (wasCurrentECBPolyIDErased != triangleTracker.shatteredTriangleErasedOREs.end())	// it was found
		{
			//std::cout << "(processContouredPolysAgainstPersistentMass) !! Value of next ECBPoly, before dissolve: " << *in_nextECBPolyIDTrackerRef << std::endl;
			//std::cout << "(processContouredPolysAgainstPersistentMass) .Contoured Set ECBPoly with ID " << *currentContouredSetPolyIDIter << " was dissolved, stats are: " << std::endl;
			std::unordered_set<EnclaveKeyDef::EnclaveKey, EnclaveKeyDef::KeyHasher> processableKeys;


			// get the remaining OREs; figure out which ones should remain. Should remain meaning that the ORE the new ECBPoly would go into isn't full.
			// For the first pass, that means to compare each of the remaining OREs to the persistentMassContentsRef.
			auto remainingOresBegin = triangleTracker.shatteredTriangleRemainingOREs[*currentContouredSetPolyIDIter].begin();
			auto remainingOresEnd = triangleTracker.shatteredTriangleRemainingOREs[*currentContouredSetPolyIDIter].end();
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
			auto printErasedOREsBegin = triangleTracker.shatteredTriangleErasedOREs[*currentContouredSetPolyIDIter].begin();
			auto printErasedOREsEnd = triangleTracker.shatteredTriangleErasedOREs[*currentContouredSetPolyIDIter].end();
			for (; printErasedOREsBegin != printErasedOREsEnd; printErasedOREsBegin++)
			{
				std::cout << "(" << printErasedOREsBegin->x << ", " << printErasedOREsBegin->y << ", " << printErasedOREsBegin->z << ") " << std::endl;
			}
			*/

			// if there are processable Keys post-shattering, run them here.
			if (!processableKeys.empty())
			{
				// the processableKeys equate to the list of OREs that will receive a ECBPoly that is spawned from dissolving the parent ECBPOly;
				//std::cout << ":: Printing processable OREs: " << std::endl;
				ECBPolyPoint currentECBPolyMRP = serverBlueprintRef->getPolyMRP(*currentContouredSetPolyIDIter);
				auto currentProcessableOREKey = processableKeys.begin();
				auto processablePrintEnd = processableKeys.end();
				for (; currentProcessableOREKey != processablePrintEnd; currentProcessableOREKey++)
				{
					if
						(
						(reformerBlueprintKey.x == 0)
							&&
							(reformerBlueprintKey.y == -1)
							&&
							(reformerBlueprintKey.z == 1)

							&&
							(currentProcessableOREKey->x == 6)
							&&
							(currentProcessableOREKey->y == 7)
							&&
							(currentProcessableOREKey->z == 1)
							)
					{
						//std::cout << "!!!!!!!!!!!!!!! SPECIAL DEBUG: OrganicServerLib, ECBPolyReformer::processContouredPolysAgainstPersistentMass" << std::endl;
						//int processContouredWait = 3;
						//std::cin >> processContouredWait;
					}


					//std::cout << "(" << currentProcessableOREKey->x << ", " << currentProcessableOREKey->y << ", " << currentProcessableOREKey->z << ") " << std::endl;
					OrganicRawEnclave* currentProcessableKeyedORERef = &contouredMassContentsRef->fractureResultsContainerMap[*currentProcessableOREKey];
					auto fetchedEnclaveTriangleVector = currentProcessableKeyedORERef->retriveAllEnclaveTrianglesForSupergroup(*currentContouredSetPolyIDIter);

					//std::cout << "Skeleton Supergroups size of current ORE: " << currentProcessableKeyedORERef->skeletonSGM.triangleSkeletonSupergroups.size() << std::endl;
					//std::cout << "etcSgm Supergroups size of current ORE: " << currentProcessableKeyedORERef->etcSGM.enclaveTriangleSupergroups.size() << std::endl;
					//std::cout << "organicTriangle Supergroups size of current ORE: " << currentProcessableKeyedORERef->organicTriangleSecondarySGM.secondarySupergroups.size() << std::endl;
					//std::cout << "Size of fetched triangles for poly with ID: " << *currentContouredSetPolyIDIter << " is " << fetchedEnclaveTriangleVector.size() << std::endl;

					ShatteredResults currentShatterResults = produceShatteredResultsForORE(in_nextECBPolyIDTrackerRef,
						fetchedEnclaveTriangleVector,
						currentECBPolyMRP,
						reformerBlueprintKey,
						*currentProcessableOREKey);
					firstPassResultingShatteredECBPolys[*currentContouredSetPolyIDIter] = currentShatterResults.shatterResultECBPolys;
					(firstPassResultingShatteredEnclaveTriangles[*currentContouredSetPolyIDIter])[*currentProcessableOREKey] = currentShatterResults.shatteredResultEnclaveTriangles;

					auto currentECBPolyIDInOreFinder = currentProcessableKeyedORERef->organicTriangleSecondarySGM.secondarySupergroups.find(*currentContouredSetPolyIDIter);
					if (currentECBPolyIDInOreFinder != currentProcessableKeyedORERef->organicTriangleSecondarySGM.secondarySupergroups.end())
					{
						//std::cout << "!! EnclaveTriangleSkeletonSupergroup with an ID matching " << *currentContouredSetPolyIDIter << " was found!" << std::endl;
					}
					//currentProcessableKeyedORERef->setOREasIndependent();
					firstPassShatteredORESet.insert(*currentProcessableOREKey);
				}

				//std::cout << "(processContouredPolysAgainstPersistentMass) !! Value of next ECBPoly ID will be: " << *in_nextECBPolyIDTrackerRef << std::endl;
				//int waitVal = 3;
				//std::cin >> waitVal;
			}
			else if (processableKeys.empty())
			{
				//std::cout << "!! Notice: contoured ECBPoly was shattered, but also completely consumed (no processable keys)" << std::endl;
				firstPassUnshatteredECBPolysToErase.insert(*currentContouredSetPolyIDIter);
				//int waitVal = 3;
				//std::cin >> waitVal;
			}
		}

		// none of the OREs of this ECBPoly were erased (unshattered); we will have to later check whether or not this exists entirely
		// within the persistent mass.
		else		
		{
			//std::cout << "!!!!! Notice ECBPoly with ID " << *currentContouredSetPolyIDIter << " (generated by contoured mass) wasn't shattered! " << std::endl;
		
			// because the triangle wasn't shattered, it also means we have to use the tracked ORE keys that the contoured ECBPoly covered to see if
			// any of the matching ones in the persistent mass are full. If any are full, it would mean this ECBPoly is consumed by a persistent mass, and we 
			// can then break out of the loop.
			auto remainingContouredECBPolyOREsBegin = triangleTracker.triangleTrackedOREKeys.find(*currentContouredSetPolyIDIter)->second.begin();
			auto remainingContouredECBPolyOREsEnd = triangleTracker.triangleTrackedOREKeys.find(*currentContouredSetPolyIDIter)->second.end();
			bool wasECBPolyConsumedByPersistentMass = true;
			for (; remainingContouredECBPolyOREsBegin != remainingContouredECBPolyOREsEnd; remainingContouredECBPolyOREsBegin++)
			{
				// compare each tracked ORE of the contoured ECBPoly against the matching ORE in the persistent mass (done via the ORE's respective key) 
				// as soon as a condition is detected where the ECBPoly isn't consumed by mass, break out of the loop -- because this means it isn't entirely
				// consumed by the persistent mass.
				if
				(
					(persistentMassContentsRef->fractureResultsContainerMap[*remainingContouredECBPolyOREsBegin].checkIfFull() == false)
				)
				{
					wasECBPolyConsumedByPersistentMass = false;
					break;
				}
			}

			if (wasECBPolyConsumedByPersistentMass == true)
			{
				//std::cout << "!! ECBPoly with ID " << *currentContouredSetPolyIDIter << " (generated by contoured mass) was entirely consumed by persistent mass. " << std::endl;
				//int persistentMassConsumptionWait = 3;
				//std::cin >> persistentMassConsumptionWait;
				firstPassUnshatteredECBPolysToErase.insert(*currentContouredSetPolyIDIter);
			}

		}
	}
}

void ECBPolyReformer::processPersistentPolysAgainstContouredMass(int* in_nextECBPolyIDTrackerRef)
{
	// Second pass: build new ECBPolys from dissolved ECBPolys, for the ECBPolys that are persistent (that is, existed before the contour plan was run).
	//
	// In other words, compare the already-existing ECBPolys in this blueprint, to the contoured mass.
	auto currentPersistentSetPolyIDIter = persistentMassShellECBPolyIDs.intSet.begin();
	auto persistentSetEnd = persistentMassShellECBPolyIDs.intSet.end();
	for (; currentPersistentSetPolyIDIter != persistentSetEnd; currentPersistentSetPolyIDIter++)
	{
		auto wasPersistentECBPolyErased = triangleTracker.shatteredTriangleErasedOREs.find(*currentPersistentSetPolyIDIter);
		if (wasPersistentECBPolyErased != triangleTracker.shatteredTriangleErasedOREs.end())
		{
			//std::cout << "(processPersistentPolysAgainstContouredMass) !! Value of next ECBPoly, before dissolve: " << *in_nextECBPolyIDTrackerRef << std::endl;
			//std::cout << "(processPersistentPolysAgainstContouredMass) .Contoured Set ECBPoly with ID " << *currentPersistentSetPolyIDIter << " was dissolved, stats are: " << std::endl;
			std::unordered_set<EnclaveKeyDef::EnclaveKey, EnclaveKeyDef::KeyHasher> processableKeys;

			// get the remaining OREs; figure out which ones should remain. Should remain meaning that the ORE the new ECBPoly would go into isn't full.
			// For the second pass, that means to compare each of the remaining OREs to the contouredMassContentsRef.
			auto remainingOresBegin = triangleTracker.shatteredTriangleRemainingOREs[*currentPersistentSetPolyIDIter].begin();
			auto remainingOresEnd = triangleTracker.shatteredTriangleRemainingOREs[*currentPersistentSetPolyIDIter].end();
			for (; remainingOresBegin != remainingOresEnd; remainingOresBegin++)
			{
				if
				(
					//(contouredMassContentsRef->checkIfSpecificOREExists(*remainingOresBegin) == true)	// the ORE was found
					//&&
					(contouredMassContentsRef->fractureResultsContainerMap[*remainingOresBegin].checkIfFull() == false)	// and the ORE to append to wasn't full
				)
				{
					processableKeys.insert(*remainingOresBegin);
				}
			}

			// if there are processable Keys post-shattering, run them here.
			if (!processableKeys.empty())
			{
				// the processableKeys equate to the list of OREs that will receive a ECBPoly that is spawned from dissolving the parent ECBPOly;
				ECBPolyPoint currentECBPolyMRP = serverBlueprintRef->getPolyMRP(*currentPersistentSetPolyIDIter);
				auto currentProcessableOREKey = processableKeys.begin();
				auto processablePrintEnd = processableKeys.end();
				for (; currentProcessableOREKey != processablePrintEnd; currentProcessableOREKey++)
				{
					if
					(
						(reformerBlueprintKey.x == 0)
						&&
						(reformerBlueprintKey.y == -1)
						&&
						(reformerBlueprintKey.z == 1)

						&&
						(currentProcessableOREKey->x == 6)
						&&
						(currentProcessableOREKey->y == 7)
						&&
						(currentProcessableOREKey->z == 1)
					)
					{
						//std::cout << "!!!!!!!!!!!!!!! SPECIAL DEBUG: OrganicServerLib, ECBPolyReformer::processPersistentPolysAgainstContouredMass" << std::endl;
						//int processPersistentWait = 3;
						//std::cin >> processPersistentWait;
					}

					//std::cout << "(" << currentProcessableOREKey->x << ", " << currentProcessableOREKey->y << ", " << currentProcessableOREKey->z << ") " << std::endl;
					OrganicRawEnclave* currentProcessableKeyedORERef = &persistentMassContentsRef->fractureResultsContainerMap[*currentProcessableOREKey];
					auto fetchedEnclaveTriangleVector = currentProcessableKeyedORERef->retriveAllEnclaveTrianglesForSupergroup(*currentPersistentSetPolyIDIter);

					//std::cout << "Skeleton Supergroups size of current ORE: " << currentProcessableKeyedORERef->skeletonSGM.triangleSkeletonSupergroups.size() << std::endl;
					//std::cout << "etcSgm Supergroups size of current ORE: " << currentProcessableKeyedORERef->etcSGM.enclaveTriangleSupergroups.size() << std::endl;
					//std::cout << "organicTriangle Supergroups size of current ORE: " << currentProcessableKeyedORERef->organicTriangleSecondarySGM.secondarySupergroups.size() << std::endl;
					//std::cout << "Size of fetched triangles for poly with ID: " << *currentPersistentSetPolyIDIter << " is " << fetchedEnclaveTriangleVector.size() << std::endl;

					ShatteredResults currentShatterResults = produceShatteredResultsForORE(in_nextECBPolyIDTrackerRef,
						fetchedEnclaveTriangleVector,
						currentECBPolyMRP,
						reformerBlueprintKey,
						*currentProcessableOREKey);
					secondPassResultingShatteredECBPolys[*currentPersistentSetPolyIDIter] = currentShatterResults.shatterResultECBPolys;
					(secondPassResultingShatteredEnclaveTriangles[*currentPersistentSetPolyIDIter])[*currentProcessableOREKey] = currentShatterResults.shatteredResultEnclaveTriangles;

					//currentProcessableKeyedORERef->setOREasIndependent();
					secondPassShatteredORESet.insert(*currentProcessableOREKey);
				}
			}
			else if (processableKeys.empty())
			{
				//std::cout << "!! Notice: persistent ECBPoly was shattered, but also completely consumed (no processable keys)" << std::endl;
				secondPassUnshatteredECBPolysToErase.insert(*currentPersistentSetPolyIDIter);
				//int waitVal = 3;
				//std::cin >> waitVal;
			}
			//std::cout << "(processPersistentPolysAgainstContouredMass) !! Value of next ECBPoly ID will be: " << *in_nextECBPolyIDTrackerRef << std::endl;
		}

		// none of the OREs of this ECBPoly were erased (unshattered); we will have to later check whether or not this exists entirely
		// within the contoured mass.
		else
		{
			//std::cout << "!!!!! Notice ECBPoly with ID " << *currentPersistentSetPolyIDIter << " (generated by persistent mass) wasn't shattered! " << std::endl;

			// because the triangle wasn't shattered, it also means we have to use the tracked ORE keys that the persistent ECBPoly covered to see if
			// any of the matching ones in the contoured mass are full. If any are full, it would mean this ECBPoly is consumed by a persistent mass, and we 
			// can then break out of the loop.
			auto remainingContouredECBPolyOREsBegin = triangleTracker.triangleTrackedOREKeys.find(*currentPersistentSetPolyIDIter)->second.begin();
			auto remainingContouredECBPolyOREsEnd = triangleTracker.triangleTrackedOREKeys.find(*currentPersistentSetPolyIDIter)->second.end();
			bool wasECBPolyConsumedByContouredMass = true;
			for (; remainingContouredECBPolyOREsBegin != remainingContouredECBPolyOREsEnd; remainingContouredECBPolyOREsBegin++)
			{
				// compare each tracked ORE of the persistent ECBPoly against the matching ORE in the contoured mass (done via the ORE's respective key) 
				// as soon as a condition is detected where the ECBPoly isn't consumed by mass, break out of the loop -- because this means it isn't entirely
				// consumed by the contoured mass.
				if
				(
					(contouredMassContentsRef->fractureResultsContainerMap[*remainingContouredECBPolyOREsBegin].checkIfFull() == false)
				)
				{
					wasECBPolyConsumedByContouredMass = false;
					break;
				}
			}

			if (wasECBPolyConsumedByContouredMass == true)
			{
				//std::cout << "!! ECBPoly with ID " << *currentPersistentSetPolyIDIter << " (generated by persistent mass) was entirely consumed by persistent mass. " << std::endl;
				//int persistentMassConsumptionWait = 3;
				//std::cin >> persistentMassConsumptionWait;
				secondPassUnshatteredECBPolysToErase.insert(*currentPersistentSetPolyIDIter);
			}
		}
	}
}

ECBPolyReformer::ShatteredResults ECBPolyReformer::produceShatteredResultsForORE(int* in_nextECBPolyIDTrackerRef, 
																				 std::vector<EnclaveTriangle> in_enclaveTriangleVector, 
																				 ECBPolyPoint in_currentECBPolyMRP,
																				 EnclaveKeyDef::EnclaveKey in_blueprintKey,
																				 EnclaveKeyDef::EnclaveKey in_oreKey)
{
	//std::cout << ">>> calling produceShatteredResultsForORE: " << std::endl;
	//std::cout << "Blueprint key: " << in_blueprintKey.x << ", " << in_blueprintKey.y << ", " << in_blueprintKey.z << std::endl;
	//std::cout << "ORE key: " << in_oreKey.x << ", " << in_oreKey.y << ", " << in_oreKey.z << std::endl;

	ShatteredResults returnResults;
	ShatteredECBPolys producedPolys;
	ShatteredEnclaveTriangles producedEnclaveTriangles;

	auto triangleVectorBegin = in_enclaveTriangleVector.begin();
	auto triangleVectorEnd = in_enclaveTriangleVector.end();
	for (; triangleVectorBegin != triangleVectorEnd; triangleVectorBegin++)
	{
		// first, grab the current value representing the next ID to use for the blueprint we are looking at, and increment it
		int currentValue = (*in_nextECBPolyIDTrackerRef)++;

		ECBPoly convertedPoly = IndependentUtils::buildECBPolyFromEnclaveTriangle(*triangleVectorBegin, 
																				 in_currentECBPolyMRP,
																				 in_blueprintKey,
																				 in_oreKey);
		//convertedPoly.printLineData();

		producedPolys.insertECBPoly(currentValue, convertedPoly);
		producedEnclaveTriangles.insertTriangle(currentValue, *triangleVectorBegin);
	}

	//std::cout << "!! Printing IDs of the produced ECBPolys: " << std::endl;
	//producedPolys.printPolyIDs();
	//int waitVal = 3;
	//std::cin >> waitVal;

	returnResults.shatterResultECBPolys = producedPolys;
	returnResults.shatteredResultEnclaveTriangles = producedEnclaveTriangles;
	return returnResults;
}