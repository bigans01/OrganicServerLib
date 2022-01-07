#pragma once

#ifndef SPJRUNSINGLEMOUNTTEST_H
#define SPJRUNSINGLEMOUNTTEST_H

#include "ServerPhasedJobBase.h"
#include "SJRunSingleMountTest.h"
#include "SJRunContourPlanWorldTracing.h"
#include "SJBuildContourPlanAffectedBlueprints.h"
#include "SJSendRequestForOGLMRMCBlueprints.h"
#include "SJRunContourPlanFracturingAndMassDriving.h"

class SPJRunSingleMountTest : public ServerPhasedJobBase
{
	public:
		void interpretMessage(Message in_message);
		void initializeAndSetOptionalSPJMetadata(Message in_message);	// initialization steps are always dependent on whether or not the message locality is LOCAL or REMOTE.
		void initializeCurrentPhase();
	private:
		Message mountainMetadataMessage;
		std::string planName = "";
};

#endif
