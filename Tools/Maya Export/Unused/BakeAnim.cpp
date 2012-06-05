/*****************************************************************************
**  BakeAnim.cpp
**
**     Command to bake results of ik simulation into each joint at
**	each frame.  This is needed before outputting single skin animation.  
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <BakeAnim.hpp>
#include <SceneFuncs.hpp>

#include <maya/MArgList.h>
#include <maya/MCommandResult.h>
#include <maya/MFileIO.h>
#include <maya/MGlobal.h>


namespace
{
}

void *BakeAnim::creator()
{
	// Return new instance of command
	//
	return new BakeAnim();
}

MStatus BakeAnim::ParseArgs(const MArgList& args)
{
	MStatus     	stat;
	MString     	arg;
	const MString	sampleFlag				("-sb");

	// Parse the arguments.
	for ( size_t i = 0; i < args.length(); i++ ) {
		arg = args.asString( i, &stat );
		if (!stat)              
			continue;
				
		if ( arg == sampleFlag ) {
			sample = args.asInt(i + 1, &stat);
			i++;
		}
		else {
			//just ignore extraneous args
		}
	}
	return stat;
}

MStatus BakeAnim::doIt(const MArgList &args)
{
	if(!ParseArgs(args))
		return MS::kFailure; 

	MCommandResult result;
	MGlobal::executeCommand("playbackOptions -query -min;", result);
	double minval;
	MStatus status = result.getResult(minval);
	MGlobal::executeCommand("playbackOptions -query -max;", result);
	double maxval;
	status = result.getResult(maxval);

	cout << "Baking joint animation frames: " << minval << " to " << maxval << endl;

	char buffer[256];
	if (sample <= 1)
		sprintf(buffer, "bakeResults -t \"%f:%f\" -sm true \"joint*\"", minval, maxval);
	else
		sprintf(buffer, "bakeResults -t \"%f:%f\" -sb %d -sm true \"joint*\"", 
			minval, maxval, sample);

	MGlobal::executeCommand(buffer);
	
	return MS::kSuccess;
}


