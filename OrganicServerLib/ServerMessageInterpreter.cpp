#include "stdafx.h"
#include "ServerMessageInterpreter.h"

void ServerMessageInterpreter::initialize(OSServer* in_organicSystemServerRef, MessageCable* in_messageCableRef)
{
	serverPtr = in_organicSystemServerRef;
	messageCableRef = in_messageCableRef;
}

void ServerMessageInterpreter::interpretRequests()
{
	while (!messageCableRef->incomingMessages.empty())
	{
		Message currentMessage = messageCableRef->incomingMessages.front();
		switch (currentMessage.messageType)
		{
			case MessageType::REQUEST_BLUEPRINTS_FOR_OGLMBUFFERMANAGER :
			{
				std::cout << "Switched case found! " << std::endl;
				insertResponseToPending(currentMessage);			// insert into pending
				// do stuff here
				// ...
				// ...

				moveResponseToCompleted(currentMessage.messageID);	// indicate that its done (if we completed, of course)
				messageCableRef->incomingMessages.pop();
				break;
			}

			// other cases follow here...
		}
	}
}
