/*****************************************************************************
**  WriteVerts.cpp
**
**     Command to write vertex animation of meshes.  With argument "-geom"
**	it writes .vtx file and with "-anim" it writes .vta file.     
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include <MayaUtil.hpp>
#include <WriteVerts.hpp>
#include <SceneFuncs.hpp>
#include <VertexFuncs.hpp>

#include <maya/MArgList.h>
#include <maya/MCommandResult.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnMesh.h>
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
#include "Core/fs/fsFileStream.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/gf/gfFileBin.hpp"



WriteVerts::WriteVerts() 
{
	bDoGeom = false;
	bDoAnim = false;
	bShareMaterials = true;
}

WriteVerts::~WriteVerts() 
{

}

void* WriteVerts::creator()
{
	return new WriteVerts;
}

MStatus	WriteVerts::doIt( const MArgList& args )
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

MStatus WriteVerts::parseArgs(const MArgList& args)
{
	MStatus     	stat;
	MString     	arg;
	const MString	animFlag			("-anim");
	const MString	geomFlag			("-geom");
	const MString	noShareFlag			("-nosharemats");
	const MString	fileFlag			("-f");
	const MString	fileFlagLong		("-File");

	// Parse the arguments.
	for ( size_t i = 0; i < args.length(); i++ ) {
		arg = args.asString( i, &stat );
		if (!stat)              
			continue;
				
		if ( arg == fileFlag || arg == fileFlagLong ) {
			filePath = args.asString(i + 1, &stat);
			i++;
		}
		else if ( arg == animFlag  ) {
			bDoAnim = true;
		}
		else if ( arg == geomFlag  ) {
			bDoGeom = true;
		}
		else if ( arg == noShareFlag ) {
			bShareMaterials = false;
		}
		else {
			//just ignore extraneous args
		}
	}
	return stat;
}


MStatus WriteVerts::doScan()
{   
	MayaUtil::SetWaitCursor();

	MStatus status;

	// Get meshes in selection list
	//
	MSelectionList activeList;
	status = MGlobal::getActiveSelectionList(activeList);
	MItSelectionList iter( activeList, MFn::kMesh );

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
			cmd = "getFilenameDialog -ext \"*.vtx\";";
		}
		else if (bDoAnim)
		{
			cmd = "getFilenameDialog -ext \"*.vta\";";
		}
		else
		{
			cout << "WriteVerts: Neither geom or anim chosen to write" << endl;
			return MS::kFailure;
		}

		MCommandResult result;
		MGlobal::executeCommand(cmd, result);
		status = result.getResult(path);
	}

	if ((status != MS::kSuccess) || (path.length() == 0))
	{
		cout << "WriteVerts: No file path specified." << endl;
		return MS::kFailure;
	}

	// Open file
	fsLocator locator;
	if (!MayaUtil::CreateFile(path, (bDoGeom) ? ".vtx" : ".vta", locator))
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

	}
	else if (bDoAnim)
	{
		// Output vertex animation
		//
		for ( ; !iter.isDone(); iter.next() )
		{							
			iter.getDagPath( dagPath, component );
									
			MFnMesh mesh(dagPath, &status);
			if (status)
			{
				//cout << "found mesh!!!" << endl;
				VertexFuncs::WriteMeshVertexAnimation(dagPath, writer);
			}
		}	

	}

	// Stamp version into end of file
	SceneFuncs::WriteExporterVersionStamp(writer);

	MayaUtil::PrintStatus("Done.");

	if (efficiencyMsg.length() > 0)
	{
		// Confirmation dialog.
		MayaUtil::DisplayConfirmation( efficiencyMsg, "Confirm flags" );
	}

	MayaUtil::UnsetWaitCursor();

	return MS::kSuccess;
}
