/*****************************************************************************
**  ShadeAmbient.cpp
**
**     Command to compute ambient occlusion term at each vertex,
**	putting value in color per vertex.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <ShadeAmbient.hpp>
#include <ShadeUtil.hpp>

#include <maya/MArgList.h>
#include <maya/MCommandResult.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnMesh.h>
#include <maya/MGlobal.h>
#include <maya/MItSelectionList.h>
#include <maya/MPlug.h>
#include <maya/MSelectionList.h>

namespace
{
}

void *ShadeAmbient::creator()
{
	// Return new instance of command
	//
	return new ShadeAmbient();
}

MStatus ShadeAmbient::ParseArgs(const MArgList& args)
{
	MStatus     	stat;
	MString     	arg;
	//const MString	sampleFlag				("-sb");

	// Parse the arguments.
	for ( size_t i = 0; i < args.length(); i++ ) {
		arg = args.asString( i, &stat );
		if (!stat)              
			continue;
				
		//if ( arg == sampleFlag ) {
		//	sample = args.asInt(i + 1, &stat);
		//	i++;
		//}
		//else {
		//	//just ignore extraneous args
		//}
	}
	return stat;
}

MStatus ShadeAmbient::doIt(const MArgList &args)
{
	if(!ParseArgs(args))
		return MS::kFailure; 

	MStatus status;

	// Just do selected meshes
	//
	MSelectionList activeList;
	status = MGlobal::getActiveSelectionList(activeList);
	MItSelectionList iter( activeList, MFn::kMesh );


	if (iter.isDone())
	{
		MGlobal::displayWarning("No meshes selected.");
		return MS::kFailure;
	}

	// Remove old colors
	MGlobal::executeCommand("polyColorPerVertex -rem");

	MayaUtil::SetWaitCursor();

	// Process meshes
	//
	MDagPath dagPath;			
	MObject component;
	for ( ; !iter.isDone(); iter.next() )
	{							
		iter.getDagPath( dagPath, component );
								
		MFnMesh mesh(dagPath, &status);
		if (status)
		{
			ShadeUtil::ComputeAmbientOcclusion(mesh);
		}
	}	

	MayaUtil::PrintStatus("Done.");
	MayaUtil::UnsetWaitCursor();

	return MS::kSuccess;
}


