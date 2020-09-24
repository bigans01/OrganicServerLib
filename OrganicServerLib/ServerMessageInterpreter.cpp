#include "stdafx.h"
#include "ServerMessageInterpreter.h"
#include "OSServer.h"

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
			
			// uses a BlueprintScanningCuboid that is constructed based off the client's current center collection key, to scan for existing blueprints. The blueprints are then sent the client.
			//
			//
			//
			case MessageType::REQUEST_BLUEPRINTS_FOR_OGLMBUFFERMANAGER :
			{
				std::cout << "Switched case found! " << std::endl;
				insertResponseToPending(currentMessage);			// insert into pending
				// do stuff here
				// ...
				// ...

				// extract the data
				auto intVectorBegin = currentMessage.intVector.begin();
				EnclaveKeyDef::EnclaveKey extractedKey;				// we will put the extracted key from the message into this.
				extractedKey.x = *intVectorBegin;
				intVectorBegin++;
				extractedKey.y = *intVectorBegin;
				intVectorBegin++;
				extractedKey.z = *intVectorBegin;
				intVectorBegin++;

				// get the cuboid dimension
				int cuboidDimension = *intVectorBegin;	// get the dimension of the cuboid

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
				serverPtr->client.callMaterializeAllCollectionsInRenderListFromMessage();

				moveResponseToCompleted(currentMessage.messageID);	// indicate that its done (if we completed, of course)
				messageCableRef->incomingMessages.pop();
				break;
			}

			// other cases follow here...
		}
	}
}
