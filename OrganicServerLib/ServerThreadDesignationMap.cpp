#include "stdafx.h"
#include "ServerThreadDesignationMap.h"

void ServerThreadDesignationMap::initialize(OrganicStemcellManager* in_organicStemcellManagerRef)
{
	organicStemcellManagerRef = in_organicStemcellManagerRef;
}

void ServerThreadDesignationMap::buildInitialUndesignatedPool()
{
	auto stemcellsBegin = organicStemcellManagerRef->freeCellMap.begin();
	auto stemcellsEnd = organicStemcellManagerRef->freeCellMap.end();
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

void ServerThreadDesignationMap::designateTerrainThread(int in_key)
{
	designations[ServerThreadDesignation::TERRAIN] = in_key;
	removeFromUndesignatedPool(in_key);
}

bool ServerThreadDesignationMap::doesDesignatedThreadExist(std::string in_designationString)
{
	if (in_designationString == "ANY")
	{
		return true;
	}
	else
	{
		if (in_designationString == "TERRAIN")
		{
			auto terrainFinder = designations.find(ServerThreadDesignation::TERRAIN);
			if (terrainFinder != designations.end())	// it was found.
			{
				return true;
			}
			else   // designated thread not found, return false.
			{
				return false;
			}
		}
		
		if (in_designationString == "COMMAND_LINE")
		{
			auto commandLineFinder = designations.find(ServerThreadDesignation::COMMAND_LINE);
			if (commandLineFinder != designations.end())
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
}

AcquiredServerThread ServerThreadDesignationMap::fetchDesignatedThread(std::string in_designationString)
{
	AcquiredServerThread returnThread;
	if (in_designationString == "ANY")
	{
		auto terrainFinder = designations.find(ServerThreadDesignation::TERRAIN);
		AcquiredServerThread acquisition(terrainFinder->second, organicStemcellManagerRef->freeCellMap[terrainFinder->second].threadPtr.get());
		returnThread = acquisition;
	}
	else if (in_designationString == "TERRAIN")
	{
		auto terrainFinder = designations.find(ServerThreadDesignation::TERRAIN);
		AcquiredServerThread acquisition(terrainFinder->second, organicStemcellManagerRef->freeCellMap[terrainFinder->second].threadPtr.get());
		returnThread = acquisition;
	}
	return returnThread;
}

OrganicThread* ServerThreadDesignationMap::getCommandLineThread()
{
	int keyToLookup = designations[ServerThreadDesignation::COMMAND_LINE];	// fetch the key that uses the command line
	return organicStemcellManagerRef->freeCellMap[keyToLookup].threadPtr.get();	// fetch the thread, return it
}

AcquiredServerThread ServerThreadDesignationMap::getFirstAvailableThread()		// for testing, name may change soon.
{
	auto firstUndesignated = unDesignatedPool.begin();
	std::cout << "Retrieved thread ID: " << firstUndesignated->first << std::endl;

	AcquiredServerThread acquisition(firstUndesignated->first, organicStemcellManagerRef->freeCellMap[firstUndesignated->first].threadPtr.get());
	return acquisition;

	//return organicStemcellManagerRef->freeCellMap[firstUndesignated->first].threadPtr.get();	// fetch the thread, return it
}

void ServerThreadDesignationMap::incrementWorkload(int in_monitorID, float in_workload)
{
	unDesignatedPool[in_monitorID].incrementWorkLoad(in_workload);
}
void ServerThreadDesignationMap::decrementWorkload(int in_monitorID, float in_workload)
{
	unDesignatedPool[in_monitorID].decrementWorkLoad(in_workload);
}