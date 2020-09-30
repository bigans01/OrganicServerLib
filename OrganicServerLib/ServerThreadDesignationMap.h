#pragma once

#ifndef SERVERTHREADDESIGNATIONMAP_H
#define SERVERTHREADDESIGNATIONMAP_H

#include <map>
#include "ServerThreadDesignation.h"
#include "OrganicStemcellManager.h"
#include "OrganicThread.h"

class ServerThreadDesignationMap
{
	public:
		OrganicStemcellManager* organicStemcellManagerRef = NULL;
		std::map<ServerThreadDesignation, int> designations;
		std::map<int, int> unDesignatedPool;

		void initialize(OrganicStemcellManager* in_organicStemcellManagerRef);
		void buildInitialUndesignatedPool();
		void removeFromUndesignatedPool(int in_key);
		void designateCommandLineThread(int in_key);
		OrganicThread* getCommandLineThread();
		OrganicThread* getFirstAvailableThread();
};

#endif
