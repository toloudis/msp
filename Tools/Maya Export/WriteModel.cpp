/*****************************************************************************
**  WriteModel.cpp
**
**      
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <WriteModel.hpp>
#include <AnimFuncs.hpp>
#include <BlendShapeKeys.hpp>
#include <SceneFuncs.hpp>
#include <SceneKeys.hpp>
#include <CharacterFuncs.hpp>
#include <HierarchyUtil.hpp>
#include <JointFuncs.hpp>
#include <SubdivFuncs.hpp>
#include <VertexFuncs.hpp>
#include <MayaFlagUtil.hpp>

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
	//	Check the list of dagPaths and see if any paths
	//	are contained within other paths. This would cause
	//	some parts of the hierarchy to export twice, so this
	//	needs to be reported as an error.
	//=============================================================================
	bool check_multiple_selection(MDagPathArray& i_SelPaths)
	{
		int num_paths = i_SelPaths.length();
		for (int i=0; i<num_paths; i++)
		{
			int path_len1 = i_SelPaths[i].length();
			for (int j=0; j<num_paths; j++)
			{
				if (i != j)
				{
					MDagPath copy_path(i_SelPaths[j]);
					if (copy_path.length() > path_len1)
					{
						copy_path.pop( copy_path.length() - path_len1 );
					}

					// Now the paths are either equal or path_len1 is greater.
					// Since we are looking through all i and j combinations, we can
					// ignore the path_len1 greater case because it will come up some
					// other time when i=j and j=i.

					if (copy_path == i_SelPaths[i])
						return true;
				}
			}
		}
		return false;
	}
	
	//=============================================================================
	//	Count the number of meshes or subdivs within the graph rooted at this node.
	//=============================================================================
	int count_surfaces(const shared_ptr<mdlNodeInfo> &i_Graph)
	{
		int count = 0;

		if (i_Graph->m_MeshInfo) count++;
		if (i_Graph->m_SubdivInfo) count++;

		for (int i=0; i<i_Graph->m_Children.size(); i++)
		{
			count += count_surfaces(i_Graph->m_Children[i]);
		}

		return count;
	}

	//=============================================================================
	// Recrusive version of print_graph_info - gathers counts of node types
	//=============================================================================
	void print_graph_info(const shared_ptr<mdlNodeInfo> &i_Graph,
						  int &o_NumTransforms,
						  int &o_NumJoints,
						  int &o_NumRigidMeshes,
						  int &o_NumSkinnedMeshes,
						  int &o_NumSubdivs)
	{
		o_NumTransforms++;
		if (i_Graph->m_bIsJoint) o_NumJoints++;
		if (i_Graph->m_MeshInfo)
		{
			if (i_Graph->m_SkinInfo)
				o_NumSkinnedMeshes++;
			else 
				o_NumRigidMeshes++;
		}
		if (i_Graph->m_SubdivInfo) o_NumSubdivs++;

		if (i_Graph->m_SkinInfo)
		{
			int num_morphs = i_Graph->m_SkinInfo->m_MorphTargets.size();
			if (num_morphs > 0)
				cout << "Surface " << i_Graph->m_NodeName << " has " << num_morphs << " morph targets." << endl;
		}

		for (int i=0; i<i_Graph->m_Children.size(); i++)
		{
			print_graph_info( i_Graph->m_Children[i], 
							  o_NumTransforms,
							  o_NumJoints,
							  o_NumRigidMeshes,
							  o_NumSkinnedMeshes,
							  o_NumSubdivs);
		}

	}

	//=============================================================================
	// When we add a root node when multiple roots are selected, this alters
	//	the instance references so that we have to add that root node name
	//	to the beginning of all instance info structures
	//=============================================================================
	void prefix_instance_info(const shared_ptr<mdlNodeInfo> &i_Graph,
						  const std::string &i_NewRootNodeName)
	{
		if (i_Graph->m_InstanceInfo.get())
		{
			i_Graph->m_InstanceInfo->m_Path.push_front( i_NewRootNodeName );
		}

		for (int i=0; i<i_Graph->m_Children.size(); i++)
		{
			prefix_instance_info( i_Graph->m_Children[i], i_NewRootNodeName);
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
			MayaUtil::DisplayError("sgpuWriteModel : Only static geometry can be merged by material, use '-static' flag.");
		}
	}

	//=============================================================================
	// Print out debugging information about graph so that there is some
	// feedback to user about what was exported.
	//=============================================================================
	void print_graph_info(const shared_ptr<mdlNodeInfo> &i_Graph)
	{
		int numTransforms = 0,
		    numJoints = 0,
		    numRigidMeshes = 0,
		    numSkinnedMeshes = 0,
		    numSubdivs = 0;
		print_graph_info(i_Graph, 
						 numTransforms,
						 numJoints,
						 numRigidMeshes,
						 numSkinnedMeshes,
						 numSubdivs);
		cout << "Exporting ..." << endl;
		if (numTransforms > 0)
			cout << "  " << numTransforms << " nodes" << endl;
		if (numJoints > 0)
			cout << "  " << numJoints << " joints" << endl;
		if (numRigidMeshes > 0)
			cout << "  " << numRigidMeshes << " rigid meshes" << endl;
		if (numSkinnedMeshes > 0)
			cout << "  " << numSkinnedMeshes << " animating meshes" << endl;
		if (numSubdivs > 0)
			cout << "  " << numSubdivs << " subdivs" << endl;
	}
}

void *WriteModel::creator()
{
	return new WriteModel();
}

WriteModel::WriteModel()
	 : shareMaterials(true), bDoGeom(false), bDoAnim(false), bStatic(false),
	   /*bDoSubAnim(false),*/ bDoPose(false), bDoDeltas(false),
	   bForceAllSubdivs(false), bUseMayaAnimCurves(false),
	   bConfirmFlags(true), bSuggest(false), bAutoDetect(true), bMergeMaterials(false),
	   bUseCompressStream(false), m_Tolerance(-1.0f)
{
}

MStatus WriteModel::ParseArgs(const MArgList& args)
{
	MStatus     	stat;
	MString     	arg;
	const MString	animFlag			("-anim");
	//const MString	subanimFlag			("-subanim");
	const MString	geomFlag			("-geom");
	const MString	staticFlag			("-static");
	const MString	fileFlag			("-f");
	const MString	fileFlagLongOld		("-File");
	const MString	fileFlagLong		("-file");
	const MString	noShareFlag			("-nosharemats");
	const MString	poseFlag			("-pose");
	const MString	expressionFlag		("-expression");
	const MString	expressionAnimFlag	("-expressionanim");
	const MString	exportSubdivFlag	("-exportassubdiv");
	const MString	nobakedFlag			("-nobake");
	const MString	noBakedFlag			("-noBake");
	const MString	confirmFlag			("-confirm");
	const MString	suggestFlag			("-suggest");
	const MString	mergeFlag			("-merge");
	const MString	noAutoDetectFlag	("-noAutoDetect");
	const MString	compressFlag		("-compress");
	const MString	toleranceFlag		("-tolerance");

	// Parse the arguments.
	for ( size_t i = 0; i < args.length(); i++ ) {
		arg = args.asString( i, &stat );
		if (!stat)              
			continue;
				
		if ( arg == fileFlag || arg == fileFlagLong || arg == fileFlagLongOld ) {
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
		//else if ( arg == subanimFlag  ) {
		//	bDoSubAnim = true;
		//}
		else if ( arg == geomFlag  ) {
			bDoGeom = true;
		}
		else if ( arg == staticFlag  ) {
			// Static flag has the potential for optimizations,
			// for now it just turns off the auto detection of deforming animations
			bDoGeom = true;
			bStatic = true;
			bAutoDetect = false;
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
			bDoAnim = true;
			//bDoSubAnim = true;
		}
		else if ( arg == expressionAnimFlag  ) {
			bDoPose = false;
			bDoDeltas = true;
			bDoAnim = true;
			//bDoSubAnim = true;
		}
		else if ((arg == nobakedFlag ) || (arg == noBakedFlag )) {
			bUseMayaAnimCurves = true;
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
		else if (arg == noAutoDetectFlag ) {
			bAutoDetect = false;
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

MStatus WriteModel::doIt(const MArgList &args)
{
	try
	{
		return safeDoIt(args);
	}
	catch (envExceptionX &i_Ex)
	{
		std::string msg = "sgpuWriteModel Error : " + i_Ex.GetErrorMessage();
		MayaUtil::PrintError(msg.c_str());
		return MS::kFailure;
	}
	catch(...)
	{
		MayaUtil::PrintError("sgpuWriteModel : General exception, command could not complete.");
		return MS::kFailure;
	}
}

MStatus WriteModel::safeDoIt(const MArgList &args)
{
	cout << "-----------------------------------------" << endl;

	MStatus status;	
	MDagPath dagPath;			
	MObject component;

	// Parse command line
	status = ParseArgs (args);
	if (!status)
		return status;

	// In "-confirm" mode, don't display errors
	MayaUtil::SetQuietMode(!bConfirmFlags);
	MayaUtil::SetSuggestMode(bSuggest);

	// Setting auto detect flagt according to command line argument
	CharacterFuncs::SetAutoDetectDeformation(bAutoDetect);

	// If no command line arguments, go to geom writing mode
	if (!bDoAnim && !bDoGeom)
	{
		cout << "No command line arguments, defaulting to geom" << endl;
		bDoGeom = true;
	}

	// First of all, get list of selected transforms. If no transforms
	//	are selected, then abort.
	//
	MSelectionList activeList;
	status = MGlobal::getActiveSelectionList(activeList);
	//MItSelectionList mesh_iter( activeList, MFn::kMesh );
	
	MItSelectionList xform_iter( activeList, MFn::kTransform );
	if (xform_iter.isDone())
	{
		MayaUtil::PrintError("sgpuWriteModel : No transform nodes selected.");
		return MS::kFailure;
	}

	// Try to see if the selection list contains nodes that are underneath other nodes
	MDagPathArray sel_paths;
	for (int i=0; i<activeList.length(); i++)
	{
		activeList.getDagPath(i, dagPath);
		sel_paths.append( dagPath );
	}
	if (check_multiple_selection(sel_paths))
	{
		MayaUtil::PrintError("sgpuWriteModel : Problem with selection list, some components of selection are nested under other components.");
		return MS::kFailure;
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
		cout << "sgpuWriteModel: No file path specified." << endl;
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

		// Stores usages of instances within the multiple subgraphs.
		HierarchyUtil::InstanceMap instance_map;

		// List of nodes to export
		std::vector< shared_ptr<mdlNodeInfo> > root_nodes;

		// First, make a pass over the transforms in order to gather the joints.
		// This allows us to use an index for the joint influences and also to
		// check to make sure influences are only coming from joints that we will be
		// exporting.
		std::vector<MString> joint_names;
		MItSelectionList xform_iter_for_joints( activeList, MFn::kTransform );
		for ( ; !xform_iter_for_joints.isDone(); xform_iter_for_joints.next() )
		{			
			status = xform_iter_for_joints.getDagPath( dagPath, component );
			if ( status && dagPath.hasFn(MFn::kTransform)) 
			{
				MFnTransform transform(dagPath, &status);	
				if (status)
				{
					HierarchyUtil::GatherJoints(transform, joint_names);
				}
			}
		}

		// Iterate through transforms
		bool bWarnedExclMatrix = false;
		int mesh_count = 0;
		for ( ; !xform_iter.isDone(); xform_iter.next() ) 
		{
			xform_iter.getDagPath( dagPath, component );

			//cout << "Selection Dag Node: " << dagNode.name() << ", type = " << dagNode.typeName() << endl;

			if (dagPath.hasFn(MFn::kTransform)) 
			{
				MFnTransform transform(dagPath, &status);

				//cout << "Selection name = " << transform.name() << endl;
				//cout << "Selection path = " << dagPath.fullPathName() << endl;

				// Transformation above this node (check for identity?)
				MMatrix exclMatrix = dagPath.exclusiveMatrix(); 
				if (!exclMatrix.isEquivalent(MMatrix::identity))
				{
					if (!bWarnedExclMatrix)
					{
						MayaUtil::DisplayError( MString("Transformation above selected node will not be exported, ") + transform.name() );
						bWarnedExclMatrix = true;
					}
					else
					{
						cout << "Transformation above selected node will not be exported, " << transform.name() << endl;
					}
				}

				shared_ptr<mdlNodeInfo> node_graph;
				std::deque< std::string > rel_path;
				rel_path.push_back( MayaUtil::PrepareName(transform.name()).asUTF8() );
				HierarchyUtil::GatherNodeInfo(transform, rel_path, exclMatrix, node_graph, material_table, joint_names, instance_map, efficiencyMsg);
				if (node_graph)
				{
					root_nodes.push_back( node_graph );
					mesh_count += count_surfaces( node_graph );
				}
			}
		}

		if (root_nodes.empty())
		{
			MayaUtil::PrintError("sgpuWriteModel : No transform nodes with joints or visible geometry selected.");
			return MS::kFailure;
		}
		if (mesh_count == 0)
		{
			MayaUtil::PrintError("sgpuWriteModel : No visible geometry selected for export.");
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
			prefix_instance_info(gxb_root, "gxb_root");

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

		// Get default slider range, allow it to be changed by the arguments
		double minTime, maxTime;
		BakeCommand::GetTimelineRange(minTime, maxTime);
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

		// If using compression streams, have to bake all animation data
		// or else anim curves will end up in the geometry cache.
		if (bUseCompressStream)
			bUseMayaAnimCurves = false;

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
		if (!bDoPose)
		{
			AnimFuncs::WriteBeginFrame(writer, begin_frame);
		}

		// This used to be written only for subanims, now only
		// for animations with additive deltas.
		if (bDoDeltas)
		{
			// write out whether deltas are being written
			writer.WriteChunkHeader(c_DELT, 0, false);
			writer.Write(bDoDeltas);
			writer.FinishChunk();
		}

		//MGlobal::doErrorLogEntry("Starting gather");

		// If we are using compression streams, create the geometry cache to hold
		// the reference frames and compressed delta blocks.
		shared_ptr<vtxGeometryCacheWriter> geom_cache;
		if (bUseCompressStream)
			geom_cache.reset(new vtxGeometryCacheWriter(writer, file));

		// Data structures for baked key data
		std::list<HierarchyUtil::BakedKeyData> baked_key_data;

		// Iterate through transforms in selection list
		bool bNeedTimelineBake = (!bUseMayaAnimCurves);
		int vertex_anim_count = 0, stream_anim_count = 0;
		envType::UInt64 total_est_size = 0, est_size = 0;
		const int num_keys = int(maxTime - minTime +1);
		for ( ; !xform_iter.isDone(); xform_iter.next() ) 
		{
			xform_iter.getDagPath( dagPath, component );
			if (dagPath.hasFn(MFn::kTransform)) 
			{
				MFnTransform transform(dagPath, &status);
				
				// Gather animation information from hierarchy
				baked_key_data.push_back(HierarchyUtil::BakedKeyData());
				HierarchyUtil::GatherAnimHierarchy(transform, baked_key_data.back(), bUseMayaAnimCurves,
					bDoPose, bDoDeltas, bUseCompressStream, m_Tolerance, 
					keys, begin_frame, end_frame, writer, file, geom_cache.get());

				bNeedTimelineBake |= (HierarchyUtil::NeedsTimelineSimulation(baked_key_data.back()));

				// Count number of vertex streams
				stream_anim_count += baked_key_data.back().mesh_streams.size();

				// Estimate amount of vertex animation data that will be exported
				std::list<VertexKeys::DelayedMeshAnim> &delayed_meshes = baked_key_data.back().delayed_meshes;
				if (!delayed_meshes.empty())
				{
					vertex_anim_count += delayed_meshes.size();
					std::list<VertexKeys::DelayedMeshAnim>::iterator it = delayed_meshes.begin();
					for (; it != delayed_meshes.end(); ++it) 
					{
						if (it->m_DagPath.hasFn(MFn::kMesh)) 
						{
							MFnMesh mesh(it->m_DagPath, &status);
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
				}
			}
		}

		if (total_est_size > c_MaxVertexCacheSize)
		{
			cout << "Estimated vertex cache size: " << total_est_size/c_Megabyte << "MB max is " << c_MaxVertexCacheSize/c_Megabyte << "MB" << endl;
			MayaUtil::PrintError("WriteVerts : Baked vertex animation would exceed allowed maximum size of vertex cache.");
			return MS::kFailure;
		}

		bool bCancelled = false;
		std::list<HierarchyUtil::BakedKeyData>::iterator baked_key_it;
		for ( baked_key_it = baked_key_data.begin(); baked_key_it != baked_key_data.end(); ++baked_key_it ) 
		{
			if (bCancelled) break;

			std::list<VertexKeys::DelayedMeshAnim> &delayed_meshes = baked_key_it->delayed_meshes;
			if (!delayed_meshes.empty())
			{
				int num_meshes = delayed_meshes.size();
				if (c_bDoProgressWindow)
				{
					MString beginProgress("progressWindow -title \"Preparing Vertex Cache\" -isInterruptable true -min 0 ");
					beginProgress += MString(" -max ") + num_meshes;
					MGlobal::executeCommand( beginProgress );
					bHaveProgressWindow = true;
				}

				std::list<VertexKeys::DelayedMeshAnim>::iterator it = delayed_meshes.begin();
				for (int mi=0; mi<num_meshes; ++mi, ++it) 
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

					// Prepare space within the GAB for the vertex animation data
					VertexKeys::PrepareVertexAnimation(*it, keys,writer, file);
					vertex_anim_count++;
				}
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
				if (bUseCompressStream && (stream_anim_count > 0))
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

					if (!bDoPose)
					{
						// Move the Maya timeline to the current frame, except
						// when using pose animations, in which case we export the
						// current time's values.

						//MTime time(*it);
						MTime time(frame, MTime::uiUnit());
						MayaUtil::SetCurrentTime(time);
					}

					// Gather animation data
					std::list<HierarchyUtil::BakedKeyData>::iterator baked_it;
					for (baked_it = baked_key_data.begin(); baked_it != baked_key_data.end(); ++baked_it )
					{			
						HierarchyUtil::BakeFrameData(frame, *baked_it, writer, file, begin_frame);
					}
				}

				// Close the geometry cache now that the streams are over.
				if (bUseCompressStream && (stream_anim_count > 0))
				{
					// Finish the open streams
					std::list<HierarchyUtil::BakedKeyData>::iterator baked_it;
					for (baked_it = baked_key_data.begin(); baked_it != baked_key_data.end(); ++baked_it )
					{			
						VertexKeys::FinishStreams(baked_it->mesh_streams);
					}
					geom_cache->CloseCache();

					// Write out the frame data that references into the geometry cache
					for (baked_it = baked_key_data.begin(); baked_it != baked_key_data.end(); ++baked_it )
					{			
						VertexKeys::WriteAnimationStreams(writer, baked_it->mesh_streams);
					}
				}

				// Moved down lower
				//MGlobal::executeCommand("progressWindow -endProgress");

				// Restore the original timeline time, helps restore the
				// visibility states back so that they match when we traverse
				// the hierarchy again.
				MayaUtil::SetCurrentTime(currentTime);

				//MGlobal::doErrorLogEntry("Done seeking.");
				cout << "Done seeking." << endl;
			}
			
			// Move the file cursor to the end of the file to continue writing
			file.SetFilePos(0, fsFileStream::e_End);

			// Write the baked transformation information that we gathered the keys for earlier
			MItSelectionList xform_iter_writing( activeList, MFn::kTransform );
			std::list<HierarchyUtil::BakedKeyData>::const_iterator baked_it = baked_key_data.begin();
			for (; !xform_iter_writing.isDone(); xform_iter_writing.next() )
			{			
				xform_iter_writing.getDagPath( dagPath, component );
				if (dagPath.hasFn(MFn::kTransform)) 
				{
					MFnTransform transform(dagPath, &status);
					
					HierarchyUtil::WriteBakedAnimation(transform, writer, *baked_it, begin_frame, bDoDeltas);
					++baked_it;
				}
			}
			cout << " " << vertex_anim_count+stream_anim_count << " baked vertex animations" << endl;

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

//
//MStatus WriteModel::doSubAnim()
//{   
//	MayaUtil::SetWaitCursor();
//
//	MStatus status;
//	
//	//ParseSkinClusters();
//
//	// Just do selected joint
//	//
//	MSelectionList activeList;
//	status = MGlobal::getActiveSelectionList(activeList);
//	MItSelectionList iter( activeList, MFn::kJoint );
//
//	if (iter.isDone())
//	{
//		MGlobal::displayWarning("Writing subanim: No joint selected.");
//		return MS::kFailure;
//	}
//
//	// Get filename into which to save subanim
//	//
//	cout << "Asking for filename." << endl;
//	MString path;
//	if (filePath.length() > 0)
//	{
//		cout << "Have filename from command line " << endl;
//		path = filePath;
//	}
//	else
//	{
//		cout << "Asking for filename." << endl;
//		//MString cmd = "getFilenameDialog -ext \"*.cha\";";
//		MString cmd = "fileDialog -m 1 -dm \"*.gab\";";
//		MCommandResult result;
//		MGlobal::executeCommand(cmd, result);
//		status = result.getResult(path);
//	}
//
//	if ((status != MS::kSuccess) || (path.length() == 0))
//	{
//		cout << "writeModel: No file path specified." << endl;
//		return MS::kFailure;
//	}
//
//	cout << "-----------------------------------------" << endl;
//	
//	// Output joint anim
//	//
//	int jointCount = 0;
//	MString jointName;
//	for ( ; !iter.isDone(); iter.next() )
//	{								
//		MDagPath dagPath;			
//		MObject component;		
//		iter.getDagPath( dagPath, component );
//								
//		if (dagPath.hasFn(MFn::kJoint)) 
//		{
//			cout << "Joint!" << endl;
//			MFnIkJoint joint(dagPath, &status);
//			if ( !status ) {
//				status.perror("MFnIkJoint constructor");
//				continue;
//			}
//
//			cout << "name = " << joint.name() << endl;
//			cout << "path = " << dagPath.fullPathName() << endl;
//			
//			// check for visibility (including layers)
//			// only write joint if it is visible
//			// since we're pruning, we don't have to worry 
//			// about children's visibility
//			//
//
//			if (MayaUtil::DagNodeVisible(joint)) 
//			{
//				//only write first skeleton encountered
//				if (jointCount == 0) 
//				{
//					// must be true, but just in case
//					if (bDoSubAnim)
//					{
//						MString fname = MayaUtil::ConfirmExtension(path, ".cha");
//
//						cout << "Writing character sub animation: " << path << endl;
//
//						fsLocator locator;
//						fsFileUtil::ANSIFilenameToLocator(fname.asUTF8(), locator);
//
//						if( fsFileUtil::FileExists(locator) )
//							fsFileUtil::DeleteFile(locator);
//
//						fsFileUtil::CreateFile(locator);
//
//						gfFileBin file(locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
//						file.WriteHeader();
//
//						chBinWriter writer(file);
//						writer.WriteChunkHeader(c_ACHR, 0, true);	// Character Animation
//
//						cout << "Writing joint sub animation: " << path << endl;
//						JointFuncs::WriteJointSubAnim(joint, writer, bDoPose, bDoDeltas);
//
//						MItSelectionList subdiv_iter( activeList, MFn::kSubdiv );
//						for ( ; !subdiv_iter.isDone(); subdiv_iter.next() )
//						{							
//							cout << "Iterating through subdivs in selection." << endl;
//							subdiv_iter.getDagPath( dagPath, component );
//													
//							MFnSubd subdiv(dagPath, &status);
//							if (!status) continue;
//						
//							CharacterFuncs::WriteBlendShapeAnimation(subdiv, writer, bDoPose);
//						}
//						// Look for vertex animation on meshes
//						MItSelectionList mesh_iter( activeList, MFn::kMesh );
//						for ( ; !mesh_iter.isDone(); mesh_iter.next() )
//						{							
//							cout << "Iterating through meshes in selection." << endl;
//							mesh_iter.getDagPath( dagPath, component );
//													
//							MFnMesh mesh(dagPath, &status);
//							if (!status) continue;
//						
//							// blend shape
//							CharacterFuncs::WriteBlendShapeAnimation(mesh, writer, bDoPose);
//						}
//
//						writer.FinishChunk(); // c_ACHR
//
//						// Stamp version into end of file
//						SceneFuncs::WriteExporterVersionStamp(writer);
//						cout << "Finished file." << endl;
//
//					}
//
//					jointName = joint.name();
//				}
//				jointCount++;
//			}
//
//		}
//
//	}
//
//	if (jointCount == 0) 
//	{
//		cerr << "No joints found.  Created empty file." << endl;
//		MayaUtil::PrintError("No joints found.  Created empty file.");
//	}
//	else if (jointCount > 1) 
//	{
//		cerr << "More than one joint visible.  Only wrote " << jointName << endl;
//		MString err = MString("More than one joint visible.  Only wrote ") + jointName + ".";
//		MayaUtil::PrintWarning(err);
//	}
//	else 
//	{
//		MayaUtil::PrintStatus("Done.");
//	}
//
//	MayaUtil::UnsetWaitCursor();
//
//	return MS::kSuccess;
//}
