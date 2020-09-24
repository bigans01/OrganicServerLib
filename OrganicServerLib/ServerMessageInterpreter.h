#pragma once

#ifndef SERVERMESSAGEINTERPRETER_H
#define SERVERMESSAGEINTERPRETER_H

#include "MessageCable.h"
#include "MessageInterpreter.h"
#include <iostream>

class OSServer;	
class ServerMessageInterpreter : public MessageInterpreter
{
	public:
		OSServer* serverPtr = NULL;
		MessageCable* messageCableRef = NULL;
		void initialize(OSServer* in_organicSystemServerRef, MessageCable* in_messageCableRef);
		void interpretRequests();
};

#endif
