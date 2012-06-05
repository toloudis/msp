/*****************************************************************************
**  SuggestTolerance.cpp
**
**      
**
**	Extra Large Technology
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <SuggestTolerance.hpp>

#include <maya/MArgList.h>
#include <maya/MCommandResult.h>
#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnMesh.h>
#include <maya/MGlobal.h>
#include <maya/MItDag.h>
#include <maya/MItSelectionList.h>
#include <maya/MMatrix.h>
#include <maya/MPointArray.h>
#include <maya/MPlug.h>
#include <maya/MSelectionList.h>

namespace
{	
	const double c_MinEdgePercentage = 0.1;
	const double c_Epsilon = 0.001;
}

void *SuggestTolerance::creator()
{
	return new SuggestTolerance();
}

SuggestTolerance::SuggestTolerance()
{
}

MStatus SuggestTolerance::doIt(const MArgList &args)
{
	try
	{
		return safeDoIt(args);
	}
	catch(...)
	{
		MayaUtil::PrintError("sgpuSuggestTolerance : General exception, command could not complete.");
		return MS::kFailure;
	}
}

MStatus SuggestTolerance::safeDoIt(const MArgList &args)
{
	MStatus status;	

	// Get meshes in selection list
	//
	MSelectionList activeList;
	status = MGlobal::getActiveSelectionList(activeList);
	MItSelectionList iter( activeList, MFn::kMesh );
	if (iter.isDone())
	{
		MGlobal::displayWarning("No meshes selected.");
		setResult(0);
		return MS::kFailure;
	}
	
	bool bFirstValue = true;
	double min_edge_length = 0.0;

	// Iterate through meshes, gathering all materials
	// into one table
	MDagPath dagPath;			
	MObject component;
	for ( ; !iter.isDone(); iter.next() )
	{							
		iter.getDagPath( dagPath, component );
								
		MFnMesh mesh(dagPath, &status);
		if (status)
		{
			MPointArray positions;
			mesh.getPoints(positions, MSpace::kWorld);

			int num_edges = mesh.numEdges();
			int2 vertexList;
			MPoint pt1, pt2;
			for (int e=0; e<num_edges; ++e)
			{ 	
				mesh.getEdgeVertices(e, vertexList);
				pt1 = positions[vertexList[0]];
				pt2 = positions[vertexList[1]];

				double length = pt1.distanceTo(pt2);
				if (length > c_Epsilon)	// skip degenerate edges
				{
					if (bFirstValue)
					{
						min_edge_length = length;
						bFirstValue = false;
					}
					else if (length < min_edge_length)
						min_edge_length = length;
				}
				
			}
		}
	}

	// percentage of minimum edge length is suggested tolerance
	double suggested_tolerance = c_MinEdgePercentage * min_edge_length;
	setResult(suggested_tolerance);

	return MS::kSuccess;
}
