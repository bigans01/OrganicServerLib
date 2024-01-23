#include "stdafx.h"
#include "ContouredPlanV2Base.h"

void ContouredPlanV2Base::insertPreferredMaterial(TriangleMaterial in_materialID)
{
	int currentLookupSize = preferredMaterialLookup.size();
	preferredMaterialLookup[currentLookupSize] = in_materialID;
}

TriangleMaterial ContouredPlanV2Base::getPreferredMaterialAtIndex(int in_indexToLookup)
{
	TriangleMaterial returnMaterialID = TriangleMaterial::DIRT;	// 1, which is DIRT -- should be returned if nothing is found.
	if (auto materialFinder = preferredMaterialLookup.find(in_indexToLookup); materialFinder != preferredMaterialLookup.end())
	{
		returnMaterialID = materialFinder->second;
	}
	return returnMaterialID;
}


void ContouredPlanV2Base::copyOverProducedECBPolys(std::vector<ContouredTriangleV2*> in_ctv2Vector, ECBMap* in_blueprintMapRef)
{
	for (auto& currentCTV2 : in_ctv2Vector)
	{
		auto producedFTriangle = currentCTV2->produceEquivalentFTriangle();	// generate the FTriangle that we will be producing results from.
		ECBPolyType currentCTV2PolyType = currentCTV2->ctv2PolyType;	// the ECBPolyType that must be used to set the poly type in each ECBPoly
																		// that this CTV2 creates.


		producedFTriangle.fracture();
		auto outputsRef = producedFTriangle.fetchOutputContainerRef();



		// Below switch: optional debug output
		/*
		switch (currentCTV2PolyType)
		{
			case ECBPolyType::ROGUE: { std::cout << "Why is this rogue???" << std::endl; break; }
			case ECBPolyType::FREE: {std::cout << "!!!! WARNING: poly is FREE " << std::endl; break;}
			case ECBPolyType::SHELL: {std::cout << "ECBPoly instances to insert will be of the type ECBPolyType::SHELL. " << std::endl; break;}
			case ECBPolyType::SHELL_MASSDRIVER: {std::cout << "ECBPoly instances to insert will be of the type ECBPolyType::SHELL_MASSDRIVER. " << std::endl; break;}
		}
		*/
	
		//std::cout << "!!! Number of keys from this FTriangle: " << (*outputsRef).size() << std::endl;

		// the ForgedPolySetRegistry needs to be updated accordingly, for each pointed-to instance of ContouredTriangleV2.
		// This must be done for each EnclaveKey that the ContouredTriangleV2 touches, so it must be done per iteration in the loop that follows.
		for (auto& currentOutput : *outputsRef)
		{
			EnclaveKeyDef::EnclaveKey currentKeyCopy = currentOutput.first;	// this key reflects the current blueprint that each ECBPoly (constructed from FTriangleOutput)
																			// will need to be inserted into.


			//std::cout << "Triangles at key: ";
			//currentKeyCopy.printKey();
			//std::cout << std::endl;

			int numberOfTrianglesToAdd = currentOutput.second.fracturedTriangles.size();
			//std::cout << "Number of triangles in this container: " << numberOfTrianglesToAdd << std::endl;

			// get a pointer to the blueprint, so that we can get the appropriate IDs of the ECBPolys that we are about to insert;
			// also, insert the ECBPoly with the corresponding ID into the EnclaveCollectionBlueprint.
			auto currentBlueprintPtr = in_blueprintMapRef->getBlueprintRef(currentKeyCopy);
			auto idSet = currentBlueprintPtr->fetchNextIDs(numberOfTrianglesToAdd);
			for (auto& currentID : idSet)
			{
				// Regardless of whether or not the ECBPolyType is SHELL or SHELL_MASSDRIVER, it must always be inserted into the 
				// allForgedPolysRegistryRef
				currentCTV2->allForgedPolysRegistryRef->addToPolyset(currentKeyCopy, currentID);

				// ...but if currentCTV2PolyType is ECBPolyType::SHELL_MASSDRIVER, we must *ALSO* insert it appropriately, into shellMassDriverRegistryRef.
				// This will prevent us from having to re-scan all ECBPolys the ContourPlanV2Base-derived instance to find out which ones are SHELL_MASSDRIVER,
				// which would be done by the runMassDriverV2 function (virtually defined) of the ContourPlanV2Base-derived class.
				if (currentCTV2PolyType == ECBPolyType::SHELL_MASSDRIVER)
				{
					currentCTV2->shellMassDriverRegistryRef->addToPolyset(currentKeyCopy, currentID);
					//std::cout << "!!! Because the type is SHELL_MASSDRIVER, this poly will be sent to the shellMassDriverRegistryRef." << std::endl;
				}
			}

			// Below: option to output poly data.
			/*
			// optional: print out IDs of the idSet.
			for (auto& currentID : idSet)
			{
				std::cout << "Found ID that would be inserted: " << currentID << std::endl;
			}
			*/

			// correlate the triangle ID with each triangle we are reading, so that we have the key/value to insert into the EnclaveCollectionBlueprint.
			auto currentTriangleID = idSet.begin();
			for (auto& currentOutputTriangle : currentOutput.second.fracturedTriangles)
			{

				ECBPoly polyToInsert(currentOutputTriangle.second);	// the ECBPoly that is constructed from the FTriangleOutput.
				polyToInsert.polyType = currentCTV2PolyType;

				// Below: option to output poly data.
				/*
				std::cout << "Found triangle to convert." << std::endl;
				std::cout << "Checking ECBPoly data: " << std::endl;
				polyToInsert.printLineData();
				*/

				// insert the ECBPoly, with the value of currentTriangleID, into the blueprint.
				currentBlueprintPtr->insertPolyWithKeyValue(*currentTriangleID, polyToInsert);

				currentTriangleID++;
			}

			//std::cout << "Enter a number to go to the next key." << std::endl;
			//int doneKeyWait = 3;
			//std::cin >> doneKeyWait;

		}
	}
}

void ContouredPlanV2Base::constructEstimatedAffectedBlueprints(std::vector<ContouredTriangleV2*> in_ctv2Vector, HotBlueprints* in_trackedBlueprintsRef)
{
	for (auto& currentCTV2 : in_ctv2Vector)
	{
		auto producedFTriangle = currentCTV2->produceEquivalentFTriangle();	// generate the FTriangle that we will be producing results from.
		producedFTriangle.fracture();
		auto outputsRef = producedFTriangle.fetchOutputContainerRef();

		for (auto& currentOutput : *outputsRef)
		{
			EnclaveKeyDef::EnclaveKey currentKeyCopy = currentOutput.first;	// this is the key that we will insert into the referenced HotBlueprints member.
			in_trackedBlueprintsRef->insertKeyIntoPillar(currentKeyCopy);
		}

		// When all keys have been inserted, construct.
		in_trackedBlueprintsRef->produceKeysFromPillars();
	}
}

void ContouredPlanV2Base::copyOverForSPJ(std::vector<ContouredTriangleV2*> in_ctv2Vector, 
										ECBMap* in_blueprintMapRef, 
										ECBMap* in_backupBlueprintsMapRef,
										HotBlueprints* in_trackedBlueprintsRef)
{
	/*
	for (auto& currentCTV2ForPillar : in_ctv2Vector)
	{
		auto producedFTriangle = currentCTV2ForPillar->produceEquivalentFTriangle();	// generate the FTriangle that we will be producing results from.
		producedFTriangle.fracture();
		auto outputsRef = producedFTriangle.fetchOutputContainerRef();

		for (auto& currentOutput : *outputsRef)
		{
			EnclaveKeyDef::EnclaveKey currentKeyCopy = currentOutput.first;	// this is the key that we will insert into the referenced HotBlueprints member.
			in_trackedBlueprintsRef->insertKeyIntoPillar(currentKeyCopy);
		}

		// When all keys have been inserted, construct.
		in_trackedBlueprintsRef->produceKeysFromPillars();
	}
	*/
	// Step 1: Produce all estimated affected blueprints.

	auto estimatedStart = std::chrono::high_resolution_clock::now();
	constructEstimatedAffectedBlueprints(in_ctv2Vector, in_trackedBlueprintsRef);
	auto estimatedEnd = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> estimatedElapsed = estimatedEnd - estimatedStart;
	std::cout << "Time spent to consruct estimated affected blueprints (ms): " << estimatedElapsed.count() << std::endl;

	// Step 2: Created backups of the affected blueprints, if they exist.
	auto ecbPolyCopyStart = std::chrono::high_resolution_clock::now();

	for (auto& currentAffectedBlueprint : in_trackedBlueprintsRef->hotKeys)
	{
		// Only bother doing a backup if it currently exists
		if (in_backupBlueprintsMapRef->checkIfBlueprintExists(currentAffectedBlueprint))
		{
			in_backupBlueprintsMapRef->addBlueprintViaCopy(currentAffectedBlueprint, *in_blueprintMapRef->getBlueprintRef(currentAffectedBlueprint));
		}
	}


	for (auto& currentCTV2 : in_ctv2Vector)
	{
		auto producedFTriangle = currentCTV2->produceEquivalentFTriangle();	// generate the FTriangle that we will be producing results from.
		ECBPolyType currentCTV2PolyType = currentCTV2->ctv2PolyType;	// the ECBPolyType that must be used to set the poly type in each ECBPoly
																		// that this CTV2 creates.


		producedFTriangle.fracture();
		auto outputsRef = producedFTriangle.fetchOutputContainerRef();



		// Below switch: optional debug output
		/*
		switch (currentCTV2PolyType)
		{
			case ECBPolyType::ROGUE: { std::cout << "Why is this rogue???" << std::endl; break; }
			case ECBPolyType::FREE: {std::cout << "!!!! WARNING: poly is FREE " << std::endl; break;}
			case ECBPolyType::SHELL: {std::cout << "ECBPoly instances to insert will be of the type ECBPolyType::SHELL. " << std::endl; break;}
			case ECBPolyType::SHELL_MASSDRIVER: {std::cout << "ECBPoly instances to insert will be of the type ECBPolyType::SHELL_MASSDRIVER. " << std::endl; break;}
		}
		*/

		//std::cout << "!!! Number of keys from this FTriangle: " << (*outputsRef).size() << std::endl;

		// the ForgedPolySetRegistry needs to be updated accordingly, for each pointed-to instance of ContouredTriangleV2.
		// This must be done for each EnclaveKey that the ContouredTriangleV2 touches, so it must be done per iteration in the loop that follows.
		for (auto& currentOutput : *outputsRef)
		{
			EnclaveKeyDef::EnclaveKey currentKeyCopy = currentOutput.first;	// this key reflects the current blueprint that each ECBPoly (constructed from FTriangleOutput)
																			// will need to be inserted into.


			//std::cout << "Triangles at key: ";
			//currentKeyCopy.printKey();
			//std::cout << std::endl;

			int numberOfTrianglesToAdd = currentOutput.second.fracturedTriangles.size();
			//std::cout << "Number of triangles in this container: " << numberOfTrianglesToAdd << std::endl;

			// get a pointer to the blueprint, so that we can get the appropriate IDs of the ECBPolys that we are about to insert;
			// for each element in the idSet, we will use tat 
			auto currentBlueprintPtr = in_blueprintMapRef->getBlueprintRef(currentKeyCopy);
			auto idSet = currentBlueprintPtr->fetchNextIDs(numberOfTrianglesToAdd);
			for (auto& currentID : idSet)
			{
				// Regardless of whether or not the ECBPolyType is SHELL or SHELL_MASSDRIVER, it must always be inserted into the 
				// allForgedPolysRegistryRef
				currentCTV2->allForgedPolysRegistryRef->addToPolyset(currentKeyCopy, currentID);

				// ...but if currentCTV2PolyType is ECBPolyType::SHELL_MASSDRIVER, we must *ALSO* insert it appropriately, into shellMassDriverRegistryRef.
				// This will prevent us from having to re-scan all ECBPolys the ContourPlanV2Base-derived instance to find out which ones are SHELL_MASSDRIVER,
				// which would be done by the runMassDriverV2 function (virtually defined) of the ContourPlanV2Base-derived class.
				if (currentCTV2PolyType == ECBPolyType::SHELL_MASSDRIVER)
				{
					currentCTV2->shellMassDriverRegistryRef->addToPolyset(currentKeyCopy, currentID);
					//std::cout << "!!! Because the type is SHELL_MASSDRIVER, this poly will be sent to the shellMassDriverRegistryRef." << std::endl;
				}
			}

			// Below: option to output poly data.
			/*
			// optional: print out IDs of the idSet.
			for (auto& currentID : idSet)
			{
				std::cout << "Found ID that would be inserted: " << currentID << std::endl;
			}
			*/

			// correlate the triangle ID with each triangle we are reading, so that we have the key/value to insert into the EnclaveCollectionBlueprint.
			auto currentTriangleID = idSet.begin();
			for (auto& currentOutputTriangle : currentOutput.second.fracturedTriangles)
			{

				ECBPoly polyToInsert(currentOutputTriangle.second);	// the ECBPoly that is constructed from the FTriangleOutput.
				polyToInsert.polyType = currentCTV2PolyType;

				// Below: option to output poly data.
				/*
				std::cout << "Found triangle to convert." << std::endl;
				std::cout << "Checking ECBPoly data: " << std::endl;
				polyToInsert.printLineData();
				*/

				// insert the ECBPoly, with the value of currentTriangleID, into the blueprint.
				currentBlueprintPtr->insertPolyWithKeyValue(*currentTriangleID, polyToInsert);

				currentTriangleID++;
			}

			//std::cout << "Enter a number to go to the next key." << std::endl;
			//int doneKeyWait = 3;
			//std::cin >> doneKeyWait;

		}
	}

	auto ecbPolyCopyEnd = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> ecbPolyCopyElapsed = ecbPolyCopyEnd - ecbPolyCopyStart;
	std::cout << "Time spent to copy over affected blueprint ECBPolys (ms): " << ecbPolyCopyElapsed.count() << std::endl;
}