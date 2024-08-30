#include "stdafx.h"
#include "ServerThreadDesignationMap.h"

void ServerThreadDesignationMap::initialize(OrganicStemcellManager* in_organicStemcellManagerRef)
{
	organicStemcellManagerRef = in_organicStemcellManagerRef;
	designateCommandLineThread();
	designateTerrainThread();
}

void ServerThreadDesignationMap::buildInitialUndesignatedPool()
{
	auto stemcellsBegin = organicStemcellManagerRef->getPrimaryIndexerRef()->jobEngineThreadDataMap.begin();
	auto stemcellsEnd = organicStemcellManagerRef->getPrimaryIndexerRef()->jobEngineThreadDataMap.end();
	for (; stemcellsBegin != stemcellsEnd; stemcellsBegin++)
	{
		ServerThreadWorkloadMonitor newMonitor(stemcellsBegin->second->getTargetThreadPtr());
		unDesignatedPool[stemcellsBegin->first] = newMonitor;
	}
}

void ServerThreadDesignationMap::removeFromUndesignatedPool(int in_key)
{
	unDesignatedPool.erase(in_key);
}

void ServerThreadDesignationMap::designateCommandLineThread()
{
	designations[ServerThreadDesignation::COMMAND_LINE] = organicStemcellManagerRef->getServerCommandLineThreadID();	// add the designation key for the command line thread.
}

void ServerThreadDesignationMap::designateTerrainThread()
{
	designations[ServerThreadDesignation::TERRAIN] = organicStemcellManagerRef->getServerTerrainThreadID();
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
		AcquiredServerThread acquisition(organicStemcellManagerRef->getServerTerrainThreadID(), organicStemcellManagerRef->getServerTerrainThread());
		returnThread = acquisition;
	}
	else if (in_designationString == "TERRAIN")
	{
		AcquiredServerThread acquisition(organicStemcellManagerRef->getServerTerrainThreadID(), organicStemcellManagerRef->getServerTerrainThread());
		returnThread = acquisition;
	}
	return returnThread;
}

OrganicThread* ServerThreadDesignationMap::getCommandLineThread()
{
	return organicStemcellManagerRef->getServerCommandLineThread();
}

void ServerThreadDesignationMap::incrementWorkload(int in_monitorID, float in_workload)
{
	unDesignatedPool[in_monitorID].incrementWorkLoad(in_workload);
}
void ServerThreadDesignationMap::decrementWorkload(int in_monitorID, float in_workload)
{
	unDesignatedPool[in_monitorID].decrementWorkLoad(in_workload);
}