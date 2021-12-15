#pragma once

#ifndef SERVERTHREADDESIGNATIONMAP_H
#define SERVERTHREADDESIGNATIONMAP_H

#include <map>
#include "ServerThreadDesignation.h"
#include "OrganicStemcellManager.h"
#include "OrganicThread.h"
#include "ServerThreadWorkloadMonitor.h"
#include "AcquiredServerThread.h"

class ServerThreadDesignationMap
{
	public:
		OrganicStemcellManager* organicStemcellManagerRef = NULL;
		std::map<ServerThreadDesignation, int> designations;
		//std::map<int, int> unDesignatedPool;
		std::map<int, ServerThreadWorkloadMonitor> unDesignatedPool;

		void initialize(OrganicStemcellManager* in_organicStemcellManagerRef);
		void buildInitialUndesignatedPool();
		void removeFromUndesignatedPool(int in_key);
		void designateCommandLineThread(int in_key);
		void designateTerrainThread(int in_key);
		bool doesDesignatedThreadExist(std::string in_designationString);
		OrganicThread* getCommandLineThread();
		//OrganicThread* getFirstAvailableThread();
		AcquiredServerThread getFirstAvailableThread();
		AcquiredServerThread fetchDesignatedThread(std::string in_designationString);
		void incrementWorkload(int in_monitorID, float in_workload);
		void decrementWorkload(int in_monitorID, float in_workload);
		//void resetWorkload(int in_monitorID);
};

#endif
