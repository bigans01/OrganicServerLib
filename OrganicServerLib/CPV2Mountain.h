#pragma once

#ifndef CPV2MOUNTAIN_H
#define CPV2MOUNTAIN_H

#include "ContouredPlanV2Base.h"
#include "ContouredCircuit.h"

class CPV2Mountain : public ContouredPlanV2Base
{
	public:
		void initialize(DoublePoint in_startPoint,			// Step 1: initializer function that must be called once the derived class is instantiated.
						int in_numberOfLayers,
						float in_distanceBetweenLayers,
						float in_startRadius,
						float in_expansionValue);

		void amplifyAllContourLinePoints();
		void buildContouredTriangles();
		std::vector<ContouredTriangleV2*> getProcessableContouredTriangles();
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
