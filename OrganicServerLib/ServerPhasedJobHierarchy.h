#pragma once

#ifndef SERVERPHASEDJOBHIERARCHY_H
#define SERVERPHASEDJOBHIERARCHY_H

#include <map>
#include <mutex>
#include "ServerPhasedJobBase.h"
#include "ServerJobContainerType.h"
#include "SmartIntMap.h"
#include "Message.h"

class OSServer;
class ServerPhasedJobHierarchy
{
	private:
		friend class ServerJobManager;	// required friend declaration, as the ServerJobManager is heavily integrated with this class.
		void setServerPtr(OSServer* in_server);	// used by ServerJobManager to set the OSServer ptr.
		void insertSPJ(int in_layerToInsertTo, std::shared_ptr<ServerPhasedJobBase>* in_jobRef, Message in_initialMessage);	

		void cleanupJobsInPhasedJobs(); 	// needs to be called every server tick; ultimately calls runPostCompleteTasks for completed SJs in a SPJ, before flagging those SJs for removal,
											// and erasing them. Additionally, ServerPhasedJobs with their last phase completted will have their jobState set to ServerJobState::COMPLETE;

		void removeCompletedPhasedJobs(); 	// needs to be called every server tick; inserts completed SPJs that have a jobState == ServerJobState::COMPLETE into the removalQueue


		void handleUpdateMessage(Message in_updateMessage); // called via ServerJobManager::checkForUpdateMessages(); 
															// sends an update Message to the appropriate SPJ for interpretation.
		std::vector<std::shared_ptr<ServerPhasedJobBase>> produceRunnableJobs();	// produces a list of runnable jobs for the ServerJobManager instance to iterate through 
																					// and attempt to run.
		int numberOfJobsInHierarchy();	// returns the total number of jobs, cumatively, from each layer of the hierarchy
	
		OSServer* server = nullptr;
		std::map<int, SmartIntMap<std::shared_ptr<ServerPhasedJobBase>>> hierarchy;		// where the ServerPhasedJobs are stored in their appropriate layer

		// the HierarchyLocation struct stores the location of SPJs to be removed.
		struct HierarchyLocation
		{
			HierarchyLocation() {};
			HierarchyLocation(int in_layerID, int in_layerMapID) :
				layerID(in_layerID),
				layerMapID(in_layerMapID)
			{};

			int layerID = 0;	// the layer in the hierarchy
			int layerMapID = 0; // the actual location within the SmartIntMap, in the layer of the hierarchy
		};
		std::queue<HierarchyLocation> removalQueue;	// queue for removing finished SPJs.

		void processRemovalQueue();		// needs to be called every server tick; iterates through all elements in the removal queue, to erase the actual SPJs marked for removal.
};

#endif
