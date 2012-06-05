/*****************************************************************************
**  WriteVerts.cpp
**
**      
**
**	Extra Large Technology
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <WriteVerts.hpp>
#include <AnimFuncs.hpp>
#include <SceneFuncs.hpp>
#include <MayaFlagUtil.hpp>
#include <OrderingUtil.hpp>
#include <SurfaceUtil.hpp>
#include <SceneKeys.hpp>
#include <VertexKeys.hpp>
#include <CharacterFuncs.hpp>

#include <maya/MArgList.h>
#include <maya/MCommandResult.h>
#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
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
#include "Core/fs/fsLocator.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlNodeInfo.hpp"
#include "Graphics/mdl/mdlNodeUtil.hpp"
#include "Graphics/mdl/mdlWriter.hpp"
#include "Graphics/smdl/smdlCharacterSkin.hpp"
#include "Graphics/vtx/vtxGeometryCacheWriter.hpp"

#include <list>

namespace
{
	// Progress window might cause complications with the Maya MFn handles.
	// So, make it easy to turn it off with this constant:
	const bool c_bDoProgressWindow = true;

	// Define limit for vertex cache size. Out chunk file formats have 32-bit
	// size fields, so we can't go higher than can be expressed in 32-bits.
	const envType::UInt64 c_Megabyte = 1024*1024;
	const envType::UInt64 c_MaxVertexCacheSize = 4096*c_Megabyte; // 4GB
	
	// Merge material maximum number of triangles per mesh
	const int c_MaxNumTrisInMergedMesh = 30000;

	//=============================================================================
	//	Chunk types
	//=============================================================================
	const chDefs::Name c_MCHR = chDefs::MakeName('M', 'C', 'H', 'R');
	const chDefs::Name c_SKIN = chDefs::MakeName('S', 'K', 'I', 'N');
	const chDefs::Name c_ACHR = chDefs::MakeName('A', 'C', 'H', 'R');
	const chDefs::Name c_DELT = chDefs::MakeName('D', 'E', 'L', 'T');


	//=============================================================================
	// Object to restore the wait cursor when it goes out of scope.
	//=============================================================================
	struct WaitCursorWrapper
	{
		WaitCursorWrapper() { MayaUtil::SetWaitCursor(); }
		~WaitCursorWrapper() { MayaUtil::UnsetWaitCursor(); }
	};

	
	//=============================================================================
	// Recrusive version of print_graph_info - gathers counts of node types
	//=============================================================================
	void print_graph_info(const shared_ptr<mdlNodeInfo> &i_Graph,
						  int &o_NumSkinnedMeshes,
						  int &o_NumSubdivs)
	{
		if (i_Graph->m_MeshInfo)
		{
			if (i_Graph->m_SkinInfo)
				o_NumSkinnedMeshes++;
		}
		if (i_Graph->m_SubdivInfo) o_NumSubdivs++;

		for (int i=0; i<i_Graph->m_Children.size(); i++)
		{
			print_graph_info( i_Graph->m_Children[i], 
							  o_NumSkinnedMeshes,
							  o_NumSubdivs);
		}

	}
	
	//=============================================================================
	//=============================================================================
	void do_material_merge(shared_ptr< mdlNodeInfo >  &io_RootNode, bool i_bStatic)
	{
		if (i_bStatic)
		{
			int numberOfMeshesSubjectedToMerge = 0;
			int numberOfMergedMeshesResulted = 0;
			mdlNodeUtil::DoMerge("shape", io_RootNode, c_MaxNumTrisInMergedMesh, 
				numberOfMeshesSubjectedToMerge, numberOfMergedMeshesResulted);
			cout << "Merged materials, " << numberOfMeshesSubjectedToMerge << " meshes merged into " << numberOfMergedMeshesResulted << endl;
		}
		else
		{
			MayaUtil::DisplayError("sgpuWriteVerts : Only static geometry can be merged by material, use '-static' flag.");
		}
	}

	//=============================================================================
	// Print out debugging information about graph so that there is some
	// feedback to user about what was exported.
	//=============================================================================
	void print_graph_info(const shared_ptr<mdlNodeInfo> &i_Graph)
	{
		int numSkinnedMeshes = 0,
		    numSubdivs = 0;
		print_graph_info(i_Graph, 
						 numSkinnedMeshes,
						 numSubdivs);
		cout << "Exporting ..." << endl;
		if (numSkinnedMeshes > 0)
			cout << "  " << numSkinnedMeshes << " animating meshes" << endl;
		if (numSubdivs > 0)
			cout << "  " << numSubdivs << " subdivs" << endl;
	}
}

void *WriteVerts::creator()
{
	return new WriteVerts();
}

WriteVerts::WriteVerts()
	 : bDoGeom(false), bDoAnim(false), 
	   bForceAllSubdivs(false), bStatic(false),
	   bConfirmFlags(true), bSuggest(false), bMergeMaterials(false),
	   bUseCompressStream(false), m_Tolerance(-1.0f)
{
}

MStatus WriteVerts::ParseArgs(const MArgList& args)
{
	MStatus     	stat;
	MString     	arg;
	const MString	animFlag			("-anim");
	const MString	geomFlag			("-geom");
	const MString	staticFlag			("-static");
	const MString	fileFlag			("-f");
	const MString	fileFlagLongOld		("-File");
	const MString	fileFlagLong		("-file");
	const MString	exportSubdivFlag	("-exportassubdiv");
	const MString	confirmFlag			("-confirm");
	const MString	suggestFlag			("-suggest");
	const MString	compressFlag		("-compress");
	const MString	toleranceFlag		("-tolerance");
	const MString	mergeFlag			("-merge");

	// Parse the arguments.
	for ( size_t i = 0; i < args.length(); i++ ) {
		arg = args.asString( i, &stat );
		if (!stat)              
			continue;
				
		if ( arg == fileFlag || arg == fileFlagLong  || arg == fileFlagLongOld) {
			filePath = args.asString(i + 1, &stat);
			i++;
		}
		else if ( arg == animFlag  ) {
			cout << "Got anim flag" << endl;
			bDoAnim = true;
		}
		else if ( arg == geomFlag  ) {
			bDoGeom = true;
		}
		else if ( arg == staticFlag  ) {
			// Static flag keeps the "vertex anim" flag false,
			// basically making a way to export selected meshes as 
			// static geometry that will never be vertex animated.
			bDoGeom = true;
			bStatic = true;
		}
		else if ( arg == exportSubdivFlag  ) {
			bForceAllSubdivs = true;
		}
		else if (arg == confirmFlag ) {
			bConfirmFlags = false;
		}
		else if (arg == suggestFlag ) {
			bSuggest = true;
		}
		else if (arg == mergeFlag) {
			bMergeMaterials = true;
		}
		else if (arg == compressFlag) {
			bUseCompressStream = true;
		}
		else if (arg == toleranceFlag) {
			bUseCompressStream = true;

			double tol = args.asDouble(i + 1, &stat);
			i++;
			if (stat)
				m_Tolerance = (float) tol;
		}
		else if ( BakeCommand::ParseArg(args, i) ) {
			// BakeCommand read a frame start or frame end flag
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

MStatus WriteVerts::doIt(const MArgList &args)
{
	try
	{
		return safeDoIt(args);
	}
	catch (envExceptionX &i_Ex)
	{
		std::string msg = "sgpuWriteVerts Error : " + i_Ex.GetErrorMessage();
		MayaUtil::PrintError(msg.c_str());
		return MS::kFailure;
	}
	catch(...)
	{
		MayaUtil::PrintError("sgpuWriteVerts : General exception, command could not complete.");
		return MS::kFailure;
	}
}

MStatus WriteVerts::safeDoIt(const MArgList &args)
{
	cout << "-----------------------------------------" << endl;

	MStatus status;	

	// Parse command line
	status = ParseArgs (args);
	if (!status)
		return status;

	// In "-confirm" mode, don't display errors
	MayaUtil::SetQuietMode(!bConfirmFlags);
	MayaUtil::SetSuggestMode(bSuggest);

	// Turning this off for this animation just to be safe...
	CharacterFuncs::SetAutoDetectDeformation(false);

	// If no command line arguments, go to geom writing mode
	if (!bDoAnim && !bDoGeom)
	{
		cout << "No command line arguments, defaulting to geom" << endl;
		bDoGeom = true;
	}
	
	// Use ordering util to get paths to the objects in selection list
	// in an ordering that is based on the order of the full DAG.
	MDagPathArray sorted_paths;
	OrderingUtil::DetermineSelectionOrdering(MFn::kMesh, sorted_paths);

	if (sorted_paths.length() == 0)
	{
		MayaUtil::PrintError("WriteVerts : No meshes selected.");
		return MS::kFailure;
	}

	// Get default slider range, allow it to be changed by the arguments
	double minTime, maxTime;
	BakeCommand::GetTimelineRange(minTime, maxTime);

	if (bDoAnim)
	{
		// Estimate size of vertex animation cache
		const int num_keys = int(maxTime - minTime +1);

		envType::UInt64 total_est_size = 0, est_size = 0;
		const int num_paths = sorted_paths.length();
		for (int mi=0; mi<num_paths; ++mi) 
		{
			MDagPath dagPath = sorted_paths[mi];
			if (dagPath.hasFn(MFn::kMesh)) 
			{
				MFnMesh mesh(dagPath, &status);
				int num_verts = mesh.numVertices();
				int bWriteSubdiv = MayaFlagUtil::GetExportAsSubdivFlag(mesh);
				bool bWriteNormals = (bWriteSubdiv == 0);
				int num_norms = mesh.numNormals();

				// Each vertex position is 3 32-bit floats, so 12 bytes per frame.
				est_size = (num_verts * num_keys) * 12;
				if (bWriteNormals)
				{
					est_size += (num_norms * num_keys) * 12;
				}
				total_est_size += est_size;
			}
		}
		if (total_est_size > c_MaxVertexCacheSize)
		{
			cout << "Estimated vertex cache size: " << total_est_size/c_Megabyte << "MB max is " << c_MaxVertexCacheSize/c_Megabyte << "MB" << endl;
			MayaUtil::PrintError("WriteVerts : Baked vertex animation would exceed allowed maximum size of vertex cache.");
			return MS::kFailure;
		}
	}


	MGlobal::startErrorLogging();
	
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
			//cmd = "getFilenameDialog -ext \"*.gxb\";";
			cmd = "fileDialog -m 1 -dm \"*.gxb\";";
		}
		else if (bDoAnim)
		{
			//cmd = "getFilenameDialog -ext \"*.gab\";";
			cmd = "fileDialog -m 1 -dm \"*.gab\";";
		}
		MCommandResult result;
		MGlobal::executeCommand(cmd, result);
		status = result.getResult(path);
	}

	if ((status != MS::kSuccess) || (path.length() == 0))
	{
		cout << "sgpuWriteVerts: No file path specified." << endl;
		return MS::kFailure;
	}

	MString fname;
	if (bDoGeom)
	{
		fname = MayaUtil::ConfirmExtension(path, ".gxb");
		cout << "Writing character geometry: " << fname << endl;
	}
	else if (bDoAnim)
	{
		fname = MayaUtil::ConfirmExtension(path, ".gab");
		cout << "Writing character animation: " << fname << endl;
	}
						
	fsLocator locator;
	fsFileUtil::ANSIFilenameToLocator(fname.asUTF8(), locator);

	if( fsFileUtil::FileExists(locator) )
		fsFileUtil::DeleteFile(locator);

	fsFileUtil::CreateFile(locator);

	gfFileBin file(locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
	file.WriteHeader();

	bool bHaveProgressWindow = false;

	OrderingUtil::UniqueNameMap unique_name_map;
	if (bDoGeom)
	{
		// Geometry export
		
		// Only do wait cursor for geometry because the progress window in the
		// animation export will alter the wait cursor for us.

		// handles the wait cursor no matter how we return from this function.
		WaitCursorWrapper cursor_wrapper; 

		// Messages to display to user when export is finished
		MString efficiencyMsg;

		// Shared Material table
		mdlMatInfoTable material_table;

		// List of nodes to export
		std::vector< shared_ptr<mdlNodeInfo> > root_nodes;

		// Iterate through meshes
		int mesh_count = 0;
		const int num_paths = sorted_paths.length();
		for (int mi=0; mi<num_paths; ++mi) 
		{
			//cout << "Selection Dag Node: " << dagNode.name() << ", type = " << dagNode.typeName() << endl;
			MDagPath dagPath = sorted_paths[mi];
			if (dagPath.hasFn(MFn::kMesh)) 
			{
				MFnMesh mesh(dagPath, &status);

				// See if the matrix to this mesh has a negative scale
				MMatrix inc_mtx = dagPath.inclusiveMatrix();
				bool bNegativeScale = (inc_mtx.det3x3() < 0);

				//cout << "Selection name = " << mesh.name() << endl;
				//cout << "Selection path = " << dagPath.fullPathName() << endl;

				shared_ptr<mdlNodeInfo> mesh_node(new mdlNodeInfo());
				mesh_node->m_NodeName = MayaUtil::PrepareName(mesh.name()).asUTF8();
				OrderingUtil::CheckUniqueName(mesh_node->m_NodeName, unique_name_map);
				if (SurfaceUtil::GatherWorldSpaceMeshInfo(mesh, mesh_node->m_NodeName, !bStatic, bNegativeScale,
					*mesh_node, material_table, efficiencyMsg))
				{
					root_nodes.push_back( mesh_node );
					mesh_count++;
				}
			}
		}

		if (mesh_count == 0)
		{
			MayaUtil::PrintError("WriteVerts : No visible geometry selected for export.");
			return MS::kFailure;
		}

		// Begin writing file details
		chBinWriter writer(file);
		if (root_nodes.size() > 1)
		{
			// We handle multiple root nodes by adding a new root node
			// to group them all.
			shared_ptr<mdlNodeInfo> gxb_root(new mdlNodeInfo());
			gxb_root->m_NodeName = "gxb_root";
			gxb_root->m_Children = root_nodes;

			if (bMergeMaterials)
			{
				do_material_merge(gxb_root, bStatic);
			}

			print_graph_info(gxb_root);
			mdlWriter::WriteHierarchicalModel(writer, gxb_root, material_table);
		}
		else
		{
			if (bMergeMaterials)
			{
				do_material_merge(root_nodes[0], bStatic);
			}

			print_graph_info(root_nodes[0]);
			mdlWriter::WriteHierarchicalModel(writer, root_nodes[0], material_table);
		}

		// Stamp version into end of file
		SceneFuncs::WriteExporterVersionStamp(writer);
		cout << "Finished file." << endl;

		if (bSuggest && (efficiencyMsg.length() > 0))
		{
			// Confirmation dialog.
			MayaUtil::DisplayConfirmation( efficiencyMsg, "Confirm flags" );
		}

	}
	else if (bDoAnim)
	{
		// Animation export
		float begin_frame = (float) minTime;
		float end_frame = (float) maxTime;
		float frame_step = 1;

		// Generate our key steps instead of looking for baked keys
		std::list<float> keys;
		for (float i=begin_frame; i<=end_frame; i+=frame_step)
		{
			keys.push_back( (float) i);
		}

		chBinWriter writer(file);
		writer.WriteChunkHeader(c_ACHR, 0, true);	// Character Animation
		
		// Write in the frame rate in small chunk at top
		AnimFuncs::WriteCurrentFrameRate(writer);

		// When exporting a range of an animation, export the begin frame
		// that we used. Since our animation data is normalized to start with frame "zero"
		// when exported, we need to export the begin frame separately in order to
		// know where this animation should be placed in a timeline.
		AnimFuncs::WriteBeginFrame(writer, begin_frame);
		
		//MGlobal::doErrorLogEntry("Starting gather");

		// Data structures for baked key data
		std::list<VertexKeys::DelayedMeshAnim> delayed_meshes;
		std::list<VertexKeys::DelayedMeshStream> mesh_streams;
		std::list<SceneKeys::SurfaceVisKeys> skin_vis_keys;

		// Iterate through meshes in selection list
		bool bNeedTimelineBake = false, bCancelled = false;
		const int num_paths = sorted_paths.length();
		if (c_bDoProgressWindow && (num_paths > 0))
		{
			MString beginProgress("progressWindow -title \"Preparing Vertex Cache\" -isInterruptable true -min 0 ");
			beginProgress += MString(" -max ") + num_paths;
			MGlobal::executeCommand( beginProgress );
			bHaveProgressWindow = true;
		}

		// If we are using compression streams, create the geometry cache to hold
		// the reference frames and compressed delta blocks.
		shared_ptr<vtxGeometryCacheWriter> geom_cache;
		if (bUseCompressStream)
			geom_cache.reset(new vtxGeometryCacheWriter(writer, file));

		for (int mi=0; mi<num_paths; ++mi) 
		{
			if (c_bDoProgressWindow)
			{
				MCommandResult result;
				MGlobal::executeCommand("progressWindow -query -isCancelled", result);
				int bIsCancelled = 0;
				status = result.getResult(bIsCancelled);
				if ((status == MS::kSuccess) && (bIsCancelled != 0))
				{
					cout << "Animation baking was cancelled by user." << endl;
					bCancelled = true;
					break;
				}

				// Update progress window
				MGlobal::executeCommand(MString("progressWindow -edit -progress ") + mi);
			}

			MDagPath dagPath = sorted_paths[mi];
			if (dagPath.hasFn(MFn::kMesh)) 
			{
				MFnMesh mesh(dagPath, &status);
	
				// If flag is set, write this polygon mesh as a subdiv surface,
				// which means we don't need to export the normals for the 
				// vertex animation.
				int bWriteSubdiv = MayaFlagUtil::GetExportAsSubdivFlag(mesh);
				bool bWriteNormals = (bWriteSubdiv == 0);
 
				std::string mesh_name = MayaUtil::PrepareName(mesh.name()).asUTF8();
				OrderingUtil::CheckUniqueName(mesh_name, unique_name_map);

				if (bUseCompressStream)
				{
					// A negative value for tolerance produces a lossless stream (default is -1)
					VertexKeys::CreateAnimationStream(mesh, mesh_name, dagPath, bWriteNormals,
							   *geom_cache, mesh_streams, m_Tolerance);
				}
				else
				{
					// This allocates space in the exported file in order to fill
					// in the vertex buffers later.
					VertexKeys::PrepareVertexAnimation(mesh, mesh_name, dagPath,
						delayed_meshes, keys, writer, file, bWriteNormals);
				}


				// Check to see if we should export visibility animation for this mesh
				SceneKeys::GatherSurfaceVisibility(dagPath, skin_vis_keys, mesh_name);

				bNeedTimelineBake = true;
			}
		}

		if (!bCancelled)
		{
			if (bHaveProgressWindow)
			{
				MGlobal::executeCommand("progressWindow -endProgress");
				bHaveProgressWindow = false;
			}

			if (bNeedTimelineBake)
			{
				//MGlobal::doErrorLogEntry("Timeline bake");

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

				// Open the geometry cache now to receive the frames as we
				// compress them in the streams.
				if (bUseCompressStream)
					geom_cache->OpenCache();

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

					// Move the Maya timeline to the current frame, except
					// when using pose animations, in which case we export the
					// current time's values.

					//MTime time(*it);
					MTime time(frame, MTime::uiUnit());
					MayaUtil::SetCurrentTime(time);

					// Gather animation data
					const bool bWorldSpace = true;
					if (bUseCompressStream)
					{
						if (!VertexKeys::BakeAnimationStreams(mesh_streams, frame, begin_frame, bWorldSpace))
						{
							MayaUtil::PrintError("Cannot get state of mesh. Stored mesh is invalid.");
							//return MS::kFailure;
						}
					}
					else
					{
						if (!VertexKeys::BakeAnimation(writer, file, delayed_meshes, 
														frame, begin_frame, bWorldSpace))
						{
							MayaUtil::PrintError("Cannot get state of mesh. Stored mesh is invalid.");
							//return MS::kFailure;
						}
					}
					
					// Gather visibility state
					SceneKeys::GatherKeys(skin_vis_keys, frame);
				}

				// Moved down lower
				//MGlobal::executeCommand("progressWindow -endProgress");

				// Restore the original timeline time
				MayaUtil::SetCurrentTime(currentTime);

				//MGlobal::doErrorLogEntry("Done seeking.");
				cout << "Done seeking." << endl;
			}
		
			// Close the geometry cache now that the streams are over.
			if (bUseCompressStream)
			{
				VertexKeys::FinishStreams(mesh_streams);
				geom_cache->CloseCache();

				// Write out the frame data that references into the geometry cache
				VertexKeys::WriteAnimationStreams(writer, mesh_streams);
			}
			
			// Move the file cursor to the end of the file to continue writing
			file.SetFilePos(0, fsFileStream::e_End);

			// Write skin visibility that was gathered while baking
			SceneKeys::WriteAnimation(writer, skin_vis_keys, begin_frame);

			// Finish the animation chunk
			writer.FinishChunk(); // c_ACHR

			// Stamp version into end of file
			SceneFuncs::WriteExporterVersionStamp(writer);
			cout << "Finished file." << endl;
		}
	}


	MGlobal::stopErrorLogging();

	if (bHaveProgressWindow)
	{
		MGlobal::executeCommand("progressWindow -endProgress");
	}

	MayaUtil::PrintStatus("Done.");

	return MS::kSuccess;
}
