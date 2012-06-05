/*****************************************************************************
**  WriteBRep.cpp
**
**      
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <WriteBRep.hpp>
#include <SceneFuncs.hpp>

#include <maya/MArgList.h>
#include <maya/MCommandResult.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnMesh.h>
#include <maya/MGlobal.h>
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


void *WriteBRep::creator()
{
	return new WriteBRep();
}

MStatus WriteBRep::ParseArgs(const MArgList& args)
{
	MStatus     	stat;
	MString     	arg;
	const MString	fileFlag			("-f");
	const MString	fileFlagLong		("-File");
	const MString	noShareFlag			("-nosharemats");

	// Parse the arguments.
	for ( size_t i = 0; i < args.length(); i++ ) {
		arg = args.asString( i, &stat );
		if (!stat)              
			continue;
				
		if ( arg == fileFlag || arg == fileFlagLong ) {
			filePath = args.asString(i + 1, &stat);
			i++;
		}
		else if ( arg == noShareFlag ) {
			shareMaterials = false;
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



MStatus WriteBRep::doIt(const MArgList &args)
{
	if (!ParseArgs(args))
		return MS::kFailure; 

	MStatus status;

	// Just do selection
	//
	MSelectionList activeList;
	status = MGlobal::getActiveSelectionList(activeList);
	MItSelectionList iter( activeList, MFn::kMesh );


	if (iter.isDone())
	{
		MGlobal::displayWarning("No meshes selected.");
		return MS::kFailure;
	}

	// Zero transform all selected
	//
	MGlobal::executeCommand("makeIdentity -apply true;");
	
	// Get filename into which to save meshes
	//
	MString path;
	if (filePath.length() > 0)
	{
		// MEL comand should be like:
		// writeBRep -f "C:\\Data\\exportTest.mx";
		// to get filePath to be
		// C:\Data\exportTest.mx
		cout << "Have filename from command line: " << filePath << endl;
		path = filePath;
	}
	else
	{
		cout << "Asking for filename." << endl;
		MString cmd = "getFilenameDialog -ext \"*.mx\";";
		MCommandResult result;
		MGlobal::executeCommand(cmd, result);
		status = result.getResult(path);
	}

	if ((status == MS::kSuccess) && (path.length() > 0))
	{
		// Open file
		MString fname = MayaUtil::ConfirmExtension(path, ".mx");
		cout << "Path: " << fname << endl;

		fsLocator locator;
		fsFileUtil::ANSIFilenameToLocator(fname.asChar(), locator);

		if( fsFileUtil::FileExists(locator) )
		{
			if ( !fsFileUtil::IsReadOnly(locator) )
			{
				fsFileUtil::DeleteFile(locator);
			}
			else
			{
				MGlobal::displayWarning("file is READ-ONLY cannot save");
				return MS::kFailure;
			}
		}

		fsFileUtil::CreateFile(locator);

		gfFileBin file(locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
		file.WriteHeader();
	
		MDagPath dagPath;			
		MObject component;

		// clear out shared material table, 
		// start new group now
		SceneFuncs::ClearSharedMaterialTable();

		if (shareMaterials)
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
			chBinWriter writer(file);
			SceneFuncs::WriteSharedMaterialTable(writer);
		}

		// Output file into meshes
		//
		MString efficiencyMsg;
		for ( ; !iter.isDone(); iter.next() )
		{							
			iter.getDagPath( dagPath, component );
									
			MFnMesh mesh(dagPath, &status);
			if (status)
			{
				cout << "found mesh, " << mesh.name() << endl;
		
				chBinWriter writer(file);
				SceneFuncs::WriteBRepToFile(mesh, writer);

				// Check for efficiencies, gather a message to 
				// be reported in dialog
				SceneFuncs::GatherWarningMessages(mesh, efficiencyMsg);
			}
		}	

		// Stamp version into end of file
		chBinWriter stamp_writer(file);
		SceneFuncs::WriteExporterVersionStamp(stamp_writer);

		MayaUtil::PrintStatus("Done.");

		if (efficiencyMsg.length() > 0)
		{
			// Confirmation dialog.
			MayaUtil::DisplayConfirmation( efficiencyMsg, "Confirm flags" );
		}
	}
	else
	{
		cout << "Dialog ended with error?" << endl;
	}
	

	return MS::kSuccess;
}
