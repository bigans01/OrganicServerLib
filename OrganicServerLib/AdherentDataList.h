#pragma once

#ifndef ADHERENTDATALIST_H
#define ADHERENTDATALIST_H

#include "EuclideanDirection3D.h"
#include "EnclaveKeyDef.h"

class AdherentDataList
{
	public:
		AdherentDataList(EuclideanDirection3D in_adherentDirection, EnclaveKeyDef::EnclaveKey in_adherentBlueprintKey, int in_adherentOrder) : adherentDirection(in_adherentDirection), adherentBlueprintKey(in_adherentBlueprintKey), adherentOrder(in_adherentOrder) {}
		EuclideanDirection3D adherentDirection;			// direction
		EnclaveKeyDef::EnclaveKey adherentBlueprintKey;	// blueprint key of the adherent
		int adherentOrder = 0;							// the appropriate order of the adherent 
};

#endif
