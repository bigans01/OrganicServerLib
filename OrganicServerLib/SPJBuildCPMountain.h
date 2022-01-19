#pragma once

#ifndef SPJBuildCPMountain_H
#define SPJBuildCPMountain_H

#include "ServerPhasedJobBase.h"
#include "SJPrepCPMountainRun.h"
#include "SJRunContourPlanWorldTracing.h"
#include "SJBuildContourPlanAffectedBlueprints.h"
#include "SJSendRequestForOGLMRMCBlueprints.h"
#include "SJRunContourPlanFracturingAndMassDriving.h"
#include "SJGenerateBlueprintBackupsForCP.h"
#include "SJCheckContourPlanSuccess.h"

class SPJBuildCPMountain : public ServerPhasedJobBase
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
