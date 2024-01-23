#include "stdafx.h"
#include "ContouredPlanUtils.h"

void ContouredPlanUtils::appendContourPlanEnclaveTriangleSkeletons(EnclaveKeyDef::EnclaveKey in_blueprintKey,
	ECBMap* in_contourPlanGeneratedMap,
	ECBMap* in_serverECBMap,
	OrganicTriangleTracker* in_organicTriangleTrackerRef)
{
	// Below: store a pointer to the persistent blueprint.
	EnclaveCollectionBlueprint* currentPersistentTargetBlueprintRef = in_serverECBMap->getBlueprintRef(in_blueprintKey);

	// cycle through each ORE to append that exists in the contour-plan generated mass; we will ignore OREs in the
	// contour plan generated mass that have an ORELodState::FULL, as those shouldn't be appending EnclaveTriangle instances
	// at all.
	auto targetECBMapRef = in_contourPlanGeneratedMap->getFractureResultsMapRef(in_blueprintKey);
	for (auto& currentRawEnclaveToAppendFrom : targetECBMapRef->fractureResultsContainerMap)
	{
		// Ignore any ORE to append that is FULL.
		if (currentRawEnclaveToAppendFrom.second.getLodState() != ORELodState::FULL)
		{
			// check whether or not it the temp ORE to append exists  in the persistent blueprint already, using the EnclaveKeyDef value paired with the current ORE (the first value below)
			EnclaveKeyDef::EnclaveKey currentEnclaveKey = currentRawEnclaveToAppendFrom.first;
			auto findKeyResult = currentPersistentTargetBlueprintRef->fractureResults.fractureResultsContainerMap.find(currentEnclaveKey);

			// Condition 1: Check if the ORE in the contour plan mass that we wish to add EnclaveTriangle instances from exists already in the persistent mass. 
			// If it doesn't, we'll be creating a new ORE in the persistent mass, which is automatically done below.
			if (findKeyResult == currentPersistentTargetBlueprintRef->fractureResults.fractureResultsContainerMap.end())
			{
				// It is assumed that if the ORE doesn't exist in the blueprint, that it is "fresh" and completely new;
				// So we will append the contents of the contoured plan's generated ORE to the persistent blueprint 
				currentPersistentTargetBlueprintRef->fractureResults.fractureResultsContainerMap[currentEnclaveKey].appendSpawnedEnclaveTriangleSkeletonContainers(&in_serverECBMap->blueprintMapMutex,
																																									&currentRawEnclaveToAppendFrom.second.oreRTHandler);
				
			}

			// Condition 2: The ORE in the contour plan mass that we wish to append EnclaveTriangle instances from already exists;
			// it needs to append the produced EnclaveTriangle instances to the persistent ORE.
			else if (findKeyResult != currentPersistentTargetBlueprintRef->fractureResults.fractureResultsContainerMap.end())
			{
				// ***************CONDITION CHECK*********************
				//
				// The ORE already exists, so it must be analyzed to determine what to do:
				// --if the ORELodState is LOD_ENCLAVE, call OrganicRawEnclave::appendEnclaveTrianglesFromOtherORE
				// --if the ORELodState is LOD_BLOCK, call new class to handle the case.
				ORELodState selectedORELodState = currentPersistentTargetBlueprintRef->fractureResults.fractureResultsContainerMap[currentEnclaveKey].getLodState();
				if
				(	
					(selectedORELodState == ORELodState::LOD_ENCLAVE_SMATTER)
					||
					(selectedORELodState == ORELodState::LOD_ENCLAVE_RMATTER)

					// REMEMBER: the current OREMatterCollider isn't designed to work with OREs that are in a LOD_BLOCK state,
					// so make sure the check below is commented out if you wish to use the OREMatterCollider. Furthermore,
					// if wishing to use OREMatterCollider, ensure that the commented out line of currentLodState = ORELodState::LOD_ENCLAVE_RMATTER 
					// is being used in OrganicRawEnclave::updateOREForRMass() (see OrganicIndependents library).
					//
					// Logic for working out what happens when a ORELightweightCollider is instantiated/working against an ORE that is already in a ORELodState::LOD_ENCLAVE_SMATTER
					// needs to be added.


					||
					(selectedORELodState == ORELodState::LOD_BLOCK)			
				)
				{
					// The below line will append from the oreRTHandler entries from the other ORE, but only if the persistent ORE being appended to 
					// is NOT in a LOD_BLOCK lod state. An ORE in this state technically shouldn't contain any entries in its oreRTHandler object, as it will 
					// lead to data inconsistencies. For example, if appendEnclaveTrianglesFromOtherORE is called on an ORE in a LOD_BLOCK lod state, the currentAppendedState
					// of the ORE gets updated from NONE to SINGLE_APPEND, which isn't what we want. This would also cause the call to in_clientRef->OS->updateOREAsRMass in CPV2Mountain
					// to delete the block skeletons when it detects the SINGLE_APPEND, due to the data inconsistency.
					//
					// For that reason, we will not bother calling appendEnclaveTrianglesFromOtherORE
					// below under this condition, as the whole point of needing to do that in the first place was to call morphLodToBlock on the target ORE that is in 
					// an ORELodState::LOD_ENCLAVE_SMATTER lod, before the ORELightweightCollider::setupCollider function does its modifications (see OrganicCoreLib)
					//
					// therefore calling updateCurrentAppendedState() in the ORE, and setting it to SINGLE_APPEND or MULTIPLE_APPEND.
					// This needs to be done so that we have data in the oreRTHandler before we call morphLodToBlock() on the ORE.
					//
					// We do NOT need to do this if the current state of the selected ORE is LOD_BLOCK, because it shouldn't technically contain any oreRTHandlers when in that state.
					if (selectedORELodState != ORELodState::LOD_BLOCK)
					{
						GroupSetPair oreAppendedData = currentPersistentTargetBlueprintRef->fractureResults.fractureResultsContainerMap[currentEnclaveKey].appendEnclaveTrianglesFromOtherORE(&in_serverECBMap->blueprintMapMutex,
							&currentRawEnclaveToAppendFrom.second);
					}

					// testing only:
					EnclaveKeyDef::EnclaveKey testKey;
					testKey = currentRawEnclaveToAppendFrom.first;

					std::cout << "############################################# !! Beginning ORECollider, for ORE with Blueprint key (" << in_blueprintKey.x << ", " << in_blueprintKey.y << ", " << in_blueprintKey.z << "), Enclave key (" << testKey.x << ", " << testKey.y << ", " << testKey.z << ") " << std::endl;

					
					std::cout << "++++Testing out ORELightweightCollider..." << std::endl;
					auto collisionStart = std::chrono::high_resolution_clock::now();		// optional, for performance testing only	

					
					
					
					// |||||||||||||||||||| START: NEW OPTION
					ORELightweightCollider tempCollider(&currentRawEnclaveToAppendFrom.second, 
														&currentPersistentTargetBlueprintRef->fractureResults.fractureResultsContainerMap[currentEnclaveKey],
														&in_serverECBMap->blueprintMapMutex);
					// |||||||||||||||||||| END: NEW OPTION
					


					/*
					// |||||||||||||||||||| START: OLD OPTION
					std::cout << "| Number of unexposed blocks in the ORE to append from: " << currentRawEnclaveToAppendFrom.second.getTotalUnexposedBlocks() << std::endl;
					std::cout << "| Number of unexposed blocks in the persistent ORE: " << currentPersistentTargetBlueprintRef->fractureResults.fractureResultsContainerMap[currentEnclaveKey].getTotalUnexposedBlocks() << std::endl;

					int warningVal = currentPersistentTargetBlueprintRef->fractureResults.fractureResultsContainerMap[currentEnclaveKey].getTotalUnexposedBlocks();
					if (warningVal == 64)
					{
						std::cout << "!!! WARNING: detected 64 unexposed blocks (it should be FULL!)" << std::endl;
						int warningInput = 3;
						std::cin >> warningInput;
					}
					

					
					OREMatterCollider matterCollider(&currentPersistentTargetBlueprintRef->fractureResults.fractureResultsContainerMap[currentEnclaveKey],
						oreAppendedData.group0Set,
						oreAppendedData.group1Set,
						40,
						4.0f,
						4,
						12,
						MassZoneBoxType::ENCLAVE,
						RMatterCollisionMode::MATTER);


					matterCollider.runCollisionSimulation();
					matterCollider.extractResultsAndSendToORE(&in_serverECBMap->blueprintMapMutex);
					currentPersistentTargetBlueprintRef->fractureResults.fractureResultsContainerMap[currentEnclaveKey].morphLodToBlock(&in_serverECBMap->blueprintMapMutex, EnclaveKeyDef::EnclaveKey(0, 0, 0));
					// |||||||||||||||||||| END: OLD OPTION
					*/

					auto collisionEnd = std::chrono::high_resolution_clock::now();		// optional, for performance testing only	
					std::chrono::duration<double> collisionElapsed = collisionEnd - collisionStart;

					std::cout << "############################################# Finished reformation via ORECollider (Seconds: " << collisionElapsed.count() << ", for ORE with Blueprint key (" << in_blueprintKey.x << ", " << in_blueprintKey.y << ", " << in_blueprintKey.z << "), Enclave key (" << testKey.x << ", " << testKey.y << ", " << testKey.z << ") " << std::endl;

					in_organicTriangleTrackerRef->insertModifiedORE(currentEnclaveKey);
				}

				// Remember, as of 5/6/2023, the ORELightWeightCollider doesn't append to an ORE that is already in an LOD_BLOCK state,
				// so you may see strange artifacts in the OREs below if you run the  OSServer::constructMultiMountTestWithElevator() plan.
				// They will look like holes.
				//
				// This else/if can be removed when the ORELightweightCollider class has logic to deal with OREs in an LOD_BLOCK state.
				else if
				(
					(selectedORELodState == ORELodState::LOD_BLOCK)
				)

				{
					if
					(
						(in_blueprintKey.equals(-1, -1, 1) && currentEnclaveKey.equals(7, 6, 2))
						||
						(in_blueprintKey.equals(0, -1, 1) && currentEnclaveKey.equals(0, 6, 2))
						||
						(in_blueprintKey.equals(0, -1, 1) && currentEnclaveKey.equals(1, 6, 2))
					)
					{
						std::cout << "!!! ContouredPlanUtils::appendContourPlanEnclaveTriangleSkeletons: --> found ORE to debug (i.e, ORE is already in a LOD_BLOCK state;   Blueprint: ";
						in_blueprintKey.printKey();
						std::cout << ", ORE: ";
						currentEnclaveKey.printKey();
						std::cout << std::endl;

						std::cout << "Enter key to continue. " << std::endl;
						int foundKey = 3;
						std::cin >> foundKey;

					}
				}
			}
		}
	}
}

void ContouredPlanUtils::calculateTrackedOREsForAddedContourPolys(EnclaveKeyDef::EnclaveKey in_blueprintKey,
	EnclaveCollectionBlueprint* in_blueprintToCheck,
	std::set<int> in_set,
	std::mutex* in_mutexRef,
	OrganicTriangleTracker* in_organicTriangleTrackerRef)
{

	// run each organic triangle in the set
	auto setBegin = in_set.begin();
	auto setEnd = in_set.end();
	for (setBegin; setBegin != setEnd; setBegin++)
	{
		auto currentPoly = in_blueprintToCheck->getPolyIterFromMap(*setBegin);

		//std::map<int, ECBPoly> dummyMap;

		// ECBPOLY_FIX
		//ECBPoly translatedPoly = currentPoly->second;
		//dummyMap[currentPoly->first] = translatedPoly;
		//auto translatedPolyIter = dummyMap.find(currentPoly->first);

		OrganicTriangle testTriangle(&currentPoly->second, currentPoly->first);
		in_organicTriangleTrackerRef->transferTrackedOREs(currentPoly->first, &testTriangle);

	}
}

void ContouredPlanUtils::produceRawEnclavesForTempMap(ECBMap* in_tempECBMapRef,
	EnclaveKeyDef::EnclaveKey in_blueprintKey,
	EnclaveCollectionBlueprint* in_blueprintToCheck,
	std::set<int> in_set,
	OrganicTriangleTracker* in_organicTriangleTrackerRef)
{

	// run each organic triangle in the set
	auto setBegin = in_set.begin();
	auto setEnd = in_set.end();
	for (setBegin; setBegin != setEnd; setBegin++)
	{
		auto currentPoly = in_blueprintToCheck->getPolyIterFromMap(*setBegin);

		//std::map<int, ECBPoly> dummyMap;

		// ECBPOLY_FIX
		//ECBPoly translatedPoly = currentPoly->second;
		//dummyMap[currentPoly->first] = translatedPoly;
		//auto translatedPolyIter = dummyMap.find(currentPoly->first);

		OrganicTriangle testTriangle(&currentPoly->second, currentPoly->first);
		in_organicTriangleTrackerRef->transferTrackedOREs(currentPoly->first, &testTriangle);

		auto targetFractureResultsMapRef = in_tempECBMapRef->getFractureResultsMapRef(in_blueprintKey);
		targetFractureResultsMapRef->transferEnclaveTrianglesIntoOREs(currentPoly->first, &testTriangle, &in_tempECBMapRef->blueprintMapMutex);

	}
}