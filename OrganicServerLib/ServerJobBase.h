#pragma once

#ifndef SERVERJOBBASE_H
#define SERVERJOBBASE_H

#include "Message.h"
#include "OrganicThread.h"
#include "ServerJobState.h"
#include "ServerJobProxy.h"

class OSServer;
class OrganicThread;
class ServerJobBase
{
	public:
		virtual void runPrechecks() = 0;
		virtual void runJob(OrganicThread* in_threadToRunOn) = 0;
		virtual std::string getJobName() = 0;
		virtual void runPostCompleteTasks() = 0;

		void setStartMessage(Message in_message);
		void setCompletionMessage(Message in_message);
		void setServerJobState(ServerJobState in_serverJobState);
		void setServerPtr(OSServer* in_serverPtr);

		OSServer* server = NULL;
		Message startMessage;
		Message completionMessage;		// the message that must always be submitted as an additional packaged_task, once the main packaged_task called from runJob() is complete.
		ServerJobState currentJobState = ServerJobState::WAITING_TO_EXECUTE;		// always initialized with waiting to execute
};

#endif
