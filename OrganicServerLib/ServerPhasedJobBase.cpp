#include "stdafx.h"
#include "ServerPhasedJobBase.h"
#include "OSServer.h"

void ServerPhasedJobBase::setServerPtrAndLocality(OSServer* in_serverPtr, MessageLocality in_locality)
{
	server = in_serverPtr;
	locality = in_locality;
};