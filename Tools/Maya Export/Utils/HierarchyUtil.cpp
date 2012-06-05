/*****************************************************************************
**  HierarchyUtil.cpp
**
**   Namespace for gathering Maya scene graph into LibXLT scene graph info.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "MayaUtil.hpp"
#include "HierarchyUtil.hpp"

#include "AnimFuncs.hpp"
#include "CharacterFuncs.hpp"
#include "JointFuncs.hpp"
#include "MayaFlagUtil.hpp"
#include "SceneIterator.hpp"
#include "SurfaceUtil.hpp"

#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MQuaternion.h>
#include <maya/MFnIkJoint.h>
#include <maya/MFnTransform.h>
#include <maya/MFnMesh.h>
#include <maya/MMatrix.h>

#include "Core/Ch/chWriter.hpp"
#include "Graphics/mdl/mdlNodeInfo.hpp"
#include "Graphics/vtx/vtxCompressionUtil.hpp"


namespace HierarchyUtil
{
	namespace
	{
		bool	bWriteDetails = false;

		//=============================================================================
		//	Chunk types
		//=============================================================================
		const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');
		const chDefs::Name c_ANLV = chDefs::MakeName('A', 'N', 'L', 'V');


		//=============================================================================
		//	convert matrix from Maya style to our style
		//=============================================================================
		void convert_matrix(const MMatrix& i_InMtx, maMatrix4x4 &o_OutMatx)
		{
			for (int row=0; row < 4; row++)
			{
				for (int col=0; col < 4; col++)
				{
					o_OutMatx(row, col) = float(i_InMtx(row,col));
				}
			}
		}

		//=============================================================================
		// Awkward workaround for not being able to have a std::map with MObject,
		// I just want to find the path for an object.
		//=============================================================================
		struct instance_find
		{
			instance_find(MObject i_Object) : m_Object(i_Object) {}
			bool operator()(const std::pair<MObject, std::deque< std::string> > &i_Pair) const
			{
				return (i_Pair.first == m_Object);
			}
			MObject m_Object;
		};

		
		//=============================================================================
		// Is animating visibility
		//=============================================================================
		bool IsVisibleAnim(MFnTransform &transform)
		{
			MStatus status;
			bool bWriteVisible = MayaFlagUtil::GetVisibleAnimFlag(transform);
			// The flag "sgpuVisibleAnim" overrides the auto-detection 
			// for visibility based on animation channel
			if (!MayaFlagUtil::EngineFlagExists(transform, "sgpuVisibleAnim"))
			{
				MFnAnimCurve visCurve = AnimFuncs::GetAnimCurve("visibility", transform, status);
				if (status == MS::kSuccess)
				{
					bWriteVisible = true;
					if (bWriteDetails)
						cout << "Found animation channel on visibility, transform = " << transform.name() << endl;
				}
			}
			return bWriteVisible;
		}

		//=============================================================================
		// Convert path into a string with a '|' character between each node name
		//=============================================================================
		//std::string get_path_as_string(const MDagPath &i_RelativePath)
		//{
		//	std::string path;
		//	MDagPath path_copy = i_RelativePath;
		//	while (path_copy.length()> 0)
		//	{
		//		MFnDependencyNode node(path_copy.node());
		//		std::string node_name = MayaUtil::PrepareName(node.name()).asUTF8();

		//		// Prepend to path
		//		if (path.empty())
		//			path = node_name;
		//		else
		//			path = node_name + "|" + path;

		//		path_copy.pop();
		//	}
		//	return path;
		//}


		//========================================================================
		// Return true if this node should be exported. This algorithm
		// has to be consistent for geometry, animation and joint influences.
		// If a node is forced visible (including a child node), then 
		// o_bForced is set true.
		// Once a joint has been exported, export the whole joint chain
		// by setting i_ExportingJoints to true.
		//========================================================================
		bool should_use_node(MObject &obj, 
							 bool &o_bForced,
							 bool i_ExportingJoints)
		{
			// We are only interested in hierarchies that have joints or
			// meshes as children.
			//return ( MayaUtil::HasTypeAsChild(obj, MFn::kJoint) ||
			//		 MayaUtil::HasTypeAsChild(obj, MFn::kMesh) );

			// Only visible, non-intermediate meshes
			if (obj.hasFn(MFn::kMesh))
			{
				MFnMesh mesh(obj);

				// Skip intermediate objects
				if (mesh.isIntermediateObject())
					return false;

				// Skip meshes with no polygons (used as trackers or locators)
				if (mesh.numPolygons() == 0)
					return false;

				// Export only visible meshes
				if (MayaUtil::DagNodeVisible(mesh))
					return true;
			}
			else if (obj.hasFn(MFn::kTransform))
			{
				MFnTransform transform(obj);

				// Special flag to prune the exportable tree
				bool bSkipExport = MayaFlagUtil::GetNoExportFlag(transform);
				if (bSkipExport)
					return false;

				// Special flag to guarantee the node is exported
				bool bForceExport = MayaFlagUtil::GetForceExportFlag(transform);
				if (bForceExport)
				{
					o_bForced = true;
					return true;
				}

				// Joints are also transforms
				if (obj.hasFn(MFn::kJoint))
				{
					// export all joints with skin clusters
					MFnIkJoint joint(obj);
					if (i_ExportingJoints ||
						JointFuncs::HasSkinCluster(joint))
					{
						// Force out all joints that influence meshes,
						// even if invisible or a parent is invisible
						o_bForced = true;
						return true;
					}

					// Otherwise, look through children like other transforms...
				}
				
				// Transforms have to be visible or be exporting
				// their visibility animation (in which case, they could be animated invisible
				// but still need to be exported)
				//
				//bool bWriteVisible = MayaFlagUtil::GetVisibleAnimFlag(transform);
				bool bWriteVisible = IsVisibleAnim(transform);
				bool bTransformVisible = (bWriteVisible || MayaUtil::DagNodeVisible(transform));

				// Export transforms only if children are exportable
				int numChildren = transform.childCount();
				for (int i = 0; i < numChildren; i++) 
				{
					// Decide if the child node should be traversed
					bool bForced  = false;
					bool bExportingJoints = false; // false while recursing
					if (should_use_node(transform.child(i), bForced, bExportingJoints))
					{
						// Look for forced exporting before deciding if 
						// invisible nodes should be exported
						if (bForced)
						{
							o_bForced = true;
							return true;
						}
						else if (bTransformVisible)
							return true;
					}
				}
			}

			return false;
		}

		//============================================================================
		// Derived class for our style of traversing
		//============================================================================
		class HierarchyIterator : public SceneIterator
		{
		protected:
			//========================================================================
			// Virtual function for deciding whether to visit a node.
			//========================================================================
			virtual bool ShouldVisit(MObject &i_Object, MObject &i_ParentObject)
			{
				// Use the common function "should_use_node" in order to determine
				// which nodes to visit.
				bool bForced = false;
				bool bJoint = (i_ParentObject.hasFn(MFn::kJoint));
				return should_use_node(i_Object, bForced, bJoint);
			}
		};

		//============================================================================
		// Derived class for gathering names of joints
		//============================================================================
		class GatherJointsIterator : public HierarchyIterator
		{
		public:
			//========================================================================
			//========================================================================
			GatherJointsIterator(std::vector<MString>& o_JointNames)
				:	m_JointNames(o_JointNames)	{}

			//========================================================================
			// Virtual function for visiting a node in the hierarchy.
			//========================================================================
			virtual void Visit(MObject &i_Object)
			{
				MStatus status;
				if (i_Object.hasFn(MFn::kJoint))
				{
					MFnIkJoint joint(i_Object, &status);
					if (status == MS::kSuccess) 
					{
						// Storing full joint name
						//cout << "Found joint: " << joint.fullPathName() << endl;
						m_JointNames.push_back(joint.fullPathName());
					}
				}

				// Recurse on children
				HierarchyIterator::Visit(i_Object);
			}

		private:
			std::vector<MString>& m_JointNames;
		};

		//============================================================================
		// Derived class for gathering animation data
		//============================================================================
		class GatherAnimIterator : public HierarchyIterator
		{
		public:
			//========================================================================
			//========================================================================
			GatherAnimIterator(BakedKeyData& o_BakedKeyData,
							 bool i_bDoDeltas,
							 bool i_bUseCompressStream,
							 float i_Tolerance,
							 const std::list<float> &i_Keys,
							 double i_MinTime, 
							 double i_MaxTime, 
							 chWriter &o_Writer,
							 gfFileBin& io_File,
							 vtxGeometryCacheWriter *i_pGeometryCache)
				:	m_BakedKeyData(o_BakedKeyData),
				m_bDoDeltas(i_bDoDeltas),
				m_Keys(i_Keys),
				m_MinTime(i_MinTime),
				m_MaxTime(i_MaxTime),
				m_Writer(o_Writer),
				m_File(io_File),
				m_bUseCompressStream(i_bUseCompressStream),
				m_Tolerance(i_Tolerance),
				m_pGeometryCache(i_pGeometryCache)
			{}

			//========================================================================
			// Virtual function for visiting a node in the hierarchy.
			//========================================================================
			virtual void Visit(MObject &i_Object)
			{
				bool bNeedsFinishChunk = false;
				MStatus status;

				// We could be constructing the dagPath as we traverse in 
				// order to handle multiple paths to the same object. But,
				// we don't export those instancing anyway.
				// We need the dagPath in order to get a new MFn handle everyframe.
				// When we give control back to Maya for advancing the frame or displaying the
				// progress window, then the previous MFn handles may become invalid.
				MDagPath dagPath;
				MDagPath::getAPathTo(i_Object, dagPath); // if instanced meshes, this won't work

				if (i_Object.hasFn(MFn::kMesh))
				{
					MFnMesh mesh(i_Object, &status);
					if (status == MS::kSuccess) 
					{
						//bga - support for blend shapes turned off
						//if (m_BakedKeyData.bUseMayaAnimCurves)
						//	CharacterFuncs::WriteBlendShapeAnimation(mesh, m_Writer, 
						//						m_MinTime, m_MaxTime, m_BakedKeyData.bDoPose);
						//else
						//	BlendShapeKeys::GatherBlendShapes(mesh, m_BakedKeyData.blend_keys);
						
						// vertex animation
						//bool cloth = MayaFlagUtil::GetClothFlag(mesh);
						bool cloth = CharacterFuncs::IsDeformingGeometry(mesh);
						//cout << "write vertex animation?: cloth = " << cloth << endl;
						if (cloth)
						{
							// If flag is set, write this polygon mesh as a subdiv surface,
							// which means we don't need to export the normals for the 
							// vertex animation.
							int bWriteSubdiv = MayaFlagUtil::GetExportAsSubdivFlag(mesh);
							bool bWriteNormals = (bWriteSubdiv == 0);

							// Don't consider the bUseMayaAnimCurves flag here, we want
							// to gather the polygon mesh data ourselves no matter what.

							MString name = MayaUtil::PrepareName(mesh.name());

							// This allocates space in the exported file in order to fill
							// in the vertex buffers later.
							//VertexKeys::PrepareVertexAnimation(mesh, name.asUTF8(), dagPath,
							//	m_BakedKeyData.delayed_meshes, m_Keys, m_Writer, m_File, bWriteNormals);
							
							//bga - Instead of writing the vertex animation now, just 
							// gather some data for it and allow the caller to do the 
							// preparation within a Progress Dialog loop.
							if (m_bUseCompressStream)
							{
								VertexKeys::DelayedMeshStream delay_info;
								delay_info.m_DagPath = dagPath;
								delay_info.m_bWriteNormals = bWriteNormals;
								delay_info.m_NumVerts = mesh.numVertices();

								// Negative tolerance will produce a lossless compression stream
								if (m_Tolerance < 0)
									delay_info.m_CompressStream = 
										vtxCompressionUtil::CreateLosslessCompressStream(name.asUTF8(), *m_pGeometryCache);
								else
								{
									// Have to convert the world space tolerance into an object space tolerance
									MMatrix world_to_obj = dagPath.inclusiveMatrixInverse();
									//double det = world_to_obj.det3x3();
									//cout << " Inc inverse determinant: " << det << endl;
									//float local_tolerance = (det > 0) ? (float)(m_Tolerance / det) : m_Tolerance;

									MVector scale(1,1,1);
									scale *= world_to_obj;
									double local_scale = abs(scale.length() / 3); // original "scale" vector was 3 units in length
									float local_tolerance = (local_scale > 0) ? (float)(m_Tolerance * local_scale) : m_Tolerance;

									//cout << " local_scale: " << local_scale << " local_tolerance: " << local_tolerance << endl;
									delay_info.m_CompressStream = 
									vtxCompressionUtil::CreateToleranceCompressStream(local_tolerance, name.asUTF8(), *m_pGeometryCache);
								}

								m_BakedKeyData.mesh_streams.push_back( delay_info );
							}
							else
							{
								VertexKeys::DelayedMeshAnim delay_info;
								delay_info.m_DagPath = dagPath;
								delay_info.m_bWriteNormals = bWriteNormals;
								delay_info.m_NumVerts = mesh.numVertices();
								m_BakedKeyData.delayed_meshes.push_back( delay_info );
							}
						}
					}
				}
				else if (i_Object.hasFn(MFn::kTransform))
				{
					MFnTransform transform(i_Object, &status);
					if (status == MS::kSuccess) 
					{	
						if (m_BakedKeyData.bUseMayaAnimCurves)
						{
							m_Writer.WriteChunkHeader(c_ANLV, 1, true);

							// Node name
							m_Writer.WriteChunkHeader(c_NNAM, 0, false);
							m_Writer.Write(MayaUtil::PrepareName(transform.name()).asUTF8());
							m_Writer.FinishChunk();

							AnimFuncs::WriteAnimation(m_Writer, 
													  transform, 
													  m_MinTime,
													  m_MaxTime,
													  m_BakedKeyData.bDoPose,
													  m_bDoDeltas);

							bNeedsFinishChunk = true;
						}
						else
						{
							SceneKeys::PathKeys keys;
							//keys.m_Transform.setObject( i_Object );
							keys.m_DagPath = dagPath;

							// visibility animation
							//bool bWriteVisible = MayaFlagUtil::GetVisibleAnimFlag(transform);
							bool bWriteVisible = IsVisibleAnim(transform);
							//cout << "write visibility?: visibleAnim = " << bWriteVisible << endl;
							keys.m_TransformKeys.m_bWriteVisibility = bWriteVisible;

							m_BakedKeyData.path_keys.push_back(keys);
						}
					}
				}

				// Recurse on children
				HierarchyIterator::Visit(i_Object);

				if (bNeedsFinishChunk)
				{
					m_Writer.FinishChunk();	// c_ANLV
				}
			}

		private:
			BakedKeyData& m_BakedKeyData;
			bool m_bDoDeltas;
			const std::list<float> &m_Keys;
			double m_MinTime; 
			double m_MaxTime; 
			chWriter &m_Writer;
			gfFileBin& m_File;
			bool	m_bUseCompressStream;
			float   m_Tolerance;
			vtxGeometryCacheWriter *m_pGeometryCache;
		};

		//============================================================================
		// Derived class for writing animation data that was already baked
		//============================================================================
		class WriteIterator : public HierarchyIterator
		{
		public:
			//========================================================================
			//========================================================================
			WriteIterator(chWriter &o_Writer, const std::list<SceneKeys::PathKeys> &i_Keys,
						 float i_TimeOffset, bool i_bDoDeltas)
				:	m_Writer(o_Writer), m_Iterator(i_Keys.begin()), 
					m_TimeOffset(i_TimeOffset), m_bDoDeltas(i_bDoDeltas),
					m_NodeCount(0), m_VisibilityCount(0)
			{
			}

			//========================================================================
			// Virtual function for visiting a node in the hierarchy.
			//========================================================================
			virtual void Visit(MObject &i_Object)
			{
				MStatus status;
				MFnTransform transform(i_Object, &status);
				if (status)
				{
					m_Writer.WriteChunkHeader(c_ANLV, 1, true);

					// Node name
					m_Writer.WriteChunkHeader(c_NNAM, 0, false);
					m_Writer.Write(MayaUtil::PrepareName(transform.name()).asUTF8());
					m_Writer.FinishChunk();

					AnimKeys::WriteAnimation(m_Writer, m_Iterator->m_TransformKeys, m_TimeOffset, m_bDoDeltas);
					++m_NodeCount;
					if (m_Iterator->m_TransformKeys.m_bWriteVisibility)
						++m_VisibilityCount;

					++m_Iterator;
				} 
			
				// Recurse on children
				HierarchyIterator::Visit(i_Object);
				
				if (status)
				{
					m_Writer.FinishChunk();	// c_ANLV
				}
			}

		private:
			chWriter& m_Writer;
			std::list<SceneKeys::PathKeys>::const_iterator m_Iterator;
			float m_TimeOffset;
			bool m_bDoDeltas;
		public:
			int m_NodeCount;
			int m_VisibilityCount;
		};
	} // end of namespace


	//========================================================================
	// Gather information about scene graph rooted at this node.
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
						MString &o_Message)
	{
		//Note: Should we evaluate whether to export this node at all?
		// Has joints or meshes as child nodes? Has a "do not export" flag?

		// Check for whether this node is a joint
		bool bJoint = false;
		MStatus status;
		MFnIkJoint joint(transform.object(), &status);
		if (status)
			bJoint = true;

		// Create node for this transform
		o_NodeGraph.reset(new mdlNodeInfo(bJoint));

		// Get node name
		o_NodeGraph->m_NodeName = MayaUtil::PrepareName(transform.name()).asUTF8();
		//cout << "Node named: " << o_NodeGraph->m_NodeName << endl;
		 
		// Get transform matrix
		MMatrix matrix = transform.transformationMatrix();
		convert_matrix(matrix, o_NodeGraph->m_Transform);

		// Get pivot points
		MPoint scale_pvt = transform.scalePivot(MSpace::kTransform);
		o_NodeGraph->m_ScalePivot.Set((float)scale_pvt[0], (float)scale_pvt[1], (float)scale_pvt[2]);
		//cout << "scalePivot: " << scale_pvt << endl;
		
		//double scale[3];
		//transform.getScale(scale);
		//cout << "scale: " << scale[0] << "," << scale[1] << "," << scale[2] << endl;

		//double shear[3];
		//transform.getShear(shear);
		//cout << "shear: " << shear[0] << "," << shear[1] << "," << shear[2] << endl;

		MVector spt = transform.scalePivotTranslation(MSpace::kTransform);
		o_NodeGraph->m_ScalePivotTranslation.Set((float)spt[0], (float)spt[1], (float)spt[2]);
		//cout << "scalePivotTranslation: " << spt << endl;

		MPoint rot_pvt = transform.rotatePivot(MSpace::kTransform);
		o_NodeGraph->m_RotatePivot.Set((float)rot_pvt[0], (float)rot_pvt[1], (float)rot_pvt[2]);
		//cout << "rotatePivot: " << rot_pvt << endl;

		//MQuaternion rot_orient = transform.rotateOrientation(MSpace::kTransform);
		//cout << "rotateOrientation: " << rot_orient << endl;

		//double rotation[3];
		//MTransformationMatrix::RotationOrder rot_order;
		//transform.getRotation(rotation, rot_order);
		//cout << "rotation: " << rotation[0] << "," << rotation[1] << "," << rotation[2] << endl;
		//cout << "rotation order: " << rot_order << endl;

		MVector rpt = transform.rotatePivotTranslation(MSpace::kTransform);
		o_NodeGraph->m_RotatePivotTranslation.Set((float)rpt[0], (float)rpt[1], (float)rpt[2]);
		//cout << "rotatePivotTranslation: " << rpt << endl;

		// Combine to get new total matrix
		MMatrix total_matrix = matrix * i_TotalMatrix;

		if (bJoint)
		{
			// Get joint orientation
			double	threeDoubles[3];
			MTransformationMatrix::RotationOrder rOrder = MTransformationMatrix::kXYZ;
			joint.getOrientation(threeDoubles, rOrder);
			o_NodeGraph->m_JointOrientation.Set( (float) threeDoubles[0], 
												 (float) threeDoubles[1], 
												 (float) threeDoubles[2]);

			// Joint scale orientation
			joint.getScaleOrientation(threeDoubles, rOrder);
			o_NodeGraph->m_JointScaleOrientation.Set( (float) threeDoubles[0], 
												 (float) threeDoubles[1], 
												 (float) threeDoubles[2]);

			//cout << " Bind pose for " << o_NodeGraph->m_NodeName.c_str() << endl;
			//cout << total_matrix << endl;

			// Use current total transformation as bind pose (inverted)
			convert_matrix(total_matrix, o_NodeGraph->m_InverseBindPose);
			o_NodeGraph->m_InverseBindPose.Invert();
		}

		// Check to see if this transform node is an instance of something 
		// we have written already
		bool bWriteChildren = true;
		if (transform.isInstanced())
		{
			//cout << "Transform: " << transform.name() << " is instanced." << endl;
			InstanceMap::iterator it = std::find_if(io_InstanceMap.begin(), io_InstanceMap.end(), instance_find(transform.object()));
			if (it == io_InstanceMap.end())
			{
				//std::string path_ref = get_path_as_string(i_RelativePath);
				std::deque< std::string > path_ref = i_RelativePath;
				//cout << "  first use of instance with path, " << path_ref << endl;
				io_InstanceMap.push_back( std::pair<MObject, std::deque< std::string > >(transform.object(), path_ref) );
			}
			else
			{
				// Add proxy using string reference to path to first instance.
				//cout << "  using path: " << it->second << endl;
				cout << "Reusing instanced transform: " << transform.name() << endl;
				o_NodeGraph->m_InstanceInfo.reset(new mdlNodeInfoProxy(it->second.begin(), it->second.end()));
				bWriteChildren = false;
			}
		}

		// If we used a proxy reference to a node instance, then don't write the
		// children (including the mesh info).
		if (bWriteChildren)
		{
			int numChildren = transform.childCount();
			std::vector<int> visible;
			for (int i = 0; i < numChildren; i++) 
			{
				// Decide if the child node should be traversed
				bool bForced = false;
				MObject obj = transform.child(i);
				// Ignoring bForced, in this case we only care about the return value.
				// If we are exporting a joint, then continue to export all joints in 
				// this joint chain.
				if (should_use_node(obj, bForced, bJoint))	
				{
					visible.push_back(i);
				}
				else if ((obj.apiType() == MFn::kTransform) ||
						 (obj.apiType() == MFn::kJoint))
				{
					// Print warning when skipping transform nodes, but 
					// not derived types like constraints or things that 
					// wouldn't have exported anyway.
					MFnTransform trans_child(obj, &status);
					if (status)
					{
						// This message is exported even if details are turned off:
						cout << "Pruning: Not exporting node " << trans_child.name() << endl;
					}
				}
			}

			if (bWriteDetails && numChildren > 0)
				cout << " number of children: " << numChildren << " num visible: " << visible.size() << endl;
			if (visible.size() > 0) 
			{
				//cout << "  ***  " << transform.name() << " has " << numChildren << endl;
				for (int i = 0; i < visible.size(); i++) 
				{
					MObject obj = transform.child(visible[i]);
					MFnDependencyNode node(obj);
						
					// Extend relative path to child, using special character as separator
					std::deque< std::string > pathToChild = i_RelativePath;
					pathToChild.push_back( MayaUtil::PrepareName(node.name()).asUTF8() );

					if (bWriteDetails)
					{
						cout << " child " << visible[i] << " (" << i << " of " << numChildren << ") is " << obj.apiTypeStr();
						cout << " named: " << node.name() << endl;
					}

					if (obj.hasFn(MFn::kTransform))
					{
						MFnTransform trans_child(obj, &status);
						if (status)
						{
							shared_ptr<mdlNodeInfo> child_info;
							GatherNodeInfo(trans_child, pathToChild, total_matrix, child_info, 
								io_MaterialTable, i_Joints, io_InstanceMap, o_Message);
							if (child_info)
								o_NodeGraph->m_Children.push_back( child_info );
						}
					}
					else if (obj.hasFn(MFn::kMesh))
					{			
						// Each node in out node graph can only have one mesh in it.
						// Since a Maya transform can have more than one mesh child,
						// we need to decide how to handle multiple meshes.
						// In this case, we always introduce a new node for each mesh.
						//
						MFnMesh mesh(obj, &status);
						if (status == MS::kSuccess) 
						{
							if (bWriteDetails)
								cout << "child " << i << " of " << numChildren << " is mesh." << endl;

							shared_ptr<mdlNodeInfo> mesh_node(new mdlNodeInfo());
							mesh_node->m_NodeName = MayaUtil::PrepareName(mesh.name()).asUTF8();
							bool bWriteMesh = true;

							// Check for reuse of an existing mesh
							if (mesh.isInstanced())
							{
								//cout << "Mesh: " << mesh.name() << " is instanced." << endl;
								InstanceMap::iterator it = std::find_if(io_InstanceMap.begin(), io_InstanceMap.end(), instance_find(mesh.object()));
								if (it == io_InstanceMap.end())
								{
									//std::string path_ref = get_path_as_string(i_RelativePath);
									std::deque< std::string > path_ref = i_RelativePath;
									path_ref.push_back( mesh_node->m_NodeName );
									//cout << "  first use of instance with path, " << path_ref << endl;
									io_InstanceMap.push_back( std::pair<MObject, std::deque< std::string > >(mesh.object(), path_ref) );
								}
								else
								{
									// Add proxy using string reference to path to first instance.
									//cout << "  using path: " << it->second << endl;
									cout << "Reusing instanced mesh: " << mesh.name() << endl;
									mesh_node->m_InstanceInfo.reset(new mdlNodeInfoProxy(it->second.begin(), it->second.end()));
									o_NodeGraph->m_Children.push_back( mesh_node );
									bWriteMesh = false;
								}
							}

							// If we used a proxy reference to a node instance, then don't write the
							// children (including the mesh info).
							if (bWriteMesh)
							{
								maMatrix4x4 bind_pose;
								convert_matrix(total_matrix, bind_pose);
								bool bNegativeScale = (total_matrix.det3x3() < 0); // reverse indices when negative scaled
								if (SurfaceUtil::GatherMeshInfo(mesh, *mesh_node, io_MaterialTable, 
																i_Joints, bind_pose, bNegativeScale, o_Message))
									o_NodeGraph->m_Children.push_back( mesh_node );
							}
						}
					}
				}
			}
		}
	}


	//========================================================================
	// Traverse the children of this transform and append names of
	//	the found joints into the given return vector.
	//	This wil be used when exporting influences later.
	//========================================================================
	void GatherJoints(MFnTransform &transform, 
					  std::vector<MString>& o_JointNames)
	{
		GatherJointsIterator joint_name_iter(o_JointNames);
		joint_name_iter.Traverse(transform);
	}

	//========================================================================
	// Gather data needed for baking hierarchy.
	//========================================================================
	void GatherAnimHierarchy(MFnTransform &transform, 
							 BakedKeyData& o_BakedKeyData,
							 bool i_bUseMayaAnimCurves,
							 bool i_bDoPose,
							 bool i_bDoDeltas,
							 bool i_bUseCompressStream,
							 float i_Tolerance,
							 const std::list<float> &i_Keys,
							 double i_MinTime, 
							 double i_MaxTime, 
							 chWriter &o_Writer,
							 gfFileBin& io_File,
							 vtxGeometryCacheWriter *i_pGeometryCache)
	{
		o_BakedKeyData.bUseMayaAnimCurves = i_bUseMayaAnimCurves;
		o_BakedKeyData.bDoPose = i_bDoPose;
		GatherAnimIterator gather_anim_iter(o_BakedKeyData, i_bDoDeltas, 
			i_bUseCompressStream, i_Tolerance,
			i_Keys, i_MinTime, i_MaxTime, o_Writer, io_File, i_pGeometryCache);
		gather_anim_iter.Traverse(transform);
	}

	//========================================================================
	// Returns true if there is key data that needs to be gathered
	//	by advancing the Maya timline.
	//========================================================================
	bool NeedsTimelineSimulation(const BakedKeyData& i_BakedKeyData)
	{
		return (!i_BakedKeyData.delayed_meshes.empty() || !i_BakedKeyData.mesh_streams.empty());
	}

	//========================================================================
	// Gather baked data for given frame
	//========================================================================
	void BakeFrameData(float i_Frame, 
					   BakedKeyData& io_BakedKeyData,
					   chWriter &o_Writer,
					   gfFileBin& io_File,
					   float i_BeginFrame)
	{
		MStatus status;

		// Gather position and visibility of transforms
		std::list<SceneKeys::PathKeys>::iterator it;
		for (it = io_BakedKeyData.path_keys.begin(); it != io_BakedKeyData.path_keys.end(); ++it)
		{
			MFnTransform transform(it->m_DagPath, &status);	
			if (status == MS::kSuccess) 
			{	
				AnimKeys::GatherKeys( it->m_TransformKeys, i_Frame, transform );	
			}
			else
			{
				cout << "Error getting transform node from MDagPath handle." << endl;
			}
		}

		// Gather blend shape weights
		//BlendShapeKeys::GatherKeys(io_BakedKeyData.blend_keys, i_Frame);

		// Gather the visibility animations 
		//SceneKeys::GatherKeys(io_BakedKeyData.surface_keys, i_Frame);

		const bool bWorldSpace = false;
		if (!VertexKeys::BakeAnimation(o_Writer, io_File, io_BakedKeyData.delayed_meshes, 
										i_Frame, i_BeginFrame, bWorldSpace))
		{
			MayaUtil::PrintError("Cannot get state of mesh. Stored mesh is invalid.");
			//return MS::kFailure;
		}
		if (!VertexKeys::BakeAnimationStreams(io_BakedKeyData.mesh_streams, 
										i_Frame, i_BeginFrame, bWorldSpace))
		{
			MayaUtil::PrintError("Cannot get state of mesh. Stored mesh is invalid.");
			//return MS::kFailure;
		}
	}

	//========================================================================
	// Write all of the gathered animation data to file.
	//========================================================================
	void WriteBakedAnimation(MFnTransform &transform, 
							 chWriter &o_Writer,
							 const BakedKeyData& i_BakedKeyData,
							 float i_BeginFrame,
							 bool i_bDoDeltas)
	{
		if (!i_BakedKeyData.bUseMayaAnimCurves) 
		{
			WriteIterator write_anim_iter(o_Writer, i_BakedKeyData.path_keys, i_BeginFrame, i_bDoDeltas);
			write_anim_iter.Traverse(transform);

			// Write the processed blend shape animations to file
			//BlendShapeKeys::WriteAnimation(o_Writer, i_BakedKeyData.blend_keys, i_BeginFrame);

			cout << "Exporting baked animation for ..." << endl;
			cout << " " << write_anim_iter.m_NodeCount << " transform nodes" << endl;
			cout << " " << write_anim_iter.m_VisibilityCount << " visibility animations" << endl;
			//cout << " " << i_BakedKeyData.blend_keys.size() << " blend shapes" << endl;


			// Write the processed surface visibility animations to file
			//SceneKeys::WriteAnimation(o_Writer, i_BakedKeyData.surface_keys, i_BeginFrame);
		}
	}

} // end of HierarchyUtil namespace
