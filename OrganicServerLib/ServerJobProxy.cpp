#include "stdafx.h"
#include "ServerJobProxy.h"
#include "OSServer.h"

void ServerJobProxy::callServerJobRunSingleMountTest(OSServer* in_serverRef)
{
	//in_serverRef->constructSingleMountTest();
	in_serverRef->constructSingleMountTestNoInput();
}