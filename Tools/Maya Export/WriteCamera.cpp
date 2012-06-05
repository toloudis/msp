/*****************************************************************************
**  WriteCamera.cpp
**
**      
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <WriteCamera.hpp>
#include <AnimFuncs.hpp>
#include <AnimKeys.hpp>
#include <SceneFuncs.hpp>

#include <maya/MAnimControl.h>
#include <maya/MArgList.h>
#include <maya/MCommandResult.h>
#include <maya/MDagPath.h>
#include <maya/MFnCamera.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnTransform.h>
#include <maya/MGlobal.h>
#include <maya/MItDag.h>
#include <maya/MItSelectionList.h>
#include <maya/MMatrix.h>
#include <maya/MPlug.h>
#include <maya/MSelectionList.h>
#include <maya/MVector.h>

#undef CreateFile
#undef DeleteFile

#include "Core/ch/chBinWriter.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/gf/gfFileBin.hpp"

namespace
{
	// Progress window might cause complications with the Maya MFn handles.
	// So, make it easy to turn it off with this constant:
	const bool c_bDoProgressWindow = true;

	//=============================================================================
	//	Chunk types
	//
	//	ACAM - Animation Camera
	//=============================================================================
	const chDefs::Name c_ACAM = chDefs::MakeName('A', 'C', 'A', 'M');
}

void *WriteCamera::creator()
{
	return new WriteCamera();
}

WriteCamera::WriteCamera()
: bUseMayaAnimCurves(false)
{

}

MStatus WriteCamera::ParseArgs(const MArgList& args)
{
	MStatus     	stat;
	MString     	arg;
	const MString	fileFlag			("-f");
	const MString	fileFlagLongOld		("-File");
	const MString	fileFlagLong		("-file");
	const MString	nobakedFlag			("-nobake");
	const MString	noBakedFlag			("-noBake");

	// Parse the arguments.
	for ( size_t i = 0; i < args.length(); i++ ) {
		arg = args.asString( i, &stat );
		if (!stat)              
			continue;
				
		if ( arg == fileFlag || arg == fileFlagLong || arg == fileFlagLongOld ) {
			filePath = args.asString(i + 1, &stat);
			i++;
		}
		else if ((arg == nobakedFlag ) || (arg == noBakedFlag )) {
			bUseMayaAnimCurves = true;
		}
		else if ( BakeCommand::ParseArg(args, i) ) {
			// BakeCommand read a frame start or frame end flag
		}
		else {
			/*
			arg += ": unknown argument";
			displayError(arg);
			return MS::kFailure;
			*/
			//just ignore extraneous args
		}
	}
	return stat;
}



MStatus WriteCamera::doIt(const MArgList &args)
{
	try
	{
		return safeDoIt(args);
	}
	catch (envExceptionX &i_Ex)
	{
		std::string msg = "sgpuWriteCamera Error : " + i_Ex.GetErrorMessage();
		MayaUtil::PrintError(msg.c_str());
		return MS::kFailure;
	}
	catch(...)
	{
		MayaUtil::PrintError("sgpuWriteCamera : General exception, command could not complete.");
		return MS::kFailure;
	}
}

MStatus WriteCamera::safeDoIt(const MArgList &args)
{
	if (!ParseArgs(args))
		return MS::kFailure; 

	// No "-confirm" mode for WriteCamera
	MayaUtil::SetQuietMode(false);
	MayaUtil::SetSuggestMode(false);

	MStatus status;

	// Just do selection
	//
	MSelectionList activeList;
	status = MGlobal::getActiveSelectionList(activeList);
	MItSelectionList iter( activeList, MFn::kCamera );

	if (iter.isDone())
	{
		MGlobal::displayWarning("No cameras selected.");
		return MS::kFailure;
	}

	MDagPath dagPath;			
	MObject component;

	// Get filename into which to save camera data
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
		//MString cmd = "getFilenameDialog -ext \"*.cam\";";
		MString cmd = "fileDialog -m 1 -dm \"*.cam\";";

		MCommandResult result;
		MGlobal::executeCommand(cmd, result);
		status = result.getResult(path);
	}

	if ((status != MS::kSuccess) || (path.length() == 0))
	{
		cout << "writeCamera: No file path specified." << endl;
		return MS::kFailure;
	}

	cout << "-----------------------------------------" << endl;
	
	// Open file
	MString fname = MayaUtil::ConfirmExtension(path, ".cam");
	cout << "Path: " << fname << endl;

	fsLocator locator;
	fsFileUtil::ANSIFilenameToLocator(fname.asUTF8(), locator);

	if( fsFileUtil::FileExists(locator) )
	{
		if ( !fsFileUtil::IsReadOnly(locator) )
		{
			fsFileUtil::DeleteFile(locator);
		}
		else
		{
			MGlobal::displayWarning("File is READ-ONLY cannot save");
			return MS::kFailure;
		}
	}

	fsFileUtil::CreateFile(locator);

	gfFileBin file(locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
	file.WriteHeader();
	chBinWriter writer(file);

	bool bHaveProgressWindow = false;
	
	// Get default slider range, allow it to be changed by the arguments
	double minTime, maxTime;
	BakeCommand::GetTimelineRange(minTime, maxTime);
	float begin_frame = (float) minTime;
	float end_frame = (float) maxTime;
	float frame_step = 1;

	// Look for camera to export within selection list
	//
	//for ( ; !iter.isDone(); iter.next() )
	if ( !iter.isDone() )
	{							
		iter.getDagPath( dagPath, component );
								
		MDagPath camera_path(dagPath); // Store copy of dagPath for when we move the timeline

		MFnCamera camera(dagPath, &status);
		if (status)
		{
			cout << "Found camera," << endl;
			//MayaUtil::PrintAttributeTypes(camera);

			//TEMP: Debug out some camera info
			//MVector cam_view = camera.viewDirection(MSpace::kWorld);
			//MVector cam_up = camera.upDirection(MSpace::kWorld);
			//MVector cam_right = camera.rightDirection(MSpace::kWorld);
			//MMatrix cam_matx = dagPath.inclusiveMatrix();

			// Confirm horizontal film fit, otherwise display warning
			if (camera.filmFit() != MFnCamera::kHorizontalFilmFit)
			{
				MayaUtil::PrintWarning("Camera should use Horizontal film fit in order to match");
				// continue with exporting anyway?
			}
			if (camera.horizontalFilmOffset() != 0)
			{
				MayaUtil::PrintWarning("Horizontal Film offset will not be exported, export the stereo camera rig itself.");
				// continue with exporting anyway?
			}
			
			if (bUseMayaAnimCurves)
			{
				dagPath.pop();
				MFnTransform xform(dagPath, &status);
				if (status)
				{
					cout << "Found parent transform." << endl;
					//MayaUtil::PrintAttributeTypes(xform);

					writer.WriteChunkHeader(c_ACAM, 0, true);
					AnimFuncs::WriteCameraAnimation(writer, camera, xform, begin_frame, end_frame);
					writer.FinishChunk();
				}
				else
				{
					MGlobal::displayWarning("Could not find transform parent of camera node");
					return MS::kFailure;
				}
			}
			else
			{
				// Bake the camera data ourselves

				// Generate our key steps instead of looking for baked keys
				std::list<float> keys;
				for (float i=begin_frame; i<=end_frame; i+=frame_step)
				{
					keys.push_back( (float) i);
				}			

				// Remember the current time in order to restore it
				// after scrubbing through timeline
				MTime currentTime = MayaUtil::GetCurrentTime();
				
				if (c_bDoProgressWindow)
				{
					MString beginProgress("progressWindow -title \"Baking Animation\" -isInterruptable true -min ");
					beginProgress += minTime;
					beginProgress += MString(" -max ") + maxTime;
					MGlobal::executeCommand( beginProgress );
					bHaveProgressWindow = true;
				}

				// Move through the Maya timeline once. During this pass,
				// we gather up the keys for the camera animation.
				cout << "Seeking through time to bake data..." << endl;
				AnimKeys::CameraKeys camera_keys;
				std::list<float>::iterator it;
				for (it = keys.begin(); it != keys.end(); ++it)
				{
					float frame = (*it);
					//MGlobal::doErrorLogEntry("Doing frame.");

					if (c_bDoProgressWindow)
					{
						MCommandResult result;
						MGlobal::executeCommand("progressWindow -query -isCancelled", result);
						int bIsCancelled = 0;
						status = result.getResult(bIsCancelled);
						if ((status == MS::kSuccess) && (bIsCancelled != 0))
						{
							cout << "Animation baking was cancelled by user." << endl;
							break;
						}

						// Update progress window
						MGlobal::executeCommand(MString("progressWindow -edit -progress ") + frame);
					}

					// Move the Maya timeline to the current frame
					//MTime time(*it);
					MTime time(frame, MTime::uiUnit());
					MayaUtil::SetCurrentTime(time);

					// Gather animation data
					MFnCamera camera_bake(camera_path, &status);
					if (status)
					{
						AnimKeys::GatherKeys(camera_keys, frame, camera_bake, camera_path.inclusiveMatrix());
					}
				}

				// Moved down lower
				//MGlobal::executeCommand("progressWindow -endProgress");

				// Restore the original timeline time
				MayaUtil::SetCurrentTime(currentTime);

				//MGlobal::doErrorLogEntry("Done seeking.");
				cout << "Done seeking." << endl;

				writer.WriteChunkHeader(c_ACAM, 0, true);
				AnimKeys::WriteCameraAnimation(writer, camera_keys, begin_frame);
				writer.FinishChunk();
			}
		}
	}	

	// Write frame rate of animation
	AnimFuncs::WriteCurrentFrameRate(writer);

	// Time offset is the beginning of the Maya timeline.
	// Our animation data was normalized to begin at time "0" by subtracting
	// this time_offset from all time data. By exporting the begin frame,
	// we can place the driver for this animation at the right place in the timeline.
	AnimFuncs::WriteBeginFrame(writer, begin_frame);

	// Stamp version into end of file
	SceneFuncs::WriteExporterVersionStamp(writer);
	cout << "Finished file." << endl;

	if (bHaveProgressWindow)
	{
		MGlobal::executeCommand("progressWindow -endProgress");
	}

	MayaUtil::PrintStatus("Done.");
	
	return MS::kSuccess;
}
