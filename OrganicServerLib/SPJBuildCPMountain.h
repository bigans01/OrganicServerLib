/*

NOTICE: THIS CLASS IS DEPRECATED, AND WILL BE DELETED SOMETIME IN THE FUTURE

This class will remain intact but inactive, until the transition to SPJs that use the new CPV2 model 
(such as SJRunCPV2) have been thoroughly tested and evaluated. At the time of this writing, executing this SPJ
will not produce a valid ContourPlan, due to the old method of producing ContourPlan (see ContourPlanBase)



*/

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
