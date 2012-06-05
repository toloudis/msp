/*****************************************************************************
**  RemovePivots.cpp
**
**     Command to remove pivot points from hierarchical model,
**	should be done before animating.         
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include <MayaUtil.hpp>
#include <RemovePivots.hpp>

#include <SceneFuncs.hpp>
#include <PivotFuncs.hpp>

#include <maya/MArgList.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnTransform.h>
#include <maya/MItDag.h>



RemovePivots::RemovePivots() 
{
}

RemovePivots::~RemovePivots() 
{

}

void* RemovePivots::creator()
{
	return new RemovePivots;
}

MStatus	RemovePivots::doIt( const MArgList& args )
{
	MItDag::TraversalType	traversalType = MItDag::kDepthFirst;
	MFn::Type				filter        = MFn::kInvalid;
	MStatus					status;
	bool					quiet = false;

	status = parseArgs (args);
	if (!status)
		return status;

	return doScan();
};

MStatus RemovePivots::parseArgs(const MArgList& args)
{
	MStatus     	stat;
	MString     	arg;
	const MString	animFlag			("-anim");
	const MString	geomFlag			("-geom");

	// Parse the arguments.
	for ( size_t i = 0; i < args.length(); i++ ) {
		arg = args.asString( i, &stat );
		if (!stat)              
			continue;
				
		// ignore args
	}
	return stat;
}


MStatus RemovePivots::doScan()
{   
	MayaUtil::SetWaitCursor();

	MStatus status;
	
	cout << "-----------------------------------------" << endl;
	
	// Iterate through scene
	//
	MItDag dagIterator( MItDag::kBreadthFirst, MFn::kInvalid, &status);

	if ( !status) 
	{
		MayaUtil::PrintError("MItDag constructor");
		MayaUtil::UnsetWaitCursor();
		return status;
	}

	
	int xformCount = 0;
	MString xformName;
	for ( ; !dagIterator.isDone(); dagIterator.next() ) 
	{

		MDagPath dagPath;

		status = dagIterator.getPath(dagPath);
		if ( !status ) 
		{
			status.perror("MItDag::getPath");
			continue;
		}

		MFnDagNode dagNode(dagPath, &status);
		if ( !status ) 
		{
			status.perror("MFnDagNode constructor");
			continue;
		}

		if (dagPath.hasFn(MFn::kTransform)) 
		{
			//this will be slow for deep hierarchies
			//
			MFnTransform transform(dagPath, &status);
			if (status == MS::kSuccess)
			{
				PivotFuncs::RemoveXformPivots(transform);
			}

			dagIterator.prune();

		}

	}

	MayaUtil::PrintStatus("Done.");
	
	MayaUtil::UnsetWaitCursor();

	return MS::kSuccess;
}
