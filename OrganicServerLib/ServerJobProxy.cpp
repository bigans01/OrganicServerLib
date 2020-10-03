#include "stdafx.h"
#include "ServerJobProxy.h"
#include "OSServer.h"

void ServerJobProxy::callServerJobRunSingleMountTest(OSServer* in_serverRef)
{
	//in_serverRef->constructSingleMountTest();
	in_serverRef->constructSingleMountTestNoInput();
}

void ServerJobProxy::callServerJobRunBigMountTest(OSServer* in_serverRef)
{
	in_serverRef->constructBigMountTestNoInput();
}

void ServerJobProxy::callServerJobSendUpdateMessageToJobManager(OSServer* in_serverRef, Message in_message)
{
	std::cout << "!!! calling send update... " << std::endl;
	in_serverRef->serverJobManager.updateMessages.insertUpdate(in_message);
}

void ServerJobProxy::callServerJobSendRequestToSendOGLMCubeFromClient(OSServer* in_serverRef)
{
	in_serverRef->jobSendRequestToSendOGLMCubeFromClient();
}