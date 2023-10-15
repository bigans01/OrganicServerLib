#pragma once

#ifndef SPJBUILDCPV2MOUNTAIN_H
#define SPJBUILDCPV2MOUNTAIN_H

/*

Summary: This SPJ is responsible for creating a contoured mass, server side, in the CPV2 style. It should
execute one CPV2 plan when it is called, not multiple.

As of 10/14/2023, the basic mechanics of this SPJ seem to work fine, but this implementation is not final
(need to work on the proper locking/unlocking of the ServerJobBlockingFlags::SERVER_RUNNING_CONTOUR_PLAN and 
ServerJobBlockingFlags::HALT_FUTURE_COLLECTION_MODIFICATION flags, which likely has to be done at SJ (server job) level)

*/

#include "ServerPhasedJobBase.h"
#include "SJRunCPV2.h"
#include "SJSendRequestForOGLMRMCBlueprints.h"

class SPJBuildCPV2Mountain : public ServerPhasedJobBase
{
		void interpretMessage(Message in_message);
		void initializeAndSetOptionalSPJMetadata(Message in_message);	// initialization steps are always dependent on whether or not the message locality is LOCAL or REMOTE.
		void initializeCurrentPhase();
	private:
		Message mountainMetadataMessage;
		std::string planName = "";
};

#endif