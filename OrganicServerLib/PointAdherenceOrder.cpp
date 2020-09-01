#include "stdafx.h"
#include "PointAdherenceOrder.h"

void PointAdherenceOrder::attemptAdherentInsertion(EnclaveKeyDef::EnclaveKey in_blueprintKey)
{
	auto findKeyResult = adherenceRelationships.find(in_blueprintKey);
	if (findKeyResult == adherenceRelationships.end())	// it doesn't exist
	{
		adherenceRelationships[in_blueprintKey] = currentAdherentID++;	// register the blueprint key, increment the adherent value
		adherenceOrder.push_back(in_blueprintKey);		// push back into the EnclaveKeyDef vector														
	}
}

void PointAdherenceOrder::printAdherentData()
{
	std::cout << "## Relationships " << std::endl;
	auto relationshipsBegin = adherenceRelationships.begin();
	auto relationshipsEnd = adherenceRelationships.end();
	for (; relationshipsBegin != relationshipsEnd; relationshipsBegin++)
	{
		std::cout << "Key: (" << relationshipsBegin->first.x << ", " << relationshipsBegin->first.y << ", " << relationshipsBegin->first.z << ") -> " << relationshipsBegin->second << std::endl;
	}

	std::cout << "## Order: " << std::endl;
	auto orderBegin = adherenceOrder.begin();
	auto orderEnd = adherenceOrder.end();
	for (; orderBegin != orderEnd; orderBegin++)
	{
		std::cout << "Order: (" << orderBegin->x << ", " << orderBegin->y << ", " << orderBegin->z << ") " << std::endl;
	}
}