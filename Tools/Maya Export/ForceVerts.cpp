/*****************************************************************************
**  ForceVerts.cpp
**
**     Command to write vertex animation of meshes.  With argument "-geom"
**	it writes .vtx file and with "-anim" it writes .vta file.     
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include <MayaUtil.hpp>
#include <ForceVerts.hpp>
#include <AnimFuncs.hpp>
#include <SceneFuncs.hpp>
#include <SubdivFuncs.hpp>
#include <VertexFuncs.hpp>
#include <CharacterFuncs.hpp>

#include <maya/MArgList.h>
#include <maya/MCommandResult.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnMesh.h>
#include <maya/MFnSubd.h>
#include <maya/MFnTransform.h>
#include <maya/MGlobal.h>
#include <maya/MItDag.h>
#include <maya/MItSelectionList.h>
#include <maya/MMatrix.h>
#include <maya/MPlug.h>
#include <maya/MSelectionList.h>

#undef CreateFile
#undef DeleteFile

#include "Core/ch/chBinWriter.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/gf/gfFileBin.hpp"



ForceVerts::ForceVerts() 
{
	bDoGeom = false;
	bDoAnim = false;
	bShareMaterials = false;
	bConfirmFlags = true;
	bSuggest = false;
	subd_depth = 1;
	end_frame = -1;
	begin_frame = 0;
	frame_step = 1;
}

ForceVerts::~ForceVerts() 
{

}

void* ForceVerts::creator()
{
	return new ForceVerts;
}

MStatus	ForceVerts::doIt( const MArgList& args )
{
	try
	{
		return safeDoIt(args);
	}
	catch (envExceptionX &i_Ex)
	{
		std::string msg = "sgpuForceVerts Error : " + i_Ex.GetErrorMessage();
		MayaUtil::PrintError(msg.c_str());
		return MS::kFailure;
	}
	catch(...)
	{
		MayaUtil::PrintError("sgpuForceVerts : General exception, command could not complete.");
		return MS::kFailure;
	}
}

MStatus ForceVerts::safeDoIt(const MArgList &args)
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

MStatus ForceVerts::parseArgs(const MArgList& args)
{
	MStatus     	stat;
	MString     	arg;
	const MString	animFlag			("-anim");
	const MString	geomFlag			("-geom");
	const MString	shareFlag			("-sharemats");
	const MString	fileFlag			("-f");
	const MString	fileFlagLongOld		("-File");
	const MString	fileFlagLong		("-file");
	const MString	onlyLevelFlag		("-subdlevel");
	const MString	frameStepFlag		("-framestep");
	const MString	confirmFlag			("-confirm");
	const MString	suggestFlag			("-suggest");

	// Parse the arguments.
	for ( size_t i = 0; i < args.length(); i++ ) {
		arg = args.asString( i, &stat );
		if (!stat)              
			continue;
				
		if ( arg == fileFlag || arg == fileFlagLong || arg == fileFlagLongOld ) {
			filePath = args.asString(i + 1, &stat);
			i++;
		}
		else if ( arg == animFlag  ) {
			bDoAnim = true;
		}
		else if ( arg == geomFlag  ) {
			bDoGeom = true;
		}
		else if ( arg == shareFlag ) {
			bShareMaterials = true;
		}
		else if (arg == confirmFlag ) {
			bConfirmFlags = false;
		}
		else if (arg == suggestFlag ) {
			bSuggest = true;
		}
		else if ( arg == onlyLevelFlag ) {
			subd_depth = args.asInt(i + 1, &stat);
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


MStatus ForceVerts::doScan()
{   
	// In "-confirm" mode, don't display errors
	MayaUtil::SetQuietMode(!bConfirmFlags);
	MayaUtil::SetSuggestMode(bSuggest);

	// Turning this off for this animation just to be safe...
	CharacterFuncs::SetAutoDetectDeformation(false);

	MayaUtil::SetWaitCursor();

	// Get slider range
	double minTime, maxTime;
	BakeCommand::GetTimelineRange(minTime, maxTime);
	begin_frame = (int) minTime;
	end_frame = (int) maxTime;
	frame_step = 1;

	MStatus status;

	// Get meshes in selection list
	//
	MSelectionList activeList;
	status = MGlobal::getActiveSelectionList(activeList);
	MItSelectionList iter( activeList, MFn::kMesh );
	//MItSelectionList subd_iter( activeList, MFn::kSubdiv );
	//if (iter.isDone() && subd_iter.isDone())
	if (iter.isDone())
	{
		MGlobal::displayWarning("No meshes selected.");
		return MS::kFailure;
	}
	
	// Get filename into which to save meshes
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
			//cmd = "getFilenameDialog -ext \"*.vtx\";";
			cmd = "fileDialog -m 1 -dm \"*.vtx\";";
		}
		else if (bDoAnim)
		{
			//cmd = "getFilenameDialog -ext \"*.vta\";";
			cmd = "fileDialog -m 1 -dm \"*.vta\";";
		}
		else
		{
			cout << "ForceVerts: Neither geom or anim chosen to write" << endl;
			return MS::kFailure;
		}

		MCommandResult result;
		MGlobal::executeCommand(cmd, result);
		status = result.getResult(path);
	}

	if ((status != MS::kSuccess) || (path.length() == 0))
	{
		cout << "ForceVerts: No file path specified." << endl;
		return MS::kFailure;
	}

	// Open file
	fsLocator locator;
	if (!MayaUtil::MUCreateFile(path, (bDoGeom) ? ".vtx" : ".vta", locator))
	{
		MGlobal::displayWarning("file is READ-ONLY cannot save");
		return MS::kFailure;
	}

	gfFileBin file(locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
	file.WriteHeader();
			
	chBinWriter writer(file);

	cout << "-----------------------------------------" << endl;
	
	MDagPath dagPath;			
	MObject component;
	MString efficiencyMsg;

	if (bDoGeom)
	{
		// clear out shared material table, 
		// start new group now
		SceneFuncs::ClearSharedMaterialTable();

		if (bShareMaterials)
		{
			// Iterate through meshes, gathering all materials
			// into one table
			MItSelectionList iter2( activeList, MFn::kMesh );
			for ( ; !iter2.isDone(); iter2.next() )
			{							
				iter2.getDagPath( dagPath, component );
										
				MFnMesh mesh(dagPath, &status);
				if (status)
					SceneFuncs::GatherSharedMaterials(mesh.object());
			}

			// Write out table 
			SceneFuncs::WriteSharedMaterialTable(writer);
		}

		// Output meshes into file
		//
		for ( ; !iter.isDone(); iter.next() )
		{							
			iter.getDagPath( dagPath, component );
									
			MFnMesh mesh(dagPath, &status);
			if (status)
			{
				//cout << "found mesh!!!" << endl;
				const bool bWorldSpace = true;
				SceneFuncs::WriteBRepToFile(mesh, writer, bWorldSpace);

				// Check for efficiencies, gather a message to 
				// be reported in dialog
				SceneFuncs::GatherWarningMessages(mesh, efficiencyMsg);
			}
		}	

		// Output subdivs into meshes
		//
		//for ( ; !subd_iter.isDone(); subd_iter.next() )
		//{							
		//	subd_iter.getDagPath( dagPath, component );
		//							
		//	MMatrix objToWorld = dagPath.inclusiveMatrix();
		//	cout << "Subd Matrix= " << objToWorld << endl;

		//	MFnSubd subdiv(dagPath, &status);
		//	if (status)
		//	{
		//		cout << "found subdiv!!!" << endl;		
		//		chBinWriter writer(file);
		//		int sample = 0;
		//		SubdivFuncs::WriteSubdivMeshToFile(subdiv, (subd_depth+1), sample, writer, objToWorld);
		//	}
		//}	
	}
	else if (bDoAnim)
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
									
			MFnMesh mesh(dagPath, &status);
			if (status)
			{
				//cout << "found mesh!!!" << endl;
				VertexFuncs::WriteMeshVertexAnimation(dagPath, mesh.name(), writer, keys, (float)begin_frame);
			}
		}	

		// Output subdivs into baked vertex mesh animation
		//
		//for ( ; !subd_iter.isDone(); subd_iter.next() )
		//{							
		//	subd_iter.getDagPath( dagPath, component );
		//							
		//	MMatrix objToWorld = dagPath.inclusiveMatrix();
		//	//cout << "Subd Matrix= " << objToWorld << endl;

		//	MFnSubd subdiv(dagPath, &status);
		//	if (status)
		//	{
		//		//cout << "found subdiv!!!" << endl;
		//		VertexFuncs::BakeVertexAnimation(subdiv, (subd_depth+1), writer, keys, objToWorld, begin_frame);
		//	}
		//}	
			
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
