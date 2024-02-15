#pragma once

#ifndef CPV2MOUNTAIN_H
#define CPV2MOUNTAIN_H

#include "ContouredPlanV2Base.h"
#include "ContouredCircuit.h"
#include "BPMassManagerV2.h"

/*

Description: not really a "mountain", but a CPV2 that starts at a summit, and creates more layers of the mountain, going downward;
the bottom of the mountain is float. Essentially mimics a cone.

*/

class CPV2Mountain : public ContouredPlanV2Base
{
	public:
		// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
		// ||||||||||||| Public virtual functions from ContouredPlanV2Base.
		// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

		// Below, Step 1: set up the contour points that will be later amplified (by the call to amplifyAllContourLinePoints),
		// add the mass reference points (MRP), and set the number of layers for the "mountain." Must be called in addPlanV2.
		void initialize(DoublePoint in_startPoint,			
						int in_numberOfLayers,
						float in_distanceBetweenLayers,
						float in_startRadius,
						float in_expansionValue);

		void initializeFromMessage(Message in_messageToInitFrom);	// NOTE: remember, the plan name (string) and OSTerrainFormation enum (int) shouldn't be in this Message!
																	// Expects a Message that is formatted in the following manner and order:
																	//
																	// int:
																	//	1.	Number of layers for the mountain
																	//
																	// float:
																	//	1.	Summit point (3 floats, use readPoint)
																	//	2.	distance between layers (1 float)
																	//	3.	start radius (1 float)
																	//	4.	expansion value (1 float)


		void amplifyAllContourLinePoints(); // Step 2: assuming the plan was added and initialized via a call to addPlanV2, amplify the points. "Amplify," in this context,
											// really means to multiply each circuit point by an amount of radians, to have the points in a circuit form a circle.									

		void buildContouredTriangles();		// Step 3: build the individual ContouredTriangleV2 instances for the top and bottom strips, now that the points have been amplified.

		std::vector<ContouredTriangleV2*> getProcessableContouredTriangles();	// Step 4: Used in the call to executePlanV2, retrieve a vector of ContouredTriangleV2 pointers that refer to the produced
																				// ContouredTriangleV2 instances from the triangle strips.

		// Below, Step 5: the CPV2Mountain mesh will be used to created contoured mass; the contoured mass will then have mass-driving ops run against it, and 
		// then be compared against the persistent mass. This function will also add EnclaveTriangle data produced from the contoured mesh, into the appropriate OREs
		// of the target blueprint map.
		void runMassDriversV2(OrganicClient* in_clientRef,
								ECBMap* in_ecbMapRef,
								EnclaveFractureResultsMap* in_fractureResultsMapRef);
	private:
		map<int, ContouredCircuit> upperContourLineCircuits;
		map<int, ContouredCircuit> bottomContourLineCircuits;

		map<int, DoublePoint> triangleUpperStripMRPMap;
		map<int, DoublePoint> triangleBottomStripMRPMap;

		float currentRadius = 0;
		float currentNumberOfPoints = 0;
		int topCircuitCount = 0;			// the number of circuits in the upperContourLineCircuits
		int bottomCircuitCount = 0;			// the number of circuits in the bottomContourLineCircuits
		DoublePoint startPoint;			// startPoint for the top layer of the contoured mountain
		DoublePoint bottomStartPoint;      // startPoint for the bottom layer of the contoured mountain
		int numberOfLayers = 0;				// the number of layers in the mountain.

		// plan-specific functions for CPV2Mountain
		void setMountainUpperLayersContourPoints(DoublePoint in_startPoint, 
										int in_numberOfLayers, 
										float in_distanceBetweenLayers, 
										float in_startRadius, 
										float in_expansionValue);

		void setMountainLowerLayersContourPoints(DoublePoint in_startPoint,
										int in_numberOfLayers,
										float in_distanceBetweenLayers,
										float in_startRadius,
										float in_expansionValue);

		void insertDoubleMRP(std::map<int, DoublePoint>* in_doubleMRPMapRef, 
							int in_stripValue, 
							DoublePoint in_doublePoint);


		void addContouredCircuit(std::map<int, ContouredCircuit>* in_contouredCircuitMapRef,	// add a circuit, and increment the value of bottomContourLineCount by 1.
								int* in_circuitCounterRef,
								float in_baseDistance, 
								double in_contourElevation, 
								int in_numberOfPoints, 
								DoublePoint in_startPoint);

		void setMRPsForBottomLayers();

		void constructMountainTopStripTriangles(int in_stripID);
		void constructMountainBottomStripTriangles(int in_stripID);
		void constructStripTrianglesForLayer(std::map<int, ContouredCircuit>* in_circuitMapRef,
											std::map<int, DoublePoint>* in_triangleStripMRPMapRef,
											int in_stripID,
											TriangleMaterial in_materialID,
											std::unordered_map<int, CTV2Strip>* in_contouredTriangleStripRef,
											ECBPolyType in_polyType,
											DoublePoint in_startPoint);

		void constructTriangle(std::unordered_map<int, CTV2Strip>* in_contouredTriangleStripRef,
							DoublePoint in_point0,
							DoublePoint in_point1,
							DoublePoint in_point2,
							DoublePoint in_massReferencePoint,
							int in_triangleStripID,
							TriangleMaterial in_materialID,
							ECBPolyType in_polyType);

		void constructOuterQuadrantShell(std::unordered_map<int, CTV2Strip>* in_contouredTriangleStripRef,
										ContouredCircuit* in_currentCircuit,
										ContouredCircuit* in_previousCircuit, 
										int in_pointsPerQuadrant, 
										int in_quadrantID, 
										int in_triangleStripID, 
										TriangleMaterial in_materialID, 
										DoublePoint in_mrp, 
										ECBPolyType in_type);

		void constructInnerQuadrantShell(std::unordered_map<int, CTV2Strip>* in_contouredTriangleStripRef,
										ContouredCircuit* in_currentCircuit,
										ContouredCircuit* in_previousCircuit,
										int in_pointsPerQuadrant,
										int in_quadrantID,
										int in_triangleStripID,
										TriangleMaterial in_materialID,
										DoublePoint in_mrp,
										ECBPolyType in_type);

		void amplifyContourLinePoints(int in_lineID);

};

#endif
