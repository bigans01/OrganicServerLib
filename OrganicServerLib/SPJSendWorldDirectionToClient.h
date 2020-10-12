#pragma once

#ifndef SPJSENDWORLDDIRECTIONTOCLIENT_H
#define SPJSENDWORLDDIRECTIONTOCLIENT_H

#include "ServerPhasedJobBase.h"
#include "SJBuildAndSendWorldDirection.h"

class SPJSendWorldDirectionToClient : public ServerPhasedJobBase
{
public:
	void interpretMessage(Message in_message);
	void initialize();	// initialization steps are always dependent on whether or not the message locality is LOCAL or REMOTE.
	void initializeCurrentPhase();
private:
};

#endif
