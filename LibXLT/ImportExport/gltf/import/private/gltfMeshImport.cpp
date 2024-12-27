/****************************************************************************\
**  gltfMeshImport.cpp
**
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/gltf/import/private/gltfMeshImport.hpp"
#include "ImportExport/gltf/import/private/gltfMaterialImport.hpp"
#include "ImportExport/gltf/import/private/gltfLayerMapper.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mdl/private/mdlIndexUtil.hpp"
#include "Graphics/mdl/mdlFragCreate.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"
#include "Graphics/mdl/mdlSplitFragInfo.hpp"

#include <set>

#undef FindResource


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace gltfMeshImport
{

	namespace
	{
		//--------------------------------------------------------------------
		// Convert from KFBX vectors to our vector3d
		//--------------------------------------------------------------------
		template<class T>
		maVector3d conv_vec3(T &i_Vec)
		{
			return maVector3d((float)i_Vec[0], (float)i_Vec[1], (float)i_Vec[2]);
		}

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

		//--------------------------------------------------------------------
		// Get material mapping per polygon from FBX Mesh object
		//--------------------------------------------------------------------
		void process_poly_materials(tinygltf::Mesh& i_Mesh,
							        std::vector<MaterialPolyGroup> &o_PolyGroups)
		{
			// materials are in the containing node
			const int num_materials = i_Mesh.GetNode()->GetMaterialCount();
			o_PolyGroups.resize(num_materials);

			for (int m=0; m<num_materials; m++)
			{
				KFbxSurfaceMaterial* pMaterial = i_Mesh.GetNode()->GetMaterial(m);
				o_PolyGroups[m].m_MaterialName = pMaterial->GetName();
			}

			// Get material id per polygon mapping
			int lPolygonCount = i_Mesh.GetPolygonCount();
			for (int l = 0; l < i_Mesh.GetLayerCount(); l++)
			{
				KFbxLayerElementMaterial* pLayerMaterial = i_Mesh.GetLayer(l)->GetMaterials();
				if (pLayerMaterial) 
				{
					 if ( pLayerMaterial->GetMappingMode() == KFbxLayerElement::eBY_POLYGON ) 
					 {
						for (int i = 0; i < lPolygonCount; i++)
						{ 
							int mat_id =  pLayerMaterial->GetIndexArray().GetAt(i);
							if (mat_id >= 0 && mat_id < num_materials)
							{
								// Add this polygon index to this material's list
								o_PolyGroups[mat_id].m_PolyIndices.push_back( i );
							}
						}
					 }
					 else if ( pLayerMaterial->GetMappingMode() == KFbxLayerElement::eALL_SAME ) 
					 {
						if (num_materials == 1)
						{
							// Just fill in a simple list 
							for (int i = 0; i < lPolygonCount; i++)
							{ 
								o_PolyGroups[0].m_PolyIndices.push_back( i );
							}
						}
					 }
				}
			}

			// Now check for error cases: if material name is empty or the polygon list is empty,
			// remove this material group
			o_PolyGroups.erase(std::remove_if(o_PolyGroups.begin(), o_PolyGroups.end(), bad_poly_group),
							   o_PolyGroups.end());
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

		//--------------------------------------------------------------------
		// Get fragment information from FBX Mesh object
		//--------------------------------------------------------------------
		void get_frag_info(tinygltf::Mesh& i_Mesh, 
						   mdlFragInfo& o_MeshInfo,
						   mdlMatInfoTable& io_MaterialTable)
		{
			std::vector<MaterialPolyGroup> poly_groups;
			process_poly_materials(i_Mesh, poly_groups);

			int num_points = i_Mesh.GetControlPointsCount();
			KFbxVector4* pControlPoints = i_Mesh.GetControlPoints();

			// Get normals from the layer, have to consider different mappings
			fbxLayerMapper<KFbxVector4> normal_mapper;
			for (int j = 0; j < i_Mesh.GetLayerCount(); j++)
			{
				KFbxLayerElementNormal* pNormals = i_Mesh.GetLayer(j)->GetNormals();
				if (pNormals && normal_mapper.Attach(pNormals)) break;
			}

			// If we couldn't get normals, then try to create new ones
			if (!normal_mapper.IsAttached())
			{
				const bool bCWNormals = false;
				i_Mesh.ComputeVertexNormals(bCWNormals);

				// Try to attach again
				for (int j = 0; j < i_Mesh.GetLayerCount(); j++)
				{
					KFbxLayerElementNormal* pNormals = i_Mesh.GetLayer(j)->GetNormals();
					if (pNormals && normal_mapper.Attach(pNormals)) break;
				}
			}

			// Check to see if we should compress the normal indices
			bool bCompressedNormals = false;
			std::vector<maVector3d> compressed_normals;
			std::vector<envType::UInt32> normal_remap;
			if (normal_mapper.ShouldCompressIndices())
			{
				// Get values in Direct Array and then come up with our own indexing scheme
				// for the direct array based on duplicate values within an epsilon.
				int num_normals = normal_mapper.GetDirectArraySize();
				std::vector<maVector3d> org_normals(num_normals);
				for (int i=0; i<num_normals; ++i)
				{
					org_normals[i] = conv_vec3(normal_mapper.GetValue(i));
				}
				// Compress normals into shorter list based on shared values
				mdlIndexUtil::CompressValues<maVector3d>(org_normals, compressed_normals, normal_remap);
				bCompressedNormals = true;
			}

			// Get UVs
			fbxLayerMapper<KFbxVector2> uv_mapper;
			for (int j = 0; j < i_Mesh.GetLayerCount(); j++)
			{
				KFbxLayerElementUV* pUVs = i_Mesh.GetLayer(j)->GetUVs();
				if (pUVs && uv_mapper.Attach(pUVs)) break;
			}
			
			// Have texture coords?
			bool has_texture_coords = (uv_mapper.IsAttached());
				
			//	Each unique conbination of geometry vertex index, normal index, and texture vertex index
			//	will turn into one combined vertex.
			std::set<VertexInfo> vertex_set;

			//	Indices for each material
			int num_Materials = poly_groups.size();
			//DBG_LOG("Mesh " << i_Mesh.GetName() << " num poly groups: " << num_Materials); 
			for (int mat_ind = 0 ; mat_ind < num_Materials ; mat_ind++ )
			{
				if ( mat_ind != 0 )
					o_MeshInfo.m_MaterialChanges.push_back(o_MeshInfo.m_Indices.size() / 3);

				MaterialPolyGroup &poly_group = poly_groups[mat_ind];
				const int num_indices = poly_group.m_PolyIndices.size();

				// Add material pointer for this polygon group
				o_MeshInfo.m_Materials.push_back( io_MaterialTable[poly_group.m_MaterialName] );	
				//shared_ptr<mdlMatInfo> material_info(new mdlMatInfo());
				//fbxMaterialImport::CreateSimpleMaterial(material_info->m_Info);
				//material_info->m_Info.SetMaterialName( poly_group.m_MaterialName );
				//o_MeshInfo.m_Materials.push_back(material_info);

				// Here we are iterating through polygon indices, each polygon may need
				// to be represented by multiple triangles.
				for (int pi=0; pi<num_indices; pi++)
				{
					int poly_ind = poly_group.m_PolyIndices[pi];
					int num_poly_verts = i_Mesh.GetPolygonSize(poly_ind);

					std::vector<int> new_indices;
					int gind, nind, tind = -1;
					int	vind = i_Mesh.GetPolygonVertexIndex(poly_ind);
					for (int vi=0; vi<num_poly_verts; vi++, vind++)
					{
						gind = i_Mesh.GetPolygonVertex(poly_ind, vi);

						if (normal_mapper.IsAttached())
							nind = normal_mapper.GetIndex(gind, vind);
						else
							nind = gind; 

						// If we compressed the indices on our own, 
						// use this new index
						if (bCompressedNormals)
							nind = normal_remap[nind];

						if (has_texture_coords)
						{
							tind = uv_mapper.GetIndex(gind, vind);
						}

						VertexInfo new_vertex(	gind,
												nind,
												tind,
												o_MeshInfo.m_Vertices.size());

						std::pair< std::set<VertexInfo>::iterator, bool> insert_result = vertex_set.insert(new_vertex);
						if ( insert_result.second ) // actually inserted
						{
							// This will only be needed when doing vertex animation
							//if ( bFillInVertexRemap )
							//{
							//	o_MeshInfo.m_VertexRemap.insert(std::pair<int, int>(gind, o_MeshInfo.m_Vertices.size()));
							//	o_MeshInfo.m_NormalRemap.insert(std::pair<int, int>(nind, o_MeshInfo.m_Vertices.size()));
							//}

							// add new vertex
							new_indices.push_back(new_vertex.m_NewIndex);
							o_MeshInfo.m_Vertices.push_back(conv_vec3(pControlPoints[new_vertex.m_VertexIndex]));

							if (bCompressedNormals)
								o_MeshInfo.m_Normals.push_back(compressed_normals[nind]);
							else if (normal_mapper.IsAttached())
								o_MeshInfo.m_Normals.push_back(conv_vec3(normal_mapper.GetValue(new_vertex.m_NormalIndex)));
							else
								o_MeshInfo.m_Normals.push_back(maVector3d(0,1,0));

							// Notice that the V coordinate needs to be flipped when exporting for DirectX
							if (has_texture_coords)
							{
								KFbxVector2 uv = uv_mapper.GetValue(new_vertex.m_UVIndex);
								o_MeshInfo.m_UVs.push_back(maVector2d(uv[0], 1 - uv[1]));
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
						o_MeshInfo.m_Indices.push_back( new_indices[v-1] );
						o_MeshInfo.m_Indices.push_back( new_indices[v] );
					}		
				}
			}	

			// Flags for the surface. Not sure how to get them yet.
			// So, make everything double-sided until we can
			// figure out the CW vs. CCW winding?
			//o_MeshInfo.m_Flags.m_bDoubleSided = true;  (didn't help)

			DBG_LOG("Total expanded vertices: " << o_MeshInfo.m_Vertices.size());
			DBG_LOG("Total expanded indices: " << o_MeshInfo.m_Indices.size() << "(" << (o_MeshInfo.m_Indices.size() / 3) << " tris)");
			DBG_LOG("avg vertices per tri: " << std::setprecision(4) << (3.0f * float(o_MeshInfo.m_Vertices.size()) / float(o_MeshInfo.m_Indices.size())) );
		}

	}	// end of local namespace

	//------------------------------------------------------------------------
	//	ConvertMesh converts the geometry in the mesh from the FBX SDK 
	//	into our fragment type.
	//------------------------------------------------------------------------
	void ConvertMesh(tinygltf::Model* i_pModel, tinygltf::Mesh* i_Mesh,
					  g3dSceneNode*& io_pSceneNode,
					  //const fsResourceFinder& i_TextureFinder,
					  mdlMatInfoTable& io_MaterialTable,
					  std::vector<g3dFragment*>& o_Fragments,
					  std::vector<matMaterial*>& o_Materials,
					  const fsLocator& i_ContainingFile)
					  //std::vector<matTexture*>& o_Textures)
	{
		const bool bCreateMaterials = true;
		//fbxMaterialImport::GetNodeMaterials(*i_Mesh.GetNode(),
		//		io_MaterialTable, i_TextureFinder, o_Materials, o_Textures, bLoadTextures);
		fbxMaterialImport::GetNodeMaterials(*i_Mesh.GetNode(),
				io_MaterialTable, o_Materials, bCreateMaterials, i_ContainingFile);

		mdlFragInfo frag_info;
		get_frag_info(i_Mesh, frag_info, io_MaterialTable);

		std::vector<mdlSplitFragInfo> split_frags;
		mdlFragUtil::SplitFragments(frag_info, split_frags);

		std::vector<g3dFragment*> new_fragments;
		int num_split_frags = split_frags.size();
		for (int i=0; i<num_split_frags; i++)
		{
			//DBG_LOG("Number of vertices in fragment: " << split_frags[i].m_Vertices.size());
			//DBG_LOG("Number of triangles in fragment: " << split_frags[i].m_Indices.size() / 3);
			if (split_frags[i].m_Vertices.size() > 0)
			{
				mdlFragCreate::OptimizeFragment( split_frags[i] );
				const bool bMorphable = false; // Just static meshes through FBX for now
				g3dFragment *fragment = mdlFragCreate::CreateFragment(split_frags[i], bMorphable);

				o_Fragments.push_back(fragment);
				new_fragments.push_back(fragment);
			}
		}

		if (new_fragments.size() == 1)
		{
			// Single fragment, just set into scene node
			io_pSceneNode->SetFragment(new_fragments[0]);
		}
		else if (new_fragments.size() > 1)
		{
			// Multiple fragments, make a new set of nodes
			// and add them as children to the current node
			int num_frags = new_fragments.size();
			for (int i=0; i<num_frags; i++)
			{
				g3dSceneNode *node = new g3dSceneNode;
				node->SetFragment(new_fragments[i]);
				io_pSceneNode->AddChild(node);
			}
		}

	}

}	// end of namespace

