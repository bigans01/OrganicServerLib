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
					(persistentMassContentsRef->fractureResultsContainerMap[*remainingOresBegin].getLodState() != ORELodState::FULL)

				)
				{
					processableKeys.insert(*remainingOresBegin);
				}
			}


			// if there are processable Keys post-shattering, run them here.
			if (!processableKeys.empty())
			{
				// For each ORE in the processableList for the current shattered ECBPoly, we must create a new ECBPoly for each ORE.
				// We can do this by calling retriveAllEnclaveTrianglesForSupergroup, assuming that the supergroup ID related to the ECBPoly 
				// being shattered is still contained within the ORE. If not, this will produce strange OpenGL artifacts.
				auto currentProcessableOREKey = processableKeys.begin();
				auto processablePrintEnd = processableKeys.end();
				for (; currentProcessableOREKey != processablePrintEnd; currentProcessableOREKey++)
				{
					// Retrieve the EnclaveTriangles associated with the supergroup that has an ID matching this ECBPoly we are shattering.
					OrganicRawEnclave* currentProcessableKeyedORERef = &contouredMassContentsRef->fractureResultsContainerMap[*currentProcessableOREKey];
					auto fetchedEnclaveTriangleVector = currentProcessableKeyedORERef->retriveAllEnclaveTrianglesForSupergroup(*currentContouredSetPolyIDIter);

					
					// The below code block is temporarily enabled for debugging; it doesn't break the system, it's just here.
					if (fetchedEnclaveTriangleVector.size() == 0)
					{
						std::cout << "!!!~~~~ (ECBPolyReformer::processContouredPolysAgainstPersistentMass): FOUND no triangles for supergroup!!! Enter number to continue." << std::endl;
						int warningWait = 3;
						std::cin >> warningWait;
					}

					// Create new ECBPolys from the current ORE, and assign those ECBPoly instances appropriate IDs via the in_nextECBPolyIDTrackerRef.
					ShatteredResults currentShatterResults = produceShatteredResultsForORE(in_nextECBPolyIDTrackerRef,
						fetchedEnclaveTriangleVector,
						reformerBlueprintKey,
						*currentProcessableOREKey);
	
					// Take the current shattered results, and move them into the appropriate map. Also, 
					// be sure to insert the ID of this ECBPoly being shattered.
					moveShatterResultsIntoFirstPassECBPolys(currentShatterResults);
					firstPassShatteredPolysSet.insert(*currentContouredSetPolyIDIter);

					(firstPassResultingShatteredEnclaveTriangles[*currentContouredSetPolyIDIter])[*currentProcessableOREKey] = currentShatterResults.shatteredResultEnclaveTriangles;

					// the below call will be deprecated in the near future; but needs to be done to be friendly with the old version of the mass manager 
					// (BlueprintMassManager), until it is deleted/removed.
					firstPassShatteredORESet.insert(*currentProcessableOREKey);
				}
			}

			// If this was a shattered triangle, but there is nothing left to process because all of it's remaining ORE's are FULL,
			// then we must mark it for deletion. We must mark it here, because it wouldn't get caught in the call to BPMassManagerV2::updatePersistentBlueprintPolys(),
			// since that function only deletes old ECBPolys that were shattered and had entries in the processableKeys. 
			else if (processableKeys.empty())
			{
				firstPassUnshatteredECBPolysToErase.insert(*currentContouredSetPolyIDIter);
			}
		}

		// none of the OREs of this ECBPoly were erased (unshattered); we will have to later check whether or not this exists entirely
		// within the persistent mass.
		else		
		{
			//std::cout << "!!!!! Notice ECBPoly with ID " << *currentContouredSetPolyIDIter << " (generated by contoured mass) wasn't shattered! " << std::endl;
		
			// because the triangle wasn't shattered, it also means we have to use the tracked ORE keys that the contoured ECBPoly covered to see if
			// any of the matching ones in the persistent mass are full. If all of them are truly full, it would mean this ECBPoly is consumed by a persistent mass, and we 
			// can then break out of the loop. So, we will scan for any ORE that is FULL, which would indicate that it isn't entirely consumed.
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
					(persistentMassContentsRef->fractureResultsContainerMap[*remainingContouredECBPolyOREsBegin].getLodState() != ORELodState::FULL)
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

void ECBPolyReformer::moveShatterResultsIntoFirstPassECBPolys(ShatteredResults in_shatterResults)
{
	for (auto& currentPoly : in_shatterResults.shatterResultECBPolys.shatteredEcbPolyMap)
	{
		int currentShatteredECBPolyID = currentPoly.first;
		ECBPoly currentShatteredECBPoly = currentPoly.second;

		ShatteredECBPolys tempPolyGroup;
		tempPolyGroup.insertECBPoly(currentShatteredECBPolyID, currentShatteredECBPoly);

		firstPassResultingShatteredECBPolys[currentShatteredECBPolyID] = tempPolyGroup;
	}
}

void ECBPolyReformer::moveShatterResultsIntoSecondPassECBPolys(ShatteredResults in_shatterResults)
{
	for (auto& currentPoly : in_shatterResults.shatterResultECBPolys.shatteredEcbPolyMap)
	{
		int currentShatteredECBPolyID = currentPoly.first;
		ECBPoly currentShatteredECBPoly = currentPoly.second;

		ShatteredECBPolys tempPolyGroup;
		tempPolyGroup.insertECBPoly(currentShatteredECBPolyID, currentShatteredECBPoly);

		secondPassResultingShatteredECBPolys[currentShatteredECBPolyID] = tempPolyGroup;
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
			std::unordered_set<EnclaveKeyDef::EnclaveKey, EnclaveKeyDef::KeyHasher> processableKeys;

			// get the remaining OREs; figure out which ones should remain. Should remain meaning that the ORE the new ECBPoly would go into isn't full.
			// For the second pass, that means to compare each of the remaining OREs to the contouredMassContentsRef.
			auto remainingOresBegin = triangleTracker.shatteredTriangleRemainingOREs[*currentPersistentSetPolyIDIter].begin();
			auto remainingOresEnd = triangleTracker.shatteredTriangleRemainingOREs[*currentPersistentSetPolyIDIter].end();
			for (; remainingOresBegin != remainingOresEnd; remainingOresBegin++)
			{
				if
				(
					(contouredMassContentsRef->fractureResultsContainerMap[*remainingOresBegin].getLodState() != ORELodState::FULL)
				)
				{
					processableKeys.insert(*remainingOresBegin);
				}
			}
		
			// if there are processable Keys post-shattering, run them here.
			if (!processableKeys.empty())
			{
					// For each ORE in the processableList for the current shattered ECBPoly, we must create a new ECBPoly for each ORE.
					// We can do this by calling retriveAllEnclaveTrianglesForSupergroup, assuming that the supergroup ID related to the ECBPoly 
					// being shattered is still contained within the ORE. If not, this will produce strange OpenGL artifacts.
					auto currentProcessableOREKey = processableKeys.begin();
					auto processablePrintEnd = processableKeys.end();
					for (; currentProcessableOREKey != processablePrintEnd; currentProcessableOREKey++)
					{

						// Retrieve the EnclaveTriangles associated with the supergroup that has an ID matching this ECBPoly we are shattering.
						OrganicRawEnclave* currentProcessableKeyedORERef = &persistentMassContentsRef->fractureResultsContainerMap[*currentProcessableOREKey];
						auto fetchedEnclaveTriangleVector = currentProcessableKeyedORERef->retriveAllEnclaveTrianglesForSupergroup(*currentPersistentSetPolyIDIter);

						// Create new ECBPolys from the current ORE, and assign those ECBPoly instances appropriate IDs via the in_nextECBPolyIDTrackerRef.
						ShatteredResults currentShatterResults = produceShatteredResultsForORE(in_nextECBPolyIDTrackerRef,
							fetchedEnclaveTriangleVector,
							reformerBlueprintKey,
							*currentProcessableOREKey);

						// Take the current shattered results, and move them into the appropriate map. Also, 
						// be sure to insert the ID of this ECBPoly being shattered
						moveShatterResultsIntoSecondPassECBPolys(currentShatterResults);
						secondPassShatteredPolysSet.insert(*currentPersistentSetPolyIDIter);

						(secondPassResultingShatteredEnclaveTriangles[*currentPersistentSetPolyIDIter])[*currentProcessableOREKey] = currentShatterResults.shatteredResultEnclaveTriangles;

						// the below call will be deprecated in the near future; but needs to be done to be friendly with the old version of the mass manager 
						// (BlueprintMassManager), until it is deleted/removed.
						secondPassShatteredORESet.insert(*currentProcessableOREKey);			
				}
			}

			// If this was a shattered triangle, but there is nothing left to process because all of it's remaining ORE's are FULL,
			// then we must mark it for deletion. We must mark it here, because it wouldn't get caught in the call to BPMassManagerV2::updatePersistentBlueprintPolys(),
			// since that function only deletes old ECBPolys that were shattered and had entries in the processableKeys. 
			else if (processableKeys.empty())
			{
					secondPassUnshatteredECBPolysToErase.insert(*currentPersistentSetPolyIDIter);
			}
		}

		// none of the OREs of this ECBPoly were erased (unshattered); we will have to later check whether or not this exists entirely
		// within the contoured mass.
		else
		{
			// because the triangle wasn't shattered, it also means we have to use the tracked ORE keys that the persistent ECBPoly covered to see if
			// any of the matching ones in the contoured mass are full. If any are full, it would mean this ECBPoly is consumed by a persistent mass, and we 
			// can then break out of the loop. So, we will scan for any ORE that is FULL, which would indicate that it isn't entirely consumed.
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
					(contouredMassContentsRef->fractureResultsContainerMap[*remainingContouredECBPolyOREsBegin].getLodState() != ORELodState::FULL)
				)
				{
					wasECBPolyConsumedByContouredMass = false;
					break;
				}
			}

			if (wasECBPolyConsumedByContouredMass == true)
			{
				secondPassUnshatteredECBPolysToErase.insert(*currentPersistentSetPolyIDIter);
			}
		}
	}
}

ECBPolyReformer::ShatteredResults ECBPolyReformer::produceShatteredResultsForORE(int* in_nextECBPolyIDTrackerRef, 
																				 std::vector<EnclaveTriangle> in_enclaveTriangleVector, 
																				 EnclaveKeyDef::EnclaveKey in_blueprintKey,
																				 EnclaveKeyDef::EnclaveKey in_oreKey)
{
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
																				 in_blueprintKey,
																				 in_oreKey);

		producedPolys.insertECBPoly(currentValue, convertedPoly);
		producedEnclaveTriangles.insertTriangle(currentValue, *triangleVectorBegin);
	}

	returnResults.shatterResultECBPolys = producedPolys;
	returnResults.shatteredResultEnclaveTriangles = producedEnclaveTriangles;
	return returnResults;
}