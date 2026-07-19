#pragma once

#ifndef OSECTORMANAGER_H
#define OSECTORMANAGER_H

#include "PerlinFactory.h"
#include "OSector.h"

class OSectorManager
{
	public:	
		OSectorManager();
		void setup(int in_processingColumnBounds);	// call this first before anything else, obviously.


		// Below: setup a new grid.
		void setupGridForFactory(std::string in_gridName, short in_tileDim, short in_gSectorSize, double in_gridStartY, float in_thresholdValue, int in_seedValue);

		// Below: setup a process order.
		void insertGridProcessOrderForFactory(int in_order, std::string in_gridName);

		// Below: print out art for ALL clusters.
		void printOutClusterArts();

		// Below: take in a processing point, determine the column to check by the X and Z value of the input
		void checkProcessingColumn(DoublePoint in_processingPoint);


	private:

		// The factory contains the grids we can use and operate on.
		PerlinFactory osmFactory;

		// Produced OSector values go here in this map.
		std::unordered_map<EnclaveKeyDef::EnclaveKey, OSector, EnclaveKeyDef::KeyHasher> oSectorMap;

		int processingColumnBounds = 0;	// needs to be set by a call to setup(); determines the bounds of a sector column.
										// For example 32, 64, 128 (32 would be for a blueprint, but we began testing with 1024 so that is also acceptable)
										// 
		// Call populateSectorInGrid for the associated sector, fetch it's result, and update OSector files accordingly.
		void runProcessingForSector(std::string in_gridName, int in_coordA, int in_coordB);
		EnclaveKeyDef::Enclave2DKey convertPerlinSectorCoordToOSectorCoord(EnclaveKeyDef::Enclave2DKey in_twoDkeyToConvert, std::string in_noiseGridName);

		int findOSectorDimCoordinate(double in_coordinateValue);	// Calls NoiseGridUtils::findTileCoordinate, by passing in the in_coordinateValue and processingColumnBounds.
																	// Primarily utilized by OSectorManager::checkProcessingColumn.
};

#endif
