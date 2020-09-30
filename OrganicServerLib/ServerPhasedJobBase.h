#pragma once

#ifndef SERVERPHASEDJOBBASE_H
#define SERVERPHASEDJOBBASE_H

#include "Message.h"
#include "MessageLocality.h"

class OSServer;
class ServerPhasedJobBase
{
	public:

		virtual void initialize() = 0;	// initialization steps are always dependent on whether or not the message locality is LOCAL or REMOTE.
		virtual void initializeCurrentPhase() = 0;

		OSServer* server;
		MessageLocality locality = MessageLocality::LOCAL;		// default is LOCAL, but may be changed to REMOTE at a later time.

		void setServerPtrAndLocality(OSServer* in_serverPtr, MessageLocality in_locality);
};

#endif
