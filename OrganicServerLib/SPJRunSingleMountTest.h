#pragma once

#ifndef SPJRUNSINGLEMOUNTTEST_H
#define SPJRUNSINGLEMOUNTTEST_H

#include "ServerPhasedJobBase.h"
#include "SJRunSingleMountTest.h"
#include "SJSendRequestForOGLMRMCBlueprints.h"

class SPJRunSingleMountTest : public ServerPhasedJobBase
{
	public:
		void interpretMessage(Message in_message);
		void initializeAndSetOptionalSPJMetadata(Message in_message);	// initialization steps are always dependent on whether or not the message locality is LOCAL or REMOTE.
		void initializeCurrentPhase();
	private:
		Message mountainMetadataMessage;
};

#endif
