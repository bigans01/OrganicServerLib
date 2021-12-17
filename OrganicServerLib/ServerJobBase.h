#pragma once

#ifndef SERVERJOBBASE_H
#define SERVERJOBBASE_H

#include "Message.h"
#include "OrganicThread.h"
#include "ServerJobState.h"
#include "ServerJobProxy.h"
#include "ServerJobRunVerdict.h"

class OSServer;
class OrganicThread;
class ServerJobBase
{
	public:
		virtual void runJob(OrganicThread* in_threadToRunOn) = 0;
		virtual std::string getJobName() = 0;
		virtual void runPostCompleteTasks() = 0;
		virtual ServerJobRunVerdict getCurrentVerdict() = 0;

		void setStartMessage(Message in_message);
		void setCompletionMessage(Message in_message);
		void setServerJobState(ServerJobState in_serverJobState);
		void setServerPtr(OSServer* in_serverPtr);
		void appendMatchedThreadAndWorkLoadToMessage(int in_threadMonitorID);

		OSServer* server = NULL;
		float estimatedWorkLoad = 0.0f;		// the estimated work load produced by the runPrechecks call
		Message startMessage;
		Message completionMessage;		// the message that must always be submitted as an additional packaged_task, once the main packaged_task called from runJob() is complete.
		ServerJobState currentJobState = ServerJobState::WAITING_TO_EXECUTE;		// always initialized with waiting to execute
};

#endif
