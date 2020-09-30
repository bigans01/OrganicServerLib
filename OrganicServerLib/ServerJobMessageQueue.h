#pragma once

#ifndef SERVERJOBMESSAGEQUEUE_H
#define SERVERJOBMESSAGEQUEUE_H

#include "Message.h"
#include <queue>
#include <mutex>

class ServerJobMessageQueue
{
	std::queue<Message> messageQueue;
	std::mutex queueGuard;
public:
	void insertMessage(Message in_message)
	{
		std::lock_guard<std::mutex> lock(queueGuard);
		messageQueue.push(in_message);
	}
	void safePopQueue()
	{
		std::lock_guard<std::mutex> lock(queueGuard);
		messageQueue.pop();
	}
	bool isEmpty()
	{
		std::lock_guard<std::mutex> lock(queueGuard);
		return messageQueue.empty();
	}
	Message* getMessageRefFromFront()
	{
		return &messageQueue.front();
	}
};

#endif
