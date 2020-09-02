#include "stdafx.h"
#include "AdhesiveRunner.h"

void AdhesiveRunner::performAdhesion()
{
	// load the adherent data map
	auto adherentListBegin = adherentDataListVector.begin();
	auto adherentListEnd = adherentDataListVector.end();
	for (; adherentListBegin != adherentListEnd; adherentListBegin++)
	{
		int keyValue = adherentListBegin->adherentOrder;
		adherentDataMap[keyValue].adherentBlueprintKey = adherentListBegin->adherentBlueprintKey;
		adherentDataMap[keyValue].adherentDirection = adherentListBegin->adherentDirection;
	}

	// cycle through each element in the map, and use the adherentDirection to find the appropriate subset of rawEnclaves
	auto adherentDataMapBegin = adherentDataMap.begin();
	auto adherentDataMapEnd = adherentDataMap.end();
	for (; adherentDataMapBegin != adherentDataMapEnd; adherentDataMapBegin++)
	{
		scanForOrganicRawEnclavesViaDirection(adherentDataMapBegin->second.adherentDirection);
	}
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


	auto originalResultsBegin = originalResults->fractureResultsContainerMap.begin();
	auto originalResultsEnd = originalResults->fractureResultsContainerMap.end();

	// scan for east
	if (in_euclideanDirection == EuclideanDirection3D::POS_X)
	{
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
		for (; originalResultsBegin != originalResultsEnd; originalResultsBegin++)
		{
			// to qualify as an adhesive to the blueprint border at +Y, the Y value of the OrganicRawEnclave's key must be 7.
			if (originalResultsBegin->first.y == 7)
			{
				insertAdhesiveData(originalResultsBegin->first, &originalResultsBegin->second, in_euclideanDirection);
			}
		}
	}

	// scan below
	if (in_euclideanDirection == EuclideanDirection3D::NEG_Y)
	{
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