#include "stdafx.h"
#include "AdhesiveRunner.h"

void AdhesiveRunner::performAdhesion()
{
	std::cout << "++++++running adhesion for blueprint: " << originalBlueprintKey.x << ", " << originalBlueprintKey.y << ", " << originalBlueprintKey.z << std::endl;

	// load the adherent data map
	auto adherentListBegin = adherentDataListVector.begin();
	auto adherentListEnd = adherentDataListVector.end();
	for (; adherentListBegin != adherentListEnd; adherentListBegin++)
	{
		int keyValue = adherentListBegin->adherentOrder;

		// store/copy the directions we have to scan
		adherentDataMap[keyValue].adherentBlueprintKey = adherentListBegin->adherentBlueprintKey;
		adherentDataMap[keyValue].adherentDirection = adherentListBegin->adherentDirection;

		// create the directional lookup, insert the AdherentDataList for the corresponding lookup.
		directionLookup[adherentListBegin->adherentDirection] = *adherentListBegin;
	}

	// cycle through each element in the map, and use the adherentDirection to find the appropriate subset of rawEnclaves
	auto adherentDataMapBegin = adherentDataMap.begin();
	auto adherentDataMapEnd = adherentDataMap.end();
	for (; adherentDataMapBegin != adherentDataMapEnd; adherentDataMapBegin++)
	{
		scanForOrganicRawEnclavesViaDirection(adherentDataMapBegin->second.adherentDirection);
	}

	// once the adhesive data has been appropriately inserted into OrganicRawEnclaveAdhesives, cycle through each one and perform the point calibration.
	runPointAdhesions();
}

void AdhesiveRunner::scanForOrganicRawEnclavesViaDirection(EuclideanDirection3D in_euclideanDirection)
{
	// set up the iterators that point to the OrganicRawEnclaves in the original blueprint
	EnclaveFractureResultsMap* originalResults = &enclaveFractureResultsMapMapRef->find(originalBlueprintKey)->second;	// get the ref (could use auto here if we wanted but this is better for readability imo)

	if (enclaveFractureResultsMapMapRef->find(originalBlueprintKey) == (*enclaveFractureResultsMapMapRef).end())
	{
		std::cout << "+++++++++++++++++++++WARNING, original fracture results not found!! " << std::endl;
		int someVal = 3;
		std::cin >> someVal;
	}

	// print out the directions for testing


	auto originalResultsBegin = originalResults->fractureResultsContainerMap.begin();
	auto originalResultsEnd = originalResults->fractureResultsContainerMap.end();

	// scan for east
	if (in_euclideanDirection == EuclideanDirection3D::POS_X)
	{
		//std::cout << "Scanning pos X..." << std::endl;
		for (; originalResultsBegin != originalResultsEnd; originalResultsBegin++)
		{
			// to qualify as an adhesive to the blueprint border at +X, the X value of the OrganicRawEnclave's key must be 7.
			if (originalResultsBegin->first.x == 7)
			{
				insertAdhesiveData(originalResultsBegin->first, &originalResultsBegin->second, in_euclideanDirection);
			}
		}
	}

	// scan for north
	if (in_euclideanDirection == EuclideanDirection3D::POS_Z)
	{
		//std::cout << "Scanning pos Z..." << std::endl;
		for (; originalResultsBegin != originalResultsEnd; originalResultsBegin++)
		{
			// to qualify as an adhesive to the blueprint border at +Z, the Z value of the OrganicRawEnclave's key must be 7.
			if (originalResultsBegin->first.z == 7)
			{
				insertAdhesiveData(originalResultsBegin->first, &originalResultsBegin->second, in_euclideanDirection);
			}
		}
	}

	// scan for west
	if (in_euclideanDirection == EuclideanDirection3D::NEG_X)
	{
		//std::cout << "Scanning neg X..." << std::endl;
		for (; originalResultsBegin != originalResultsEnd; originalResultsBegin++)
		{
			// to qualify as an adhesive to the blueprint border at -X, the X value of the OrganicRawEnclave's key must be 0.
			if (originalResultsBegin->first.x == 0)
			{
				insertAdhesiveData(originalResultsBegin->first, &originalResultsBegin->second, in_euclideanDirection);
			}
		}
	}

	// scan south
	if (in_euclideanDirection == EuclideanDirection3D::NEG_Z)
	{
		//std::cout << "Scanning neg Z..." << std::endl;
		for (; originalResultsBegin != originalResultsEnd; originalResultsBegin++)
		{
			// to qualify as an adhesive to the blueprint border at -Z, the Z value of the OrganicRawEnclave's key must be 0.
			if (originalResultsBegin->first.z == 0)
			{
				insertAdhesiveData(originalResultsBegin->first, &originalResultsBegin->second, in_euclideanDirection);
			}
		}
	}

	// scan above
	if (in_euclideanDirection == EuclideanDirection3D::POS_Y)
	{
		//std::cout << "Scanning pos Y..." << std::endl;
		for (; originalResultsBegin != originalResultsEnd; originalResultsBegin++)
		{
			// to qualify as an adhesive to the blueprint border at +Y, the Y value of the OrganicRawEnclave's key must be 7.
			if (originalResultsBegin->first.y == 7)
			{
				//std::cout << "Adherent found: " << originalResultsBegin->first.x << ", " << originalResultsBegin->first.y << ", " << originalResultsBegin->first.z << std::endl;
				insertAdhesiveData(originalResultsBegin->first, &originalResultsBegin->second, in_euclideanDirection);
			}
		}
	}

	// scan below
	if (in_euclideanDirection == EuclideanDirection3D::NEG_Y)
	{
		//std::cout << "Scanning neg Y..." << std::endl;
		for (; originalResultsBegin != originalResultsEnd; originalResultsBegin++)
		{

			// to qualify as an adhesive to the blueprint border at -Y, the Y value of the OrganicRawEnclave's key must be 0.
			if (originalResultsBegin->first.y == 0)
			{
				insertAdhesiveData(originalResultsBegin->first, &originalResultsBegin->second, in_euclideanDirection);
			}
		}
	}
}

void AdhesiveRunner::insertAdhesiveData(EnclaveKeyDef::EnclaveKey in_adhesiveEnclaveKey, OrganicRawEnclave* in_organicRawEnclaveRef, EuclideanDirection3D in_euclideanDirection)
{
	auto searchForExisting = adhesiveRawEnclavesMap.find(in_adhesiveEnclaveKey);
	if (searchForExisting == adhesiveRawEnclavesMap.end())	// doesn't exist
	{
		OrganicRawEnclaveAdhesives newAdhesive(in_organicRawEnclaveRef, in_euclideanDirection);	// insert the new adhesive
		adhesiveRawEnclavesMap[in_adhesiveEnclaveKey] = newAdhesive;
	}

	else if (searchForExisting != adhesiveRawEnclavesMap.end()) // it does exist, so just update with the direction.
	{
		adhesiveRawEnclavesMap[in_adhesiveEnclaveKey].insertDirection(in_euclideanDirection);
	}
}

void AdhesiveRunner::runPointAdhesions()
{
	auto adhesiveRawEnclavesMapBegin = adhesiveRawEnclavesMap.begin();
	auto adhesiveRawEnclavesMapEnd = adhesiveRawEnclavesMap.end();
	for (; adhesiveRawEnclavesMapBegin != adhesiveRawEnclavesMapEnd; adhesiveRawEnclavesMapBegin++)
	{
		std::cout << "Performing point adhesion for OrganicRawEnclave with key: " << adhesiveRawEnclavesMapBegin->first.x << ", " << adhesiveRawEnclavesMapBegin->first.y << ", " << adhesiveRawEnclavesMapBegin->first.z << std::endl;

		// the vector where we will store the actual directions to compare against, once we've checked that an OrganicRawEnclave exists that we can check against.
		std::map<int, DiscoveredORELocation> validCheckableDirections;	// will automatically order these for us

		// get the directions for the current raw enclave adhesive; check to see if the OrganicRawEnclave to adhere to exists. If it does not, exclude it from the adhesion process.
		auto directionsBegin = adhesiveRawEnclavesMapBegin->second.adhesiveDirections.begin();
		auto directionsEnd = adhesiveRawEnclavesMapBegin->second.adhesiveDirections.end();
		for (; directionsBegin != directionsEnd; directionsBegin++)
		{
			// use the direction value to get the blueprint key to lookup against
			EnclaveKeyDef::EnclaveKey blueprintKeyToCompareTo = directionLookup[*directionsBegin].adherentBlueprintKey;								// fetch the EnclaveFractureResultsMap of the blueprint we're comparing against
			EnclaveFractureResultsMap* neighboringResultsMapRef = &enclaveFractureResultsMapMapRef->find(blueprintKeyToCompareTo)->second;
			EnclaveKeyDef::EnclaveKey neighboringOrganicRawEnclaveKey = findBorderingOrganicRawEnclaveToCompareAgainst(*directionsBegin, adhesiveRawEnclavesMapBegin->first);

			// ensure the neighboring OrganicRawEnclave exists; if it does, we'll put it into the new direction list. If it doesn't exist, don't bother comparing against it.
			std::string neighborRawEnclaveFound = "NOT FOUND";
			auto wasNeighboringRawEnclaveFound = neighboringResultsMapRef->fractureResultsContainerMap.find(neighboringOrganicRawEnclaveKey);
			if (wasNeighboringRawEnclaveFound != neighboringResultsMapRef->fractureResultsContainerMap.end())
			{
				neighborRawEnclaveFound = "FOUND";
				DiscoveredORELocation oreLocation;
				oreLocation.direction = *directionsBegin;													// store the euclidean direction		
				oreLocation.neighboringBlueprintKey = blueprintKeyToCompareTo;								// the key of the EnclaveFractureResultsMap to look in 
				oreLocation.keyInNeighboringBlueprint = neighboringOrganicRawEnclaveKey;					// the key of the OrganicRawEnclave in the neighboring EnclaveFractureResultsMap
				validCheckableDirections[directionLookup[*directionsBegin].adherentOrder] = oreLocation;	// the key value of this map is equal to the order of the direction we looked at. This is 																						
																											// done so that we perform the point adhesion in the appropriate order, since std::map orders automatically.
			}

			// cycle through the validCheckableDirections map, and use the value in each iteration for a function call to perform the point adhesion.
			auto validsBegin = validCheckableDirections.begin();
			auto validsEnd = validCheckableDirections.end();
			for (; validsBegin != validsEnd; validsBegin++)
			{

			}

			//std::cout << "Check for the neighboring OrganicRawEnclave at (" << neighboringOrganicRawEnclaveKey.x << ", " << neighboringOrganicRawEnclaveKey.y << ", " << neighboringOrganicRawEnclaveKey.z
			//	<< "), in blueprint (" << blueprintKeyToCompareTo.x << ", " << blueprintKeyToCompareTo.y << ", " << blueprintKeyToCompareTo.z << ") "
			//	<< " was " << neighborRawEnclaveFound << std::endl;
		}

		//std::cout << "++ Number of valids to compare against is: " << validCheckableDirections.size() << std::endl;
	}
}

EnclaveKeyDef::EnclaveKey AdhesiveRunner::findBorderingOrganicRawEnclaveToCompareAgainst(EuclideanDirection3D in_euclideanDirection, EnclaveKeyDef::EnclaveKey in_currentOrganicRawEnclaveKey)
{
	EnclaveKeyDef::EnclaveKey returnKey = in_currentOrganicRawEnclaveKey;	// copy the original key into this value, so we can calibrate it.
	if (in_euclideanDirection == EuclideanDirection3D::POS_X)
	{
		// the original raw enclave had a X value of 7, the one we compare to must have a X value of 0.
		returnKey.x = 0;
	}
	if (in_euclideanDirection == EuclideanDirection3D::POS_Z)
	{
		// the original raw enclave had a Z value of 7, the one we compare to must have a Z value of 0.
		returnKey.z = 0;
	}
	if (in_euclideanDirection == EuclideanDirection3D::NEG_X)
	{
		// the original raw enclave had a X value of 0, the one we compare to must have a X value of 7.
		returnKey.x = 7;
	}
	if (in_euclideanDirection == EuclideanDirection3D::NEG_Z)
	{
		// the original raw enclave had a Z value of 0, the one we compare to must have a Z value of 7.
		returnKey.z = 7;
	}
	if (in_euclideanDirection == EuclideanDirection3D::POS_Y)
	{
		// the original raw enclave had a Y value of 7, the one we compare to must have a Y value of 0.
		returnKey.y = 0;
	}
	if (in_euclideanDirection == EuclideanDirection3D::NEG_Y)
	{
		// the original raw enclave had a Y value of 0, the one we compare to must have a Y value of 7.
		returnKey.y = 7;
	}
	return returnKey;
}