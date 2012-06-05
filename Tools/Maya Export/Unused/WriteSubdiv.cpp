/*****************************************************************************
**  WriteSubdiv.cpp
**
**      
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <WriteSubdiv.hpp>
#include <SceneFuncs.hpp>
#include <SubdivFuncs.hpp>

#include <maya/MArgList.h>
#include <maya/MCommandResult.h>
#include <maya/MDagPath.h>
#include <maya/MFnBlendShapeDeformer.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnMesh.h>
#include <maya/MGlobal.h>
#include <maya/MItDag.h>
#include <maya/MItSelectionList.h>
#include <maya/MMatrix.h>
#include <maya/MPlug.h>
#include <maya/MSelectionList.h>
#include <maya/MFnSubd.h>

#undef CreateFile
#undef DeleteFile

#include "Core/ch/chBinWriter.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileStream.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/gf/gfFileBin.hpp"


void *WriteSubdiv::creator()
{
	return new WriteSubdiv();
}

WriteSubdiv::WriteSubdiv() 
 : shareMaterials(true), begin_depth(0), end_depth(2)
{

}

MStatus WriteSubdiv::ParseArgs(const MArgList& args)
{
	MStatus     	stat;
	MString     	arg;
	const MString	fileFlag			("-f");
	const MString	fileFlagLong		("-File");
	const MString	noShareFlag			("-nosharemats");
	const MString	numLevelsFlag		("-numlevels");
	const MString	onlyLevelFlag		("-onlylevel");

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
		else if ( arg == numLevelsFlag ) {
			end_depth = args.asInt(i + 1, &stat);
			i++;
		}
		else if ( arg == onlyLevelFlag ) {
			begin_depth = end_depth = args.asInt(i + 1, &stat);
			i++;
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



MStatus WriteSubdiv::doIt(const MArgList &args)
{
	if (!ParseArgs(args))
		return MS::kFailure; 

	MStatus status;

	// Just do selection
	//
	MSelectionList activeList;
	status = MGlobal::getActiveSelectionList(activeList);
	MItSelectionList iter( activeList, MFn::kSubdiv );

	if (iter.isDone())
	{
		MGlobal::displayWarning("No subdivision surfaces selected.");
		return MS::kFailure;
	}

	MDagPath dagPath;			
	MObject component;

	// Confirm that all subdivision meshes have a single material
	for ( ; !iter.isDone(); iter.next() )
	{							
		iter.getDagPath( dagPath, component );
		MFnSubd subdiv(dagPath, &status);
		if (status)
		{
			int count = SubdivFuncs::CountMaterials(subdiv);
			if (count > 1) 
			{
				MGlobal::displayWarning("Each subdivision surface must have a single material.");
				return MS::kFailure;
			}
		}
	}	
	iter.reset(); // reset iterator to beginning of list

	
	// Get filename into which to save meshes
	//
	cout << "Asking for filename." << endl;

	MString cmd = "getFilenameDialog -ext \"*.mx\";";
	MCommandResult result;
	MGlobal::executeCommand(cmd, result);

	MString path;
	status = result.getResult(path);
	if ((status == MS::kSuccess) && (path.length() > 0))
	{
		// Open file
		MString fname = MayaUtil::ConfirmExtension(path, ".mx");
		MString root;
		bool bAddExtension = (begin_depth != end_depth);
		if (bAddExtension)
		{
			// Will need to Add in _l# to filename to represent level of detail depth
			root = fname.substring(0, fname.length()-4); // take off .mx
			if (root.length() > 3)
			{
				const char *str = root.asChar();
				int pos = root.length()-3;
				cout << "Comparing root end to " << (str+pos) << endl;
				if (str[pos] == '_' && str[pos+1] == 'l' && isdigit(str[pos+2]))
				{
					cout << "Trimming root" << endl;
					root = root.substring(0, root.length()-4); // take off _l#
				}
			}
		}

		for (int depth = begin_depth; depth <= end_depth; depth++)
		{
			if (bAddExtension)
			{
				// Add in _l# to filename to represent level of detail depth
				fname = root + "_l" + depth + ".mx";
			}

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

			// clear out shared material table, 
			// start new group now
			SceneFuncs::ClearSharedMaterialTable();

			// Output file into meshes
			//
			for ( ; !iter.isDone(); iter.next() )
			{							
				iter.getDagPath( dagPath, component );
										
				MFnSubd subdiv(dagPath, &status);
				if (status)
				{
					cout << "found subdiv!!!" << endl;
					SubdivFuncs::DebugSubdivision(subdiv);
			
					MMatrix identity;
					chBinWriter writer(file);
					int sample = 0;
					SubdivFuncs::WriteSubdivMeshToFile(subdiv, (depth+1), sample, writer, identity);
				}
			}	

			// Stamp version into end of file
			chBinWriter stamp_writer(file);
			SceneFuncs::WriteExporterVersionStamp(stamp_writer);

			iter.reset(); // reset iterator to beginning of list
		}

		MayaUtil::PrintStatus("Done.");
	}
	else
	{
		cout << "Dialog ended with error?" << endl;
	}
	

	return MS::kSuccess;
}
