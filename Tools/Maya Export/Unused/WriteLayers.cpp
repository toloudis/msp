/*****************************************************************************
**  WriteLayers.cpp
**
**      
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <WriteLayers.hpp>
#include <LayerFuncs.hpp>
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


void *WriteLayers::creator()
{
	return new WriteLayers();
}

MStatus WriteLayers::ParseArgs(const MArgList& args)
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



MStatus WriteLayers::doIt(const MArgList &args)
{
	if (!ParseArgs(args))
		return MS::kFailure;
	
	//LayerFuncs::ParseLayers();
	std::vector<MString> layer_names;
	LayerFuncs::GetLayers(layer_names);

	int num_layers = layer_names.size();
	cout << "Num Layers " << num_layers << endl;
	for (int layer=0; layer<num_layers; layer++)
	{
		cout << "Layer: " << layer_names[layer] << endl;	

		MString cmd = "getFilenameDialog -ext \"*.mx\" -f ";
		cmd += layer_names[layer];
		cmd += ";";
		MCommandResult result;
		MGlobal::executeCommand(cmd, result);

		MString path;
		MStatus status = result.getResult(path);

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

	
			// Output meshes into file
			//
			chBinWriter writer(file);
			LayerFuncs::WriteBReps(layer_names[layer], writer);

			// Stamp version into end of file
			SceneFuncs::WriteExporterVersionStamp(writer);

		}
	}

	MayaUtil::PrintStatus("Done.");
	return MS::kSuccess;
}
