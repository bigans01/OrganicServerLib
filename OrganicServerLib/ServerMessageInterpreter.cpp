#include "stdafx.h"
#include "ServerMessageInterpreter.h"
#include "OSServer.h"

void ServerMessageInterpreter::initialize(OSServer* in_organicSystemServerRef, MessageCable* in_messageCableRef)
{
	serverPtr = in_organicSystemServerRef;
	messageCableRef = in_messageCableRef;
}

void ServerMessageInterpreter::interpretIncomingRequestsFromClient()	// for interpreting inbound requests from a client
{
	while (!messageCableRef->incomingMessages.empty())
	{
		Message currentMessage = messageCableRef->incomingMessages.front();
		currentMessage.open();					// open the message for reading (sets the iterators)
		switch (currentMessage.messageType)											
		{
			
			// ###################
			// PREVIOUS CALL/MESSAGE: Calling function from client (OrganicSystem): CoreMessageInterpreter::sendMessageRequestAllBlueprintsInOGLMRMC(EnclaveKeyDef::EnclaveKey in_OGLMCenterKey)
			//
			// uses a BlueprintScanningCuboid that is constructed based off the client's current center collection key, to scan for existing blueprints. The blueprints are then sent the client.
			//
			//
			//
			case MessageType::REQUEST_FROM_CLIENT_BLUEPRINTS_FOR_OGLMBUFFERMANAGER :
			{
				std::cout << "Switched case found! " << std::endl;
				insertResponseToPending(currentMessage);			// insert into pending
				// do stuff here
				// ...
				// ...

				// extract the data
				EnclaveKeyDef::EnclaveKey extractedKey = currentMessage.readEnclaveKey();
				int cuboidDimension = currentMessage.readInt();


				std::cout << "Message key is: " << extractedKey.x << ", " << extractedKey.y << ", " << extractedKey.z << std::endl;
				std::cout << "Message cuboid dimension is: " << cuboidDimension << std::endl;

				// form a blueprint key cuboid, check each value in the array, then send over the results for everything that was found to the client
				BlueprintScanningCuboid cuboid(cuboidDimension, extractedKey);
				int cuboidArraySize = cuboid.totalCuboidElements;
				for (int x = 0; x < cuboidArraySize; x++)
				{
					EnclaveKeyDef::EnclaveKey currentKey = cuboid.keyArray[x];
					auto blueprintFinder = serverPtr->blueprintMap.find(currentKey);
					if (blueprintFinder != serverPtr->blueprintMap.end())	// it was found, so it needs to be sent over.
					{
						std::cout << "Key to send was found: " << currentKey.x << ", " << currentKey.y << ", " << currentKey.z << std::endl;
						serverPtr->sendAndRenderBlueprintToLocalOS(currentKey);		// toggle on/off for testing as needed (9/24/2020)
					}
				}

				// now, tell the client to render everything in the list.
				//serverPtr->client.callMaterializeAllCollectionsInRenderListFromMessage();

				// version 2.0
				Message responseMessage(currentMessage.messageID, MessageType::RESPONSE_FROM_SERVER_PROCESS_BLUEPRINT_WHEN_RECEIVED);
				serverPtr->client.insertResponseMessage(responseMessage);

				moveResponseToCompleted(currentMessage.messageID);	// indicate that its done (if we completed, of course)
				messageCableRef->incomingMessages.pop();
				break;
			}

			// other cases follow here...
		}
	}
}

void ServerMessageInterpreter::interpretIncomingResponsesFromClient()
{

}