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
		ServerThreadWorkloadMonitor newMonitor(stemcellsBegin->second.threadPtr.get());
		unDesignatedPool[stemcellsBegin->first] = newMonitor;
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

AcquiredServerThread ServerThreadDesignationMap::getFirstAvailableThread()		// for testing, name may change soon.
{
	auto firstUndesignated = unDesignatedPool.begin();
	std::cout << "Retrieved thread ID: " << firstUndesignated->first << std::endl;

	AcquiredServerThread acquisition(firstUndesignated->first, organicStemcellManagerRef->stemcellMap[firstUndesignated->first].threadPtr.get());
	return acquisition;

	//return organicStemcellManagerRef->stemcellMap[firstUndesignated->first].threadPtr.get();	// fetch the thread, return it
}

void ServerThreadDesignationMap::incrementWorkload(int in_monitorID, float in_workload)
{
	unDesignatedPool[in_monitorID].incrementWorkLoad(in_workload);
}
void ServerThreadDesignationMap::decrementWorkload(int in_monitorID, float in_workload)
{
	unDesignatedPool[in_monitorID].decrementWorkLoad(in_workload);
}