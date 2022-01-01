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
	public:
		OSServer* server;
		void insertJob(int in_layerToInsertTo, std::shared_ptr<ServerPhasedJobBase>* in_jobRef, Message in_initialMessage)	// SPJ-F1
		{
			std::cout << "!! Insert job into int container, at index: " << in_layerToInsertTo << std::endl;
			int targetLayerJobID = hierarchy[in_layerToInsertTo].getNextAvailableKey();
			(hierarchy[in_layerToInsertTo])[targetLayerJobID] = *in_jobRef;
			(hierarchy[in_layerToInsertTo])[targetLayerJobID]->initializeBaseClass(server, in_initialMessage.messageLocality, ServerJobContainerType::INT);
			(hierarchy[in_layerToInsertTo])[targetLayerJobID]->setLocation(in_layerToInsertTo, targetLayerJobID);	// ensure that the SPJ knows where it's at (needed for completion messages)
			(hierarchy[in_layerToInsertTo])[targetLayerJobID]->initialize();
		}

		void cleanupJobsInPhasedJobs()	// SPJ-F2
		{
			auto currentLayerBegin = hierarchy.begin();
			auto currentLayerEnd = hierarchy.end();
			for (; currentLayerBegin != currentLayerEnd; currentLayerBegin++)
			{
				auto currentPhasedJobBegin = currentLayerBegin->second.begin();
				auto currentPhasedJobEnd = currentLayerBegin->second.end();
				for (; currentPhasedJobBegin != currentPhasedJobEnd; currentPhasedJobBegin++)
				{
					currentPhasedJobBegin->second->removeCompletedPhaseSubJobs();
				}
			}
		}

		void removeCompletedPhasedJobs()	// SPJ-F3
		{
			auto currentLayerBegin = hierarchy.begin();
			auto currentLayerEnd = hierarchy.end();
			for (; currentLayerBegin != currentLayerEnd; currentLayerBegin++)
			{
				auto currentPhasedJobBegin = currentLayerBegin->second.begin();
				auto currentPhasedJobEnd = currentLayerBegin->second.end();
				for (; currentPhasedJobBegin != currentPhasedJobEnd; currentPhasedJobBegin++)
				{
					if (currentPhasedJobBegin->second->jobState == ServerJobState::COMPLETE)
					{
						std::cout << "!! Job to remove found! " << std::endl; 
						HierarchyLocation newLocation(currentLayerBegin->first, currentPhasedJobBegin->first);
						removalQueue.push(newLocation);
					}
				}
			}
			processRemovalQueue();
		}

		void handleUpdateMessage(Message in_updateMessage)	// SPJ-F4
		{
			in_updateMessage.open();
			int messageLayerID = in_updateMessage.readInt();
			int messageLayerUniqueID = in_updateMessage.readInt();
			(hierarchy[messageLayerID])[messageLayerUniqueID]->interpretMessage(std::move(in_updateMessage));
		}


		std::vector<std::shared_ptr<ServerPhasedJobBase>> produceRunnableJobs()	// SPJ-F5
		{
			std::vector<std::shared_ptr<ServerPhasedJobBase>> returnJobs;
			auto currentLayerBegin = hierarchy.begin();
			auto currentLayerEnd = hierarchy.end();
			for (; currentLayerBegin != currentLayerEnd; currentLayerBegin++)
			{
				auto currentPhasedJobBegin = currentLayerBegin->second.begin();
				auto currentPhasedJobEnd = currentLayerBegin->second.end();
				for (; currentPhasedJobBegin != currentPhasedJobEnd; currentPhasedJobBegin++)
				{
					returnJobs.push_back(currentPhasedJobBegin->second);
				}
			}
			return returnJobs;
		}

		int numberOfJobsInHierarchy()	// SPJ-F6
		{
			int jobCount = 0;
			auto currentLayerBegin = hierarchy.begin();
			auto currentLayerEnd = hierarchy.end();
			for (; currentLayerBegin != currentLayerEnd; currentLayerBegin++)
			{
				auto currentPhasedJobBegin = currentLayerBegin->second.begin();
				auto currentPhasedJobEnd = currentLayerBegin->second.end();
				for (; currentPhasedJobBegin != currentPhasedJobEnd; currentPhasedJobBegin++)
				{
					jobCount++;
				}
			}
			return jobCount;
		}

	private:
		std::map<int, SmartIntMap<std::shared_ptr<ServerPhasedJobBase>>> hierarchy;
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
		std::queue<HierarchyLocation> removalQueue;

		void processRemovalQueue()
		{
			while (!removalQueue.empty())
			{
				auto currentLocation = removalQueue.front();
				hierarchy[currentLocation.layerID].erase(currentLocation.layerMapID);
				std::cout << "Removing job at layer " << currentLocation.layerID << ", unique ID " << currentLocation.layerMapID << std::endl;
				removalQueue.pop();
			}
		}
};

#endif
