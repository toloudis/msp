/*****************************************************************************
**  WriteCharacter.cpp
**
**      
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <WriteCharacter.hpp>
#include <AnimFuncs.hpp>
#include <BlendShapeKeys.hpp>
#include <SceneFuncs.hpp>
#include <SceneKeys.hpp>
#include <CharacterFuncs.hpp>
#include <JointFuncs.hpp>
#include <SubdivFuncs.hpp>
#include <VertexFuncs.hpp>
#include <VertexKeys.hpp>

#include <maya/MArgList.h>
#include <maya/MCommandResult.h>
#include <maya/MDagPath.h>
#include <maya/MFnBlendShapeDeformer.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnIkJoint.h>
#include <maya/MFnMesh.h>
#include <maya/MFnSubd.h>
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

#include <list>

namespace
{
	//=============================================================================
	//	Chunk types
	//=============================================================================
	const chDefs::Name c_MCHR = chDefs::MakeName('M', 'C', 'H', 'R');
	const chDefs::Name c_SKIN = chDefs::MakeName('S', 'K', 'I', 'N');
	const chDefs::Name c_ACHR = chDefs::MakeName('A', 'C', 'H', 'R');

}

void *WriteCharacter::creator()
{
	return new WriteCharacter();
}

WriteCharacter::WriteCharacter()
	 : shareMaterials(true), bDoGeom(false), bDoAnim(false), 
	   bDoSubAnim(false), bDoPose(false), bDoDeltas(false),
	   bForceAllSubdivs(false), bUseMayaBaking(true)
{
}

MStatus WriteCharacter::ParseArgs(const MArgList& args)
{
	MStatus     	stat;
	MString     	arg;
	const MString	animFlag			("-anim");
	const MString	subanimFlag			("-subanim");
	const MString	geomFlag			("-geom");
	const MString	fileFlag			("-f");
	const MString	fileFlagLong		("-File");
	const MString	noShareFlag			("-nosharemats");
	const MString	poseFlag			("-pose");
	const MString	expressionFlag		("-expression");
	const MString	expressionAnimFlag	("-expressionanim");
	const MString	exportSubdivFlag	("-exportassubdiv");
	const MString	bakedFlag			("-bake");

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
		else if ( arg == animFlag  ) {
			cout << "Got anim flag" << endl;
			bDoAnim = true;
		}
		else if ( arg == subanimFlag  ) {
			bDoSubAnim = true;
		}
		else if ( arg == geomFlag  ) {
			bDoGeom = true;
		}
		else if ( arg == poseFlag  ) {
			bDoPose = true;
		}
		else if ( arg == exportSubdivFlag  ) {
			bForceAllSubdivs = true;
		}
		else if ( arg == expressionFlag  ) {
			bDoPose = true;
			bDoDeltas = true;
			bDoSubAnim = true;
		}
		else if ( arg == expressionAnimFlag  ) {
			bDoPose = false;
			bDoDeltas = true;
			bDoSubAnim = true;
		}
		else if (arg == bakedFlag ) {
			bUseMayaBaking = false;
		}
		else {
			//arg += ": unknown argument";
			//displayError(arg);
			//return MS::kFailure;
			//
			//just ignore extraneous args
		}
	}
	return stat;
}



MStatus WriteCharacter::debugIt(const MArgList &args)
{
	MayaUtil::SetWaitCursor();

	cout << endl;
	cout << "------ WriteCharacter::debugIt() pass." << endl;

	MStatus status;			
	MObject component;
	MDagPath dagPath;

	// Get list of selected subdivs. 
	//
	MSelectionList activeList;
	status = MGlobal::getActiveSelectionList(activeList);
	MItSelectionList iter( activeList, MFn::kSubdiv );
	if (iter.isDone())
	{
		MGlobal::displayWarning("No subdivision surfaces selected.");
		return MS::kFailure;
	}

	for ( ; !iter.isDone(); iter.next() )
	{							
		cout << "Iterating through selection." << endl;
		iter.getDagPath( dagPath, component );
								
		MFnSubd subdiv(dagPath, &status);
		if (!status) continue;

		CharacterFuncs::DebugInputs(subdiv);
	}

	MayaUtil::UnsetWaitCursor();

	return MS::kSuccess;
}

MStatus WriteCharacter::doIt(const MArgList &args)
{
	MayaUtil::SetWaitCursor();

	MStatus status;			
	MObject component;
	MDagPath dagPath;

	// Parse command line
	status = ParseArgs (args);
	if (!status)
		return status;

	if (bDoSubAnim)
	{
		return doSubAnim();
	}

	// If no command line arguments, go to geom writing mode
	if (!bDoAnim && !bDoGeom)
	{
		cout << "no command line arguments, defaulting to geom" << endl;
		bDoGeom = true;
	}

	if (bDoAnim && bDoDeltas)
	{
		MGlobal::displayWarning("WriteCharacter : Cannot export full animation with deltas.");
		return MS::kFailure;
	}

	// First of all, get list of selected subdivs.  WriteCharacter
	//	is different from WriteJoints in that the subdivs need to be selected
	//	since more than one can be written at a time.
	//
	MSelectionList activeList;
	status = MGlobal::getActiveSelectionList(activeList);
	MItSelectionList subd_iter( activeList, MFn::kSubdiv );
	MItSelectionList mesh_iter( activeList, MFn::kMesh );

	// Just do selected joint, don't search for visible skeletons in DAG anymore
	//
	MItSelectionList joint_iter( activeList, MFn::kJoint );
	if (joint_iter.isDone())
	{
		MGlobal::displayWarning("WriteCharacter : No joint selected.");
		return MS::kFailure;
	}

	// Check to see if there is any selected geometry, if we are exporting geometry
	if (bDoGeom)
	{
		// Look for meshes embedded in joint hierarchy
		MObject joint_obj;
		joint_iter.getDependNode(joint_obj);
		bool bEmbeddedMeshes = MayaUtil::HasTypeAsChild(joint_obj, MFn::kMesh);

		// Check direct selection for meshes and subdvis
		bool bSelectedGeom = (!subd_iter.isDone() || !mesh_iter.isDone());

		// If no geometry in joint tree or in selected list, then return error
		if (!bEmbeddedMeshes && !bSelectedGeom)
		{
			MGlobal::displayWarning("No subdivision or mesh surfaces selected.");
			return MS::kFailure;
		}
	}

	
	// Get filename into which to save character data
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
			cmd = "getFilenameDialog -ext \"*.chx\";";
		}
		else if (bDoAnim)
		{
			cmd = "getFilenameDialog -ext \"*.cha\";";
		}
		MCommandResult result;
		MGlobal::executeCommand(cmd, result);
		status = result.getResult(path);
	}

	if ((status != MS::kSuccess) || (path.length() == 0))
	{
		cout << "writeCharacter: No file path specified." << endl;
		return MS::kFailure;
	}

	cout << "-----------------------------------------" << endl;
	

	MItDag dagIterator( MItDag::kBreadthFirst, MFn::kInvalid, &status);

	if ( !status) {
		MayaUtil::PrintError("MItDag constructor");
		MayaUtil::UnsetWaitCursor();
		return status;
	}

	//	Scan the DAG for joints
	//
	int jointCount = 0;
	MString jointName;
	MString efficiencyMsg;
	for ( ; !joint_iter.isDone(); joint_iter.next() ) {

		MDagPath dagPath;			
		MObject component;		
		status = joint_iter.getDagPath( dagPath, component );
		if ( !status ) {
			status.perror("MItDag::getPath");
			continue;
		}

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

			//if (MayaUtil::DagNodeVisible(joint)) 
			{
				//only write first skeleton encountered
				if (jointCount == 0) 
				{
					// Get the transformation matrix above the joint
					MMatrix exclMatrix = dagPath.exclusiveMatrix();

					if (bDoGeom)
					{
						MString fname = MayaUtil::ConfirmExtension(path, ".chx");
						cout << "Writing character geometry: " << fname << endl;
						
						fsLocator locator;
						fsFileUtil::ANSIFilenameToLocator(fname.asChar(), locator);

						if( fsFileUtil::FileExists(locator) )
							fsFileUtil::DeleteFile(locator);

						fsFileUtil::CreateFile(locator);

						gfFileBin file(locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
						file.WriteHeader();

						chBinWriter writer(file);
						writer.WriteChunkHeader(c_MCHR, 0, true);	// Character Model

						// Write a matrix that transforms our root joint into world space
						SceneFuncs::WriteExclusiveMatrix(writer, exclMatrix);

						// shared material gathering
						if (shareMaterials)
						{
							cout << "-------------------" << endl;
							cout << "Writing shared material table: " << endl;

							// clear out shared material table, 
							// start new group now
							SceneFuncs::ClearSharedMaterialTable();

							// Need to start with the root joint in order to 
							// gather the materials in the embedded meshes
							// (like the low-res meshes directly in the joint tree)
							SceneFuncs::GatherSharedMaterials(joint.object());

							// Iterate through subdivs and meshes, gathering all 
							// materials into one table
							MItSelectionList iter2( activeList, MFn::kSubdiv );
							for ( ; !iter2.isDone(); iter2.next() )
							{							
								iter2.getDagPath( dagPath, component );
														
								MFnSubd subdiv(dagPath, &status);
								if (status)
									SceneFuncs::GatherSharedMaterials(subdiv.object());
							}
							MItSelectionList iter3( activeList, MFn::kMesh );
							for ( ; !iter3.isDone(); iter3.next() )
							{							
								iter3.getDagPath( dagPath, component );
														
								MFnMesh mesh(dagPath, &status);
								if (status)
									SceneFuncs::GatherSharedMaterials(mesh.object());
							}

							// Write out table 
							SceneFuncs::WriteSharedMaterialTable(writer);
						}

						cout << "-------------------" << endl;
						cout << "Writing joint info: " << endl;

						// Write joint info (without influences)
						const bool bWriteInfluences = false;
						JointFuncs::WriteJointBase(joint, exclMatrix, writer, efficiencyMsg, bWriteInfluences);

						// In multiple-skin method, Iterate through all selected subdivs
						// parsing and writing influence of skeleton on each skin separately.
						for ( ; !subd_iter.isDone(); subd_iter.next() )
						{							
							cout << "Iterating through subdivs in selection." << endl;
							subd_iter.getDagPath( dagPath, component );
													
							MFnSubd subdiv(dagPath, &status);
							if (!status) continue;

							// Parse skin clusters for this subdiv
							cout << "Parsing skin clusters for multiple skin, subdiv " << subdiv.name() << endl;
							JointFuncs::ParseSkinClusters(subdiv);

							// updating to version 1 when using subdivs
							writer.WriteChunkHeader(c_SKIN, 1, true);	// Skin in character model

							cout << "-------------------" << endl;
							cout << "Writing Subdiv info: " << endl;

							CharacterFuncs::WriteSubdivAndMorphTargets(subdiv, writer);

							// Write influences of skeleton on this Subdiv
							JointFuncs::WriteInfluenceTree(joint, writer, efficiencyMsg);

							writer.FinishChunk(); // c_SKIN
							cout << "Finished skin." << endl;

							// Check for efficiencies, gather a message to 
							// be reported in dialog
							SubdivFuncs::GatherWarningMessages(subdiv, efficiencyMsg);
							
						} // end of Subdiv iteration for loop

						cout << "Finished subdiv loop." << endl;
				
						// Now, iterate through all selected meshes
						// parsing and writing influence of skeleton on each skin separately.
						for ( ; !mesh_iter.isDone(); mesh_iter.next() )
						{							
							cout << "Iterating through meshes in selection." << endl;
							mesh_iter.getDagPath( dagPath, component );
													
							MFnMesh mesh(dagPath, &status);
							if (!status) continue;

							// Parse skin clusters for this subdiv
							cout << "Parsing skin clusters for multiple skin, mesh " << mesh.name() << endl;
							JointFuncs::ParseSkinClusters(mesh);

							// updating to version 2 when using meshes
							writer.WriteChunkHeader(c_SKIN, 2, true);	// Skin in character model

							cout << "-------------------" << endl;
							cout << "Writing Mesh info: " << endl;
							
							// If flag is set, write this polygon mesh as a subdiv surface
							int bWriteSubdiv = MayaUtil::GetEngineFlag(mesh, "exportAsSubdiv");

							// Write mesh info
							CharacterFuncs::WriteMeshAndMorphTargets(mesh, 
								writer, 
								bForceAllSubdivs || bWriteSubdiv);

							// Write influences of skeleton on this Subdiv
							JointFuncs::WriteInfluenceTree(joint, writer, efficiencyMsg);

							writer.FinishChunk(); // c_SKIN
							cout << "Finished skin." << endl;

							// Check for efficiencies, gather a message to 
							// be reported in dialog
							SceneFuncs::GatherWarningMessages(mesh, efficiencyMsg);
							
						} // end of Mesh iteration for loop

						cout << "Finished mesh loop." << endl;

						writer.FinishChunk(); // c_MCHR

						// Stamp version into end of file
						SceneFuncs::WriteExporterVersionStamp(writer);
						cout << "Finished file." << endl;
					}
					if (bDoAnim)
					{
						MGlobal::startErrorLogging();

						MString fname = MayaUtil::ConfirmExtension(path, ".cha");

						// Get default slider range, allow it to be changed by the arguments
						double minTime, maxTime;
						AnimFuncs::GetTimeSliderRange(minTime, maxTime);
						float begin_frame = (float) minTime;
						float end_frame = (float) maxTime;
						float frame_step = 1;

						if (bDoPose)
						{
							// Poses are exported as 1 frame length looping animation
							// with both frames at the same value
							begin_frame = 0;
							end_frame = 1;
						}

						// Generate our key steps instead of looking for baked keys
						std::list<float> keys;
						for (float i=begin_frame; i<=end_frame; i+=frame_step)
						{
							keys.push_back( (float) i);
						}

						cout << "Writing character animation: " << path << endl;

						fsLocator locator;
						fsFileUtil::ANSIFilenameToLocator(fname.asChar(), locator);

						if( fsFileUtil::FileExists(locator) )
							fsFileUtil::DeleteFile(locator);

						fsFileUtil::CreateFile(locator);

						gfFileBin file(locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
						file.WriteHeader();

						chBinWriter writer(file);
						writer.WriteChunkHeader(c_ACHR, 0, true);	// Character Animation

						
						//MGlobal::doErrorLogEntry("Starting gather");

						// Data structures for when we bake the animation data:
						std::list<SceneKeys::PathKeys> path_keys;
						std::list<VertexKeys::DelayedMeshAnim> delayed_meshes;
						std::list<VertexKeys::DelayedSubdAnim> delayed_subdivs;
						std::list<BlendShapeKeys::BlendKeys> blend_keys;
						std::list<SceneKeys::SurfaceVisKeys> surface_keys;

						if (bUseMayaBaking)
						{
							cout << "Writing joint animation from baked anim channels. " << endl;
							// Using SceneFuncs variation now in order to look for 
							// embedded meshes with animation (particularly visibility anims)
							//JointFuncs::WriteJointAnim(joint, writer, bDoPose);
							SceneFuncs::WriteAnimTransform(joint, writer, bDoPose);
						}
						else
						{
							// Gather data structures for baking joints
							cout << "Gathering joints to do our own baking: " << endl;
							SceneKeys::GatherHierarchy(joint, path_keys);
						}

						//MGlobal::doErrorLogEntry(" iter subdivs");


						// Look for blend shape and vertex animation on subdivs
						for ( ; !subd_iter.isDone(); subd_iter.next() )
						{							
							cout << "Iterating through subdivs in selection." << endl;
							subd_iter.getDagPath( dagPath, component );
													
							MFnSubd subdiv(dagPath, &status);
							if (!status) continue;
						
							// blend shape
							if (bUseMayaBaking)
								CharacterFuncs::WriteBlendShapeAnimation(subdiv, writer, bDoPose);
							else
								BlendShapeKeys::GatherBlendShapes(subdiv, blend_keys);

							// vertex animation
							int cloth = MayaUtil::GetEngineFlag(subdiv, "Cloth");
							cout << "write vertex animation?: cloth = " << cloth << endl;
							if (cloth)
							{
								// old style, does a time loop for each surface:
								VertexFuncs::WriteVertexAnimation(subdiv, writer, keys, begin_frame);

								// NOTE: Subdiv baking didn't work, it returns the same base mesh
								// every frame. They need to be baked by Maya.

								// This allocates space in the exported file in order to fill
								// in the vertex buffers later in one pass:
								//VertexKeys::PrepareVertexAnimation(subdiv, delayed_subdivs, keys, writer, file);
							}

							if (bUseMayaBaking)
							{
								// visibility animation occurs on the transform above the surface
								AnimFuncs::WriteSkinVisibility(writer, dagPath, subdiv.name());
							}
							else
							{
								// gather visibility anims for surfaces
								SceneKeys::GatherSurfaceVisibility(dagPath, surface_keys, subdiv.name());
							}
						}

						//MGlobal::doErrorLogEntry(" iter meshes");

						// Look for vertex animation on meshes
						for ( ; !mesh_iter.isDone(); mesh_iter.next() )
						{							
							cout << "Iterating through meshes in selection." << endl;
							mesh_iter.getDagPath( dagPath, component );
													
							MFnMesh mesh(dagPath, &status);
							if (!status) continue;
						
							// blend shape
							if (bUseMayaBaking)
								CharacterFuncs::WriteBlendShapeAnimation(mesh, writer, bDoPose);
							else
								BlendShapeKeys::GatherBlendShapes(mesh, blend_keys);
							
							if (bUseMayaBaking)
							{
								// visibility animation occurs on the transform above the surface
								AnimFuncs::WriteSkinVisibility(writer, dagPath, mesh.name());
							}
							else
							{
								// gather visibility anims for surfaces
								SceneKeys::GatherSurfaceVisibility(dagPath, surface_keys, mesh.name());
							}

							// vertex animation
							int cloth = MayaUtil::GetEngineFlag(mesh, "Cloth");
							cout << "write vertex animation?: cloth = " << cloth << endl;
							if (cloth)
							{
								// If flag is set, write this polygon mesh as a subdiv surface,
								// which means we don't need to export the normals for the 
								// vertex animation.
								int bWriteSubdiv = MayaUtil::GetEngineFlag(mesh, "exportAsSubdiv");
								bool bWriteNormals = (bWriteSubdiv == 0);

								// Don't consider the bUseMayaBaking flag here, we want
								// to gather the polygon mesh data ourselves no matter what.
								
								// This version will move the timeline in order to write out the data
								//VertexFuncs::WriteVertexAnimation(mesh, writer, keys, begin_frame, bWriteNormals);
							
								// This allocates space in the exported file in order to fill
								// in the vertex buffers later.
								VertexKeys::PrepareVertexAnimation(mesh, dagPath,
									delayed_meshes, keys, writer, file, bWriteNormals);
							
							}
						}

						bool bNeedTimelineBake = (!bUseMayaBaking || !delayed_meshes.empty());
						if (bNeedTimelineBake)
						{
							//MGlobal::doErrorLogEntry("Timeline bake");

							// Move through the Maya timeline once. During this pass,
							// we gather up the keys for the joint animation and at the same
							// time, we store the baked vertex animation into the space
							// that has already been allocated in the file. The joint data
							// won't be written until after we have gone through the file
							// because we don't know how many keys are relevant for each joint yet.
							cout << "Seeking through time to bake data..." << endl;
							std::list<float>::iterator it;
							for (it = keys.begin(); it != keys.end(); ++it)
							{
								float frame = (*it);
								//MGlobal::doErrorLogEntry("Doing frame.");

								if (!bDoPose)
								{
									// Move the Maya timeline to the current frame, except
									// when using pose animations, in which case we export the
									// current time's values.


									//MTime time(*it);
									MTime time(frame, MTime::uiUnit());
									MayaUtil::SetCurrentTime(time);
								}

								if (!bUseMayaBaking)
								{
									//MGlobal::doErrorLogEntry("SceneKeys::GatherKeys");

									// Gather position of joints
									SceneKeys::GatherKeys(path_keys, frame);

									// Gather blend shape weights
									BlendShapeKeys::GatherKeys(blend_keys, frame);

									// Gather the visibility animations 
									SceneKeys::GatherKeys(surface_keys, frame);
								}

								// If we are writing pose data, then we have actually already
								// written the current poses and don't need to write this again
								if (!bDoPose)
								{
									//MGlobal::doErrorLogEntry("VertexKeys::BakeAnimation");
									// Bake current position of vertex animation
									//VertexKeys::BakeAnimation(writer, file, delayed_subdivs, frame, begin_frame);
									const bool bWorldSpace = false;
									if (!VertexKeys::BakeAnimation(writer, file, delayed_meshes, frame, begin_frame, bWorldSpace))
									{
										MayaUtil::PrintError("Cannot get state of mesh. Stored mesh is invalid.");
										return MS::kFailure;
									}
								}
							}
							//MGlobal::doErrorLogEntry("Done seeking.");
							cout << "Done seeking." << endl;

							if (!bUseMayaBaking)
							{
								// Move the file cursor to the end of the file to continue writing
								file.SetFilePos(0, fsFileStream::e_End);

								// Write the processed joint keys to file
								cout << "Writing joint animation to file." << endl;
								SceneKeys::WriteAnimation(writer, joint, path_keys, begin_frame);

								// Write the processed blend shape animations to file
								BlendShapeKeys::WriteAnimation(writer, blend_keys, begin_frame);

								// Write the processed surface visibility animations to file
								SceneKeys::WriteAnimation(writer, surface_keys, begin_frame);
							}
						}

						writer.FinishChunk(); // c_ACHR

						// Stamp version into end of file
						SceneFuncs::WriteExporterVersionStamp(writer);
						cout << "Finished file." << endl;

						MGlobal::stopErrorLogging();
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


	if (efficiencyMsg.length() > 0)
	{
		// Confirmation dialog.
		MayaUtil::DisplayConfirmation( efficiencyMsg, "Confirm flags" );
	}

	MayaUtil::UnsetWaitCursor();

	return MS::kSuccess;
}


MStatus WriteCharacter::doSubAnim()
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
	MString path;
	if (filePath.length() > 0)
	{
		cout << "Have filename from command line " << endl;
		path = filePath;
	}
	else
	{
		cout << "Asking for filename." << endl;
		MString cmd = "getFilenameDialog -ext \"*.cha\";";
		MCommandResult result;
		MGlobal::executeCommand(cmd, result);
		status = result.getResult(path);
	}

	if ((status != MS::kSuccess) || (path.length() == 0))
	{
		cout << "writeCharacter: No file path specified." << endl;
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
						MString fname = MayaUtil::ConfirmExtension(path, ".cha");

						cout << "Writing character sub animation: " << path << endl;

						fsLocator locator;
						fsFileUtil::ANSIFilenameToLocator(fname.asChar(), locator);

						if( fsFileUtil::FileExists(locator) )
							fsFileUtil::DeleteFile(locator);

						fsFileUtil::CreateFile(locator);

						gfFileBin file(locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
						file.WriteHeader();

						chBinWriter writer(file);
						writer.WriteChunkHeader(c_ACHR, 0, true);	// Character Animation

						cout << "Writing joint sub animation: " << path << endl;
						JointFuncs::WriteJointSubAnim(joint, writer, bDoPose, bDoDeltas);

						MItSelectionList subdiv_iter( activeList, MFn::kSubdiv );
						for ( ; !subdiv_iter.isDone(); subdiv_iter.next() )
						{							
							cout << "Iterating through subdivs in selection." << endl;
							subdiv_iter.getDagPath( dagPath, component );
													
							MFnSubd subdiv(dagPath, &status);
							if (!status) continue;
						
							CharacterFuncs::WriteBlendShapeAnimation(subdiv, writer, bDoPose);
						}
						// Look for vertex animation on meshes
						MItSelectionList mesh_iter( activeList, MFn::kMesh );
						for ( ; !mesh_iter.isDone(); mesh_iter.next() )
						{							
							cout << "Iterating through meshes in selection." << endl;
							mesh_iter.getDagPath( dagPath, component );
													
							MFnMesh mesh(dagPath, &status);
							if (!status) continue;
						
							// blend shape
							CharacterFuncs::WriteBlendShapeAnimation(mesh, writer, bDoPose);
						}

						writer.FinishChunk(); // c_ACHR

						// Stamp version into end of file
						SceneFuncs::WriteExporterVersionStamp(writer);
						cout << "Finished file." << endl;

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
