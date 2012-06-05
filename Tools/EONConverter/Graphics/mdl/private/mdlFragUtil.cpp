/****************************************************************************\
**  mdlFragUtil.cpp
**
**      mdlFragUtil supplies functions for manipulating between
**	our different fragment data structures.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/mdlFragUtil.hpp"

//#include "Graphics/mdl/mdlFragCreate.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlSplitFragInfo.hpp"

#include "Core/dbg/dbgLog.hpp"
//#include "Core/geo/geoKDTree.hpp"
//#include "Graphics/mat/matMaterial.hpp"

#include <set>
#include <sstream>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace mdlFragUtil
{

	namespace
	{
		//--------------------------------------------------------------------
		// Comparison based on old index, but stores new index
		//--------------------------------------------------------------------
		struct VertexInfo
		{
			VertexInfo() :	m_OldIndex(-1),
							m_NewIndex(-1) {}

			VertexInfo(	int i_OldIndex) :	m_OldIndex(i_OldIndex),
											m_NewIndex(-1) {}

			bool operator == (const VertexInfo& i_Vertex) const
			{	return	(m_OldIndex == i_Vertex.m_OldIndex); }

			bool operator < (const VertexInfo& i_Vertex) const
			{
				return m_OldIndex < i_Vertex.m_OldIndex;
			}

			int m_OldIndex;
			int m_NewIndex;
		};

		// Go through index list and find only the vertices that are
		// really used by the index list.  Puts these vertices into
		// the o_Vertices list and returns the number of vertices
		// used.  The o_RemapArray maps from the shorter vertex list
		// into the larger original list.
		int find_used_vertices( //const std::vector<maPoint3d> &i_Vertices,
								const std::vector<envType::UInt32> &i_Indices,
								//std::vector<maPoint3d> &o_Vertices,
								std::vector<envType::UInt32> &o_Indices,
								std::vector<int> &o_RemapArray)
		{
			// find unique used vertices
			std::set<VertexInfo> vert_set;
			int num_inds = i_Indices.size();
			int i;
			for (i=0; i<num_inds; i++)
				vert_set.insert(VertexInfo(i_Indices[i]));

			int num_verts = vert_set.size();
			//o_Vertices.resize(num_verts);
			o_RemapArray.resize(num_verts);

			// Form remap array
			int ind = 0;
			std::set<VertexInfo>::iterator it = vert_set.begin();
			for (; it != vert_set.end(); ++it, ++ind)
			{
				o_RemapArray[ind] = it->m_OldIndex;
				//o_Vertices[ind] = i_Vertices[o_RemapArray[ind]];
				it->m_NewIndex = ind;
			}

			// Remap indices
			o_Indices.resize(num_inds);
			for (i=0; i<num_inds; i++)
			{
				std::set<VertexInfo>::iterator it = vert_set.find(VertexInfo(i_Indices[i]));
				o_Indices[i] = it->m_NewIndex;
			}

			return num_verts;
		}

	}
	//------------------------------------------------------------------------
	//	Make mdlSplitFragInfo structs from a single mdlFragInfo struct,
	//  one for each material.  Returns mapping from fragment indices
	//	back into original mesh's vertex list.
	//------------------------------------------------------------------------
	void SplitFragments(const mdlFragInfo &i_FragInfo,
						std::vector< shared_ptr<mdlSplitFragInfo> > &o_SplitFrags)
	{
		int num_mats = i_FragInfo.m_MaterialNames.size();

		// Handle num_mats == 1 case specially
		if (num_mats == 1)
		{
			shared_ptr<mdlSplitFragInfo> split_frag(new mdlSplitFragInfo);
			o_SplitFrags.push_back(split_frag);
			std::vector<int> &remap = split_frag->m_RemapArray;

			split_frag->m_Name = i_FragInfo.m_Name;
			split_frag->m_Vertices = i_FragInfo.m_Vertices;
			split_frag->m_Normals = i_FragInfo.m_Normals;
			split_frag->m_UVs = i_FragInfo.m_UVs;
			split_frag->m_Colors = i_FragInfo.m_Colors;
			//split_frag->m_Material = i_FragInfo.m_Materials[0]->m_pMaterial;
			split_frag->m_MaterialName = i_FragInfo.m_MaterialNames[0];
			//DBG_ASSERT0(split_frag->m_Material, "Splitting fragments with null pointer for material.");

			split_frag->m_Flags.m_bCastsShadow = i_FragInfo.m_Flags.m_bCastsShadow;
			split_frag->m_Flags.m_bReceivesShadow = i_FragInfo.m_Flags.m_bReceivesShadow;
			split_frag->m_Flags.m_bShadowHull = i_FragInfo.m_Flags.m_bShadowHull;
			split_frag->m_Flags.m_bComponentSort = i_FragInfo.m_Flags.m_bTriangleSort;
			split_frag->m_Flags.m_bDoubleSided = i_FragInfo.m_Flags.m_bDoubleSided;

			split_frag->m_Indices = i_FragInfo.m_Indices;
			// keep track of number of non-welding indices
			split_frag->m_WeldIndex = split_frag->m_Indices.size();

			// Add in welding for stencil shadows
			if (!i_FragInfo.m_WeldIndices.empty())
			{
				//DBG_LOG2("Welding: m_Indices.size %d m_WeldIndices.size %d", split_frag->m_Indices.size(), i_FragInfo.m_WeldIndices.size());
				std::copy(i_FragInfo.m_WeldIndices.begin(),i_FragInfo.m_WeldIndices.end(), std::back_inserter(split_frag->m_Indices));
			}
			// Decide if bump map is necessary
			int num_texcoords = (i_FragInfo.m_UVs.size() > 0) ? 1 : 0;
			split_frag->m_Flags.m_bBumpMap = true;

			// Simple remapping
			int num_verts = i_FragInfo.m_Vertices.size();
			remap.resize( num_verts );
			for (int v=0; v<num_verts; v++)
				remap[v] = v;

			return;
		}


		// Map multiple materials
		for (int i=0; i<num_mats; i++)
		{
			shared_ptr<mdlSplitFragInfo> split_frag(new mdlSplitFragInfo);
			o_SplitFrags.push_back(split_frag);
			//std::vector<int> &remap = split_frag->m_RemapArray;

			//split_frag->m_Vertices = i_FragInfo.m_Vertices;
			//split_frag->m_Normals = i_FragInfo.m_Normals;
			//split_frag->m_UVs = i_FragInfo.m_UVs;
			//split_frag->m_Material = i_FragInfo.m_Materials[i]->m_pMaterial;

			std::ostringstream name_str;
			name_str << i_FragInfo.m_Name << "_" << i;
			split_frag->m_Name = name_str.str();

			split_frag->m_MaterialName = i_FragInfo.m_MaterialNames[i];

			split_frag->m_Flags.m_bCastsShadow = i_FragInfo.m_Flags.m_bCastsShadow;
			split_frag->m_Flags.m_bReceivesShadow = i_FragInfo.m_Flags.m_bReceivesShadow;
			split_frag->m_Flags.m_bShadowHull = i_FragInfo.m_Flags.m_bShadowHull;
			split_frag->m_Flags.m_bComponentSort = i_FragInfo.m_Flags.m_bTriangleSort;
			split_frag->m_Flags.m_bDoubleSided = i_FragInfo.m_Flags.m_bDoubleSided;

			int first_index = 0, num_indices = i_FragInfo.m_Indices.size();
			i_FragInfo.GetIndexRange(i, first_index, num_indices);

			std::vector<envType::UInt32> new_indices(num_indices);
			for (int ii=0; ii<num_indices; ii++)
				new_indices[ii] = i_FragInfo.m_Indices[ii+first_index];

			// keep track of number of non-welding indices
			split_frag->m_WeldIndex = new_indices.size();

			if (i==0)
			{
				// Add in welding for stencil shadows
				// to first fragment only
				if (!i_FragInfo.m_WeldIndices.empty())
				{
					std::copy(i_FragInfo.m_WeldIndices.begin(),i_FragInfo.m_WeldIndices.end(), std::back_inserter(new_indices));
				}
			}

			// Decide if bump map is necessary
			int num_texcoords = (i_FragInfo.m_UVs.size() > 0) ? 1 : 0;
			split_frag->m_Flags.m_bBumpMap = true;

			// Simple remapping
			//int num_verts = i_FragInfo.m_Vertices.size();
			//remap.resize( num_verts );
			//for (int v=0; v<num_verts; v++)
			//	remap[v] = v;

			// Find only used vertices
			int num_verts = find_used_vertices( new_indices,
												split_frag->m_Indices,
												split_frag->m_RemapArray);


			bool have_uvs = (!i_FragInfo.m_UVs.empty());
			bool have_colors = (!i_FragInfo.m_Colors.empty());

			split_frag->m_Vertices.resize(num_verts);
			split_frag->m_Normals.resize(num_verts);

			if (have_uvs) 
				split_frag->m_UVs.resize(num_verts);
			if (have_colors) 
				split_frag->m_Colors.resize(num_verts);

			for (int v=0; v<num_verts; v++)
			{
				split_frag->m_Vertices[v] = i_FragInfo.m_Vertices[ split_frag->m_RemapArray[v] ];
				split_frag->m_Normals[v] = i_FragInfo.m_Normals[ split_frag->m_RemapArray[v] ];
				if (have_uvs)
					split_frag->m_UVs[v] = i_FragInfo.m_UVs[ split_frag->m_RemapArray[v] ];
				if (have_colors)
					split_frag->m_Colors[v] = i_FragInfo.m_Colors[ split_frag->m_RemapArray[v] ];
			}
		}

	}

	//------------------------------------------------------------------------
	// Convert remapping from "old index->new index" map to a vector
	//	which maps "new index->old index"
	//------------------------------------------------------------------------
	//void ConvertRemapping(const std::multimap<int, int>& i_OrgRemapping,
	//					 std::vector<int>& o_NewRemapping,
	//					 int i_NumExpandedVertices)
	//{
	//	if (i_OrgRemapping.empty())
	//	{
	//		// Empty remapping array, so need to generate a simple one
	//		o_NewRemapping.resize( i_NumExpandedVertices );
	//		for (int i=0; i<i_NumExpandedVertices; ++i)
	//			o_NewRemapping[i] = i;
	//	}
	//	else
	//	{
	//		// I am not sure if it is necessary for the number of expanded 
	//		// vertices to equal the size of the original remapping multimap
	//		//DBG_ASSERT2(i_OrgRemapping.size() == i_NumExpandedVertices, "Not all vertices are remapped, %d of %d", i_OrgRemapping.size(), i_NumExpandedVertices);
	//		o_NewRemapping.resize( i_NumExpandedVertices, 0 );
	//		std::multimap<int, int>::const_iterator it, end = i_OrgRemapping.end();
	//		for( it = i_OrgRemapping.begin(); it != end; ++it )
	//		{
	//			int nOldVertexIndex = it->first;
	//			int nNewVertexIndex = it->second;
	//			o_NewRemapping[nNewVertexIndex] = nOldVertexIndex;
	//		}
	//	}
	//}

	//------------------------------------------------------------------------
	// Combine the remappings in the original MayFragInfo with the 
	//	remapping arrays from the split fragments generated in order
	//	to create new arrays which go all the way back to the Maya arrays.
	//------------------------------------------------------------------------
	//void GenerateFullRemappings(const mdlFragInfo& i_FragInfo,
	//	const std::vector<mdlSplitFragInfo>& i_SplitFrags,
	//	std::vector< std::vector<int> >& o_FullVertexRemapping,
	//	std::vector< std::vector<int> >& o_FullNormalRemapping)
	//{
	//	// Expand out remapping of vertices:
	//	int num_verts = i_FragInfo.m_Vertices.size();;
	//	//int num_orig_verts = i_FragInfo.m_VertexRemap.size();
	//	std::vector<int> primary_vremap;
	//	ConvertRemapping(i_FragInfo.m_VertexRemap, primary_vremap, num_verts);

	//	//int num_orig_norms = i_FragInfo.m_NormalRemap.size();
	//	std::vector<int> primary_nremap;
	//	ConvertRemapping(i_FragInfo.m_NormalRemap, primary_nremap, num_verts);

	//	int num_split_frags = i_SplitFrags.size();
	//	o_FullVertexRemapping.resize(num_split_frags);
	//	o_FullNormalRemapping.resize(num_split_frags);
	//	for (int j=0; j<num_split_frags; j++)
	//	{
	//		const mdlSplitFragInfo &info = i_SplitFrags[j];

	//		// There are two remappings, 
	//		// 1 - remapping from Maya indexing to Direct3D expanded vertices
	//		// 2 - remapping from when fragments was split from multiple materials
	//		// Join them all into one remapping here.
	//		const int num_remap = info.m_RemapArray.size();
	//		std::vector<int> &vremapping = o_FullVertexRemapping[j];
	//		vremapping.resize( num_remap );
	//		std::vector<int> &nremapping = o_FullNormalRemapping[j];
	//		nremapping.resize( num_remap );
	//		for (int r=0; r<num_remap; ++r)
	//		{
	//			vremapping[r] = primary_vremap[info.m_RemapArray[r]];
	//			nremapping[r] = primary_nremap[info.m_RemapArray[r]];
	//		}
	//	}
	//}

	////------------------------------------------------------------------------
	////	Spatially partition the triangles in this fragment so that
	////	no fragment has more than i_MaxNumTris number of triangles.
	////------------------------------------------------------------------------
	//void SpatiallyPartition(const mdlSplitFragInfo &i_FragInfo,
	//					std::vector<mdlSplitFragInfo> &o_SplitFrags,
	//					int i_MaxNumTris)
	//{
	//	int num_tris = i_FragInfo.m_Indices.size() / 3;
	//	int old_num_verts = i_FragInfo.m_Vertices.size();
	//	//DBG_ASSERT2(num_tris>i_MaxNumTris, "Num tris: %d is already less than threshold %d, don't call SpatiallyPartition", num_tris, i_MaxNumTris); 

	//	// If already under limit, just return it.
	//	if (num_tris <= i_MaxNumTris)
	//	{
	//		o_SplitFrags.push_back(i_FragInfo);
	//		return;
	//	}

	//	geoKDTree kd_tree;
	//	kd_tree.SetNumDesiredPolysInLeaves(i_MaxNumTris);
	//	kd_tree.AddTriangles(&i_FragInfo.m_Vertices[0], i_FragInfo.m_Vertices.size(),
	//		&i_FragInfo.m_Indices[0], i_FragInfo.m_Indices.size());
	//	kd_tree.Create();

	//	std::vector< std::vector< int > > tri_groups;
	//	kd_tree.GetGroups( tri_groups );

	//	const int num_split_frags = tri_groups.size();
	//	//o_SplitFrags.resize( num_split_frags );
	//	for (int i=0; i<num_split_frags; i++)
	//	{
	//		mdlSplitFragInfo split_frag;
	//		split_frag.m_Material = i_FragInfo.m_Material;
	//		split_frag.m_Flags.m_bCastsShadow = i_FragInfo.m_Flags.m_bCastsShadow;
	//		split_frag.m_Flags.m_bReceivesShadow = i_FragInfo.m_Flags.m_bReceivesShadow;
	//		split_frag.m_Flags.m_bShadowHull = i_FragInfo.m_Flags.m_bShadowHull;
	//		split_frag.m_Flags.m_bComponentSort = i_FragInfo.m_Flags.m_bComponentSort;
	//		split_frag.m_Flags.m_bDoubleSided = i_FragInfo.m_Flags.m_bDoubleSided;
	//		split_frag.m_Flags.m_bBumpMap = i_FragInfo.m_Flags.m_bBumpMap;

	//		std::vector< int > &tri_group = tri_groups[i];
	//		int num_new_indices = tri_group.size() * 3;
	//		std::vector<envType::UInt32> new_indices(num_new_indices);
	//		int tri_ind, ind = 0;
	//		for (int j=0; j<tri_group.size(); ++j)
	//		{
	//			tri_ind = tri_group[j] * 3;
	//			new_indices[ind++] = i_FragInfo.m_Indices[tri_ind];
	//			new_indices[ind++] = i_FragInfo.m_Indices[tri_ind+1];
	//			new_indices[ind++] = i_FragInfo.m_Indices[tri_ind+2];

	//		}

	//		// keep track of number of non-welding indices
	//		split_frag.m_WeldIndex = new_indices.size();

	//		// Find only used vertices
	//		int num_verts = find_used_vertices( new_indices,
	//											split_frag.m_Indices,
	//											split_frag.m_RemapArray);


	//		bool have_uvs = (!i_FragInfo.m_UVs.empty());
	//		bool have_colors = (!i_FragInfo.m_Colors.empty());

	//		split_frag.m_Vertices.resize(num_verts);
	//		split_frag.m_Normals.resize(num_verts);

	//		if (have_uvs) 
	//			split_frag.m_UVs.resize(num_verts);
	//		if (have_colors) 
	//			split_frag.m_Colors.resize(num_verts);

	//		for (int v=0; v<num_verts; v++)
	//		{
	//			split_frag.m_Vertices[v] = i_FragInfo.m_Vertices[ split_frag.m_RemapArray[v] ];
	//			split_frag.m_Normals[v] = i_FragInfo.m_Normals[ split_frag.m_RemapArray[v] ];
	//			if (have_uvs)
	//				split_frag.m_UVs[v] = i_FragInfo.m_UVs[ split_frag.m_RemapArray[v] ];
	//			if (have_colors)
	//				split_frag.m_Colors[v] = i_FragInfo.m_Colors[ split_frag.m_RemapArray[v] ];
	//		}

	//		o_SplitFrags.push_back( split_frag );
	//	}
	//}

}	// end of namespace

