#include "stdafx.h"
#include "AdhesiveResults.h"

AdhesiveResults::AdhesiveResults(ECBPolyPoint in_point0, ECBPolyPoint in_point1, ECBPolyPoint in_point2)
{
	PointAdhesiveMetaData point0Data, point1Data, point2Data;

	point0Data.originalPointValue = in_point0;
	point0Data.currentMatchedPointValue = in_point0;	// just to initialize to a default value

	point1Data.originalPointValue = in_point1;
	point1Data.currentMatchedPointValue = in_point1;

	point2Data.originalPointValue = in_point2;
	point2Data.currentMatchedPointValue = in_point2;

	pointData[0] = point0Data;
	pointData[1] = point1Data;
	pointData[2] = point2Data;
}

void AdhesiveResults::applyMetaDataMap(std::map<int, LocalizedPointsMetaData> in_metaDataMap)
{
	// cycle through each meta data set
	auto mapBegin = in_metaDataMap.begin();
	auto mapEnd = in_metaDataMap.end();
	for (; mapBegin != mapEnd; mapBegin++)
	{
		// for each set, check against each pointData
		for (int x = 0; x < 3; x++)
		{
			runLocalizedPointMetaDataAgainstPoint(mapBegin->second, x);
		}

		// when done, set true flags
		//for (int x = 0; x < 3; x++)
		//{

		//}
	}
}

void AdhesiveResults::runLocalizedPointMetaDataAgainstPoint(LocalizedPointsMetaData in_pointMetaData, int in_pointIndex)
{
	if (in_pointMetaData.euclideanDirection == EuclideanDirection3D::POS_X)		// check if the point is on POS_X
	{
		// does the point fit the direction? (X == 4, EAST)
		if (pointData[in_pointIndex].originalPointValue.x == 4.0f)
		{
			// set the touchedYet flag, if it wasn't set.
			if (pointData[in_pointIndex].touchedYet == false)
			{
				pointData[in_pointIndex].touchedYet = true;
				pointData[in_pointIndex].currentDistance = 1000.0f;	// this will never stay this way, could be any large value
			}
			pointData[in_pointIndex].adhesionAttempted = true;

			// if it does, calculate the distance between the localized point and the current point of the triangle.
			// a lower distance overrides a higher distance.
			auto currentLocalizedsBegin = in_pointMetaData.points.begin();
			auto currentLocalizedsEnd = in_pointMetaData.points.end();
			for (; currentLocalizedsBegin != currentLocalizedsEnd; currentLocalizedsBegin++)
			{
				float currentDistance = calculateDistanceBetweenPoints(pointData[in_pointIndex].originalPointValue, *currentLocalizedsBegin);
				if (currentDistance < pointData[in_pointIndex].currentDistance)	// its less than what's in there, so update it.
				{
					pointData[in_pointIndex].currentDistance = currentDistance;	// ""
					pointData[in_pointIndex].currentMatchedPointValue = *currentLocalizedsBegin;	// update the matched point with the one we're currently on
				}
			}
		}
	}
	else if (in_pointMetaData.euclideanDirection == EuclideanDirection3D::POS_Z)
	{
		// does the point fit the direction? (Z == 4, NORTH)
		if (pointData[in_pointIndex].originalPointValue.z == 4.0f)
		{
			// set the touchedYet flag, if it wasn't set.
			if (pointData[in_pointIndex].touchedYet == false)
			{
				pointData[in_pointIndex].touchedYet = true;
				pointData[in_pointIndex].currentDistance = 1000.0f;	// this will never stay this way, could be any large value
			}
			pointData[in_pointIndex].adhesionAttempted = true;

			// if it does, calculate the distance between the localized point and the current point of the triangle.
			// a lower distance overrides a higher distance.
			auto currentLocalizedsBegin = in_pointMetaData.points.begin();
			auto currentLocalizedsEnd = in_pointMetaData.points.end();
			for (; currentLocalizedsBegin != currentLocalizedsEnd; currentLocalizedsBegin++)
			{
				float currentDistance = calculateDistanceBetweenPoints(pointData[in_pointIndex].originalPointValue, *currentLocalizedsBegin);
				if (currentDistance < pointData[in_pointIndex].currentDistance)	// its less than what's in there, so update it.
				{
					pointData[in_pointIndex].currentDistance = currentDistance;	// ""
					pointData[in_pointIndex].currentMatchedPointValue = *currentLocalizedsBegin;	// update the matched point with the one we're currently on
				}
			}
		}
	}
	else if (in_pointMetaData.euclideanDirection == EuclideanDirection3D::NEG_X)
	{
		// does the point fit the direction? (X == 0, WEST)
		if (pointData[in_pointIndex].originalPointValue.x == 0.0f)
		{
			// set the touchedYet flag, if it wasn't set.
			if (pointData[in_pointIndex].touchedYet == false)
			{
				pointData[in_pointIndex].touchedYet = true;
				pointData[in_pointIndex].currentDistance = 1000.0f;	// this will never stay this way, could be any large value
			}
			pointData[in_pointIndex].adhesionAttempted = true;

			// if it does, calculate the distance between the localized point and the current point of the triangle.
			// a lower distance overrides a higher distance.
			auto currentLocalizedsBegin = in_pointMetaData.points.begin();
			auto currentLocalizedsEnd = in_pointMetaData.points.end();
			for (; currentLocalizedsBegin != currentLocalizedsEnd; currentLocalizedsBegin++)
			{
				float currentDistance = calculateDistanceBetweenPoints(pointData[in_pointIndex].originalPointValue, *currentLocalizedsBegin);
				if (currentDistance < pointData[in_pointIndex].currentDistance)	// its less than what's in there, so update it.
				{
					pointData[in_pointIndex].currentDistance = currentDistance;	// ""
					pointData[in_pointIndex].currentMatchedPointValue = *currentLocalizedsBegin;	// update the matched point with the one we're currently on
				}
			}
		}
	}
	else if (in_pointMetaData.euclideanDirection == EuclideanDirection3D::NEG_Z)
	{
		// does the point fit the direction? (Z == 0, SOUTH)
		//std::cout << "Running localized points of type SOUTH (NEG_Z) " << std::endl;
		if (pointData[in_pointIndex].originalPointValue.z == 0.0f)
		{
			// set the touchedYet flag, if it wasn't set.
			if (pointData[in_pointIndex].touchedYet == false)
			{
				pointData[in_pointIndex].touchedYet = true;
				pointData[in_pointIndex].currentDistance = 1000.0f;	// this will never stay this way, could be any large value
			}
			pointData[in_pointIndex].adhesionAttempted = true;

			// if it does, calculate the distance between the localized point and the current point of the triangle.
			// a lower distance overrides a higher distance.
			auto currentLocalizedsBegin = in_pointMetaData.points.begin();
			auto currentLocalizedsEnd = in_pointMetaData.points.end();
			for (; currentLocalizedsBegin != currentLocalizedsEnd; currentLocalizedsBegin++)
			{
				
				float currentDistance = calculateDistanceBetweenPoints(pointData[in_pointIndex].originalPointValue, *currentLocalizedsBegin);
				//std::cout << "!! Comparing against point: " << currentLocalizedsBegin->x << ", " << currentLocalizedsBegin->y << ", " << currentLocalizedsBegin->z << " | distance was: " << currentDistance << std::endl;
				if (currentDistance < pointData[in_pointIndex].currentDistance)	// its less than what's in there, so update it.
				{
					pointData[in_pointIndex].currentDistance = currentDistance;	// ""
					pointData[in_pointIndex].currentMatchedPointValue = *currentLocalizedsBegin;	// update the matched point with the one we're currently on
				}
			}
			//std::cout << "### nearest point to " << pointData[in_pointIndex].originalPointValue.x << ", " << pointData[in_pointIndex].originalPointValue.y << ", " << pointData[in_pointIndex].originalPointValue.z << " at index [" << in_pointIndex  << "] is now: " << pointData[in_pointIndex].currentMatchedPointValue.x << ", " << pointData[in_pointIndex].currentMatchedPointValue.y << ", " << pointData[in_pointIndex].currentMatchedPointValue.z << std::endl;
		}
	}
	else if (in_pointMetaData.euclideanDirection == EuclideanDirection3D::POS_Y)
	{
		// does the point fit the direction? (Y == 4, ABOVE)
		if (pointData[in_pointIndex].originalPointValue.y == 4.0f)
		{
			// set the touchedYet flag, if it wasn't set.
			if (pointData[in_pointIndex].touchedYet == false)
			{
				pointData[in_pointIndex].touchedYet = true;
				pointData[in_pointIndex].currentDistance = 1000.0f;	// this will never stay this way, could be any large value
			}
			pointData[in_pointIndex].adhesionAttempted = true;

			// if it does, calculate the distance between the localized point and the current point of the triangle.
			// a lower distance overrides a higher distance.
			auto currentLocalizedsBegin = in_pointMetaData.points.begin();
			auto currentLocalizedsEnd = in_pointMetaData.points.end();
			for (; currentLocalizedsBegin != currentLocalizedsEnd; currentLocalizedsBegin++)
			{
				float currentDistance = calculateDistanceBetweenPoints(pointData[in_pointIndex].originalPointValue, *currentLocalizedsBegin);
				if (currentDistance < pointData[in_pointIndex].currentDistance)	// its less than what's in there, so update it.
				{
					pointData[in_pointIndex].currentDistance = currentDistance;	// ""
					pointData[in_pointIndex].currentMatchedPointValue = *currentLocalizedsBegin;	// update the matched point with the one we're currently on
				}
			}
		}
	}
	else if (in_pointMetaData.euclideanDirection == EuclideanDirection3D::NEG_Y)
	{
		// does the point fit the direction? (Y == 0, BELOW)
		if (pointData[in_pointIndex].originalPointValue.y == 0.0f)
		{
			// set the touchedYet flag, if it wasn't set.
			if (pointData[in_pointIndex].touchedYet == false)
			{
				pointData[in_pointIndex].touchedYet = true;
				pointData[in_pointIndex].currentDistance = 1000.0f;	// this will never stay this way, could be any large value
			}
			pointData[in_pointIndex].adhesionAttempted = true;

			// if it does, calculate the distance between the localized point and the current point of the triangle.
			// a lower distance overrides a higher distance.
			auto currentLocalizedsBegin = in_pointMetaData.points.begin();
			auto currentLocalizedsEnd = in_pointMetaData.points.end();
			for (; currentLocalizedsBegin != currentLocalizedsEnd; currentLocalizedsBegin++)
			{
				float currentDistance = calculateDistanceBetweenPoints(pointData[in_pointIndex].originalPointValue, *currentLocalizedsBegin);
				if (currentDistance < pointData[in_pointIndex].currentDistance)	// its less than what's in there, so update it.
				{
					pointData[in_pointIndex].currentDistance = currentDistance;	// ""
					pointData[in_pointIndex].currentMatchedPointValue = *currentLocalizedsBegin;	// update the matched point with the one we're currently on
				}
			}
		}
	}
}

float AdhesiveResults::calculateDistanceBetweenPoints(ECBPolyPoint in_pointA, ECBPolyPoint in_pointB)
{
	float returnVal = 0.0f;

	//std::cout << "Comparing A: (" << in_pointA.x << ", " << in_pointA.y << ", " << in_pointA.z << ") to B: (" << in_pointB.x << ", " << in_pointB.y << ", " << in_pointB.z << ") " << std::endl;
	/*
	if  // most of the time, the below will NOT qualify
	(
		((in_pointB.x - in_pointA.x) != 0.0f)
		&&
		((in_pointB.y - in_pointA.y) != 0.0f)
		&&
		((in_pointB.z - in_pointA.z) != 0.0f)
	)
	{
		std::cout << "### Condition 1 entered " << std::endl;
		float dist_x = pow((in_pointB.x - in_pointA.x), 2.0);
		float dist_y = pow((in_pointB.y - in_pointA.y), 2.0);
		float dist_z = pow((in_pointB.z - in_pointA.z), 2.0);
		returnVal =  sqrt(dist_x + dist_y + dist_z);
	}
	else // the points match equally
	{
		returnVal = 0;
	}
	*/
	//std::cout << "### Condition 1 entered " << std::endl;
	float dist_x = pow((in_pointB.x - in_pointA.x), 2.0);
	float dist_y = pow((in_pointB.y - in_pointA.y), 2.0);
	float dist_z = pow((in_pointB.z - in_pointA.z), 2.0);
	returnVal = sqrt(dist_x + dist_y + dist_z);
	return returnVal;
}

bool AdhesiveResults::scanResults()
{
	bool requiresReform = false;	// most f the time, this value will be false.
	for (int x = 0; x < 3; x++)
	{
		// was the point touched? if it was, check the distance
		if (pointData[x].touchedYet == true)
		{
			//std::cout << "### current distance for this touched point is: " << pointData[x].currentDistance << std::endl;
			if (pointData[x].currentDistance != 0.0f)
			{
				requiresReform = true;		// a non-zero value indicates that the closest point wasn't a match to what was originally there, so we will have to reform the EnclaveTriangle.
			}
		}
	}
	return requiresReform;
}