#include "stdafx.h"
#include "OSectorManager.h"

OSectorManager::OSectorManager()
{

}

void OSectorManager::setup(int in_processingColumnBounds)
{
	processingColumnBounds = in_processingColumnBounds;
}

void OSectorManager::setupGridForFactory(std::string in_gridName, short in_tileDim, short in_gSectorSize, double in_gridStartY, float in_thresholdValue, int in_seedValue)
{
	osmFactory.setupNewGrid(in_gridName, in_tileDim, in_gSectorSize, in_gridStartY, in_thresholdValue, in_seedValue);
}

void OSectorManager::insertGridProcessOrderForFactory(int in_order, std::string in_gridName)
{
	osmFactory.insertGridProcessOrder(in_order, in_gridName);
}

void OSectorManager::printOutClusterArts()
{
	osmFactory.printOutClusterArt();
}

void OSectorManager::runProcessingForSector(std::string in_gridName, int in_coordA, int in_coordB)
{
	// Step 1: run the processing, fetch the results for phase 1 (aka, hash and tile generation)
	std::vector<PerlinClusterGenResult> phaseOneResults = osmFactory.populateSectorInGrid(in_gridName, in_coordA, in_coordB);

	// Step 2: Cycle through each entry in phaseOneResults; we will have to check whether each result has
	// been generated previously
	for (auto& currentResult : phaseOneResults)
	{
		auto containerOfCurrentResult = currentResult.getClusterPtr()->generateMappingContainer();

		// Step 2.1: fetch the origin key
	}
}

EnclaveKeyDef::Enclave2DKey OSectorManager::convertPerlinSectorCoordToOSectorCoord(EnclaveKeyDef::Enclave2DKey in_twoDkeyToConvert, std::string in_noiseGridName)
{
	EnclaveKeyDef::Enclave2DKey returnKey;
	int currentDimDivisor = osmFactory.fetchNoiseGridSectorDim(in_noiseGridName);
	returnKey.a = in_twoDkeyToConvert.a / currentDimDivisor;
	returnKey.b = in_twoDkeyToConvert.b / currentDimDivisor;
	return returnKey;
}

int OSectorManager::findOSectorDimCoordinate(double in_coordinateValue)
{ 
	return NoiseGridUtils::findTileCoordinate(in_coordinateValue, processingColumnBounds) / processingColumnBounds;
}

void OSectorManager::checkProcessingColumn(DoublePoint in_processingPoint)
{
	// Step 1: Determine the column to check; simply divide the DoublePoint's X and Z by the value of
	// processingColumnBounds.
	//int columnX = in_processingPoint.x / processingColumnBounds;
	//int columnZ = in_processingPoint.z / processingColumnBounds;

	//int columnX = NoiseGridUtils::findTileCoordinate(in_processingPoint.x, processingColumnBounds) / processingColumnBounds;
	int columnX = findOSectorDimCoordinate(in_processingPoint.x);
	//int columnZ = NoiseGridUtils::findTileCoordinate(in_processingPoint.z, processingColumnBounds) / processingColumnBounds;
	int columnZ = findOSectorDimCoordinate(in_processingPoint.z);

	EnclaveKeyDef::Enclave2DKey currentColumnToScan(columnX, columnZ);

	std::cout << "!!! currentColumnToScan: "; 
	currentColumnToScan.printKey();
	std::cout << std::endl;

	// Target sector key
	EnclaveKeyDef::Enclave2DKey targetSectorKey(columnX * processingColumnBounds, columnZ * processingColumnBounds);

	std::cout << "!!! targetSectorKey: ";
	targetSectorKey.printKey();
	std::cout << std::endl;

	
	// Step 2: fetch the processing order from the osmFactory, to determine the order to process;
	// process each one in order.
	std::map<int, std::string> fetchedOrder = osmFactory.fetchGridProcessOrderMap();
	for (auto& currentGrid : fetchedOrder)
	{
		std::cout << "!!! processing grid at index: " << currentGrid.first << std::endl;
		std::cout << "!!! Grid name: " << currentGrid.second << std::endl;

		double currentGridStartingY = osmFactory.fetchNoiseGridStartY(currentGrid.second);
		std::cout << "!!! Current grid start Y: " << currentGridStartingY << std::endl;

		// Fetch the starting Y field of the current NoiseGrid we're looking at, so that we can translate this to the appropriate y-value to use in a OSector file keys,
		// that are generated from this field.


		// Get the phaseOneResults of the current grid we're looking at.
		std::vector<PerlinClusterGenResult> phaseOneResults = osmFactory.populateSectorInGrid(currentGrid.second, targetSectorKey.a, targetSectorKey.b);
		for (auto& currentResult : phaseOneResults)
		{
			auto containerOfCurrentResult = currentResult.getClusterPtr()->generateMappingContainer();

			// (TEST): fetch the origin key
			EnclaveKeyDef::Enclave2DKey fetchedOriginKey = containerOfCurrentResult.fetchContainerOriginKey();
			std::cout << "!! OsectorManager: origin key of currently fetched container -> ";
			fetchedOriginKey.printKey();
			std::cout << std::endl;

			// (TEST): print the hash.
			std::cout << "!! Current container hash -> "; 
			std::cout << currentResult.getClusterPtr()->produceHash() << std::endl;

			// (TEST): fetch the vector of PerinClusterSectorState from the container; iterate through it.
			auto fetchedStateVector = containerOfCurrentResult.fetchContainerSectorStates();
			for (auto& currentState : fetchedStateVector)
			{
				// Get a copy of the current key, and then translate it to relative OSector coordinates, which is done 
				// by calling fetchNoiseGridSectorDim for the named NoiseGrid we are working with.
				EnclaveKeyDef::Enclave2DKey currentGridSectorXZ = convertPerlinSectorCoordToOSectorCoord(currentState.currentKey, currentGrid.second);
				EnclaveKeyDef::EnclaveKey oSectorKey(currentGridSectorXZ.a, findOSectorDimCoordinate(currentGridStartingY), currentGridSectorXZ.b);

				PerlinClusterHashMeta currentHashMeta = currentState.generateClusterHashMeta(currentGrid.second);

				std::cout << "Stats for curent hash meta -> Target OSector file key: ";
				oSectorKey.printKey();
				std::cout << " | ";
				currentHashMeta.printHashMetaData();
			}
		}


	}
	
}