#pragma once

#ifndef SERVERTHREADDESIGNATIONMAP_H
#define SERVERTHREADDESIGNATIONMAP_H

#include <map>
#include "ServerThreadDesignation.h"
#include "OrganicStemcellManager.h"
#include "OrganicThread.h"
#include "ServerThreadWorkloadMonitor.h"
#include "AcquiredServerThread.h"

/*

Description: The main purpose of this class is to serve as an interface for the ServerJobManager class to acquire threads to run work on.
It is also responsible for acquiring metadata about dedicated threads that the server needs, such as the Command Line thread and the Terrain processing
thread.

This class assumes that the referenced OrganicStemcellManager has created the appropriate threads, before its initialize() function is called.

*/

class ServerThreadDesignationMap
{
	public:

		void initialize(OrganicStemcellManager* in_organicStemcellManagerRef);
		void buildInitialUndesignatedPool();
		bool doesDesignatedThreadExist(std::string in_designationString);
		OrganicThread* getCommandLineThread();
		AcquiredServerThread fetchDesignatedThread(std::string in_designationString);
		void incrementWorkload(int in_monitorID, float in_workload);
		void decrementWorkload(int in_monitorID, float in_workload);
	private:
		OrganicStemcellManager* organicStemcellManagerRef = NULL;
		std::map<ServerThreadDesignation, int> designations;
		std::map<int, ServerThreadWorkloadMonitor> unDesignatedPool;
		void removeFromUndesignatedPool(int in_key);
		void designateCommandLineThread();	// use the organicStemcellManagerRef to get the ID of the server command line thread from the OSC,
											// and put the value into designations map with the key ServerThreadDesignation::COMMAND_LINE.

		void designateTerrainThread();	// use the organicStemcellManagerRef to get the ID of the server terrain generating thread from the OSC,
										// and put the value into designations map with the key ServerThreadDesignation::TERRAIN.
};
#endif
