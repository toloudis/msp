/*****************************************************************************
**  WriteParticles.cpp
**
**     Command to write vertex animation of particles.     
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include <MayaUtil.hpp>
#include <WriteParticles.hpp>
#include <AnimFuncs.hpp>
#include <SceneFuncs.hpp>
#include <VertexFuncs.hpp>

#include <maya/MArgList.h>
#include <maya/MCommandResult.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnParticleSystem.h>
#include <maya/MFnTransform.h>
#include <maya/MGlobal.h>
#include <maya/MItDag.h>
#include <maya/MItSelectionList.h>
#include <maya/MPlug.h>
#include <maya/MSelectionList.h>

#undef CreateFile
#undef DeleteFile

#include "Core/ch/chBinWriter.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/gf/gfFileBin.hpp"



WriteParticles::WriteParticles() 
{
	bDoGeom = false;
	bDoAnim = true;
	bConfirmFlags = true;
	bSuggest = false;
	end_frame = -1;
	begin_frame = 0;
	frame_step = 1;
}

WriteParticles::~WriteParticles() 
{

}

void* WriteParticles::creator()
{
	return new WriteParticles;
}

MStatus	WriteParticles::doIt( const MArgList& args )
{
	try
	{
		return safeDoIt(args);
	}
	catch (envExceptionX &i_Ex)
	{
		std::string msg = "sgpuWriteParticles Error : " + i_Ex.GetErrorMessage();
		MayaUtil::PrintError(msg.c_str());
		return MS::kFailure;
	}
	catch(...)
	{
		MayaUtil::PrintError("sgpuWriteParticles : General exception, command could not complete.");
		return MS::kFailure;
	}
}

MStatus WriteParticles::safeDoIt(const MArgList &args)
{
	MItDag::TraversalType	traversalType = MItDag::kDepthFirst;
	MFn::Type				filter        = MFn::kInvalid;
	MStatus					status;
	bool					quiet = false;

	cout << "-----------------------------------------" << endl;

	status = parseArgs (args);
	if (!status)
		return status;

	return doScan();
};

MStatus WriteParticles::parseArgs(const MArgList& args)
{
	MStatus     	stat;
	MString     	arg;
	const MString	animFlag			("-anim");
	const MString	geomFlag			("-geom");
	const MString	fileFlag			("-f");
	const MString	fileFlagLongOld		("-File");
	const MString	fileFlagLong		("-file");
	const MString	endFrameFlag		("-endframe");
	const MString	beginFrameFlag		("-beginframe");
	const MString	frameStepFlag		("-framestep");
	const MString	confirmFlag			("-confirm");
	const MString	suggestFlag			("-suggest");

	// Parse the arguments.
	for ( size_t i = 0; i < args.length(); i++ ) {
		arg = args.asString( i, &stat );
		if (!stat)              
			continue;
				
		if ( arg == fileFlag || arg == fileFlagLong  || arg == fileFlagLongOld) {
			filePath = args.asString(i + 1, &stat);
			i++;
		}
		else if ( arg == animFlag  ) {
			bDoAnim = true;
		}
		else if ( arg == geomFlag  ) {
			bDoGeom = true;
		}
		else if (arg == confirmFlag ) {
			bConfirmFlags = false;
		}
		else if (arg == suggestFlag ) {
			bSuggest = true;
		}
		else if ( arg == endFrameFlag ) {
			end_frame = args.asInt(i + 1, &stat);
			i++;
		}
		else if ( arg == beginFrameFlag ) {
			begin_frame = args.asInt(i + 1, &stat);
			i++;
		}
		else if ( arg == frameStepFlag ) {
			frame_step = args.asInt(i + 1, &stat);
			i++;
		}
		else if ( BakeCommand::ParseArg(args, i) ) {
			// BakeCommand read a frame start or frame end flag
		}
		else {
			//just ignore extraneous args
		}
	}
	return stat;
}


MStatus WriteParticles::doScan()
{   
	// In "-confirm" mode, don't display errors
	MayaUtil::SetQuietMode(!bConfirmFlags);
	MayaUtil::SetSuggestMode(bSuggest);

	MayaUtil::SetWaitCursor();

	MStatus status;

	// Get default slider range, allow it to be changed by the arguments
	double minTime, maxTime;
	BakeCommand::GetTimelineRange(minTime, maxTime);
	begin_frame = (int) minTime;
	end_frame = (int) maxTime;
	frame_step = 1;
	
	if (bDoGeom || !bDoAnim)
	{
		MGlobal::displayWarning("Can only export particle animation for now, use -anim flag");
		return MS::kFailure;
	}

	if (bDoAnim && (end_frame < 0))
	{
		MGlobal::displayWarning("Need to set -endframe flag to length of animation.");
		return MS::kFailure;
	}

	// Get particles in selection list
	//
	MSelectionList activeList;
	status = MGlobal::getActiveSelectionList(activeList);
	MItSelectionList iter( activeList, MFn::kParticle );

	if (iter.isDone())
	{
		MGlobal::displayWarning("No particles selected.");
		return MS::kFailure;
	}
	
	// Get filename into which to save particles
	//
	MString path;
	if (filePath.length() > 0)
	{
		cout << "Have filename from command line " << endl;
		path = filePath;
	}
	else
	{
		cout << "Asking for filename." << endl;

		MString cmd;
		if (bDoGeom)
		{
			//cmd = "getFilenameDialog -ext \"*.ptx\";";
			cmd = "fileDialog -m 1 -dm \"*.ptx\";";
		}
		else if (bDoAnim)
		{
			//cmd = "getFilenameDialog -ext \"*.pta\";";
			cmd = "fileDialog -m 1 -dm \"*.pta\";";
		}
		else
		{
			cout << "WriteParticles: Neither geom or anim chosen to write" << endl;
			return MS::kFailure;
		}

		MCommandResult result;
		MGlobal::executeCommand(cmd, result);
		status = result.getResult(path);
	}

	if ((status != MS::kSuccess) || (path.length() == 0))
	{
		cout << "WriteParticles: No file path specified." << endl;
		return MS::kFailure;
	}

	// Open file
	fsLocator locator;
	if (!MayaUtil::CreateFile(path, (bDoGeom) ? ".ptx" : ".pta", locator))
	{
		MGlobal::displayWarning("file is READ-ONLY cannot save");
		return MS::kFailure;
	}

	gfFileBin file(locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
	file.WriteHeader();
			
	chBinWriter writer(file);

	
	MDagPath dagPath;			
	MObject component;
	MString efficiencyMsg;

	//if (bDoGeom)
	//{
	//	// Output particles into file
	//	//
	//	for ( ; !iter.isDone(); iter.next() )
	//	{							
	//		iter.getDagPath( dagPath, component );
	//								
	//		MFnParticleSystem particle(dagPath, &status);
	//		if (status)
	//		{
	//			//cout << "found particle!!!" << endl;
	//			const bool bWorldSpace = true;
	//			SceneFuncs::WriteParticleToFile(particle, writer, bWorldSpace);

	//			// Check for efficiencies, gather a message to 
	//			// be reported in dialog
	//			SceneFuncs::GatherWarningMessages(particle, efficiencyMsg);
	//		}
	//	}	

	//}
	//else 
	if (bDoAnim)
	{
		// Write in the frame rate in small chunk at top
		AnimFuncs::WriteCurrentFrameRate(writer);

		// Write start frame
		AnimFuncs::WriteBeginFrame(writer, (float)begin_frame);

		// Generate our key steps instead of looking for baked keys
		std::list<float> keys;
		for (int i=begin_frame; i<=end_frame; i+=frame_step)
		{
			keys.push_back( (float) i);
		}

		// Output vertex animation
		//
		for ( ; !iter.isDone(); iter.next() )
		{							
			iter.getDagPath( dagPath, component );
									
			MFnParticleSystem particle(dagPath, &status);
			if (status)
			{
				//cout << "found particle!!!" << endl;
				VertexFuncs::WriteVertexAnimation(particle, writer, keys, (float)begin_frame);
			}
		}	

	}

	// Stamp version into end of file
	SceneFuncs::WriteExporterVersionStamp(writer);

	MayaUtil::PrintStatus("Done.");

	if (bSuggest && (efficiencyMsg.length() > 0))
	{
		// Confirmation dialog.
		MayaUtil::DisplayConfirmation( efficiencyMsg, "Confirm flags" );
	}

	MayaUtil::UnsetWaitCursor();

	return MS::kSuccess;
}
