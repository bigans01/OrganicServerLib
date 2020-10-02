#include "stdafx.h"
#include "ServerJobBase.h"
#include "OSServer.h"

void ServerJobBase::setStartMessage(Message in_message)
{
	startMessage = in_message;
}

void ServerJobBase::setCompletionMessage(Message in_message)
{
	completionMessage = in_message;
}

void ServerJobBase::setServerJobState(ServerJobState in_serverJobState)
{
	currentJobState = in_serverJobState;
}

void ServerJobBase::setServerPtr(OSServer* in_serverPtr)
{
	server = in_serverPtr;
}

void ServerJobBase::appendMatchedThreadAndWorkLoadToMessage(int in_threadMonitorID)
{
	completionMessage.insertInt(in_threadMonitorID);
	completionMessage.insertFloat(estimatedWorkLoad);		// should be calculated by the call to the virtual function ServerJobBase::runPreChecks()
}