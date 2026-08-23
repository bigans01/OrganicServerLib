#pragma once

#ifndef OSECTORMANAGER_H
#define OSECTORMANAGER_H

#include "PerlinFactory.h"
#include "OSector.h"
#include <windows.h>
#include <filesystem>

/*

Description: The OSectorManager class is meant to be the main class object that controls the generation, storage, and management of OSector data.
It uses the underlying PerlinFactory contained within the class to generate PerlinCluster data, and utilizes the various read/write functions of the
OSector class to load data into memory and save it to disk.

The main code of this class that will be used extensively is the checkProcessingColumn function. This function will take the X/Z 2d coords
of a given 3d sector X/Y/Z, and check what existing grids in that column have been completed. If an existing grid has not been completed, 
data for that noise grid is generated via a call to osmFactory.populateSectorInGrid, and then processed. The processed data then contains an output
map, which contains entries for each OSector for how and if it should write corresponding OSector data to file. See the checkProcessingColumnTest
and checkProcessingColumn functions for more details on how this works.

*/

class OSectorManager
{
	public:	
		OSectorManager();
		void setup(int in_processingColumnBounds);	// call this first before anything else, obviously.


		// Below: setup a new grid.
		void setupGridForFactory(std::string in_gridName, short in_tileDim, short in_gSectorSize, double in_gridStartY, float in_thresholdValue, int in_seedValue,
								PerlinClusterGeneratorEnum in_generationType);

		// Below: setup a process order.
		void insertGridProcessOrderForFactory(int in_order, std::string in_gridName);

		// Below: print out art for ALL clusters.
		void printOutClusterArts();

		// Below (TEST ONLY): take in a processing point, determine the column to check by the X and Z value of the input
		void checkProcessingColumnTest(DoublePoint in_processingPoint);

		// Below (TEST ONLY) main prototype function; still in development (7/26/2026); would be called only after the
		// a PerlinCluster has been fully materialized.
		void checkProcessingColumn(DoublePoint in_processingPoint);


	private:

		enum class PerlinHashUpdateMode
		{
			UNSET_MODE,
			CREATE_AND_INSERT_AS_REFERENCED,
			CREATE_AND_INSERT_AS_PROCESSED,
			INSERT_AS_REFERENCED,
			INSERT_AS_PROCESSED,
			UPDATE_AS_PROCESSED
		};

		class PerlinHashUpdateMap
		{
			public:
				PerlinHashUpdateMap() {}

				void insertEntry(EnclaveKeyDef::EnclaveKey in_oSectorKey, PerlinHashUpdateMode in_updateMode)
				{
					updateMap[in_oSectorKey] = in_updateMode;
				}

				bool isEmpty()
				{
					return updateMap.empty();
				}

				int size()
				{
					return updateMap.size();
				}

				void printEntries()
				{
					std::cout << "#####----PerlinHashUpdateMap, printing entries: " << std::endl;
					for (auto& currentEntry : updateMap)
					{
						EnclaveKeyDef::EnclaveKey currentCopy(currentEntry.first);

						currentCopy.printKey();
						std::cout << " -> ";

						switch (currentEntry.second)
						{
							case PerlinHashUpdateMode::UNSET_MODE:
							{
								std::cout << "UNSET_MODE (do nothing) " << std::endl;
								break;
							}

							case PerlinHashUpdateMode::INSERT_AS_REFERENCED: 
							{
								std::cout << "INSERT_AS_REFERENCED (insert a new hash on an existing file, as referenced) ";
								break;
							}

							case PerlinHashUpdateMode::CREATE_AND_INSERT_AS_PROCESSED:
							{
								std::cout << "CREATE_AND_INSERT_AS_PROCESSED (create a new file, insert the hash as processed) ";
								break;
							}

							case PerlinHashUpdateMode::CREATE_AND_INSERT_AS_REFERENCED:
							{
								std::cout << "CREATE_AND_INSERT_AS_REFERENCED (create a new file, insert the hash as referenced) ";
								break;
							}

							case PerlinHashUpdateMode::INSERT_AS_PROCESSED:
							{
								std::cout << "INSERT_AS_PROCESSED (insert as processed, into an existing file) ";
								break;
							}

							case PerlinHashUpdateMode::UPDATE_AS_PROCESSED:
							{
								std::cout << "UPDATE_AS_PROCESSED (update as processed, into an existing file) ";
								break;
							}
						}


						std::cout << std::endl;
					}
				}

				std::unordered_map<EnclaveKeyDef::EnclaveKey, PerlinHashUpdateMode, EnclaveKeyDef::KeyHasher> fetchMapCopy() { return updateMap;  }

				std::unordered_set<EnclaveKeyDef::EnclaveKey, EnclaveKeyDef::KeyHasher> fetchPerlinClusterOutputTargets()
				{
					std::unordered_set<EnclaveKeyDef::EnclaveKey, EnclaveKeyDef::KeyHasher> returnTargets;

					// Remember, don't return entries that are UPDATE_AS_PROCESSED, as those only require hash updates 
					// and do not require perlin cluster generation
					for (auto& currentCandidate : updateMap)
					{
						if (currentCandidate.second != PerlinHashUpdateMode::UPDATE_AS_PROCESSED)
						{
							returnTargets.insert(currentCandidate.first);
						}
					}

					return returnTargets;
				}

			private:
				std::unordered_map<EnclaveKeyDef::EnclaveKey, PerlinHashUpdateMode, EnclaveKeyDef::KeyHasher> updateMap;

		};

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
																	// Primarily utilized by OSectorManager::checkProcessingColumnTest.

		std::vector<EnclaveKeyDef::EnclaveKey> scanForPerlinClusterBaseSectors(std::unordered_map<EnclaveKeyDef::EnclaveKey, PerlinClusterHashMeta, EnclaveKeyDef::KeyHasher> in_sectorKeys, std::string in_clusterHash);

		std::string generateOSectorFileName(EnclaveKeyDef::EnclaveKey in_sectorKey);

		void checkForOSectorFolder();

		std::filesystem::path oSectorFullPath;	// stores the location of the OSector folder at runtime, so we know where the directory is to look for
												// OSector files.

		bool searchForOSectorFile(std::string in_oSectorFileName);	// search for an OSector file in the oSectorFullPath directory; make sure that oSectorFullPath
																	// is set before calling!
		bool doesHashExistInSector(std::string in_oSectorFileName, std::string in_hashToSearch);

		PerlinHashUpdateMap generateUpdateMap(std::unordered_map<EnclaveKeyDef::EnclaveKey, PerlinClusterHashMeta, EnclaveKeyDef::KeyHasher>* in_metaRef,
											std::string in_hashToSearch,
											EnclaveKeyDef::EnclaveKey in_originKey);

		bool doesGridCompletionExistInSector(std::string in_completionToSearch, EnclaveKeyDef::EnclaveKey in_originKey);
		void updateGridAsCompleted(std::string in_completionToInsert, EnclaveKeyDef::EnclaveKey in_originKey);

};

#endif
