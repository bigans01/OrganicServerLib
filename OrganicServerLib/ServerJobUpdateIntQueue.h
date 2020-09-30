#pragma once

#ifndef SERVERJOBUPDATEINTQUEUE_H
#define SERVERJOBUPDATEINTQUEUE_H

#include "ServerJobUpdateInt.h"
#include <queue>
#include <mutex>

class ServerJobUpdateIntQueue
{
		std::queue<ServerJobUpdateInt> updateQueue;
		std::mutex queueGuard;
public:
		void insertUpdate(ServerJobUpdateInt in_updateByInt)
		{
			std::lock_guard<std::mutex> lock(queueGuard);	// protection 
			updateQueue.push(in_updateByInt);
		};
};

#endif
