/*****************************************************************************
**  SurfaceUtil.cpp
**
**   Namespace for gathering Maya scene graph into LibXLT scene graph info.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "MayaUtil.hpp"
#include "SurfaceUtil.hpp"

#include "CharacterFuncs.hpp"
#include "MaterialUtil.hpp"
#include "MayaFlagUtil.hpp"
#include "SceneFuncs.hpp"
#include "JointFuncs.hpp"

#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MFnMesh.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MIntArray.h>
#include <maya/MPointArray.h>
#include <maya/MMatrix.h>

#include "Core/Ma/maAxisBox.hpp"
#include "Core/Ma/maConstants.hpp"
#include "Graphics/mdl/mdlNodeInfo.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"
#include "Graphics/mdl/mdlSkinUtil.hpp"
#include "Graphics/mdl/mdlSubdivInfo.hpp"
#include "Graphics/smdl/smdlCharacterSkin.hpp"

#include <set>

namespace
{

	bool	bWriteMeshDetails = false;
	bool	bWriteMorphDetails  = false;

	//------------------------------------------------------------------------
	// group of polygon indices per material
	//------------------------------------------------------------------------
	struct MaterialPolyGroup
	{
		std::string m_MaterialName;
		std::vector<int> m_PolyIndices;
	};

	//------------------------------------------------------------------------
	// predicate for if this material group should be removed from
	// material list. Returns true if either list is empty.
	//------------------------------------------------------------------------
	bool bad_poly_group(const MaterialPolyGroup& i_Group)
	{
		return (i_Group.m_MaterialName.empty() || i_Group.m_PolyIndices.empty());
	}

	//------------------------------------------------------------------------
	// Group polygons in mesh by material
	//------------------------------------------------------------------------
	void process_poly_materials(MFnMesh &i_Mesh,
							    std::vector<MaterialPolyGroup> &o_PolyGroups, 
								mdlMatInfoTable& io_MaterialTable)
	{
		MObjectArray shaders;
		MIntArray indices;
		i_Mesh.getConnectedShaders(0, shaders, indices);  //hmmm, how to handle instances? (using 0 for now)

		// Get material names for the shaders
		int num_shaders = shaders.length();
		o_PolyGroups.resize(num_shaders);
		for (int s=0; s<num_shaders; s++)
		{
			//o_PolyGroups[s].m_MaterialName = MaterialUtil::GetMaterialName(shaders[s]);
			o_PolyGroups[s].m_MaterialName = MaterialUtil::ProcessMaterial(shaders[s], io_MaterialTable);
		}

		//Note: We could make one pass through the polygon lists in order to get the size
		// of the arrays and then another pass to fill in the arrays.

		// Sort polygon indices per material
		int num_polys = i_Mesh.numPolygons();
		for (int i=0; i<num_polys; i++)
		{
			// Some material indices returned from Maya may be -1 if no material was assigned
			// to that polygon
			if (indices[i] >= 0)
			{
				// Add this polygon index to this material's list
				o_PolyGroups[indices[i]].m_PolyIndices.push_back( i );
			}
		}

		// Look for error cases where there were polygons with a material 
		// we couldn't process.
		for (int i=0; i<o_PolyGroups.size(); i++)
		{
			if ( o_PolyGroups[i].m_MaterialName.empty() &&
				(!o_PolyGroups[i].m_PolyIndices.empty()) )
			{
				MayaUtil::PrintError("Unrecognized or missing material on mesh " + i_Mesh.name() + " some polygons will not export correctly.");
			}
		}

		// Now check for error cases: if material name is empty or the polygon list is empty,
		// remove this material group
		o_PolyGroups.erase(std::remove_if(o_PolyGroups.begin(), o_PolyGroups.end(), bad_poly_group),
						   o_PolyGroups.end());
	}

	//------------------------------------------------------------------------
	// Make sure all normals are normalized and non-zero
	//------------------------------------------------------------------------
	void confirm_normals(std::vector<maVector3d> &io_Normals)
	{
		std::vector<maVector3d>::iterator it;
		for (it = io_Normals.begin(); it != io_Normals.end(); ++it)
		{
			if (!it->Normalize())
				it->Set(0,1,0); // Give zero-length normals a new unit vector value
		}
	}

	//------------------------------------------------------------------------
	// Make sure that there are useful UVs, if any are given
	//------------------------------------------------------------------------
	void check_uvs(const MString &i_MeshName,
				   const std::vector<maVector2d> &i_UVs,
				   MString &o_Message)
	{	
		// It is okay to not have any UVs
		if (i_UVs.empty())
			return;

		// use the maAxisBox union call to accumulate a min and max.
		maAxisBox uvBounds;
		for (int i = 0; i < i_UVs.size(); ++i)
		{
			uvBounds.Union(maPoint3d(i_UVs[i].m_X, i_UVs[i].m_Y, 0.5f));
		}

		// if the uv bounds are smaller than epsilon, then they are degenerate, and we should treat
		// the mesh as if it has no UVs.
		if ((fabs(uvBounds.GetDiffX()) < maConstants::c_fEpsilon) || 
			(fabs(uvBounds.GetDiffY()) < maConstants::c_fEpsilon))
		{
			MString msg = MString("Mesh ") + i_MeshName + MString(" has degenerate texture coordinates.");
			o_Message += (msg + "\\n");
		}
	}

	//--------------------------------------------------------------------
	// Covnert from Maya vector3d to our vector3d
	//--------------------------------------------------------------------
	template<class T>
	maVector3d conv_vec3(T &i_Vec)
	{
		return maVector3d((float)i_Vec[0], (float)i_Vec[1], (float)i_Vec[2]);
	}

	//--------------------------------------------------------------------
	// Represents information about a vertex with common indices.
	// This vertex will be shared in the file format if the 
	// indices are all the same.
	//--------------------------------------------------------------------
	struct VertexInfo
	{
		VertexInfo() :	m_NormalIndex(-1),
						m_UVIndex(-1),
						m_VertexIndex(-1){}

		VertexInfo(	int i_VertexIndex,
					int i_NormalIndex,
					int i_UVIndex,
					int i_NewIndex) :	m_VertexIndex(i_VertexIndex),
										m_NormalIndex(i_NormalIndex),
										m_UVIndex(i_UVIndex),
										m_NewIndex(i_NewIndex) {}

		bool operator == (const VertexInfo& i_Vertex) const {	return	(m_NormalIndex == i_Vertex.m_NormalIndex) &&
																		(m_UVIndex == i_Vertex.m_UVIndex) &&
																		(m_VertexIndex == i_Vertex.m_VertexIndex); }

		bool operator < (const VertexInfo& i_Vertex) const
		{
			if( m_VertexIndex == i_Vertex.m_VertexIndex )
			{
				if( m_NormalIndex == i_Vertex.m_NormalIndex )
					return m_UVIndex < i_Vertex.m_UVIndex;
				else
					return m_NormalIndex < i_Vertex.m_NormalIndex;
			}
			else
				return m_VertexIndex < i_Vertex.m_VertexIndex;
		}

		int m_NormalIndex;
		int m_UVIndex;
		int m_VertexIndex;
		int m_NewIndex;
	};

	//========================================================================
	//========================================================================
	void get_subdiv_flags(MFnMesh &mesh,
						mdlSubdivInfo& o_SubdivInfo)
	{
		// Flags
		o_SubdivInfo.m_Flags.m_bDoubleSided = MayaFlagUtil::GetDoubleSidedFlag(mesh);
		o_SubdivInfo.m_Flags.m_bTriangleSort = MayaFlagUtil::GetTriangleSortFlag(mesh);
		o_SubdivInfo.m_Flags.m_bCastsShadow = MayaFlagUtil::GetCastsShadowsFlag(mesh);
		o_SubdivInfo.m_Flags.m_bReceivesShadow = MayaFlagUtil::GetReceiveShadowsFlag(mesh);
		o_SubdivInfo.m_Flags.m_bShadowHull = MayaFlagUtil::GetShadowHullFlag(mesh);
		int low_res = MayaFlagUtil::GetResolutionLevel(mesh);
		o_SubdivInfo.m_Flags.m_bAutoGenLowRes = (low_res == 0); // if no flag exists, then auto-gen low res.
	}

	//========================================================================
	// Gather information about mesh into a subdiv info structure.
	//========================================================================
	bool gather_subdiv_info(MFnMesh &mesh, 
						mdlSubdivInfo& o_SubdivInfo, 
						mdlMatInfoTable& io_MaterialTable,
						MString &o_Message,
						bool i_bFillInVertexRemap,
						bool i_bReversePolygonOrder,
						bool i_bWorldSpace = false)
	{
		if (mesh.numPolygons() == 0)
		{
			if (bWriteMeshDetails)
				cout << "No polygons in mesh " << mesh.name() << endl;
			return false;
		}

		// Get mesh name
		o_SubdivInfo.m_Name = MayaUtil::PrepareName(mesh.name()).asUTF8();

		// Gather polygons per material
		std::vector<MaterialPolyGroup> poly_groups;
		process_poly_materials(mesh, poly_groups, io_MaterialTable);
		if (poly_groups.empty())
		{
			cout << "No polygons with materials in subdiv " << mesh.name() << endl;
			return false;
		}

		if (bWriteMeshDetails)
		{
			cout << "nVerts: " << mesh.numVertices() << endl;
			cout << "nPolys: " << mesh.numPolygons() << endl;
			cout << "nUVs: " << mesh.numUVs() << endl;
			cout << "nNormals: " << mesh.numNormals() << endl;
		}

		bool has_texture_coords = (mesh.numUVs() > 0);

		// Maya has lists of positions and normals and uvs that have different indexing.
		// We need to convert this into an indexing so that each position-normal-uv
		// combination is a unique vertex and that we then have only a single indices list.

		MSpace::Space use_space = i_bWorldSpace ? MSpace::kWorld : MSpace::kObject;
		MPointArray positions;
		mesh.getPoints(positions, use_space);
		//MFloatVectorArray normals;
		//mesh.getNormals(normals, use_space);
		MFloatArray uArray, vArray;
		if (has_texture_coords)
			mesh.getUVs(uArray, vArray, NULL);

		// Get number of vertices before sorting
		o_SubdivInfo.m_NumOrigVertices = positions.length();

		//	Each unique conbination of geometry vertex index, normal index, and texture vertex index
		//	will turn into one combined vertex.
		std::set<VertexInfo> vertex_set;

		//	do this for each material
		int num_Materials = poly_groups.size();
		if (num_Materials > 1)
		{
			//MayaUtil::DisplayError( MString("Subdivision surfaces can only have one material, ") + mesh.name() );
			cerr << "MachStudioPro 1.0.x versions require subdivision surfaces to have one material, " << mesh.name() << endl;
		}

		o_SubdivInfo.m_NumFaces = 0;

		for (int mat_ind = 0 ; mat_ind < num_Materials ; mat_ind++ )
		{
			if ( mat_ind != 0 )
				o_SubdivInfo.m_MaterialChanges.push_back(o_SubdivInfo.m_NumFaces);

			MaterialPolyGroup &poly_group = poly_groups[mat_ind];
			const int num_indices = poly_group.m_PolyIndices.size();

			// Sum up the total number of faces of all materials
			o_SubdivInfo.m_NumFaces += num_indices;

			// Add material pointer for this polygon group
			o_SubdivInfo.m_Materials.push_back( io_MaterialTable[poly_group.m_MaterialName] );

			// Here we are iterating through polygon indices, 
			// subdivision indices are grouped with first the number of vertices in polygon
			// and then list of the vertex indices in the polygon.
			for (int pi=0; pi<num_indices; pi++)
			{
				int poly_ind = poly_group.m_PolyIndices[pi];
				MIntArray ginds;
				mesh.getPolygonVertices(poly_ind, ginds);

				std::vector<int> new_indices;
				int ti = -1;
				int num_poly_verts = ginds.length();
				o_SubdivInfo.m_Indices.push_back( num_poly_verts );
				for (int vii=0; vii<num_poly_verts; vii++)
				{
					int vi = (i_bReversePolygonOrder) ? (num_poly_verts-vii-1) : vii;

					if (has_texture_coords)
						mesh.getPolygonUVid(poly_ind, vi, ti);

					VertexInfo new_vertex(	ginds[vi],
											-1,	// no normals
											ti,
											o_SubdivInfo.m_Vertices.size());

					std::pair< std::set<VertexInfo>::iterator, bool> insert_result = vertex_set.insert(new_vertex);
					if ( insert_result.second ) // actually inserted
					{
						if ( i_bFillInVertexRemap )
						{
							o_SubdivInfo.m_VertexRemap.insert(std::pair<int, int>(ginds[vi], o_SubdivInfo.m_Vertices.size()));
						}

						// add new vertex
						o_SubdivInfo.m_Indices.push_back(new_vertex.m_NewIndex);
						o_SubdivInfo.m_Vertices.push_back(conv_vec3(positions[new_vertex.m_VertexIndex]));

						// Notice that the V coordinate needs to be flipped when exporting for DirectX
						if (has_texture_coords)
						{
							if (new_vertex.m_UVIndex < 0)
								o_SubdivInfo.m_UVs.push_back(maVector2d(0,0));
							else
								o_SubdivInfo.m_UVs.push_back(maVector2d(uArray[new_vertex.m_UVIndex],
																  1 - vArray[new_vertex.m_UVIndex]));
						}
					}
					else
					{
						//	reuse old vertex
						o_SubdivInfo.m_Indices.push_back( insert_result.first->m_NewIndex );
					}
				}
			}
		}

		// get fragment flags for subdiv
		get_subdiv_flags(mesh, o_SubdivInfo);

		// Gather messages about efficiency
		SceneFuncs::GatherWarningMessages(mesh, o_Message);
		return true;
	}

	//========================================================================
	// Change right/left handedness of mesh by reversing the order
	// of the polygon indices.
	//========================================================================
	//void reverse_polygon_order(std::vector<envType::UInt32> &io_Indices)
	//{
	//	for( size_t ui =0; ui < io_Indices.size(); ui += 3 )
	//	{
	//		swap( io_Indices[ui+1], io_Indices[ui+2] );
	//	}
	//}

	//========================================================================
	//========================================================================
	void get_mesh_flags(MFnMesh &mesh,
						mdlFragInfo& o_MeshInfo)
	{
		// Flags
		o_MeshInfo.m_Flags.m_bDoubleSided = MayaFlagUtil::GetDoubleSidedFlag(mesh);
		o_MeshInfo.m_Flags.m_bTriangleSort = MayaFlagUtil::GetTriangleSortFlag(mesh);
		o_MeshInfo.m_Flags.m_bCastsShadow = MayaFlagUtil::GetCastsShadowsFlag(mesh);
		o_MeshInfo.m_Flags.m_bReceivesShadow = MayaFlagUtil::GetReceiveShadowsFlag(mesh);
		o_MeshInfo.m_Flags.m_bShadowHull = MayaFlagUtil::GetShadowHullFlag(mesh);
	
		// Flags only for meshes
		o_MeshInfo.m_ResolutionLevel = MayaFlagUtil::GetResolutionLevel(mesh);
		//o_MeshInfo.m_Flags.m_bVertexAnimation = MayaFlagUtil::GetClothFlag(mesh);
		o_MeshInfo.m_Flags.m_bVertexAnimation = CharacterFuncs::IsDeformingGeometry(mesh);
	}

	//========================================================================
	// Gather information about mesh into a mesh info structure.
	//========================================================================
	bool gather_mesh_info(MFnMesh &mesh, 
						mdlFragInfo& o_MeshInfo, 
						mdlMatInfoTable& io_MaterialTable,
						MString &o_Message,
						bool i_bFillInVertexRemap,
						bool i_bReversePolygonOrder,
						bool i_bWorldSpace = false)
	{
		if (mesh.numPolygons() == 0)
		{
			if (bWriteMeshDetails)
				cout << "No polygons with materials in mesh " << mesh.name() << endl;
			return false;
		}

		// Get mesh name
		o_MeshInfo.m_Name = MayaUtil::PrepareName(mesh.name()).asUTF8();

		// Gather polygons per material
		std::vector<MaterialPolyGroup> poly_groups;
		process_poly_materials(mesh, poly_groups, io_MaterialTable);
		if (poly_groups.empty())
		{
			cout << "No polygons with materials in mesh " << mesh.name() << endl;
			return false;
		}

		if (bWriteMeshDetails)
		{
			cout << "nVerts: " << mesh.numVertices() << endl;
			cout << "nPolys: " << mesh.numPolygons() << endl;
			cout << "nUVs: " << mesh.numUVs() << endl;
			cout << "nNormals: " << mesh.numNormals() << endl;
		}

		if (mesh.numNormals() == 0)
		{
			cout << "No normals in mesh " << mesh.name() << endl;
			return false;
		}
		bool has_texture_coords = (mesh.numUVs() > 0);

		// Maya has lists of positions and normals and uvs that have different indexing.
		// We need to convert this into an indexing so that each position-normal-uv
		// combination is a unique vertex and that we then have only a single indices list.
		MSpace::Space use_space = i_bWorldSpace ? MSpace::kWorld : MSpace::kObject;
		MPointArray positions;
		mesh.getPoints(positions, use_space);
		MFloatVectorArray normals;
		mesh.getNormals(normals, use_space);
		MFloatArray uArray, vArray;
		if (has_texture_coords)
			mesh.getUVs(uArray, vArray, NULL);

		// Get number of vertices before sorting
		o_MeshInfo.m_NumOrigVertices = positions.length();
		o_MeshInfo.m_NumOrigNormals = normals.length();

		// Get fragment flags for mesh (includes low res and cloth)
		get_mesh_flags(mesh, o_MeshInfo);

		// Now check the VertexAnimation flag, because we need the vertex remap
		// if we are going to do vertex animation
		bool bFillInVertexRemap = (i_bFillInVertexRemap || o_MeshInfo.m_Flags.m_bVertexAnimation);

		//	Each unique conbination of geometry vertex index, normal index, and texture vertex index
		//	will turn into one combined vertex.
		std::set<VertexInfo> vertex_set;

		//	do this for each material
		int num_Materials = poly_groups.size();
		for (int mat_ind = 0 ; mat_ind < num_Materials ; mat_ind++ )
		{
			if ( mat_ind != 0 )
				o_MeshInfo.m_MaterialChanges.push_back(o_MeshInfo.m_Indices.size() / 3);

			MaterialPolyGroup &poly_group = poly_groups[mat_ind];
			const int num_indices = poly_group.m_PolyIndices.size();

			// Add material pointer for this polygon group
			o_MeshInfo.m_Materials.push_back( io_MaterialTable[poly_group.m_MaterialName] );

			// Here we are iterating through polygon indices, each polygon may need
			// to be represented by multiple triangles.
			for (int pi=0; pi<num_indices; pi++)
			{
				int poly_ind = poly_group.m_PolyIndices[pi];
				MIntArray ginds, ninds;
				mesh.getPolygonVertices(poly_ind, ginds);
				mesh.getFaceNormalIds(poly_ind, ninds);

				std::vector<int> new_indices;
				int ti = -1;
				int num_poly_verts = ginds.length();
				for (int vi=0; vi<num_poly_verts; vi++)
				{
					if (has_texture_coords)
						mesh.getPolygonUVid(poly_ind, vi, ti);

					VertexInfo new_vertex(	ginds[vi],
											ninds[vi],
											ti,
											o_MeshInfo.m_Vertices.size());

					std::pair< std::set<VertexInfo>::iterator, bool> insert_result = vertex_set.insert(new_vertex);
					if ( insert_result.second ) // actually inserted
					{
						if ( bFillInVertexRemap )
						{
							o_MeshInfo.m_VertexRemap.insert(std::pair<int, int>(ginds[vi], o_MeshInfo.m_Vertices.size()));
							o_MeshInfo.m_NormalRemap.insert(std::pair<int, int>(ninds[vi], o_MeshInfo.m_Vertices.size()));
						}

						// add new vertex
						new_indices.push_back(new_vertex.m_NewIndex);
						o_MeshInfo.m_Vertices.push_back(conv_vec3(positions[new_vertex.m_VertexIndex]));
						o_MeshInfo.m_Normals.push_back(conv_vec3(normals[new_vertex.m_NormalIndex]));

						// Notice that the V coordinate needs to be flipped when exporting for DirectX
						if (has_texture_coords)
						{
							if (new_vertex.m_UVIndex < 0)
								o_MeshInfo.m_UVs.push_back(maVector2d(0,0));
							else
								o_MeshInfo.m_UVs.push_back(maVector2d(uArray[new_vertex.m_UVIndex],
																  1 - vArray[new_vertex.m_UVIndex]));
						}
					}
					else
					{
						//	reuse old vertex
						new_indices.push_back( insert_result.first->m_NewIndex );
					}
				}

				// Convert polygon into triangles
				for (int v = 2; v < num_poly_verts; v++) 
				{
					o_MeshInfo.m_Indices.push_back( new_indices[0] );
					if (i_bReversePolygonOrder)
					{
						o_MeshInfo.m_Indices.push_back( new_indices[v] );
						o_MeshInfo.m_Indices.push_back( new_indices[v-1] );
					}
					else
					{
						o_MeshInfo.m_Indices.push_back( new_indices[v-1] );
						o_MeshInfo.m_Indices.push_back( new_indices[v] );
					}
				}		
			}
		}	


		// Check the normals and make sure all are normalized
		confirm_normals(o_MeshInfo.m_Normals);

		// Check UVs to make sure that they are useful
		check_uvs(mesh.name(), o_MeshInfo.m_UVs, o_Message);

		// Create basis vectors now to write into file and save time on import
		mdlFragUtil::CreateBasisVectors(o_MeshInfo);

		if ( bWriteMeshDetails )
		{
			cout << "Total expanded vertices: " << o_MeshInfo.m_Vertices.size() << endl;;
			cout << "Total expanded indices: " << o_MeshInfo.m_Indices.size() << " (" << o_MeshInfo.m_Indices.size() / 3 << " tris)" << endl;
			cout << "Avg vertices per tri: " << (3.0f * float(o_MeshInfo.m_Vertices.size()) / float(o_MeshInfo.m_Indices.size())) << endl;
		}

		// Gather messages about efficiency
		SceneFuncs::GatherWarningMessages(mesh, o_Message);
		return true;
	}

	//========================================================================
	// Gather information about skinning for the given mesh.
	//========================================================================
	bool gather_skin_info(MFnMesh &mesh, 
						 int i_NumVertices,
						 const std::multimap<int, int> &i_VertexRemap,
						 smdlCharacterSkin& o_SkinInfo, 
						 const std::vector<MString>& i_Joints,
						 JointFuncs::ClusterTable& i_ClusterTable,
						 const maMatrix4x4& i_BindPose,
						 MString &o_Message)
	{	
		std::vector<mdlSkinUtil::JointInfluence> joint_influences;

		// Go through the joints that we will export and see which ones influence this mesh
		const int num_joints = i_Joints.size();
		const int num_clusters = i_ClusterTable.clusters.size();
		int joints_matched = 0;
		for (int ji=0; ji<num_joints; ji++)
		{
			for (int ci=0; ci<num_clusters; ci++)
			{
				JointFuncs::SkinCluster *cluster = i_ClusterTable.clusters[ci].get();
				if (cluster) 
				{
					//int gi = cluster->FindGroupByName(joint.fullPathName());
					int gi = cluster->FindGroupByName(i_Joints[ji]);
					if (gi >= 0) 
					{
						JointFuncs::InfluenceGroup group = cluster->groups[gi];
						const int num_infls = group.influences.size();
						if (num_infls > 0)
						{
							joints_matched++;
							if (bWriteMeshDetails)
								cout << "Have " << num_infls << " influences in group " << gi << " from joint " << i_Joints[ji] << endl;
						}
						for (int i = 0; i < num_infls; i++) 
						{
							mdlSkinUtil::JointInfluence infl;
							infl.m_nJointIndex = ji;
							infl.m_nVertexIndex = group.influences[i].index;
							infl.m_fWeight = group.influences[i].weight;
							joint_influences.push_back( infl );
						}
					}
				}
			}
		}

		if (joints_matched == 1)
		{
			MString msg = MString("Mesh ") + mesh.name() + MString(" is influenced by a single joint.");
			cout << msg << " This would be more efficient if parented." << endl;
			o_Message += (msg + "\\n");
		}

		if (!joint_influences.empty())
		{
			mdlSkinUtil::BuildBoneVertices(	joint_influences, 
											i_NumVertices, 
											i_VertexRemap, 
											o_SkinInfo.m_BoneVertices );

			// Set the bind pose into the smdlCharacterSkin
			if (!i_BindPose.IsIdentity())
			{
				MString msg = MString("Skinned mesh ") + mesh.name() + MString(" has not been frozen.");
				cout << msg << " This would be more efficient if there was no transformation above the skinned mesh." << endl;
				o_Message += (msg + "\\n");
			}
			o_SkinInfo.m_BindPose = i_BindPose;

			return true;
		}

		return false;
	}

	//------------------------------------------------------------------------
	// build_morph_target - remap positions or deltas from a morph target
	// into a sorted vertex list that matches the fragment info.
	//------------------------------------------------------------------------
	void build_morph_target(const MPointArray &i_PositionVecs,
							const MIntArray& i_SparseIndices,
							int i_NumVertices,
							const std::multimap<int, int> &i_VertexRemap,
							smdlMorphTarget& o_MorphTarget)
	{
		o_MorphTarget.m_Offsets.resize( i_NumVertices );

		const MPointArray* pVertices = &i_PositionVecs;
		MPointArray unsparse;
		if (i_SparseIndices.length() > 0)
		{
			unsparse.setLength(i_NumVertices);
			for (int i=0; i<i_NumVertices; ++i)
			{
				unsparse.set(i, 0,0,0,0);
			}
			for (int i=0; i<i_SparseIndices.length(); ++i)
			{
				if (i_SparseIndices[i] < i_NumVertices)
					unsparse[i_SparseIndices[i]] = i_PositionVecs[i];
				else if (bWriteMorphDetails)
					cout << "Error in morph sparse inds remapping, " << i_SparseIndices[i] << "<?" << i_NumVertices << endl;
			}
			pVertices = &unsparse;
		}

		std::multimap<int, int>::const_iterator it;
		for( it = i_VertexRemap.begin(); it != i_VertexRemap.end(); ++it )
		{
			int nOldVertexIndex = it->first;
			int nNewVertexIndex = it->second;

			if ((nNewVertexIndex < i_NumVertices) && (nOldVertexIndex < pVertices->length()))
				o_MorphTarget.m_Offsets[ nNewVertexIndex ] = conv_vec3((*pVertices)[ nOldVertexIndex ]);
			else
			{
				cout << "Error in morph target remapping, " << nNewVertexIndex << "<?" << i_NumVertices;
				cout << " or " << nOldVertexIndex << "<?" << pVertices->length() << endl;
			}
		}	
	}


} // end of namespace

//========================================================================
// Gather information about mesh into a node info structure.
//========================================================================
bool SurfaceUtil::GatherMeshInfo(MFnMesh &mesh, 
					mdlNodeInfo& o_NodeForShape, 
					mdlMatInfoTable& io_MaterialTable,
					const std::vector<MString>& i_Joints,
					const maMatrix4x4& i_BindPose,
					bool i_bReversePolygonOrder,
					MString &o_Message)
{
	// If flag is set, write this polygon mesh as a subdiv surface
	bool bWriteSubdiv = MayaFlagUtil::GetExportAsSubdivFlag(mesh);

	// Check for any skinning information on this mesh
	bool bHaveSkinning = false;
	JointFuncs::ClusterTable cluster_table;
	if (!i_Joints.empty())
	{
		JointFuncs::ParseSkinClusters(mesh, cluster_table);
		bHaveSkinning = (!cluster_table.clusters.empty());
	}

	// Look for blend shapes
	bool bHaveMorphTargets = false; //bga - turning off blend shape support
	//std::vector< shared_ptr<CharacterFuncs::MorphTargetInfo> > morph_targets;
	//CharacterFuncs::GetMorphTargets(mesh, morph_targets, o_Message);
	//bool bHaveMorphTargets = (!morph_targets.empty());

	// Gather information about the mesh itself
	bool bNeedVertexRemapping = (bHaveSkinning || bHaveMorphTargets);

	bool bHaveSurface = false;
	bool bHavePotentialVertexAnim = false;
	if (bWriteSubdiv)
	{
		shared_ptr<mdlSubdivInfo> subdiv_info(new mdlSubdivInfo());
		bNeedVertexRemapping  = true; // always need remapping for subdivs?
		if (gather_subdiv_info(mesh, *subdiv_info, io_MaterialTable, o_Message, bNeedVertexRemapping, i_bReversePolygonOrder))
		{
			o_NodeForShape.m_SubdivInfo = subdiv_info;
			bHaveSurface = true;
			bHavePotentialVertexAnim = true; // always true?
		}
	}
	else
	{
		shared_ptr<mdlFragInfo> mesh_info(new mdlFragInfo());
		if (gather_mesh_info(mesh, *mesh_info, io_MaterialTable, o_Message, bNeedVertexRemapping, i_bReversePolygonOrder))
		{
			o_NodeForShape.m_MeshInfo = mesh_info;
			bHaveSurface = true;
			bHavePotentialVertexAnim = mesh_info->m_Flags.m_bVertexAnimation;
		}
	}

	if (bHaveSurface)
	{
		// Gather skinning info and remap morph targets,
		// using mesh information that was just parsed
		if (bHaveSkinning || bHaveMorphTargets || bHavePotentialVertexAnim)
		{
			// Get remapping information so that it works for both mesh and subdivs
			int num_sorted_vertices = (bWriteSubdiv) ? 
				o_NodeForShape.m_SubdivInfo->m_Vertices.size() : 
				o_NodeForShape.m_MeshInfo->m_Vertices.size();
			const std::multimap<int, int> vertex_remap = (bWriteSubdiv) ? 
				o_NodeForShape.m_SubdivInfo->m_VertexRemap : 
				o_NodeForShape.m_MeshInfo->m_VertexRemap;

			// Gather skinning info from joints and skin clusters
			shared_ptr<smdlCharacterSkin> skin_info(new smdlCharacterSkin());	
			if (bHaveSkinning)
			{
				if (gather_skin_info(mesh, num_sorted_vertices, vertex_remap, 
									  *skin_info, i_Joints, cluster_table, 
									  i_BindPose, o_Message))
				{
					o_NodeForShape.m_SkinInfo = skin_info;
				}
			}

			// SkinInfo also contains morph targets which can exist without
			// joint skinning.
			//if (bHaveMorphTargets)
			//{
			//	const int num_morphs = morph_targets.size();
			//	for (int i=0; i<num_morphs; i++)
			//	{
			//		CharacterFuncs::MorphTargetInfo &morph_info = (*morph_targets[i]);
			//		shared_ptr<smdlMorphTarget> remapped_morph(new smdlMorphTarget);
			//		remapped_morph->m_Name = morph_info.m_BlendShapeName.asUTF8();
			//		remapped_morph->m_Alias = morph_info.m_AliasName.asUTF8();
			//		remapped_morph->m_bOffsetsAreDeltas = morph_info.m_bVecsAreDeltas;
			//		build_morph_target(	morph_info.m_PositionVecs,
			//							morph_info.m_SparseIndices,
			//							num_sorted_vertices, 
			//							vertex_remap,
			//							*remapped_morph );

			//		skin_info->m_MorphTargets.push_back( remapped_morph );
			//	}
			//	o_NodeForShape.m_SkinInfo = skin_info;
			//}

			// If the cloth flag is set, but there is no skinning information
			// or blend targets, then we still put an empty (but non-NULL) skin_info
			// into the node. This causes the mesh to be placed in a SKIN chunk
			// and warns the importer to be ready for vertex animation.
			if (bHavePotentialVertexAnim)
			{
				o_NodeForShape.m_SkinInfo = skin_info;
			}
		}

		return true;
	}

	return false;
}

//========================================================================
// Gather information about mesh into a node info structure.
//	This variation will write the mesh into world space and will
//	not look for skinning or blend shape deformers.
//========================================================================
bool SurfaceUtil::GatherWorldSpaceMeshInfo(MFnMesh &mesh, 
					const std::string &i_SurfaceName,
					bool i_bPrepareForVertexAnim,
					bool i_bReversePolygonOrder,
					mdlNodeInfo& o_NodeForShape, 
					mdlMatInfoTable& io_MaterialTable,
					MString &o_Message)
{
	// If flag is set, write this polygon mesh as a subdiv surface
	bool bWriteSubdiv = MayaFlagUtil::GetExportAsSubdivFlag(mesh);

	// Gather information about the mesh itself
	bool bHaveSurface = false;
	bool bHavePotentialVertexAnim = i_bPrepareForVertexAnim;
	bool bNeedVertexRemapping = i_bPrepareForVertexAnim;
	bool bWorldSpace = true;
	if (bWriteSubdiv)
	{
		shared_ptr<mdlSubdivInfo> subdiv_info(new mdlSubdivInfo());
		bNeedVertexRemapping = true; // subdivs always need remapping in order to handle seams
		if (gather_subdiv_info(mesh, *subdiv_info, io_MaterialTable, o_Message, bNeedVertexRemapping, i_bReversePolygonOrder, bWorldSpace))
		{
			subdiv_info->m_Name = i_SurfaceName;
			o_NodeForShape.m_SubdivInfo = subdiv_info;
			bHaveSurface = true;
			bHavePotentialVertexAnim = true; // static subdivs have to be exported with skin also for now
		}
	}
	else
	{
		shared_ptr<mdlFragInfo> mesh_info(new mdlFragInfo());
		if (gather_mesh_info(mesh, *mesh_info, io_MaterialTable, o_Message, bNeedVertexRemapping, i_bReversePolygonOrder, bWorldSpace))
		{
			mesh_info->m_Name = i_SurfaceName;

			// Set the "vertex anim" flag to true if preparing for vertex animation.
			// This is not needed for the subdivision surface case above.
			mesh_info->m_Flags.m_bVertexAnimation = true;

			o_NodeForShape.m_MeshInfo = mesh_info;
			bHaveSurface = true;
		}
	}

	if (bHaveSurface)
	{
		if (bHavePotentialVertexAnim) 
		{
			// If the cloth flag is set, but there is no skinning information
			// or blend targets, then we still put an empty (but non-NULL) skin_info
			// into the node. This causes the mesh to be placed in a SKIN chunk
			// and warns the importer to be ready for vertex animation.
			shared_ptr<smdlCharacterSkin> skin_info(new smdlCharacterSkin());	
			o_NodeForShape.m_SkinInfo = skin_info;
		}
		return true;
	}

	return false;
}

