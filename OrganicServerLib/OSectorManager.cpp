#include "stdafx.h"
#include "OSectorManager.h"

OSectorManager::OSectorManager()
{
	checkForOSectorFolder();
}

void OSectorManager::checkForOSectorFolder()
{
	char directoryBuffer[1024];
	GetModuleFileNameA(NULL, directoryBuffer, 1024);
	namespace fs = std::filesystem;
	
	fs::path currentExeWorkingDir = fs::path(directoryBuffer).parent_path();
	std::string oSectorFolderName = "osector";

	fs::path oSectorFolder = currentExeWorkingDir / oSectorFolderName;

	if (!fs::exists(oSectorFolder))
	{
		std::cout << "!! Did not find OSector folder; will create it! " << std::endl;
		if (fs::create_directory(oSectorFolder))
		{
			std::cout << "!! Directory created; full path is: " << oSectorFolder.string() << std::endl;
		}
	}
	else
	{
		std::cout << "!! Directory at " << oSectorFolder.string() << " already existed! " << std::endl;
	}

	oSectorFullPath = oSectorFolder;
}

void OSectorManager::setup(int in_processingColumnBounds)
{
	processingColumnBounds = in_processingColumnBounds;
}

void OSectorManager::setupGridForFactory(std::string in_gridName, short in_tileDim, short in_gSectorSize, double in_gridStartY, float in_thresholdValue, int in_seedValue,
	PerlinClusterGeneratorEnum in_generationType)
{
	osmFactory.setupNewGrid(in_gridName, in_tileDim, in_gSectorSize, in_gridStartY, in_thresholdValue, in_seedValue, in_generationType);
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

void OSectorManager::checkProcessingColumnTest(DoublePoint in_processingPoint)
{
	// Step 1: Determine the column to check; simply divide the DoublePoint's X and Z by the value of
	// processingColumnBounds.
	int columnX = findOSectorDimCoordinate(in_processingPoint.x);
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

		std::cout << "!! Size of phaseOneResults: " << phaseOneResults.size() << std::endl;

		// All of the phaseOneResult should share the same origin key; use the first element in this to determine if we already did the work for this grid.
		bool continueWithGeneration = false;
		EnclaveKeyDef::EnclaveKey gridCheck3DKey;
		if (!phaseOneResults.empty())
		{
			auto beginningResult = phaseOneResults.begin();
			auto firstCurrentResult = beginningResult->getClusterPtr()->generateMappingContainer();
			EnclaveKeyDef::Enclave2DKey fetchedOriginKey = firstCurrentResult.fetchContainerOriginKey();
			EnclaveKeyDef::Enclave2DKey queryKey2D = convertPerlinSectorCoordToOSectorCoord(fetchedOriginKey, currentGrid.second);
			gridCheck3DKey = EnclaveKeyDef::EnclaveKey(queryKey2D.a, findOSectorDimCoordinate(currentGridStartingY), queryKey2D.b);
			bool didCompletionExist = doesGridCompletionExistInSector(currentGrid.second, gridCheck3DKey);
			if (!didCompletionExist)
			{
				std::cout << "!!!! ############################### Grid flag not found for this sector ################################## !!!!" << std::endl;
				continueWithGeneration = true;
			}
			else
			{
				std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~ Grid " + currentGrid.second + " has already received a processing pass in sector at ";
				gridCheck3DKey.printKey();
				std::cout << " ~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
			}
		}

		// Only continue, if we did NOT do a completion of this grid, in this origin sector.
		if (continueWithGeneration)
		{
			for (auto& currentResult : phaseOneResults)
			{
				auto containerOfCurrentResult = currentResult.getClusterPtr()->generateMappingContainer();
				bool wasLoadedIntoMemory = currentResult.didClusterExist();
				std::cout << "###################################### !! OSectorManager: did cluster exist -> " << wasLoadedIntoMemory << std::endl;

				// (TEST): fetch the origin key
				EnclaveKeyDef::Enclave2DKey fetchedOriginKey = containerOfCurrentResult.fetchContainerOriginKey();


				EnclaveKeyDef::Enclave2DKey origin3DKeyXZ = convertPerlinSectorCoordToOSectorCoord(fetchedOriginKey, currentGrid.second);
				EnclaveKeyDef::EnclaveKey fetchedOrigin3DKey(origin3DKeyXZ.a, findOSectorDimCoordinate(currentGridStartingY), origin3DKeyXZ.b);


				std::cout << "###################################### !! OsectorManager: origin key of currently fetched container -> ";
				fetchedOriginKey.printKey();
				std::cout << std::endl;

				// (TEST): check if it was loaded into memory.

				// (TEST): print the hash.
				std::cout << "###################################### !! Current container hash -> ";
				std::string currentClusterHash = currentResult.getClusterPtr()->produceHash();
				std::cout << currentResult.getClusterPtr()->produceHash() << std::endl;

				// (TEST): fetch the vector of PerinClusterSectorState from the container; iterate through it,
				// and form an std::unordered_set that's populated from each oSectorKey; this will be the 
				// search list to use for existing OSector files involved with the cluster.
				std::unordered_map<EnclaveKeyDef::EnclaveKey, PerlinClusterHashMeta, EnclaveKeyDef::KeyHasher> perlinClusterBaseSectors;
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

					//perlinClusterBaseSectors.insert(oSectorKey);
					perlinClusterBaseSectors[oSectorKey] = currentHashMeta;
				}

				// With perlinClusterBaseSectors populated, use this + the PerlinCluster hash, and check the OS
				// for OSector files containing the specific PerlinCluster hash. If there are hashes found in the corresponding files,
				// Check "A" (see PerlinFactory.cpp) is true.
				auto filesToCreate = scanForPerlinClusterBaseSectors(perlinClusterBaseSectors, currentResult.getClusterPtr()->produceHash());
				bool clusterRequiresUpdates = false;

				// If the number of files to create is equal to the fetchedStateVector, the perlin cluster can't possibly exist, and we 
				// will need to create all the files, AND also set clusterRequiresUpdates to TRUE.

				// Keep track of how we intend to update each OSector file, through a currentClusterUpdateMap.
				PerlinHashUpdateMap currentClusterUpdateMap = generateUpdateMap(&perlinClusterBaseSectors, currentClusterHash, fetchedOrigin3DKey);


				if (!currentClusterUpdateMap.isEmpty())
				{
					clusterRequiresUpdates = true;
					std::cout << "########### ->> number of clusters to satisfy: " << perlinClusterBaseSectors.size() << std::endl;
					std::cout << "########### ->> cluster updates required; size of currentClusterUpdateMap is: " << currentClusterUpdateMap.size() << std::endl;
					currentClusterUpdateMap.printEntries();
				}

				std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~ Value of clusterRequiresUpdates ----> " << clusterRequiresUpdates << std::endl;


				std::cout << "#################### START FILE WRITING, for hash: " << currentClusterHash << std::endl;
				if (clusterRequiresUpdates)
				{

					auto candidateTargets = currentClusterUpdateMap.fetchPerlinClusterOutputTargets();
					if (!candidateTargets.empty())
					{
						/* ######## ALL the magic of materializing the PerlinCluster would go here

						*/

						// < do the magic here > 
						// ######## And only after it ends, do we update the OSector files below....

						std::cout << "##############################################################################################" << std::endl;
						std::cout << "######## Cluster with hash " << currentClusterHash << " requires cluster output writing to OSectorFiles! " << std::endl;
						std::cout << "##############################################################################################" << std::endl;
						std::cout << "######## required targets are: " << std::endl;

						for (auto& currentCandidate : candidateTargets)
						{
							EnclaveKeyDef::EnclaveKey keyCopy(currentCandidate);
							keyCopy.printKey();
							std::cout << std::endl;
						}
						std::cout << "##############################################################################################" << std::endl;
					}


					// NEW METHOD: iterate over currentClusterUpdateMap to determine what to do.
					for (auto& currentClusterEntry : currentClusterUpdateMap.fetchMapCopy())
					{
						std::string currentOSectorFileName = generateOSectorFileName(currentClusterEntry.first);
						std::filesystem::path oSectorFileFullPath = oSectorFullPath / currentOSectorFileName;


						// For both the creates (CREATE_AND_INSERT_AS_PROCESSED and CREATE_AND_INSERT_AS_REFERENCED)
						// ensure the file exists before doing anything.
						if (currentClusterEntry.second == PerlinHashUpdateMode::CREATE_AND_INSERT_AS_REFERENCED
							||
							currentClusterEntry.second == PerlinHashUpdateMode::CREATE_AND_INSERT_AS_PROCESSED)
						{
							// Ensure it still doesn't exist
							if (!searchForOSectorFile(currentOSectorFileName))
							{
								auto existingSectorFinder = oSectorMap.find(currentClusterEntry.first);
								if (existingSectorFinder == oSectorMap.end())
								{
									// Creaet an OSector object that signifies that we're doing a new OSector file 
									// (OSectorMode::MEMORY_NEW), then insert the hash meta and write it out.
									OSector newOSector(oSectorFileFullPath.string(), OSectorMode::MEMORY_NEW);
									oSectorMap[currentClusterEntry.first] = newOSector;

									// Because we are doing an insert regardless of REFERENCED / PROCESSED, we can just call
									// insertPerlinClusterHashMeta on the oSector.
									oSectorMap[currentClusterEntry.first].insertPerlinClusterHashMeta(perlinClusterBaseSectors[currentClusterEntry.first]);
									oSectorMap[currentClusterEntry.first].writeAll();

								}
							}
						}

						// For inserts or updates, the file must already exist.
						else if
							(
								currentClusterEntry.second == PerlinHashUpdateMode::INSERT_AS_REFERENCED
								||
								currentClusterEntry.second == PerlinHashUpdateMode::INSERT_AS_PROCESSED
								||
								currentClusterEntry.second == PerlinHashUpdateMode::UPDATE_AS_PROCESSED
								)
						{
							// Ensure it DOES exist.
							if (searchForOSectorFile(currentOSectorFileName))
							{
								// Load up the OSector file into the map, if it hasn't been loaded already.
								// Remember that using OSectorMode::DISK will load all content automatically from the disk, 
								// and overwrite any of that object's contents in memory.
								auto existingSectorFinder = oSectorMap.find(currentClusterEntry.first);
								if (existingSectorFinder == oSectorMap.end())
								{
									std::filesystem::path oSectorFileFullPath = oSectorFullPath / currentOSectorFileName;
									OSector newOSector(oSectorFileFullPath.string(), OSectorMode::DISK);
									oSectorMap[currentClusterEntry.first] = std::move(newOSector);
									std::cout << "!!! Done inserting a new OSector, started in OSectorMode::DISK, into map." << std::endl;
									
									
								}

								// Now, check for a mode of either INSERT_AS_REFERENCED or INSERT_AS_PROCESSED;
								// INSERT means adding a new hash entry altogether into an existing file.
								if
									(
										currentClusterEntry.second == PerlinHashUpdateMode::INSERT_AS_REFERENCED
										||
										currentClusterEntry.second == PerlinHashUpdateMode::INSERT_AS_PROCESSED
										)
								{
									oSectorMap[currentClusterEntry.first].insertPerlinClusterHashMeta(perlinClusterBaseSectors[currentClusterEntry.first]);
								}

								// Otherwise, update the existing hash entry.
								else if (currentClusterEntry.second == PerlinHashUpdateMode::UPDATE_AS_PROCESSED)
								{
									// TODO: create hash update function in OSector.
									oSectorMap[currentClusterEntry.first].updatePerlinClusterHashMetaState(currentClusterHash, PerlinClusterSectorStateEnum::PROCESSED);
								}

								// When everything is done/checked, call writeAll().
								oSectorMap[currentClusterEntry.first].writeAll();
							}
						}

					}



				}
			}

			// At the end, be sure to update the completion.
			updateGridAsCompleted(currentGrid.second, gridCheck3DKey);
		}
	}
	
}

void OSectorManager::checkProcessingColumn(DoublePoint in_processingPoint)
{
	// Step 1: Determine the column to check; simply divide the DoublePoint's X and Z by the value of
	// processingColumnBounds.
	int columnX = findOSectorDimCoordinate(in_processingPoint.x);
	int sectorY = findOSectorDimCoordinate(in_processingPoint.y);
	int columnZ = findOSectorDimCoordinate(in_processingPoint.z);
	
	// Current sector 3D key, based off in_processingPoint. This is the sector that will get a hash entry
	// value of PerlinClusterSectorStateEnum::PROCESSED, if the PerlinCluster ends up producing data
	// that would go into the corresponding sector key. Regardless of if there is data or not, we will still flag 
	// that a check on the current grid we are on was done. 
	EnclaveKeyDef::EnclaveKey sectorXYZKey(columnX, sectorY, columnZ);

	EnclaveKeyDef::Enclave2DKey currentColumnToScan(columnX, columnZ);

	std::cout << "!!! currentColumnToScan: ";
	currentColumnToScan.printKey();
	std::cout << std::endl;

	// Target column key
	EnclaveKeyDef::Enclave2DKey targetSectorKey(columnX * processingColumnBounds, columnZ * processingColumnBounds);

	std::cout << "!!! targetSectorKey: ";
	targetSectorKey.printKey();
	std::cout << std::endl;

	


	// Step 2: fetch the processing order from the osmFactory, to determine the order to process;
	// process each one in order.
	std::map<int, std::string> fetchedOrder = osmFactory.fetchGridProcessOrderMap();
	for (auto& currentGrid : fetchedOrder)
	{
		// Only bother continuing working with the current grid, in the current target sector, if it's completion flag wasn't found.
		bool didCompletionExist = doesGridCompletionExistInSector(currentGrid.second, sectorXYZKey);
		if (!didCompletionExist)
		{

			/*
			Remember: for the following logic below, the perlin cluster hash has to be created every time we check a column; it is not stored anywhere,
			but is procedurally generated. We must cycle through each phaseOneResults below, acquire the hash, and check if a fully-materialized cluster
			exists already with that hash.

			*/


			// Get the phaseOneResults of the current grid sector we're looking at.
			// TODO: will probably need to verify/possibly alter the logic of osmFactory.populateSectorInGrid, to avoid repopulating a sector that has already been done,
			// but still return the results.
			std::vector<PerlinClusterGenResult> phaseOneResults = osmFactory.populateSectorInGrid(currentGrid.second, targetSectorKey.a, targetSectorKey.b);

			std::cout << "######################### Size of phaseOneResults --> " << phaseOneResults.size() << std::endl;

			for (auto& currentResult : phaseOneResults)
			{
				std::string currentClusterHash = currentResult.getClusterPtr()->produceHash();

				// STEP 1: What is the state of the current cluster data in memory? It must be fully materialized before continuing.
				// To do this, give the perlin cluster an PerlinClusterGeneratorEnum enum value that indicates how it should materialize;
				// Do this by calling PerlinFactory::getGridGenerationType, and then using this in the call to PerlinCluster::generate. 
				// 
				// The results of the materialization can then be queried in the next step. We shouldn't bother materializing, 
				// if it's already been done.
				auto currentClusterState = currentResult.getClusterPtr()->fetchClusterState();
				if (currentClusterState == PerlinClusterGenerationState::PERLIN_BASE)
				{
					std::cout << "Calling generate..." << std::endl;

					PerlinClusterGeneratorEnum fetchedGridEnum = osmFactory.getGridGenerationType(currentGrid.second);

					if (fetchedGridEnum == PerlinClusterGeneratorEnum::PERLIN_NOGENVAL)
					{
						std::cout << "!!! Warning: PERLIN_NOGENVAL detected...possible crash inbound!" << std::endl;
					}
					else
					{
						std::cout << "!!! Did not find PERLIN_NOGENVAL..." << std::endl;
					}

					// Before attempting generation, ensure that generateTileToSectorMappingsAndSamplingFields gets called
					// to properly create the sampling fields.
					currentResult.getClusterPtr()->generateTileToSectorMappingsAndSamplingFields();

					currentResult.getClusterPtr()->generate(fetchedGridEnum);
				}

				// STEP 2: Once it is completely materialized, query the results to form a PerlinHashUpdateMap; the basis of forming this
				// comes from all of the items in the call to getClusterOutputs() below. 
				//
				// Remember, that if the size of a sector dim is larger than the typical 256 (because, 256 blocks), the getClusterOutputs()
				// function should be returning a map that has subdivided the contents appropriately. For instance, if using a dim of 512,
				// this would mean that the sector existing in this PerlinCluster at 0,0,0 technically covers EIGHT different sector files:
				//
				// 0,0,0 = 0 to 255,	0 to 255,		0 to 255
				// 0,0,1 = 0 to 255,	0 to 255,		256 to 511
				// 1,0,1 = 256 to 511,	0 to 255,		256 to 511
				// 1,0,0 = 256 to 511,	0 to 255,		0 to 255
				// 0,1,0 = 0 to 255,	256 to 511,		0 to 255
				// 0,1,1 = 0 to 255,	256 to 511,		256 to 511
				// 1,1,1 = 256 to 511,	256 to 511,		256 to 511
				// 1,1,0 = 256 to 511,	256 to 511,		0 to 255
				// 
				// As an example, if all 8 subsectors of the 512 dim supersector at 0,0,0 contained data, 
				// we would see 8 different subsectors in the results of the call to getClusterOutputs() below.
				// 
				// In other words, one 512 dim sector covers up to EIGHT standard 256 dim sectors.
				// This must be accounted for, for large mass perlin clusters (such as continents, mountains?)
				//
				std::unordered_map<EnclaveKeyDef::EnclaveKey, PerlinClusterHashMeta, EnclaveKeyDef::KeyHasher> fetchedHashMeta;

				// Remember, currentClusterOutputs should be keyed by the target OSector to update/write to, and valued by all of
				// the blueprints that are associated with that sector.
				auto currentClusterOutputs = currentResult.getClusterPtr()->getClusterOutputs();
				for (auto& currentOutput : *currentClusterOutputs)
				{
					PerlinClusterHashMeta currentMeta = currentOutput.second.getSectorOutputState().generateClusterHashMeta(currentGrid.second);
					fetchedHashMeta[currentOutput.first] = currentMeta;
				}

				// STEP 3: NOTE: for the currentClusterUpdateMap, the values of the EnclaveKeys used should be equivalent to what we expect
				// to see in the OSector file name, NOT the relative sector coordinates per NoiseGrid (such as 1024,1024)
				//
				// When the cluster gets materialized for the very first time, and hasn't written its hashes into any OSector files, the
				// currentClusterUpdateMap will contain all entries. If the OSector file we are processing for missing, but the rest of the OSector files of the cluster exist,
				// this would contain just one entry (i.e, PerlinHashUpdateMode::CREATE_AND_INSERT_AS_PROCESSED)

				PerlinHashUpdateMap currentClusterUpdateMap = generateUpdateMap(&fetchedHashMeta, currentClusterHash, sectorXYZKey);


				// STEP 4: Next, get the candidate targets from the produced currentClusterUpdateMap. This will determine which OSector files 
				// need to have new data loaded into them. Iterate through each key, and use each iteration to acquire the generated
				// data for that corresponding sector. The generated data needs to contain actual blueprint data that will go into the OSector file
				//
				// So, for each currentSectorHashUpdateEntry below, we must acquire those blueprints of the sector we are looking at, write them to the disk,
				// and then remove them from memory (unless we intend to use them)

				// Iterate through each cluster produced.
				for (auto& currentSectorHashUpdateEntry : currentClusterUpdateMap.fetchMapCopy())
				{
					// Do steps 5/6 only if we are NOT looking at an entry that is PerlinHashUpdateMode::UPDATE_AS_PROCESSED
					if (currentSectorHashUpdateEntry.second != PerlinHashUpdateMode::UPDATE_AS_PROCESSED)
					{
						// STEP 4.1: Before any operations, check if the OSector file of the blueprint we're working with exists. If it DOES exist,
						// load it and all of its data by adding an OSector that is instantiated in OSectorMode::DISK, and
						// then doing a std::move to move it into the oSectorMap.
						//
						// < do sector check here, using the currentClusterEntry.first as the key lookup for the OSector > 

						// STEP 4.2: For the current sector, generate blueprints, doing a check in the OSector object to see
						// if data for each blueprint existed previously. If it did exist previously, "mix" it, otherwise it's a new
						// blueprint and we'll just add directly to the OSector.
		
						// Iterate through each blueprint of the current PerlinClusterSectorOutput that we should be looking at.
						// Each PerlinClusterSectorOutput is contained within the currentClusterOutputs (defined earlier), and the key
						// of this map is the sector. 
						for (auto& currentBlueprint : (*currentClusterOutputs)[currentSectorHashUpdateEntry.first].sectorOutputBlueprints)
						{

						}
						
						
						// 
						// IMPORTANT: For OSector files that already existed, the entire file must be loaded into memory, and THEN given
						// the updated blueprint data, and THEN written via a call to writeAll(). This will ensure that the old data + new data
						// get written correctly to the OSector file!
						//
						// Do this, once we have gone either through all blueprints for the current sector, or write out at a regular interval of blueprints

						// < do writeAll here > 


						// STEP 6: Once we have gone through all blueprints/other data to add to the OSector file, update the hash table
						// of the OSector with whatever the corresponding value of PerlinHashUpdateMode requires (see lines around 225, aka "NEW METHOD" above)

						// STEP 7: Remove unused blueprints produced by the processing step, that are sitting in memory (unless we need them)

					}

					// ...otherwise, we just need to update as processed
					else
					{

					}
				}
			}

		}

		// STEP 5:
		// Lastly, update the  "grid completion" entry for the current grid we're looking at, in the sector that is being processed.
		// (sectorXYZKey). Remember, this assumes that the OSector being updated is loaded into the OSector map of this class.
		updateGridAsCompleted(currentGrid.second, sectorXYZKey);
	}
}

OSectorManager::PerlinHashUpdateMap OSectorManager::generateUpdateMap(std::unordered_map<EnclaveKeyDef::EnclaveKey, PerlinClusterHashMeta, EnclaveKeyDef::KeyHasher>* in_metaRef,
	std::string in_hashToSearch,
	EnclaveKeyDef::EnclaveKey in_originKey)
{
	PerlinHashUpdateMap returnMap;

	for (auto& currentCluster : *in_metaRef)
	{
		// Get the OSector file to search for....
		std::string currentOSectorFileName = generateOSectorFileName(currentCluster.first);
		if (searchForOSectorFile(currentOSectorFileName))
		{
			std::filesystem::path currentOSectorFilePath = oSectorFullPath / currentOSectorFileName;
			OSector newOSector(currentOSectorFilePath.string(), OSectorMode::QUICK_READ);


			// The sector file existed, but the hash did not. Enter it as INSERT_AS_PROCESSED
			// if we're looking at the fetchedOrigin3DKey; otherwise, INSERT_AS_REFERENCED.
			if (!newOSector.doesPerlinClusterHashExist(in_hashToSearch))
			{
				std::cout << "!!!! Hash did NOT exist in an existing sector file." << std::endl;

				// Determine how we will update, then insert.
				PerlinHashUpdateMode updateMode = PerlinHashUpdateMode::UNSET_MODE;
				if (currentCluster.first == in_originKey)
				{
					updateMode = PerlinHashUpdateMode::INSERT_AS_PROCESSED;
				}
				else
				{
					updateMode = PerlinHashUpdateMode::INSERT_AS_REFERENCED;
				}
				returnMap.insertEntry(currentCluster.first, updateMode);
			}

			// The other condition would be that the file matching the 3d sector origin key we are looking for existed, 
			// AND it contained an entry flagged as PerlinClusterSectorStateEnum::REFERENCED; in this case,
			// it needs to be updated to PerlinHashUpdateMode::UPDATE_AS_PROCESSED.
			else
			{
				std::cout << "++++++ Hash found in existing sector file!!! Key: ";
				EnclaveKeyDef::EnclaveKey keyCopy = currentCluster.first;
				keyCopy.printKey();
				std::cout << " | Value: ";

				auto fetchedValue = newOSector.fetchClusterSectorStateForHash(in_hashToSearch);
				switch (fetchedValue)
				{
					case PerlinClusterSectorStateEnum::PROCESSED:
					{
						std::cout << "PerlinClusterSectorStateEnum::PROCESSED" << std::endl;
						break;
					}
					case PerlinClusterSectorStateEnum::REFERENCED:
					{
						std::cout << "PerlinClusterSectorStateEnum::REFERENCED" << std::endl;
						break;
					}
					case PerlinClusterSectorStateEnum::NOVAL:
					{
						std::cout << "PerlinClusterSectorStateEnum::NOVAL" << std::endl;
						break;
					}
				}


				if (currentCluster.first == in_originKey)
				{
					if (newOSector.fetchClusterSectorStateForHash(in_hashToSearch) == PerlinClusterSectorStateEnum::REFERENCED)
					{
						std::cout << "++++++ Found existing hash as PerlinHashUpdateMode::INSERT_AS_REFERENCED in an existing sector file! Will now proceed to UPDATE_AS_PROCESSED " << std::endl;
						returnMap.insertEntry(currentCluster.first, PerlinHashUpdateMode::UPDATE_AS_PROCESSED);
					}
				}
			}
		}

		// The file itself does not exist, so the hash cannot possibly exist; 
		else
		{
			std::cout << "++++++ Sector file did not exist, so hash entry can't possibly exist..." << std::endl;

			// Determine how we will update, then insert.
			PerlinHashUpdateMode updateMode = (currentCluster.first == in_originKey) ? PerlinHashUpdateMode::CREATE_AND_INSERT_AS_PROCESSED : PerlinHashUpdateMode::CREATE_AND_INSERT_AS_REFERENCED;
			returnMap.insertEntry(currentCluster.first, updateMode);
		}
	}


	return returnMap;
}

bool OSectorManager::doesGridCompletionExistInSector(std::string in_completionToSearch, EnclaveKeyDef::EnclaveKey in_originKey)
{
	bool completionExists = false;
	// Get the OSector file to search for....
	std::string currentOSectorFileName = generateOSectorFileName(in_originKey);
	if (searchForOSectorFile(currentOSectorFileName))
	{
		std::filesystem::path currentOSectorFilePath = oSectorFullPath / currentOSectorFileName;
		OSector newOSector(currentOSectorFilePath.string(), OSectorMode::QUICK_READ);
		if (newOSector.doesCompletedGridStringExist(in_completionToSearch))
		{
			completionExists = true;
		}
	}

	return completionExists;
}

void OSectorManager::updateGridAsCompleted(std::string in_completionToInsert, EnclaveKeyDef::EnclaveKey in_originKey)
{
	std::string currentOSectorFileName = generateOSectorFileName(in_originKey);

	std::cout << "::::::::::: OSectorManager::updateGridAsCompleted -> currentOSectorFileName: " << currentOSectorFileName << std::endl;

	if (searchForOSectorFile(currentOSectorFileName))
	{
		// We will assume that the file already exists at this point, and has been loaded into the oSectorMap.
		auto existingSectorFinder = oSectorMap.find(in_originKey);
		if (existingSectorFinder != oSectorMap.end())
		{
			std::cout << "------------>>> inserting completion string: " << in_completionToInsert << std::endl;

			oSectorMap[in_originKey].insertCompletedGridString(in_completionToInsert);
			oSectorMap[in_originKey].writeAll();
		}
		else
		{
			std::cout << "!!!!!!!!!!!! WARNING: Osector file to update not found! key was: ";
			in_originKey.printKey();
			std::cout << std::endl;
		}
	}
	else
	{
		std::cout << "Couldn't find file..." << std::endl;
	}
}

bool OSectorManager::doesHashExistInSector(std::string in_oSectorFileName, std::string in_hashToSearch)
{
	bool hashExists = false;

	// Create a temporary OSector object so we can read the file
	std::filesystem::path oSectorFileFullPath = oSectorFullPath / in_oSectorFileName;
	OSector newOSector(oSectorFileFullPath.string(), OSectorMode::QUICK_READ);



	return hashExists;
}

std::vector<EnclaveKeyDef::EnclaveKey> OSectorManager::scanForPerlinClusterBaseSectors(std::unordered_map<EnclaveKeyDef::EnclaveKey, PerlinClusterHashMeta, EnclaveKeyDef::KeyHasher> in_sectorKeys, std::string in_clusterHash)
{
	std::vector<EnclaveKeyDef::EnclaveKey> filesMissing;

	for (auto& currentKey : in_sectorKeys)
	{
		std::string currentOSectorName = generateOSectorFileName(currentKey.first);
		bool doesOSectorExist = searchForOSectorFile(currentOSectorName);

		if (doesOSectorExist)
		{
			std::cout << "!! Found at least one OSector file (" << currentOSectorName << ")" << std::endl;
		}
		else
		{
			std::cout << "!! Sector did not exist; will need to create OSector file with the corresponding key. " << std::endl;
			filesMissing.push_back(currentKey.first);
		}
	}

	return filesMissing;
}

bool OSectorManager::searchForOSectorFile(std::string in_oSectorFileName)
{
	bool wasFound = false;
	namespace fs = std::filesystem;
	fs::path targetPath = oSectorFullPath / in_oSectorFileName;
	if (fs::exists(targetPath) && fs::is_regular_file(targetPath))
	{
		//std::cout << "!! Found file: " << targetPath.string() << std::endl;
		wasFound = true;
	}
	else
	{
		//std::cout << "!! Did NOT find file: " << targetPath.string() << std::endl;
	}

	return wasFound;
}

std::string OSectorManager::generateOSectorFileName(EnclaveKeyDef::EnclaveKey in_sectorKey)
{
	std::string generatedName = "";

	std::string prefix = "O_";
	std::string keyString = std::to_string(in_sectorKey.x) + "_" + std::to_string(in_sectorKey.y) + "_" + std::to_string(in_sectorKey.z);
	std::string suffix = ".osr";

	generatedName = prefix + keyString + suffix;
	//std::cout << "Value of generatedName is: " << generatedName << std::endl;


	return generatedName;
}