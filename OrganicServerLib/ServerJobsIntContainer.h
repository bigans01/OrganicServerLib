#pragma once

#ifndef SERVERJOBSINTCONTAINER_H
#define SERVERJOBSINTCONTAINER_H

#include <map>
#include <mutex>
#include "ServerPhasedJobBase.h"
#include "ServerJobContainerType.h"

class OSServer;
class ServerJobsIntContainer
{
	public:
		//void initialize();
		OSServer* server;
		const ServerJobContainerType containerType = ServerJobContainerType::INT;		// will never change
		std::map<int, std::shared_ptr<ServerPhasedJobBase>> serverJobs;
		std::queue<int> removalQueue;
		int currentIndex = 0;
		void insertJob(std::shared_ptr<ServerPhasedJobBase>* in_jobRef, Message in_initialMessage)
		{
			std::cout << "!! Insert job into int container, at index: " << currentIndex << std::endl;
			serverJobs[currentIndex] = *in_jobRef;
			serverJobs[currentIndex]->initializeBaseClass(server, in_initialMessage.messageLocality, containerType);
			serverJobs[currentIndex]->insertParentContainerIntKey(currentIndex);
			serverJobs[currentIndex]->initialize();
		}

		void cleanupJobsInPhasedJobs()
		{
			auto serverJobsBegin = serverJobs.begin();
			auto serverJobsEnd = serverJobs.end();
			for (; serverJobsBegin != serverJobsEnd; serverJobsBegin++)
			{
				serverJobsBegin->second->removeCompletedPhaseSubJobs();
			}
		}

		void removeCompletedPhasedJobs()
		{
			auto serverJobsBegin = serverJobs.begin();
			auto serverJobsEnd = serverJobs.end();
			for (; serverJobsBegin != serverJobsEnd; serverJobsBegin++)
			{
				if (serverJobsBegin->second->jobState == ServerJobState::COMPLETE)
				{
					std::cout << "!! Job to remove found! " << std::endl;
					removalQueue.push(serverJobsBegin->first);
				}
			}
			processRemovalQueue();
		}

		void processRemovalQueue()
		{
			while (!removalQueue.empty())
			{
				int currentInt = removalQueue.front();
				serverJobs.erase(currentInt);

				std::cout << "Server jobs map size is now: " << serverJobs.size() << std::endl;

				removalQueue.pop();
			}
		}
		
};

#endif
