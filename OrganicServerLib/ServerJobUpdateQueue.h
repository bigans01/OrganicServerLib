#pragma once

#ifndef SERVERJOBUPDATEQUEUE_H
#define SERVERJOBUPDATEQUEUE_H

#include "ServerJobUpdateInt.h"
#include <queue>
#include <mutex>
#include "Message.h"

class ServerJobUpdateQueue
{
		std::queue<Message> updateQueue;
		std::mutex queueGuard;
public:
		void insertUpdate(Message in_message)
		{
			std::lock_guard<std::mutex> lock(queueGuard);	// protection 
			updateQueue.push(in_message);
		}

		void safePopQueue()
		{
			std::lock_guard<std::mutex> lock(queueGuard);
			updateQueue.pop();
		}
		bool isEmpty()
		{
			std::lock_guard<std::mutex> lock(queueGuard);
			return updateQueue.empty();
		}

		Message* getMessageRefFromFront()
		{
			std::lock_guard<std::mutex> lock(queueGuard);
			return &updateQueue.front();
		}


};

#endif
