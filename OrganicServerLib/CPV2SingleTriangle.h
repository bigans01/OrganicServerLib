#pragma once

#ifndef CPV2SINGLETRIANGLE_H
#define CPV2SINGLETRIANGLE_H

#include "ContouredPlanV2Base.h"

/*

Description: uses a single ContouredTriangleV2 to send for rendering; primarily for debug / observational purposes;
this class will likely undergo multiple passes in the coming months, to improve its functionality/usefulness. 
For the time being, the triangle uses hardcoded coordinates.

*/

class CPV2SingleTriangle : public ContouredPlanV2Base
{
	public:
		// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
		// ||||||||||||| Public virtual functions from ContouredPlanV2Base.
		// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
		void initialize(DoublePoint in_startPoint,			// does nothing
			int in_numberOfLayers,
			float in_distanceBetweenLayers,
			float in_startRadius,
			float in_expansionValue);

		void initializeFromMessage(Message in_messageToInitFrom);	// unused, but required

		void amplifyAllContourLinePoints();					// does nothing
		void buildContouredTriangles();						// will construct the single triangle, spawnedTriangle
		std::vector<ContouredTriangleV2*> getProcessableContouredTriangles();	// will retrieve the single triangle, spawnedTriangle

		// Below: takes the EnclaveTriangle data that was generated for each ORE that the spawnedTriangle touched,
		// and puts it into the affected blueprints.
		void runMassDriversV2(OrganicClient* in_clientRef,	
			ECBMap* in_ecbMapRef,
			EnclaveFractureResultsMap* in_fractureResultsMapRef);

	private:
		ContouredTriangleV2 spawnedTriangle;	// the single triangle that is used as a basis for the OrganicTriangle to use for 
												// fracturing. 
};

#endif

