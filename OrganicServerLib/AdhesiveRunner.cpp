#include "stdafx.h"
#include "AdhesiveRunner.h"

void AdhesiveRunner::performAdhesion()
{
	//std::cout << "++++++running adhesion for blueprint: " << originalBlueprintKey.x << ", " << originalBlueprintKey.y << ", " << originalBlueprintKey.z << std::endl;

	// load the adherent data map
	auto adherentListBegin = adherentDataListVector.begin();
	auto adherentListEnd = adherentDataListVector.end();
	for (; adherentListBegin != adherentListEnd; adherentListBegin++)
	{
		int keyValue = adherentListBegin->adherentOrder;

		// store/copy the directions we have to scan
		adherentDataMap[keyValue].adherentBlueprintKey = adherentListBegin->adherentBlueprintKey;
		adherentDataMap[keyValue].adherentDirection = adherentListBegin->adherentDirection;

		// create the directional lookup, insert the AdherentDataList for the corresponding lookup.
		directionLookup[adherentListBegin->adherentDirection] = *adherentListBegin;
	}

	// cycle through each element in the map, and use the adherentDirection to find the appropriate subset of rawEnclaves
	auto adherentDataMapBegin = adherentDataMap.begin();
	auto adherentDataMapEnd = adherentDataMap.end();
	for (; adherentDataMapBegin != adherentDataMapEnd; adherentDataMapBegin++)
	{
		scanForOrganicRawEnclavesViaDirection(adherentDataMapBegin->second.adherentDirection);
	}

	// once the adhesive data has been appropriately inserted into OrganicRawEnclaveAdhesives, cycle through each one and perform the point calibration.
	runPointAdhesions();
}

void AdhesiveRunner::scanForOrganicRawEnclavesViaDirection(EuclideanDirection3D in_euclideanDirection)
{
	// set up the iterators that point to the OrganicRawEnclaves in the original blueprint
	EnclaveFractureResultsMap* originalResults = &enclaveFractureResultsMapMapRef->find(originalBlueprintKey)->second;	// get the ref (could use auto here if we wanted but this is better for readability imo)

	if (enclaveFractureResultsMapMapRef->find(originalBlueprintKey) == (*enclaveFractureResultsMapMapRef).end())
	{
		std::cout << "+++++++++++++++++++++WARNING, original fracture results not found!! " << std::endl;
		int someVal = 3;
		std::cin >> someVal;
	}

	// print out the directions for testing


	auto originalResultsBegin = originalResults->fractureResultsContainerMap.begin();
	auto originalResultsEnd = originalResults->fractureResultsContainerMap.end();

	// scan for east
	if (in_euclideanDirection == EuclideanDirection3D::POS_X)
	{
		//std::cout << "Scanning pos X..." << std::endl;
		for (; originalResultsBegin != originalResultsEnd; originalResultsBegin++)
		{
			// to qualify as an adhesive to the blueprint border at +X, the X value of the OrganicRawEnclave's key must be 7.
			if (originalResultsBegin->first.x == 7)
			{
				insertAdhesiveData(originalResultsBegin->first, &originalResultsBegin->second, in_euclideanDirection);
			}
		}
	}

	// scan for north
	if (in_euclideanDirection == EuclideanDirection3D::POS_Z)
	{
		//std::cout << "Scanning pos Z..." << std::endl;
		for (; originalResultsBegin != originalResultsEnd; originalResultsBegin++)
		{
			// to qualify as an adhesive to the blueprint border at +Z, the Z value of the OrganicRawEnclave's key must be 7.
			if (originalResultsBegin->first.z == 7)
			{
				insertAdhesiveData(originalResultsBegin->first, &originalResultsBegin->second, in_euclideanDirection);
			}
		}
	}

	// scan for west
	if (in_euclideanDirection == EuclideanDirection3D::NEG_X)
	{
		//std::cout << "Scanning neg X..." << std::endl;
		for (; originalResultsBegin != originalResultsEnd; originalResultsBegin++)
		{
			// to qualify as an adhesive to the blueprint border at -X, the X value of the OrganicRawEnclave's key must be 0.
			if (originalResultsBegin->first.x == 0)
			{
				insertAdhesiveData(originalResultsBegin->first, &originalResultsBegin->second, in_euclideanDirection);
			}
		}
	}

	// scan south
	if (in_euclideanDirection == EuclideanDirection3D::NEG_Z)
	{
		//std::cout << "Scanning neg Z..." << std::endl;
		for (; originalResultsBegin != originalResultsEnd; originalResultsBegin++)
		{
			// to qualify as an adhesive to the blueprint border at -Z, the Z value of the OrganicRawEnclave's key must be 0.
			if (originalResultsBegin->first.z == 0)
			{
				insertAdhesiveData(originalResultsBegin->first, &originalResultsBegin->second, in_euclideanDirection);
			}
		}
	}

	// scan above
	if (in_euclideanDirection == EuclideanDirection3D::POS_Y)
	{
		//std::cout << "Scanning pos Y..." << std::endl;
		for (; originalResultsBegin != originalResultsEnd; originalResultsBegin++)
		{
			// to qualify as an adhesive to the blueprint border at +Y, the Y value of the OrganicRawEnclave's key must be 7.
			if (originalResultsBegin->first.y == 7)
			{
				//std::cout << "Adherent found: " << originalResultsBegin->first.x << ", " << originalResultsBegin->first.y << ", " << originalResultsBegin->first.z << std::endl;
				insertAdhesiveData(originalResultsBegin->first, &originalResultsBegin->second, in_euclideanDirection);
			}
		}
	}

	// scan below
	if (in_euclideanDirection == EuclideanDirection3D::NEG_Y)
	{
		//std::cout << "Scanning neg Y..." << std::endl;
		for (; originalResultsBegin != originalResultsEnd; originalResultsBegin++)
		{

			// to qualify as an adhesive to the blueprint border at -Y, the Y value of the OrganicRawEnclave's key must be 0.
			if (originalResultsBegin->first.y == 0)
			{
				insertAdhesiveData(originalResultsBegin->first, &originalResultsBegin->second, in_euclideanDirection);
			}
		}
	}
}

void AdhesiveRunner::insertAdhesiveData(EnclaveKeyDef::EnclaveKey in_adhesiveEnclaveKey, OrganicRawEnclave* in_organicRawEnclaveRef, EuclideanDirection3D in_euclideanDirection)
{
	auto searchForExisting = adhesiveRawEnclavesMap.find(in_adhesiveEnclaveKey);
	if (searchForExisting == adhesiveRawEnclavesMap.end())	// doesn't exist
	{
		OrganicRawEnclaveAdhesives newAdhesive(in_organicRawEnclaveRef, in_euclideanDirection);	// insert the new adhesive
		adhesiveRawEnclavesMap[in_adhesiveEnclaveKey] = newAdhesive;
	}

	else if (searchForExisting != adhesiveRawEnclavesMap.end()) // it does exist, so just update with the direction.
	{
		adhesiveRawEnclavesMap[in_adhesiveEnclaveKey].insertDirection(in_euclideanDirection);
	}
}

void AdhesiveRunner::runPointAdhesions()
{
	auto adhesiveRawEnclavesMapBegin = adhesiveRawEnclavesMap.begin();
	auto adhesiveRawEnclavesMapEnd = adhesiveRawEnclavesMap.end();
	for (; adhesiveRawEnclavesMapBegin != adhesiveRawEnclavesMapEnd; adhesiveRawEnclavesMapBegin++)
	{
		//std::cout << "Performing point adhesion for OrganicRawEnclave with key: " << adhesiveRawEnclavesMapBegin->first.x << ", " << adhesiveRawEnclavesMapBegin->first.y << ", " << adhesiveRawEnclavesMapBegin->first.z << std::endl;

		bool debugTarget = false;
		/*
		if
		(
			(originalBlueprintKey.x == -1009)
			&&
			(originalBlueprintKey.y == 993)
			&&
			(originalBlueprintKey.z == 998)
		)
		{
			
			if
			(
				(adhesiveRawEnclavesMapBegin->first.x == 1)
				&&
				(adhesiveRawEnclavesMapBegin->first.y == 1)
				&&
				(adhesiveRawEnclavesMapBegin->first.z == 7)
			)
			
			{
				//std::cout << "##### setting to TRUE" << std::endl;
				//testOutput = true;

				//std::cout << "Testing halt, enter number to continue. " << std::endl;
				//int someVal = 3;
				//std::cin >> someVal;
				debugTarget = true;
			}
		}
		*/

		// the vector where we will store the actual directions to compare against, once we've checked that an OrganicRawEnclave exists that we can check against.
		std::map<int, DiscoveredORELocation> validCheckableDirections;	// will automatically order these for us

		// get the directions for the current raw enclave adhesive; check to see if the OrganicRawEnclave to adhere to exists. If it does not, exclude it from the adhesion process.
		auto directionsBegin = adhesiveRawEnclavesMapBegin->second.adhesiveDirections.begin();
		auto directionsEnd = adhesiveRawEnclavesMapBegin->second.adhesiveDirections.end();
		for (; directionsBegin != directionsEnd; directionsBegin++)
		{
			// use the direction value to get the blueprint key to lookup against
			EnclaveKeyDef::EnclaveKey blueprintKeyToCompareTo = directionLookup[*directionsBegin].adherentBlueprintKey;								// fetch the EnclaveFractureResultsMap of the blueprint we're comparing against
			EnclaveFractureResultsMap* neighboringResultsMapRef = &enclaveFractureResultsMapMapRef->find(blueprintKeyToCompareTo)->second;
			EnclaveKeyDef::EnclaveKey neighboringOrganicRawEnclaveKey = findBorderingOrganicRawEnclaveToCompareAgainst(*directionsBegin, adhesiveRawEnclavesMapBegin->first);

			// ensure the neighboring OrganicRawEnclave exists; if it does, we'll put it into the new direction list. If it doesn't exist, don't bother comparing against it.
			std::string neighborRawEnclaveFound = "NOT FOUND";
			auto wasNeighboringRawEnclaveFound = neighboringResultsMapRef->fractureResultsContainerMap.find(neighboringOrganicRawEnclaveKey);
			if (wasNeighboringRawEnclaveFound != neighboringResultsMapRef->fractureResultsContainerMap.end())
			{
				neighborRawEnclaveFound = "FOUND";
				DiscoveredORELocation oreLocation;
				oreLocation.direction = *directionsBegin;													// store the euclidean direction		
				oreLocation.enclaveFractureResultsMapRef = neighboringResultsMapRef;
				oreLocation.neighboringBlueprintKey = blueprintKeyToCompareTo;								// the key of the EnclaveFractureResultsMap to look in 
				oreLocation.keyInNeighboringBlueprint = neighboringOrganicRawEnclaveKey;					// the key of the OrganicRawEnclave in the neighboring EnclaveFractureResultsMap
				validCheckableDirections[directionLookup[*directionsBegin].adherentOrder] = oreLocation;	// the key value of this map is equal to the order of the direction we looked at. This is 																						
																											// done so that we perform the point adhesion in the appropriate order, since std::map orders automatically.
			}

			
			if (debugTarget == true)
			{
				std::cout << "Check for the neighboring OrganicRawEnclave at (" << neighboringOrganicRawEnclaveKey.x << ", " << neighboringOrganicRawEnclaveKey.y << ", " << neighboringOrganicRawEnclaveKey.z
					<< "), in blueprint (" << blueprintKeyToCompareTo.x << ", " << blueprintKeyToCompareTo.y << ", " << blueprintKeyToCompareTo.z << ") "
					<< " was " << neighborRawEnclaveFound << std::endl;
			}
		}

		// cycle through the validCheckableDirections map, and use the value in each iteration for a function call to perform the point adhesion.
		//auto validsBegin = validCheckableDirections.begin();
		//auto validsEnd = validCheckableDirections.end();
		//for (; validsBegin != validsEnd; validsBegin++)
		//{
		
		//}

		applyAdhesions(adhesiveRawEnclavesMapBegin->first, adhesiveRawEnclavesMapBegin->second.organicRawEnclaveRef, validCheckableDirections, debugTarget);		// the enclave key parameter here is for testing only; it can be removed later (9/3/2020)
		//std::cout << "++ Number of valids to compare against is: " << validCheckableDirections.size() << std::endl;

		// ************************testing only
		/*
		if
		(
			(originalBlueprintKey.x == -1009)
			&&
			(originalBlueprintKey.y == 993)
			&&
			(originalBlueprintKey.z == 998)
		)
		{
			if
			(
				(adhesiveRawEnclavesMapBegin->first.x == 1)
				&&
				(adhesiveRawEnclavesMapBegin->first.y == 1)
				&&
				(adhesiveRawEnclavesMapBegin->first.z == 7)
			)
			{
				//std::cout << "##### setting to TRUE" << std::endl;
				//testOutput = true;

				//std::cout << "Testing halt, enter number to continue. " << std::endl;
				//int someVal = 3;
				//std::cin >> someVal;
			}
		}
		*/
	}
}

void AdhesiveRunner::applyAdhesions(EnclaveKeyDef::EnclaveKey in_originalRawEnclaveKey, OrganicRawEnclave* in_originalOrganicRawEnclaveRef, std::map<int, DiscoveredORELocation> in_oreLocations, bool in_debugFlag)
{
	//std::cout << "+++ passed in original key is: " << in_originalRawEnclaveKey.x << ", " << in_originalRawEnclaveKey.y << ", " << in_originalRawEnclaveKey.z << std::endl;
	//bool testOutput = false;
	bool testOutput = in_debugFlag;
	if
	(
		(in_originalRawEnclaveKey.x == 0)
		&&
		(in_originalRawEnclaveKey.y == 7)
		&&
		(in_originalRawEnclaveKey.z == 7)
	)
	{
		//std::cout << "##### setting to TRUE" << std::endl;
		//testOutput = true;
	}

	// for each direction in the oreLocations map, get the localized values (that is, how the points in the neighboring ORE would appear in the original ORE) of the points in the OrganicRawEnclave that the direction is associated with
	auto locationsBegin = in_oreLocations.begin();
	auto locationsEnd = in_oreLocations.end();
	//std::cout << "Size of passed-in OreLocations: " << in_oreLocations.size() << std::endl;

	std::map<int, LocalizedPointsMetaData> metaDataMap;
	for (; locationsBegin != locationsEnd; locationsBegin++)
	{
		//std::cout << "## Running location " << std::endl;
		std::vector<ECBPolyPoint> localizedPoints = acquireLocalizedPointsFromDiscoveredORELocation(locationsBegin->second, in_debugFlag);

		// if there were points found in the vector, then we know there is at least 1 normalized point.
		if (!localizedPoints.empty())
		{

			// store the resulting, newly localized points into a map
			int currentOrder = directionLookup[locationsBegin->second.direction].adherentOrder;
			LocalizedPointsMetaData currentMetaData(locationsBegin->second.direction, localizedPoints);
			metaDataMap[currentOrder] = currentMetaData;
		}

		// ************************testing only
		
		if (testOutput == true)
		{
			// output the direction we acquired from
			if (locationsBegin->second.direction == EuclideanDirection3D::POS_X)
			{
				std::cout << "Acquiried the following from the EAST: " << std::endl;
			}
			else if (locationsBegin->second.direction == EuclideanDirection3D::POS_Z)
			{
				std::cout << "Acquiried the following from the NORTH: " << std::endl;
			}
			else if (locationsBegin->second.direction == EuclideanDirection3D::NEG_X)
			{
				std::cout << "Acquiried the following from the WEST: " << std::endl;
			}
			else if (locationsBegin->second.direction == EuclideanDirection3D::NEG_Z)
			{
				std::cout << "Acquiried the following from the SOUTH: " << std::endl;
			}
			else if (locationsBegin->second.direction == EuclideanDirection3D::POS_Y)
			{
				std::cout << "Acquiried the following from the ABOVE: " << std::endl;
			}
			else if (locationsBegin->second.direction == EuclideanDirection3D::NEG_Y)
			{
				std::cout << "Acquiried the following from the BELOW: " << std::endl;
			}


			auto localizedPointsBegin = localizedPoints.begin();
			auto localizedPointsEnd = localizedPoints.end();
			for (; localizedPointsBegin != localizedPointsEnd; localizedPointsBegin++)
			{
				std::cout << "Localized new point: " << localizedPointsBegin->x << ", " << localizedPointsBegin->y << ", " << localizedPointsBegin->z << std::endl;
			}
		}
		
		//std::cout << "## Finished running location. " << std::endl;
	}
	

	// cycle through the enclave triangle containers in the OrganicRawEnclave, and have each go through the oreLocations map.
	

	// test the ref
	//int someVal = in_originalOrganicRawEnclaveRef->enclaveTriangleContainerMap.size();
	//std::cout << "### Test size: " << someVal;

	/*
	auto enclaveTriangleContainersBegin = in_originalOrganicRawEnclaveRef->enclaveTriangleContainerMap.begin();
	auto enclaveTriangleContainersEnd = in_originalOrganicRawEnclaveRef->enclaveTriangleContainerMap.end();
	for (; enclaveTriangleContainersBegin != enclaveTriangleContainersEnd; enclaveTriangleContainersBegin++)
	{
		// for each triangle in the container, apply the oreLocations
		auto currentEnclaveTriangleBegin = enclaveTriangleContainersBegin->second.triangles.begin();
		auto currentEnclaveTriangleEnd = enclaveTriangleContainersBegin->second.triangles.end();
		for (; currentEnclaveTriangleBegin != currentEnclaveTriangleEnd; currentEnclaveTriangleBegin++)
		{

			// ***************************** testing only
			
			if (testOutput == true)
			{
				std::cout << "------Points for original ORE triangle [container: " << enclaveTriangleContainersBegin->first << "][triangle index: " << currentEnclaveTriangleBegin->first << "]" << std::endl;
				for (int x = 0; x < 3; x++)
				{
					ECBPolyPoint currentPoint = currentEnclaveTriangleBegin->second.lineArray[x].pointA;
					std::cout << "Original ORE points: " << currentPoint.x << ", " << currentPoint.y << ", " << currentPoint.z << std::endl;
				}
			}
			
			adhereEnclaveTriangleToLocalizedPoints(metaDataMap, &currentEnclaveTriangleBegin->second, in_debugFlag);
			// run the metadata map through each enclave triangle

		}
	}
	*/
	
	auto etcSGMBegin = in_originalOrganicRawEnclaveRef->etcSGM.begin();
	auto etcSGMEnd = in_originalOrganicRawEnclaveRef->etcSGM.end();
	for (; etcSGMBegin != etcSGMEnd; etcSGMBegin++)
	{
		auto enclaveTriangleContainersBegin = etcSGMBegin->second.containerMap.begin();
		auto enclaveTriangleContainersEnd = etcSGMBegin->second.containerMap.end();
		for (; enclaveTriangleContainersBegin != enclaveTriangleContainersEnd; enclaveTriangleContainersBegin++)
		{
			// for each triangle in the container, apply the oreLocations
			auto currentEnclaveTriangleBegin = enclaveTriangleContainersBegin->second.triangles.begin();
			auto currentEnclaveTriangleEnd = enclaveTriangleContainersBegin->second.triangles.end();
			for (; currentEnclaveTriangleBegin != currentEnclaveTriangleEnd; currentEnclaveTriangleBegin++)
			{

				// ***************************** testing only

				if (testOutput == true)
				{
					std::cout << "------Points for original ORE triangle [container: " << enclaveTriangleContainersBegin->first << "][triangle index: " << currentEnclaveTriangleBegin->first << "]" << std::endl;
					for (int x = 0; x < 3; x++)
					{
						ECBPolyPoint currentPoint = currentEnclaveTriangleBegin->second.lineArray[x].pointA;
						std::cout << "Original ORE points: " << currentPoint.x << ", " << currentPoint.y << ", " << currentPoint.z << std::endl;
					}
				}

				adhereEnclaveTriangleToLocalizedPoints(metaDataMap, &currentEnclaveTriangleBegin->second, in_debugFlag);
				// run the metadata map through each enclave triangle

			}
		}
	}
}

void AdhesiveRunner::adhereEnclaveTriangleToLocalizedPoints(std::map<int, LocalizedPointsMetaData> in_localizedPointsMetaDataMap, EnclaveTriangle* in_enclaveTriangleRef, bool in_debugFlag)
{
	AdhesiveResults results(in_enclaveTriangleRef->lineArray[0].pointA, in_enclaveTriangleRef->lineArray[1].pointA, in_enclaveTriangleRef->lineArray[2].pointA);
	results.applyMetaDataMap(in_localizedPointsMetaDataMap);
	if (results.scanResults() == true)
	{
		if (in_debugFlag == true)
		{
			std::cout << "###++++ NOTICE: this enclave triangle requires reform!! " << std::endl;
		}
		// initialize reform...
		in_enclaveTriangleRef->reform(results.pointData[0].currentMatchedPointValue, results.pointData[1].currentMatchedPointValue, results.pointData[2].currentMatchedPointValue);
	}
}

std::vector<ECBPolyPoint> AdhesiveRunner::acquireLocalizedPointsFromDiscoveredORELocation(DiscoveredORELocation in_discoveredORELocation, bool in_debugFlag)
{
	std::vector<ECBPolyPoint> returnVector;
	UniquePointVector uniquePoints;
	if (in_discoveredORELocation.direction == EuclideanDirection3D::POS_X)		// if we're looking towards the EAST, points lying in the neighboring OrganicRawEnclave must lie on the WEST face of the OrganicRawEnlave (x must be = 0); the localized X value of each of these points will be 4
	{
		//std::cout << "Entered POS_X" << std::endl;
		
		OrganicRawEnclave* neighborRawEnclaveRef = &in_discoveredORELocation.enclaveFractureResultsMapRef->fractureResultsContainerMap[in_discoveredORELocation.keyInNeighboringBlueprint];	// get a ref to the ORE to compare against
		auto etcSGMBegin = neighborRawEnclaveRef->etcSGM.begin();
		auto etcSGMEnd = neighborRawEnclaveRef->etcSGM.end();
		for (; etcSGMBegin != etcSGMEnd; etcSGMBegin++)
		{
			auto neighborTrianglesBegin = etcSGMBegin->second.containerMap.begin();	// cycle through each EnclaveTriangleContainer
			auto neighborTrianglesEnd = etcSGMBegin->second.containerMap.end();	// cycle through each EnclaveTriangleContainer
			for (; neighborTrianglesBegin != neighborTrianglesEnd; neighborTrianglesBegin++)
			{
				// cycle through each EnclaveTriangle in the EnclaveTriangleContainer
				auto currentNeighborTriangleBegin = neighborTrianglesBegin->second.triangles.begin();
				auto currentNeighborTriangleEnd = neighborTrianglesBegin->second.triangles.end();
				for (; currentNeighborTriangleBegin != currentNeighborTriangleEnd; currentNeighborTriangleBegin++)
				{
					// cycle through each point in the triangle, via point A in its lineArray
					for (int x = 0; x < 3; x++)
					{
						ECBPolyPoint currentPoint = currentNeighborTriangleBegin->second.lineArray[x].pointA;
						if (currentPoint.x == 0.0f) // it is on the WEST border in the neighboring ORE

						{
							ECBPolyPoint localizedPoint = currentPoint; // copy, 
							localizedPoint.x = 4.0f;				    // then translate to localized coordinates, making it on the EAST border of the target ORE we're modifying
							uniquePoints.insertPoint(localizedPoint);
						}
					}
				}
			}
		}
	}

	else if (in_discoveredORELocation.direction == EuclideanDirection3D::POS_Z)	// if we're looking towards the NORTH, points lying in the neighboring OrganicRawEnclave must lie on the SOUTH face of the OrganicRawEnclave (z must be = 0); the localized Z value of each of these points will be 4
	{
		if (in_debugFlag == true)
		{
			std::cout << "Entered POS_Z" << std::endl;
		}
		OrganicRawEnclave* neighborRawEnclaveRef = &in_discoveredORELocation.enclaveFractureResultsMapRef->fractureResultsContainerMap[in_discoveredORELocation.keyInNeighboringBlueprint];	// get a ref to the ORE to compare against
		auto etcSGMBegin = neighborRawEnclaveRef->etcSGM.begin();
		auto etcSGMEnd = neighborRawEnclaveRef->etcSGM.end();
		for (; etcSGMBegin != etcSGMEnd; etcSGMBegin++)
		{
			auto neighborTrianglesBegin = etcSGMBegin->second.containerMap.begin();	// cycle through each EnclaveTriangleContainer
			auto neighborTrianglesEnd = etcSGMBegin->second.containerMap.end();	// cycle through each EnclaveTriangleContainer
			for (; neighborTrianglesBegin != neighborTrianglesEnd; neighborTrianglesBegin++)
			{
				// cycle through each EnclaveTriangle in the EnclaveTriangleContainer
				auto currentNeighborTriangleBegin = neighborTrianglesBegin->second.triangles.begin();
				auto currentNeighborTriangleEnd = neighborTrianglesBegin->second.triangles.end();
				for (; currentNeighborTriangleBegin != currentNeighborTriangleEnd; currentNeighborTriangleBegin++)
				{
					// cycle through each point in the triangle, via point A in its lineArray
					if (in_debugFlag == true)
					{
						if (currentNeighborTriangleBegin->second.isTriangleValid == false)
						{
							std::cout << "Warning, comparing against an INVALID triangle! " << std::endl;
						}
						std::cout << "> Empty normal is: " << currentNeighborTriangleBegin->second.emptyNormal.x << ", " << currentNeighborTriangleBegin->second.emptyNormal.y << ", " << currentNeighborTriangleBegin->second.emptyNormal.z << std::endl;
					}

					for (int x = 0; x < 3; x++)
					{
						ECBPolyPoint currentPoint = currentNeighborTriangleBegin->second.lineArray[x].pointA;
						if (in_debugFlag == true)
						{
							std::cout << "> Current point is: " << currentPoint.x << ", " << currentPoint.y << ", " << currentPoint.x << std::endl;
						}
						if (currentPoint.z == 0.0f) // it is on the SOUTH border in the neighboring ORE
						{
							ECBPolyPoint localizedPoint = currentPoint; // copy, 
							localizedPoint.z = 4.0f;				    // then translate to localized coordinates, making it on the NORTH border of the target ORE we're modifying
							uniquePoints.insertPoint(localizedPoint);
						}
					}
				}
			}
		}
	}

	else if (in_discoveredORELocation.direction == EuclideanDirection3D::NEG_X) // if we're looking towards the WEST, points lying in the neighboring OrganicRawEnclave must lie on the EAST face of the OrganicRawEnclave (x must be = 4); the localized X value of each of these points will be 0
	{
		//std::cout << "Entered NEG_X" << std::endl;


		OrganicRawEnclave* neighborRawEnclaveRef = &in_discoveredORELocation.enclaveFractureResultsMapRef->fractureResultsContainerMap[in_discoveredORELocation.keyInNeighboringBlueprint];	// get a ref to the ORE to compare against
		auto etcSGMBegin = neighborRawEnclaveRef->etcSGM.begin();
		auto etcSGMEnd = neighborRawEnclaveRef->etcSGM.end();
		for (; etcSGMBegin != etcSGMEnd; etcSGMBegin++)
		{
			auto neighborTrianglesBegin = etcSGMBegin->second.containerMap.begin();	// cycle through each EnclaveTriangleContainer
			auto neighborTrianglesEnd = etcSGMBegin->second.containerMap.end();	// cycle through each EnclaveTriangleContainer
			for (; neighborTrianglesBegin != neighborTrianglesEnd; neighborTrianglesBegin++)
			{
				// cycle through each EnclaveTriangle in the EnclaveTriangleContainer
				auto currentNeighborTriangleBegin = neighborTrianglesBegin->second.triangles.begin();
				auto currentNeighborTriangleEnd = neighborTrianglesBegin->second.triangles.end();
				for (; currentNeighborTriangleBegin != currentNeighborTriangleEnd; currentNeighborTriangleBegin++)
				{
					// cycle through each point in the triangle, via point A in its lineArray
					for (int x = 0; x < 3; x++)
					{
						ECBPolyPoint currentPoint = currentNeighborTriangleBegin->second.lineArray[x].pointA;
						if (currentPoint.x == 4.0f) // it is on the EAST border in the neighboring ORE
						{
							ECBPolyPoint localizedPoint = currentPoint; // copy, 
							localizedPoint.x = 0.0f;				    // then translate to localized coordinates, making it on the WEST border of the target ORE we're modifying
							uniquePoints.insertPoint(localizedPoint);
						}
					}
				}
			}
		}

		//std::cout << "finished NEG_X" << std::endl;
	}

	else if (in_discoveredORELocation.direction == EuclideanDirection3D::NEG_Z) // if we're looking towards the SOUTH, points lying in the neighboring OrganicRawEnclave must lie on the NORTH face of the OrganicRawEnclave (z must be = 4); the localized X value of each of these points will be 0
	{
		//std::cout << "Entered NEG_Z" << std::endl;


		OrganicRawEnclave* neighborRawEnclaveRef = &in_discoveredORELocation.enclaveFractureResultsMapRef->fractureResultsContainerMap[in_discoveredORELocation.keyInNeighboringBlueprint];	// get a ref to the ORE to compare against
		auto etcSGMBegin = neighborRawEnclaveRef->etcSGM.begin();
		auto etcSGMEnd = neighborRawEnclaveRef->etcSGM.end();
		for (; etcSGMBegin != etcSGMEnd; etcSGMBegin++)
		{
			auto neighborTrianglesBegin = etcSGMBegin->second.containerMap.begin();	// cycle through each EnclaveTriangleContainer
			auto neighborTrianglesEnd = etcSGMBegin->second.containerMap.end();	// cycle through each EnclaveTriangleContainer
			for (; neighborTrianglesBegin != neighborTrianglesEnd; neighborTrianglesBegin++)
			{
				// cycle through each EnclaveTriangle in the EnclaveTriangleContainer
				auto currentNeighborTriangleBegin = neighborTrianglesBegin->second.triangles.begin();
				auto currentNeighborTriangleEnd = neighborTrianglesBegin->second.triangles.end();
				for (; currentNeighborTriangleBegin != currentNeighborTriangleEnd; currentNeighborTriangleBegin++)
				{
					// cycle through each point in the triangle, via point A in its lineArray
					for (int x = 0; x < 3; x++)
					{
						ECBPolyPoint currentPoint = currentNeighborTriangleBegin->second.lineArray[x].pointA;
						if (currentPoint.z == 4.0f) // it is on the NORTH border in the neighboring ORE
						{
							ECBPolyPoint localizedPoint = currentPoint; // copy, 
							localizedPoint.z = 0.0f;				    // then translate to localized coordinates, making it on the SOUTH border of the target ORE we're modifying
							uniquePoints.insertPoint(localizedPoint);
						}
					}
				}
			}
		}
	}

	else if (in_discoveredORELocation.direction == EuclideanDirection3D::POS_Y) // if we're looking towards the ABOVE, points lying in the neighboring OrganicRawEnclave must lie on the BELOW face of the OrganicRawEnclave (y must be = 0); the localized Y value of each of these points will be 4
	{
		//std::cout << "Entered POS_Y" << std::endl;


		OrganicRawEnclave* neighborRawEnclaveRef = &in_discoveredORELocation.enclaveFractureResultsMapRef->fractureResultsContainerMap[in_discoveredORELocation.keyInNeighboringBlueprint];	// get a ref to the ORE to compare against
		auto etcSGMBegin = neighborRawEnclaveRef->etcSGM.begin();
		auto etcSGMEnd = neighborRawEnclaveRef->etcSGM.end();
		for (; etcSGMBegin != etcSGMEnd; etcSGMBegin++)
		{
			auto neighborTrianglesBegin = etcSGMBegin->second.containerMap.begin();	// cycle through each EnclaveTriangleContainer
			auto neighborTrianglesEnd = etcSGMBegin->second.containerMap.end();	// cycle through each EnclaveTriangleContainer
			for (; neighborTrianglesBegin != neighborTrianglesEnd; neighborTrianglesBegin++)
			{
				// cycle through each EnclaveTriangle in the EnclaveTriangleContainer
				auto currentNeighborTriangleBegin = neighborTrianglesBegin->second.triangles.begin();
				auto currentNeighborTriangleEnd = neighborTrianglesBegin->second.triangles.end();
				for (; currentNeighborTriangleBegin != currentNeighborTriangleEnd; currentNeighborTriangleBegin++)
				{
					// cycle through each point in the triangle, via point A in its lineArray
					for (int x = 0; x < 3; x++)
					{
						ECBPolyPoint currentPoint = currentNeighborTriangleBegin->second.lineArray[x].pointA;
						if (currentPoint.y == 0.0f) // it is on the BELOW border in the neighboring ORE
						{
							ECBPolyPoint localizedPoint = currentPoint; // copy, 
							localizedPoint.y = 4.0f;				    // then translate to localized coordinates, making it on the ABOVE border of the target ORE we're modifying
							uniquePoints.insertPoint(localizedPoint);
						}
					}
				}
			}
		}
	}

	else if (in_discoveredORELocation.direction == EuclideanDirection3D::NEG_Y)	// if we're looking towards the BELOW, points lying in the neighboring OrganicRawEnclave must lie on the ABOVE face of the OrganicRawEnclave (y must be = 4); the localized Y value of each of these points will be 0
	{
		//std::cout << "Entered NEG_Y" << std::endl;


		OrganicRawEnclave* neighborRawEnclaveRef = &in_discoveredORELocation.enclaveFractureResultsMapRef->fractureResultsContainerMap[in_discoveredORELocation.keyInNeighboringBlueprint];	// get a ref to the ORE to compare against
		auto etcSGMBegin = neighborRawEnclaveRef->etcSGM.begin();
		auto etcSGMEnd = neighborRawEnclaveRef->etcSGM.end();
		for (; etcSGMBegin != etcSGMEnd; etcSGMBegin++)
		{
			auto neighborTrianglesBegin = etcSGMBegin->second.containerMap.begin();	// cycle through each EnclaveTriangleContainer
			auto neighborTrianglesEnd = etcSGMBegin->second.containerMap.end();	// cycle through each EnclaveTriangleContainer
			for (; neighborTrianglesBegin != neighborTrianglesEnd; neighborTrianglesBegin++)
			{
				// cycle through each EnclaveTriangle in the EnclaveTriangleContainer
				auto currentNeighborTriangleBegin = neighborTrianglesBegin->second.triangles.begin();
				auto currentNeighborTriangleEnd = neighborTrianglesBegin->second.triangles.end();
				for (; currentNeighborTriangleBegin != currentNeighborTriangleEnd; currentNeighborTriangleBegin++)
				{
					// cycle through each point in the triangle, via point A in its lineArray
					for (int x = 0; x < 3; x++)
					{
						ECBPolyPoint currentPoint = currentNeighborTriangleBegin->second.lineArray[x].pointA;
						if (currentPoint.y == 4.0f) // it is on the ABOVE border in the neighboring ORE
						{
							ECBPolyPoint localizedPoint = currentPoint; // copy, 
							localizedPoint.y = 0.0f;				    // then translate to localized coordinates, making it on the BELOW border of the target ORE we're modifying
							uniquePoints.insertPoint(localizedPoint);
						}
					}
				}
			}
		}
	}

	returnVector = uniquePoints.points;	// store the result, then return
	return returnVector;
}

EnclaveKeyDef::EnclaveKey AdhesiveRunner::findBorderingOrganicRawEnclaveToCompareAgainst(EuclideanDirection3D in_euclideanDirection, EnclaveKeyDef::EnclaveKey in_currentOrganicRawEnclaveKey)
{
	EnclaveKeyDef::EnclaveKey returnKey = in_currentOrganicRawEnclaveKey;	// copy the original key into this value, so we can calibrate it.
	if (in_euclideanDirection == EuclideanDirection3D::POS_X)
	{
		// the original raw enclave had a X value of 7, the one we compare to must have a X value of 0.
		returnKey.x = 0;
	}
	if (in_euclideanDirection == EuclideanDirection3D::POS_Z)
	{
		// the original raw enclave had a Z value of 7, the one we compare to must have a Z value of 0.
		returnKey.z = 0;
	}
	if (in_euclideanDirection == EuclideanDirection3D::NEG_X)
	{
		// the original raw enclave had a X value of 0, the one we compare to must have a X value of 7.
		returnKey.x = 7;
	}
	if (in_euclideanDirection == EuclideanDirection3D::NEG_Z)
	{
		// the original raw enclave had a Z value of 0, the one we compare to must have a Z value of 7.
		returnKey.z = 7;
	}
	if (in_euclideanDirection == EuclideanDirection3D::POS_Y)
	{
		// the original raw enclave had a Y value of 7, the one we compare to must have a Y value of 0.
		returnKey.y = 0;
	}
	if (in_euclideanDirection == EuclideanDirection3D::NEG_Y)
	{
		// the original raw enclave had a Y value of 0, the one we compare to must have a Y value of 7.
		returnKey.y = 7;
	}
	return returnKey;
}