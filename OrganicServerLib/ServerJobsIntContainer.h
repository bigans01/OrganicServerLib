#pragma once

#ifndef SERVERJOBSINTCONTAINER_H
#define SERVERJOBSINTCONTAINER_H

#include <map>
#include <mutex>
#include "ServerPhasedJobBase.h"

class OSServer;
class ServerJobsIntContainer
{
	public:
		//void initialize();
		OSServer* server;
		std::map<int, std::shared_ptr<ServerPhasedJobBase>> serverJobs;
		int currentIndex = 0;
		void insertJob(std::shared_ptr<ServerPhasedJobBase>* in_jobRef, Message in_initialMessage)
		{
			serverJobs[currentIndex] = *in_jobRef;
			serverJobs[currentIndex]->setServerPtrAndLocality(server, in_initialMessage.messageLocality);
			serverJobs[currentIndex]->initialize();
		}
		
};

#endif
