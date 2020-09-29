#include "stdafx.h"
#include "ServerThreadDesignationMap.h"

void ServerThreadDesignationMap::initialize(OrganicStemcellManager* in_organicStemcellManagerRef)
{
	organicStemcellManagerRef = in_organicStemcellManagerRef;
}

void ServerThreadDesignationMap::buildInitialUndesignatedPool()
{
	auto stemcellsBegin = organicStemcellManagerRef->stemcellMap.begin();
	auto stemcellsEnd = organicStemcellManagerRef->stemcellMap.end();
	for (; stemcellsBegin != stemcellsEnd; stemcellsBegin++)
	{
		unDesignatedPool[stemcellsBegin->first] = stemcellsBegin->first;
	}
}

void ServerThreadDesignationMap::removeFromUndesignatedPool(int in_key)
{
	unDesignatedPool.erase(in_key);
}

void ServerThreadDesignationMap::designateCommandLineThread(int in_key)
{
	designations[ServerThreadDesignation::COMMAND_LINE] = in_key;	// add the designation key for the command line thread.
	removeFromUndesignatedPool(in_key); // remove from the unDesignatedPool
}

OrganicThread* ServerThreadDesignationMap::getCommandLineThread()
{
	int keyToLookup = designations[ServerThreadDesignation::COMMAND_LINE];	// fetch the key that uses the command line
	return organicStemcellManagerRef->stemcellMap[keyToLookup].threadPtr.get();	// fetch the thread, return it
}