/*****************************************************************************
**  engineExport.cpp
**
**  Entry point for DLL, initializes and deinitializes plug-in.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include <MayaUtil.hpp>
#include <maya/MGlobal.h>
#include <maya/MPxCommand.h>
#include <maya/MFnPlugin.h>

#include <ForceVerts.hpp>
//#include <SuggestTolerance.hpp>
#include <WriteCamera.hpp>
#include <WriteModel.hpp>
#include <WriteParticles.hpp>
#include <WriteVerts.hpp>

#include "Core/app/appTime.hpp"
#include "Core/ch/chPackage.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Fs/fsLocator.hpp"
#include "Core/Gf/gfPaths.hpp"
#include "Graphics/Mat/matShaderParser.hpp"


const char* c_MELScriptName = "msExportUI.mel"; // old one: "sgpuMenu.mel";

MStatus initializePlugin( MObject obj )
{ 
	MStatus status;

	MFnPlugin plugin ( obj, "StudioGPU", MayaUtil::GetExporterVersion(), "Any" );

	status = plugin.registerCommand( "sgpuWriteModel", WriteModel::creator );
	if (!status) {
		status.perror("registerCommand");
		return MS::kFailure;
	}

	status = plugin.registerCommand( "sgpuWriteCamera", WriteCamera::creator );
	if (!status) {
		status.perror("registerCommand");
		return MS::kFailure;
	}

	//status = plugin.registerCommand( "sgpuWriteParticles", WriteParticles::creator );
	//if (!status) {
	//	status.perror("registerCommand");
	//	return MS::kFailure;
	//}

	status = plugin.registerCommand( "sgpuWriteVerts", WriteVerts::creator );
	if (!status) {
		status.perror("registerCommand");
		return MS::kFailure;
	}

	//status = plugin.registerCommand( "sgpuForceVerts", ForceVerts::creator );
	//if (!status) {
	//	status.perror("registerCommand");
	//	return MS::kFailure;
	//}

	//status = plugin.registerCommand( "sgpuSuggestTolerance", SuggestTolerance::creator );
	//if (!status) {
	//	status.perror("registerCommand");
	//	return MS::kFailure;
	//}

	gfPaths::Init();
	appTime::Init();
	chPackage::Init();
	matShaderParser::Initialize();

	cout << "sgpuMachExporter for Maya version " << MayaUtil::GetExporterVersion() << endl;

	// Find path to the mll file:
	bool bFoundMELScript = false;
	MString source_cmd("source "); // will be added to later

	//bga - Tried using the load path to the plugin to find the MEL scripts,
	// but it was requested that we keep it simple and require that the user
	// copy the MEL scripts to the Maya "scripts" directory. So the next code
	// is commented out.
	//MString loadPath = plugin.loadPath(&status);
	//if (status)
	//{
	//	//cout << "Module path: " << loadPath << endl;
	//	fsLocator mel_loc;
	//	fsFileUtil::ANSIFilenameToLocator(loadPath.asUTF8(), mel_loc);
	//	mel_loc.Push(itString(c_MELScriptName));
	//	if (fsFileUtil::FileExists(mel_loc))
	//	{
	//		bFoundMELScript = true;
	//		//std::string mel_path;
	//		//fsFileUtil::LocatorToANSIFilename(mel_loc, mel_path);
	//		//cout << mel_path << endl;
	//		//source_cmd += mel_path.c_str();

	//		// Have to put quotes around the path and use the
	//		// path Maya gave us in order to get the slashes correct.
	//		source_cmd += "\"";
	//		source_cmd += loadPath;
	//		source_cmd += "/";
	//		source_cmd += c_MELScriptName;
	//		source_cmd += "\"";
	//	}
	//}

	if (!bFoundMELScript)
	{
		// Add filename without path and hope it is
		// in the "scripts" directory.
		source_cmd += c_MELScriptName;
	}

	// Now source the MEL script 
	//cout << "  sgpuMachExporter UI: " << source_cmd << endl;
	MGlobal::executeCommand(source_cmd);

	return MS::kSuccess;
}


MStatus uninitializePlugin( MObject obj )
{
	MStatus status;

	matShaderParser::DeInitialize();
	chPackage::CleanUp();
	gfPaths::CleanUp();

	MFnPlugin plugin( obj );

	status = plugin.deregisterCommand("sgpuWriteModel");
	if ( !status ) {
		status.perror("deregisterCommand");
		return MS::kFailure;
	}

	status = plugin.deregisterCommand("sgpuWriteCamera");
	if ( !status ) {
		status.perror("deregisterCommand");
		return MS::kFailure;
	}

	//status = plugin.deregisterCommand("sgpuWriteParticles");
	//if ( !status ) {
	//	status.perror("deregisterCommand");
	//	return MS::kFailure;
	//}

	status = plugin.deregisterCommand("sgpuWriteVerts");
	if ( !status ) {
		status.perror("deregisterCommand");
		return MS::kFailure;
	}

	//status = plugin.deregisterCommand("sgpuForceVerts");
	//if ( !status ) {
	//	status.perror("deregisterCommand");
	//	return MS::kFailure;
	//}

	//status = plugin.deregisterCommand("sgpuSuggestTolerance");
	//if ( !status ) {
	//	status.perror("deregisterCommand");
	//	return MS::kFailure;
	//}

	return MS::kSuccess;
}
