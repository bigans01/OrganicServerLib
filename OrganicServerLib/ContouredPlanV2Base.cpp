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
		switch (currentCTV2PolyType)
		{
			case ECBPolyType::ROGUE: { std::cout << "Why is this rogue???" << std::endl; break; }
			case ECBPolyType::FREE: {std::cout << "!!!! WARNING: poly is FREE " << std::endl; break;}
			case ECBPolyType::SHELL: {std::cout << "ECBPoly instances to insert will be of the type ECBPolyType::SHELL. " << std::endl; break;}
			case ECBPolyType::SHELL_MASSDRIVER: {std::cout << "ECBPoly instances to insert will be of the type ECBPolyType::SHELL_MASSDRIVER. " << std::endl; break;}
		}
	
		std::cout << "!!! Number of keys from this FTriangle: " << (*outputsRef).size() << std::endl;

		// the ForgedPolySetRegistry needs to be updated accordingly, for each pointed-to instance of ContouredTriangleV2.
		// This must be done for each EnclaveKey that the ContouredTriangleV2 touches, so it must be done per iteration in the loop that follows.
		for (auto& currentOutput : *outputsRef)
		{
			EnclaveKeyDef::EnclaveKey currentKeyCopy = currentOutput.first;	// this key reflects the current blueprint that each ECBPoly (constructed from FTriangleOutput)
																			// will need to be inserted into.


			std::cout << "Triangles at key: ";
			currentKeyCopy.printKey();
			std::cout << std::endl;

			int numberOfTrianglesToAdd = currentOutput.second.fracturedTriangles.size();
			std::cout << "Number of triangles in this container: " << numberOfTrianglesToAdd << std::endl;

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
				// This will prevent us from having to re-scan all ECBPolys the ContourPlanV2Base-derived instance to find out which ones are SHELL_MASSDRIVER.
				if (currentCTV2PolyType == ECBPolyType::SHELL_MASSDRIVER)
				{
					currentCTV2->shellMassDriverRegistryRef->addToPolyset(currentKeyCopy, currentID);
					std::cout << "!!! Because the type is SHELL_MASSDRIVER, this poly will be sent to the shellMassDriverRegistryRef." << std::endl;
				}
			}

			// optional: print out IDs of the idSet.
			for (auto& currentID : idSet)
			{
				std::cout << "Found ID that would be inserted: " << currentID << std::endl;
			}

			// correlate the triangle ID with each triangle we are reading, so that we have the key/value to insert into the EnclaveCollectionBlueprint.
			auto currentTriangleID = idSet.begin();
			for (auto& currentOutputTriangle : currentOutput.second.fracturedTriangles)
			{
				std::cout << "Found triangle to convert." << std::endl;

				ECBPoly polyToInsert(currentOutputTriangle.second);	// the ECBPoly that is constructed from the FTriangleOutput.
				polyToInsert.polyType = currentCTV2PolyType;
				std::cout << "Checking ECBPoly data: " << std::endl;
				polyToInsert.printLineData();

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