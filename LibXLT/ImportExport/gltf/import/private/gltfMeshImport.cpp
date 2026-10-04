/****************************************************************************\
**  gltfMeshImport.cpp
**
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/gltf/import/private/gltfMeshImport.hpp"
#include "ImportExport/gltf/import/private/gltfAccessorUtil.hpp"
#include "ImportExport/gltf/import/private/gltfMaterialImport.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mdl/private/mdlIndexUtil.hpp"
#include "Graphics/mdl/mdlFragCreate.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"
#include "Graphics/mdl/mdlSplitFragInfo.hpp"

#include <algorithm>
#include <map>
#include <string>

#undef FindResource


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace gltfMeshImport
{

	namespace
	{
		//------------------------------------------------------------------------
		// group of primitive indices per material
		//------------------------------------------------------------------------
		struct MaterialPolyGroup
		{
			MaterialPolyGroup() : m_bDoubleSided(false) {}

			std::string m_MaterialName;		// key into the material table
			bool m_bDoubleSided;
			std::vector<int> m_PolyIndices;	// indices of primitives in the mesh
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
		// Get material mapping per primitive from glTF Mesh object.
		// Each glTF primitive has exactly one material; primitives sharing
		// a material are grouped so they become one fragment.
		//--------------------------------------------------------------------
		void process_poly_materials(const tinygltf::Model* i_pModel, const tinygltf::Mesh& i_Mesh,
							        std::vector<MaterialPolyGroup> &o_PolyGroups)
		{
			const int num_primitives = (int)i_Mesh.primitives.size();
			for (int i = 0; i < num_primitives; i++)
			{
				const tinygltf::Primitive& prim = i_Mesh.primitives[i];

				if (!gltfAccessorUtil::IsTriangleMode(prim.mode))
				{
					DBG_WARNING("glTF mesh " << i_Mesh.name << ": skipping primitive " << i << " with non-triangle mode " << prim.mode);
					continue;
				}
				if (prim.attributes.find("POSITION") == prim.attributes.end())
					continue;

				int mat_id = prim.material;
				if (mat_id >= (int)i_pModel->materials.size())
					mat_id = -1;
				const std::string mat_key = gltfMaterialImport::GetMaterialKey(i_pModel, mat_id);

				std::vector<MaterialPolyGroup>::iterator group = o_PolyGroups.begin();
				for (; group != o_PolyGroups.end(); ++group)
				{
					if (group->m_MaterialName == mat_key)
						break;
				}
				if (group == o_PolyGroups.end())
				{
					MaterialPolyGroup new_group;
					new_group.m_MaterialName = mat_key;
					new_group.m_bDoubleSided = (mat_id >= 0) && i_pModel->materials[mat_id].doubleSided;
					o_PolyGroups.push_back(new_group);
					group = o_PolyGroups.end() - 1;
				}

				// Add this primitive index to this material's list
				group->m_PolyIndices.push_back(i);
			}

			// Now check for error cases: if material name is empty or the polygon list is empty,
			// remove this material group
			o_PolyGroups.erase(std::remove_if(o_PolyGroups.begin(), o_PolyGroups.end(), bad_poly_group),
							   o_PolyGroups.end());
		}

		//--------------------------------------------------------------------
		// accessor index of a primitive attribute, -1 if not present
		//--------------------------------------------------------------------
		int find_attribute(const tinygltf::Primitive& i_Prim, const char* i_Name)
		{
			std::map<std::string, int>::const_iterator it = i_Prim.attributes.find(i_Name);
			return (it == i_Prim.attributes.end()) ? -1 : it->second;
		}

		//--------------------------------------------------------------------
		// Compute smooth vertex normals for vertices [i_FirstVertex, end)
		// from triangles [i_FirstIndex, end), for primitives without normals.
		//--------------------------------------------------------------------
		void compute_normals(mdlFragInfo& io_MeshInfo, size_t i_FirstVertex, size_t i_FirstIndex)
		{
			const size_t num_verts = io_MeshInfo.m_Vertices.size();
			for (size_t v = i_FirstVertex; v < num_verts; v++)
				io_MeshInfo.m_Normals[v] = maVector3d(0, 0, 0);

			for (size_t i = i_FirstIndex; i + 2 < io_MeshInfo.m_Indices.size(); i += 3)
			{
				const envType::UInt32 i0 = io_MeshInfo.m_Indices[i];
				const envType::UInt32 i1 = io_MeshInfo.m_Indices[i + 1];
				const envType::UInt32 i2 = io_MeshInfo.m_Indices[i + 2];
				const maVector3d e1 = io_MeshInfo.m_Vertices[i1] - io_MeshInfo.m_Vertices[i0];
				const maVector3d e2 = io_MeshInfo.m_Vertices[i2] - io_MeshInfo.m_Vertices[i0];
				// counter-clockwise front face, area weighted
				const maVector3d face_normal = e1.Cross(e2);
				io_MeshInfo.m_Normals[i0] += face_normal;
				io_MeshInfo.m_Normals[i1] += face_normal;
				io_MeshInfo.m_Normals[i2] += face_normal;
			}

			for (size_t v = i_FirstVertex; v < num_verts; v++)
			{
				if (!io_MeshInfo.m_Normals[v].Normalize())
					io_MeshInfo.m_Normals[v] = maVector3d(0, 1, 0);
			}
		}

		//--------------------------------------------------------------------
		// Append the triangles of one primitive to o_MeshInfo.
		// Returns false if the primitive could not be read.
		//--------------------------------------------------------------------
		bool append_primitive(const tinygltf::Model* i_pModel, const tinygltf::Mesh& i_Mesh, int i_PrimIndex,
							  bool i_bWriteUVs, mdlFragInfo& o_MeshInfo)
		{
			const tinygltf::Primitive& prim = i_Mesh.primitives[i_PrimIndex];

			std::vector<float> positions;
			if (!gltfAccessorUtil::ReadFloats(*i_pModel, find_attribute(prim, "POSITION"), 3, positions))
			{
				DBG_WARNING("glTF mesh " << i_Mesh.name << ": could not read positions of primitive " << i_PrimIndex);
				return false;
			}
			const size_t num_verts = positions.size() / 3;

			// Normals and UVs are optional; ignore them if they don't match the positions
			std::vector<float> normals;
			if (gltfAccessorUtil::ReadFloats(*i_pModel, find_attribute(prim, "NORMAL"), 3, normals) &&
				normals.size() != num_verts * 3)
				normals.clear();

			std::vector<float> uvs;
			if (gltfAccessorUtil::ReadFloats(*i_pModel, find_attribute(prim, "TEXCOORD_0"), 2, uvs) &&
				uvs.size() != num_verts * 2)
				uvs.clear();

			// Without indices, vertices are used in order
			std::vector<uint32_t> vert_indices;
			if (prim.indices >= 0)
			{
				if (!gltfAccessorUtil::ReadIndices(*i_pModel, prim.indices, vert_indices))
				{
					DBG_WARNING("glTF mesh " << i_Mesh.name << ": could not read indices of primitive " << i_PrimIndex);
					return false;
				}
			}
			else
			{
				vert_indices.resize(num_verts);
				for (size_t v = 0; v < num_verts; v++)
					vert_indices[v] = (uint32_t)v;
			}

			std::vector<uint32_t> triangles;
			gltfAccessorUtil::Triangulate(prim.mode, vert_indices, triangles);
			for (size_t i = 0; i < triangles.size(); i++)
			{
				if (triangles[i] >= num_verts)
				{
					DBG_WARNING("glTF mesh " << i_Mesh.name << ": primitive " << i_PrimIndex << " has out of range indices");
					return false;
				}
			}
			if (triangles.empty())
				return false;

			const size_t first_vertex = o_MeshInfo.m_Vertices.size();
			const size_t first_index = o_MeshInfo.m_Indices.size();

			for (size_t v = 0; v < num_verts; v++)
			{
				o_MeshInfo.m_Vertices.push_back(maPoint3d(positions[v * 3] * c_UnitScale,
														  positions[v * 3 + 1] * c_UnitScale,
														  positions[v * 3 + 2] * c_UnitScale));
				if (!normals.empty())
					o_MeshInfo.m_Normals.push_back(maVector3d(normals[v * 3], normals[v * 3 + 1], normals[v * 3 + 2]));
				else
					o_MeshInfo.m_Normals.push_back(maVector3d(0, 1, 0));

				// glTF UVs have their origin at the top left like DirectX, so no V flip
				if (i_bWriteUVs)
				{
					if (!uvs.empty())
						o_MeshInfo.m_UVs.push_back(maPoint2d(uvs[v * 2], uvs[v * 2 + 1]));
					else
						o_MeshInfo.m_UVs.push_back(maPoint2d(0, 0));
				}
			}

			for (size_t i = 0; i < triangles.size(); i++)
				o_MeshInfo.m_Indices.push_back((envType::UInt32)(first_vertex + triangles[i]));

			if (normals.empty())
				compute_normals(o_MeshInfo, first_vertex, first_index);

			return true;
		}

		//--------------------------------------------------------------------
		// Get fragment information from glTF Mesh object.
		// o_DoubleSided gets one entry per material in o_MeshInfo.
		//--------------------------------------------------------------------
		void get_frag_info(const tinygltf::Model* i_pModel, const tinygltf::Mesh& i_Mesh, 
						   mdlFragInfo& o_MeshInfo,
						   mdlMatInfoTable& io_MaterialTable,
						   std::vector<bool>& o_DoubleSided)
		{
			std::vector<MaterialPolyGroup> poly_groups;
			process_poly_materials(i_pModel, i_Mesh, poly_groups);

			// All vertices of a fragment share one layout, so if any
			// primitive has texture coords, give them to every vertex.
			bool has_texture_coords = false;
			for (size_t g = 0; g < poly_groups.size(); g++)
			{
				for (size_t p = 0; p < poly_groups[g].m_PolyIndices.size(); p++)
				{
					if (find_attribute(i_Mesh.primitives[poly_groups[g].m_PolyIndices[p]], "TEXCOORD_0") >= 0)
						has_texture_coords = true;
				}
			}

			//	Indices for each material
			int num_Materials = poly_groups.size();
			//DBG_LOG("Mesh " << i_Mesh.name << " num poly groups: " << num_Materials); 
			for (int mat_ind = 0 ; mat_ind < num_Materials ; mat_ind++ )
			{
				MaterialPolyGroup &poly_group = poly_groups[mat_ind];

				mdlMatInfoTable::iterator material = io_MaterialTable.find(poly_group.m_MaterialName);
				if (material == io_MaterialTable.end() || !material->second->m_pMaterial)
				{
					DBG_WARNING("glTF mesh " << i_Mesh.name << ": no material created for " << poly_group.m_MaterialName);
					continue;
				}

				const size_t first_triangle = o_MeshInfo.m_Indices.size() / 3;
				const int num_indices = poly_group.m_PolyIndices.size();
				for (int pi=0; pi<num_indices; pi++)
				{
					append_primitive(i_pModel, i_Mesh, poly_group.m_PolyIndices[pi], has_texture_coords, o_MeshInfo);
				}

				// Nothing readable for this material
				if (o_MeshInfo.m_Indices.size() / 3 == first_triangle)
					continue;

				// Add material pointer for this polygon group
				if (!o_MeshInfo.m_Materials.empty())
					o_MeshInfo.m_MaterialChanges.push_back((int)first_triangle);
				o_MeshInfo.m_Materials.push_back(material->second);
				o_DoubleSided.push_back(poly_group.m_bDoubleSided);
			}

			DBG_LOG("Total expanded vertices: " << o_MeshInfo.m_Vertices.size());
			DBG_LOG("Total expanded indices: " << o_MeshInfo.m_Indices.size() << "(" << (o_MeshInfo.m_Indices.size() / 3) << " tris)");
		}

	}	// end of local namespace

	//------------------------------------------------------------------------
	//	ConvertMesh converts the geometry in the glTF mesh 
	//	into our fragment type.
	//------------------------------------------------------------------------
	void ConvertMesh(const tinygltf::Model* i_pModel, tinygltf::Mesh* i_Mesh,
					  g3dSceneNode*& io_pSceneNode,
					  //const fsResourceFinder& i_TextureFinder,
					  mdlMatInfoTable& io_MaterialTable,
					  std::vector<g3dFragment*>& o_Fragments,
					  std::vector<matMaterial*>& o_Materials,
					  const fsLocator& i_ContainingFile)
					  //std::vector<matTexture*>& o_Textures)
	{
		const bool bCreateMaterials = true;
		gltfMaterialImport::GetNodeMaterials(i_pModel, i_Mesh,
				io_MaterialTable, o_Materials, bCreateMaterials, i_ContainingFile);

		mdlFragInfo frag_info;
		std::vector<bool> double_sided;
		get_frag_info(i_pModel, *i_Mesh, frag_info, io_MaterialTable, double_sided);
		if (frag_info.m_Materials.empty())
			return;

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
				// split fragments are in the same order as frag_info's materials
				split_frags[i].m_Flags.m_bDoubleSided = double_sided[i];

				mdlFragCreate::OptimizeFragment( split_frags[i] );
				const bool bMorphable = false; // Just static meshes through glTF for now
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

