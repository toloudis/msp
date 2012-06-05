/*****************************************************************************
**  WriteXForms.cpp
**
**     Command to write hierarchical file.  With argument "-geom"
**	it writes .mhx file and with "-anim" it writes .mha file.     
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <WriteXforms.hpp>
#include <SceneFuncs.hpp>
#include <JointFuncs.hpp>

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



WriteXforms::WriteXforms() 
{
	bDoGeom = false;
	bDoAnim = false;
	bShareMaterials = true;
}

WriteXforms::~WriteXforms() 
{

}

void* WriteXforms::creator()
{
	return new WriteXforms;
}

MStatus	WriteXforms::doIt( const MArgList& args )
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

MStatus WriteXforms::parseArgs(const MArgList& args)
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


MStatus WriteXforms::doScan()
{   

	MStatus status;

	// Just do selected transform node
	//
	MSelectionList activeList;
	status = MGlobal::getActiveSelectionList(activeList);
	MItSelectionList sel_iter( activeList, MFn::kTransform );
	if (sel_iter.isDone())
	{
		MGlobal::displayWarning("WriteXforms : No transform nodes selected.");
		return MS::kFailure;
	}

	MayaUtil::SetWaitCursor();
	
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
			cmd = "getFilenameDialog -ext \"*.mhx\";";
		}
		else if (bDoAnim)
		{
			cmd = "getFilenameDialog -ext \"*.mha\";";
		}
		else
		{
			cout << "WriteXforms: Neither geom or anim chosen to write" << endl;
			return MS::kFailure;
		}

		MCommandResult result;
		MGlobal::executeCommand(cmd, result);
		status = result.getResult(path);
	}

	if ((status != MS::kSuccess) || (path.length() == 0))
	{
		cout << "WriteXforms: No file path specified." << endl;
		return MS::kFailure;
	}

	cout << "-----------------------------------------" << endl;
	
	// Iterate through scene
	//
	//MItDag dagIterator( MItDag::kBreadthFirst, MFn::kInvalid, &status);
	//
	//if ( !status) 
	//{
	//	MayaUtil::PrintError("MItDag constructor");
	//	MayaUtil::UnsetWaitCursor();
	//	return status;
	//}

	
	int xformCount = 0, meshCount = 0;
	MString xformName;
	for ( ; !sel_iter.isDone(); sel_iter.next() ) 
	{
		MDagPath dagPath;			
		MObject component;		
		status = sel_iter.getDagPath( dagPath, component );
		if ( !status ) {
			status.perror("MItDag::getPath");
			continue;
		}

		if (dagPath.hasFn(MFn::kTransform)) 
		{
			//this will be slow for deep hierarchies
			//
			MFnTransform transform(dagPath, &status);
			if (status == MS::kSuccess)
			{
				cout << "Got transform: " << transform.name() << endl;

				bool bMesh = MayaUtil::HasTypeAsChild(transform.object(), MFn::kMesh);
				cout << transform.name() << ": " << bMesh << endl;

				if (bMesh) 
				{
					if (xformCount == 0) 
					{
						xformName = transform.name();
			
						if (bDoGeom)
						{
							cout << "Write xform geometry." << endl;

							MString fname = MayaUtil::ConfirmExtension(path, ".mhx");
	
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
							
							// clear out shared material table, 
							// start new group now
							SceneFuncs::ClearSharedMaterialTable();
							if (bShareMaterials)
								SceneFuncs::GatherSharedMaterials(transform.object());

							MString efficiencyMsg;
							meshCount += SceneFuncs::WriteHierarchy(transform.object(), file, efficiencyMsg);
							cout << "Write " << meshCount << " meshes." << endl;

							// Stamp version into end of file
							chBinWriter stamp_writer(file);
							SceneFuncs::WriteExporterVersionStamp(stamp_writer);
							
							// Check for efficiencies, gather a message to 
							// be reported in dialog
							if (efficiencyMsg.length() > 0)
							{
								// Confirmation dialog.
								MayaUtil::DisplayConfirmation( efficiencyMsg, "Confirm flags" );
							}
							
						}
						if (bDoAnim)
						{
							cout << "Write xform animation." << endl;

							MString fname = MayaUtil::ConfirmExtension(path, ".mha");

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

							SceneFuncs::WriteAnimHierarchy(transform.object(), file);
							
							// Stamp version into end of file
							chBinWriter stamp_writer(file);
							SceneFuncs::WriteExporterVersionStamp(stamp_writer);
						}
					}
					xformCount++;
				}
				
			}

			//dagIterator.prune();

		}

	}

	if (xformCount == 0) 
	{
		cerr << "No xforms found.  Created empty file." << endl;
		MayaUtil::PrintError("No xforms found.  Created empty file.");
	}
	else if (xformCount > 1) 
	{
		cerr << "More than one xform selected.  Only wrote " << xformName << endl;
		MString err = MString("More than one xform selected.  Only wrote ") + xformName + ".";
		MayaUtil::PrintWarning(err);
	}
	else 
	{
		MayaUtil::PrintStatus("Done.");
	}

	MayaUtil::UnsetWaitCursor();

	return MS::kSuccess;
}
