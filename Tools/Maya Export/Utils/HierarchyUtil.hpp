/*****************************************************************************
**  HierarchyUtil.hpp
**
**   Namespace for gathering Maya scene graph into LibXLT scene graph info.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef HIERARCHYUTIL_HPP
#error HierarchyUtil.hpp multiply included
#endif
#define HIERARCHYUTIL_HPP

#ifndef MDL_MATINFOTABLE_HPP
#include "Graphics/mdl/mdlMatInfoTable.hpp"
#endif 

#ifndef SCENE_KEYS_HPP
#include "SceneKeys.hpp"
#endif
#ifndef VERTEX_KEYS_HPP
#include "VertexKeys.hpp"
#endif
#ifndef BLEND_SHAPE_KEYS_HPP
#include "BlendShapeKeys.hpp"
#endif

#include <vector>
#include <deque>

class MFnIkJoint;
class MFnTransform;
class MMatrix;
class mdlNodeInfo;

namespace HierarchyUtil
{
	//========================================================================
	// Forward declaration for a structure to hold all baked keys.
	//========================================================================
	struct BakedKeyData
	{
		bool bUseMayaAnimCurves;
		bool bDoPose;
		std::list<SceneKeys::PathKeys> path_keys;
		std::list<VertexKeys::DelayedMeshAnim> delayed_meshes;
		//std::list<VertexKeys::DelayedSubdAnim> delayed_subdivs;
		//std::list<BlendShapeKeys::BlendKeys> blend_keys;
		//std::list<SceneKeys::SurfaceVisKeys> surface_keys;
		std::list<VertexKeys::DelayedMeshStream> mesh_streams;
	};

	//========================================================================
	//========================================================================
	typedef std::vector< std::pair<MObject, std::deque< std::string > > > InstanceMap;

	//========================================================================
	// Gather information about scene graph rooted at this node.
	// TotalMatrix should represent the exclusive matrix above this transform.
	// MaterialTable will be used to look for shared materials and 
	// will be filled in with any new materials.
	// Joint names should be passed in in order to be able to assign an
	// index to a joint when writing influences.
	//========================================================================
	void GatherNodeInfo(MFnTransform &transform, 
						const std::deque< std::string > &i_RelativePath,
						const MMatrix &i_TotalMatrix,
						shared_ptr<mdlNodeInfo>& o_NodeGraph, 
						mdlMatInfoTable& io_MaterialTable,
						const std::vector<MString>& i_Joints,
						InstanceMap& io_InstanceMap,
						MString &o_Message);

	//========================================================================
	// Traverse the children of this transform and append names of
	//	the found joints into the given return vector.
	//	This wil be used when exporting influences later.
	//========================================================================
	void GatherJoints(MFnTransform &transform, 
							 std::vector<MString>& o_JointNames);

	//========================================================================
	// Gather data needed for baking hierarchy.
	//========================================================================
	void GatherAnimHierarchy(MFnTransform &transform, 
							 BakedKeyData& o_BakedKeyData,
							 bool i_bUseMayaAnimCurves,
							 bool i_bDoPose,
							 bool i_bDoDeltas,
							 bool i_bUseCompressStream,
							 float i_Tolerance,	// negative value for tolerance is lossless compression
							 const std::list<float> &i_Keys,
							 double i_MinTime, 
							 double i_MaxTime, 
							 chWriter &o_Writer,
							 gfFileBin& io_File,
							 vtxGeometryCacheWriter *i_pGeometryCache = NULL);

	//========================================================================
	// Returns true if there is key data that needs to be gathered
	//	by advancing the Maya timline.
	//========================================================================
	bool NeedsTimelineSimulation(const BakedKeyData& i_BakedKeyData);

	//========================================================================
	// Gather baked data for given frame
	//========================================================================
	void BakeFrameData(float i_Frame, 
					   BakedKeyData& io_BakedKeyData,
					   chWriter &o_Writer,
					   gfFileBin& io_File,
					   float i_BeginFrame);

	//========================================================================
	// Write all of the gathered animation data to file.
	//========================================================================
	void WriteBakedAnimation(MFnTransform &transform, 
							 chWriter &o_Writer,
							 const BakedKeyData& i_BakedKeyData,
							 float i_BeginFrame,
							 bool i_bDoDeltas);

}
