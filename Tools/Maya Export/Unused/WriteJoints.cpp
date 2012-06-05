/*****************************************************************************
**  WriteJoints.cpp
**
**     Command to write single-skin file.  With argument "-geom"
**	it writes .jnx file and with "-anim" it writes .jna file.
**
**		Note: Animation data should be baked, using MEL
**	bakeResults command. 
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <WriteJoints.hpp>
#include <SceneFuncs.hpp>
#include <JointFuncs.hpp>

#include <maya/MArgList.h>
#include <maya/MCommandResult.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnIkJoint.h>
#include <maya/MFnMesh.h>
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


//=============================================================================
//	Chunk types
//=============================================================================
const chDefs::Name c_MSSK = chDefs::MakeName('M', 'S', 'S', 'K');

WriteJoints::WriteJoints() 
{
	bDoGeom = false;
	bDoAnim = false;
	bDoSubAnim = false;
}

WriteJoints::~WriteJoints() 
{

}

void* WriteJoints::creator()
{
	return new WriteJoints;
}

MStatus	WriteJoints::doIt( const MArgList& args )
{
	MItDag::TraversalType	traversalType = MItDag::kDepthFirst;
	MFn::Type				filter        = MFn::kInvalid;
	MStatus					status;
	bool					quiet = false;

	status = parseArgs (args);
	if (!status)
		return status;

	if (bDoSubAnim)
		return doSubAnim();
	else
	return doScan();
};

MStatus WriteJoints::parseArgs(const MArgList& args)
{
	MStatus     	stat;
	MString     	arg;
	const MString	animFlag			("-anim");
	const MString	geomFlag			("-geom");
	const MString	subanimFlag			("-subanim");
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
		else if ( arg == subanimFlag  ) {
			bDoSubAnim = true;
		}
		else {
			//just ignore extraneous args
		}
	}
	return stat;
}


MStatus WriteJoints::doScan()
{   
	MStatus status;

	// Just do selected joint
	//
	MSelectionList activeList;
	status = MGlobal::getActiveSelectionList(activeList);
	MItSelectionList sel_iter( activeList, MFn::kJoint );

	if (sel_iter.isDone())
	{
		MGlobal::displayWarning("WriteJoints : No joint selected.");
		return MS::kFailure;
	}

	MayaUtil::SetWaitCursor();
	
	// Get filename into which to save jnx/jna
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
			cmd = "getFilenameDialog -ext \"*.jnx\";";
		}
		else if (bDoAnim)
		{
			cmd = "getFilenameDialog -ext \"*.jna\";";
		}
		else
		{
			cout << "writeJoints: Neither geom or anim chosen to write" << endl;
			return MS::kFailure;
		}

		MCommandResult result;
		MGlobal::executeCommand(cmd, result);
		status = result.getResult(path);
	}

	if ((status != MS::kSuccess) || (path.length() == 0))
	{
		cout << "writeJoints: No file path specified." << endl;
		return MS::kFailure;
	}

	cout << "-----------------------------------------" << endl;
	
	// I'm moving this to be dependent on a specific mesh
	//JointFuncs::ParseSkinClusters();

	// This was the old method that looked for visible joints
	//MItDag dagIterator( MItDag::kBreadthFirst, MFn::kInvalid, &status);
	//
	//if ( !status) {
	//	MayaUtil::PrintError("MItDag constructor");
	//	MayaUtil::UnsetWaitCursor();
	//	return status;
	//}


	//	Scan the selection list for joints
	//
	int jointCount = 0;
	MString jointName;
	for ( ; !sel_iter.isDone(); sel_iter.next() ) {

		MDagPath dagPath;			
		MObject component;		
		status = sel_iter.getDagPath( dagPath, component );
		if ( !status ) {
			status.perror("MItDag::getPath");
			continue;
		}

		//MFnDagNode dagNode(dagPath, &status);
		//if ( !status ) {
		//	status.perror("MFnDagNode constructor");
		//	continue;
		//}

//		cout << "Dag Node: " << dagNode.name() << ", type = " << dagNode.typeName() << endl;

		//MObject obj = dagPath.node(&status);
		//if(obj.apiType() == MFn::kJoint) {
		if (dagPath.hasFn(MFn::kJoint)) 
		{
			cout << "Joint!" << endl;
			MFnIkJoint joint(dagPath, &status);
			if ( !status ) {
				status.perror("MFnIkJoint constructor");
				continue;
			}

			cout << "name = " << joint.name() << endl;
			cout << "path = " << dagPath.fullPathName() << endl;
			//printTransformData(dagPath, false);
			//dagIterator.prune();
			
			//check for visibility (including layers)
			//only write joint if it is visible
			//since we're pruning, we don't have to worry about children's visibility

			// don't need to check visibility anymore
			//if (MayaUtil::DagNodeVisible(joint)) 
			{
				//only write first skeleton encountered
				if (jointCount == 0) 
				{
					if (bDoGeom)
					{
						cout << "Should write joint geometry." << endl;

						MFnSkinCluster cluster = JointFuncs::FindSkinCluster(joint, status);
						if (status != MS::kSuccess)
						{
							cout << "Could not find attached skin cluster." << endl;
							continue;
						}

						MObject meshobj = cluster.outputShapeAtIndex(0,&status);
						if (!status) continue;

						// Look for selected mesh
						MFnMesh mesh(meshobj, &status);
						if (status)
						{
							MString fname = MayaUtil::ConfirmExtension(path, ".jnx");

							cout << "Parsing skin clusters for single skin." << endl;
							JointFuncs::ParseSkinClusters(mesh);

							cout << "Writing joint geometry: " << fname << endl;
							
							fsLocator locator;
							fsFileUtil::ANSIFilenameToLocator(fname.asChar(), locator);

							if( fsFileUtil::FileExists(locator) )
								fsFileUtil::DeleteFile(locator);

							fsFileUtil::CreateFile(locator);

							gfFileBin file(locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
							file.WriteHeader();

							cout << "-------------------" << endl;
							cout << "Writing mesh info: " << endl;

							// clear out shared material table, 
							// start new group now
							SceneFuncs::ClearSharedMaterialTable();

							chBinWriter writer(file);
							writer.WriteChunkHeader(c_MSSK, 0, true);	// Single Skin Model

							// If we wanted to group single-skin materials
							// first we should do the following, but
							// it doesn't seem to matter with single-skin 
							// models since there is only one fragment.
							//
							// SceneFuncs::GatherSharedMaterials(mesh);
							// SceneFuncs::WriteSharedMaterialTable(writer);

							// Write mesh info
							SceneFuncs::WriteBRepToFile(mesh, writer);

							cout << "-------------------" << endl;
							cout << "Writing joint info: " << endl;

							// Write joint info
							MString efficiencyMsg;
							MMatrix root_matrix;
							JointFuncs::WriteJointBase(joint, root_matrix, writer, efficiencyMsg);

							writer.FinishChunk();

							// Stamp version into end of file
							SceneFuncs::WriteExporterVersionStamp(writer);
							
							// Check for efficiencies, gather a message to 
							// be reported in dialog
							SceneFuncs::GatherWarningMessages(mesh, efficiencyMsg);
							if (efficiencyMsg.length() > 0)
							{
								// Confirmation dialog.
								MayaUtil::DisplayConfirmation( efficiencyMsg, "Confirm flags" );
							}
						}
					}
					if (bDoAnim)
					{
						MString fname = MayaUtil::ConfirmExtension(path, ".jna");

						cout << "Writing joint animation: " << path << endl;
						JointFuncs::WriteJointAnim(joint, fname.asChar());
					}

					jointName = joint.name();
				}
				jointCount++;
			}

		}

	}

	if (jointCount == 0) 
	{
		cerr << "No joints found.  Created empty file." << endl;
		MayaUtil::PrintError("No joints found.  Created empty file.");
	}
	else if (jointCount > 1) 
	{
		cerr << "More than one joint visible.  Only wrote " << jointName << endl;
		MString err = MString("More than one joint visible.  Only wrote ") + jointName + ".";
		MayaUtil::PrintWarning(err);
	}
	else 
	{
		MayaUtil::PrintStatus("Done.");
	}

	MayaUtil::UnsetWaitCursor();

	return MS::kSuccess;
}

MStatus WriteJoints::doSubAnim()
{   
	MayaUtil::SetWaitCursor();

	MStatus status;
	
	//ParseSkinClusters();

	// Just do selected joint
	//
	MSelectionList activeList;
	status = MGlobal::getActiveSelectionList(activeList);
	MItSelectionList iter( activeList, MFn::kJoint );

	if (iter.isDone())
	{
		MGlobal::displayWarning("Writing subanim: No joint selected.");
		return MS::kFailure;
	}

	// Get filename into which to save subanim
	//
	cout << "Asking for filename." << endl;

	MString cmd = "getFilenameDialog -ext \"*.jna\";";

	MCommandResult result;
	MGlobal::executeCommand(cmd, result);

	MString path;
	status = result.getResult(path);

	if ((status != MS::kSuccess) || (path.length() == 0))
	{
		cout << "writeJoints: No file path specified." << endl;
		return MS::kFailure;
	}

	cout << "-----------------------------------------" << endl;
	
	// Output joint anim
	//
	int jointCount = 0;
	MString jointName;
	for ( ; !iter.isDone(); iter.next() )
	{								
		MDagPath dagPath;			
		MObject component;		
		iter.getDagPath( dagPath, component );
								
		if (dagPath.hasFn(MFn::kJoint)) 
		{
			cout << "Joint!" << endl;
			MFnIkJoint joint(dagPath, &status);
			if ( !status ) {
				status.perror("MFnIkJoint constructor");
				continue;
			}

			cout << "name = " << joint.name() << endl;
			cout << "path = " << dagPath.fullPathName() << endl;
			//printTransformData(dagPath, false);
			//dagIterator.prune();
			
			// check for visibility (including layers)
			// only write joint if it is visible
			// since we're pruning, we don't have to worry 
			// about children's visibility
			//

			if (MayaUtil::DagNodeVisible(joint)) 
			{
				//only write first skeleton encountered
				if (jointCount == 0) 
				{
					// must be true, but just in case
					if (bDoSubAnim)
					{
						MString fname = MayaUtil::ConfirmExtension(path, ".jna");

						cout << "Writing joint sub animation: " << path << endl;
						JointFuncs::WriteJointSubAnim(joint, fname.asChar());
					}

					jointName = joint.name();
				}
				jointCount++;
			}

		}

	}

	if (jointCount == 0) 
	{
		cerr << "No joints found.  Created empty file." << endl;
		MayaUtil::PrintError("No joints found.  Created empty file.");
	}
	else if (jointCount > 1) 
	{
		cerr << "More than one joint visible.  Only wrote " << jointName << endl;
		MString err = MString("More than one joint visible.  Only wrote ") + jointName + ".";
		MayaUtil::PrintWarning(err);
	}
	else 
	{
		MayaUtil::PrintStatus("Done.");
	}

	MayaUtil::UnsetWaitCursor();

	return MS::kSuccess;
}
