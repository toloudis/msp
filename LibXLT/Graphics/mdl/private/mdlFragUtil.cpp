/****************************************************************************\
**  mdlFragUtil.cpp
**
**      mdlFragUtil supplies functions for manipulating between
**	our different fragment data structures.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/mdlFragUtil.hpp"

#include "Core/geo/geoKDTree.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mdl/mdlFragCreate.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlSplitFragInfo.hpp"

#include <iterator>
#include <map>


//============================================================================
//============================================================================
namespace mdlFragUtil
{
	//============================================================================
	//============================================================================
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

		//----------------------------------------------------------------------------
		// Go through index list and find only the vertices that are
		// really used by the index list.  Puts these vertices into
		// the o_Vertices list and returns the number of vertices
		// used.  The o_RemapArray maps from the shorter vertex list
		// into the larger original list.
		//----------------------------------------------------------------------------
		int find_used_vertices( //const std::vector<maPoint3d> &i_Vertices,
								const std::vector<envType::UInt32> &i_Indices,
								//std::vector<maPoint3d> &o_Vertices,
								std::vector<envType::UInt32> &o_Indices,
								std::vector<int> &o_RemapArray)
		{
			// find unique used vertices
			std::map<int, VertexInfo> vert_map;
			int num_inds = i_Indices.size();
			int i;
			for (i=0; i<num_inds; i++) {
				if (vert_map.find(i_Indices[i]) == vert_map.end()) {
					vert_map[i_Indices[i]] = VertexInfo(i_Indices[i]);
				}
			}

			int num_verts = vert_map.size();
			//o_Vertices.resize(num_verts);
			o_RemapArray.resize(num_verts);

			// Form remap array
			int ind = 0;
			std::map<int, VertexInfo>::iterator it = vert_map.begin();
			for (; it != vert_map.end(); ++it, ++ind)
			{
				o_RemapArray[ind] = it->second.m_OldIndex;
				//o_Vertices[ind] = i_Vertices[o_RemapArray[ind]];
				it->second.m_NewIndex = ind;
			}

			// Remap indices
			o_Indices.resize(num_inds);
			for (i=0; i<num_inds; i++)
			{
				std::map<int, VertexInfo>::iterator it = vert_map.find(i_Indices[i]);
				o_Indices[i] = it->second.m_NewIndex;
			}

			return num_verts;
		}

		//------------------------------------------------------------------------
		// See if the material passed in needs to be remapped. If so,
		// the pointer itself is altered and true is returned.
		//------------------------------------------------------------------------
		bool remap_material(matMaterial* &io_pMaterial,
			const std::map<const matMaterial*,matMaterial*>& i_MaterialRemapping)
		{					
			// Look for remapping of material for this instance
			std::map<const matMaterial*,matMaterial*>::const_iterator it = i_MaterialRemapping.find(io_pMaterial);
			if (it != i_MaterialRemapping.end())
			{
				io_pMaterial = it->second;
				return true;
			}
			return false;
		}
	}

	//------------------------------------------------------------------------
	//	Make mdlSplitFragInfo structs from a single mdlFragInfo struct,
	//  one for each material.  Returns mapping from fragment indices
	//	back into original mesh's vertex list.
	//	A variation of this function allows you to override some
	//	material assignments when splitting the fragments.
	//------------------------------------------------------------------------
	void SplitFragments(const mdlFragInfo &i_FragInfo,
		std::vector<mdlSplitFragInfo> &o_SplitFrags)
	{
		std::map<const matMaterial*,matMaterial*> no_material_remapping;
		SplitFragments(i_FragInfo, o_SplitFrags, no_material_remapping);
	}
	void SplitFragments(const mdlFragInfo &i_FragInfo,
		std::vector<mdlSplitFragInfo> &o_SplitFrags,
		const std::map<const matMaterial*,matMaterial*>& i_MaterialRemapping)
	{
		int num_mats = i_FragInfo.m_Materials.size();
		o_SplitFrags.resize(num_mats);

		// Handle num_mats == 1 case specially
		if (num_mats == 1)
		{
			mdlSplitFragInfo &split_frag = o_SplitFrags[0];
			std::vector<int> &remap = split_frag.m_RemapArray;

			split_frag.m_Vertices = i_FragInfo.m_Vertices;
			split_frag.m_Normals = i_FragInfo.m_Normals;
			split_frag.m_UVs = i_FragInfo.m_UVs;
			split_frag.m_Ss = i_FragInfo.m_Ss;
			split_frag.m_Ts = i_FragInfo.m_Ts;
			//split_frag.m_Colors = i_FragInfo.m_Colors;
			split_frag.m_Material = i_FragInfo.m_Materials[0]->m_pMaterial;
			DBG_ASSERT(split_frag.m_Material, "Splitting fragments with null pointer for material.");

			// Look for remapping of material for this instance
			remap_material(split_frag.m_Material, i_MaterialRemapping);

			split_frag.m_Flags.m_bCastsShadow = i_FragInfo.m_Flags.m_bCastsShadow;
			split_frag.m_Flags.m_bReceivesShadow = i_FragInfo.m_Flags.m_bReceivesShadow;
			split_frag.m_Flags.m_bShadowHull = i_FragInfo.m_Flags.m_bShadowHull;
			split_frag.m_Flags.m_bComponentSort = i_FragInfo.m_Flags.m_bTriangleSort;
			split_frag.m_Flags.m_bDoubleSided = i_FragInfo.m_Flags.m_bDoubleSided;

			split_frag.m_Indices = i_FragInfo.m_Indices;
			// keep track of number of non-welding indices
			split_frag.m_WeldIndex = split_frag.m_Indices.size();

			// Add in welding for stencil shadows
			if (!i_FragInfo.m_WeldIndices.empty())
			{
				//DBG_LOG2("Welding: m_Indices.size %d m_WeldIndices.size %d", split_frag.m_Indices.size(), i_FragInfo.m_WeldIndices.size());
				std::copy(i_FragInfo.m_WeldIndices.begin(),i_FragInfo.m_WeldIndices.end(), std::back_inserter(split_frag.m_Indices));
			}
			// Decide if bump map is necessary
			int num_texcoords = (i_FragInfo.m_UVs.size() > 0) ? 1 : 0;
			split_frag.m_Flags.m_bBumpMap = mdlFragCreate::UseBumpFrag(num_texcoords,
				i_FragInfo.m_Materials[0]->m_Info.GetShader().GetNumNames() > 0);

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
			mdlSplitFragInfo &split_frag = o_SplitFrags[i];
			//std::vector<int> &remap = split_frag.m_RemapArray;

			//split_frag.m_Vertices = i_FragInfo.m_Vertices;
			//split_frag.m_Normals = i_FragInfo.m_Normals;
			//split_frag.m_UVs = i_FragInfo.m_UVs;
			split_frag.m_Material = i_FragInfo.m_Materials[i]->m_pMaterial;

			// Look for remapping of material for this instance
			remap_material(split_frag.m_Material, i_MaterialRemapping);

			split_frag.m_Flags.m_bCastsShadow = i_FragInfo.m_Flags.m_bCastsShadow;
			split_frag.m_Flags.m_bReceivesShadow = i_FragInfo.m_Flags.m_bReceivesShadow;
			split_frag.m_Flags.m_bShadowHull = i_FragInfo.m_Flags.m_bShadowHull;
			split_frag.m_Flags.m_bComponentSort = i_FragInfo.m_Flags.m_bTriangleSort;
			split_frag.m_Flags.m_bDoubleSided = i_FragInfo.m_Flags.m_bDoubleSided;

			int first_index = 0, num_indices = i_FragInfo.m_Indices.size();
			i_FragInfo.GetIndexRange(i, first_index, num_indices);

			std::vector<envType::UInt32> new_indices(num_indices);
			for (int ii=0; ii<num_indices; ii++)
				new_indices[ii] = i_FragInfo.m_Indices[ii+first_index];

			// keep track of number of non-welding indices
			split_frag.m_WeldIndex = new_indices.size();

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
			split_frag.m_Flags.m_bBumpMap = mdlFragCreate::UseBumpFrag(num_texcoords,
				i_FragInfo.m_Materials[i]->m_Info.GetShader().GetNumNames() > 0);

			// Simple remapping
			//int num_verts = i_FragInfo.m_Vertices.size();
			//remap.resize( num_verts );
			//for (int v=0; v<num_verts; v++)
			//	remap[v] = v;

			// Find only used vertices
			int num_verts = find_used_vertices( new_indices,
				split_frag.m_Indices,
				split_frag.m_RemapArray);

			bool have_uvs = (!i_FragInfo.m_UVs.empty());
			//bool have_colors = (!i_FragInfo.m_Colors.empty());
			bool have_Ss = (!i_FragInfo.m_Ss.empty());
			bool have_Ts = (!i_FragInfo.m_Ts.empty());

			split_frag.m_Vertices.resize(num_verts);
			split_frag.m_Normals.resize(num_verts);

			if (have_uvs) 
				split_frag.m_UVs.resize(num_verts);
			//if (have_colors) 
			//	split_frag.m_Colors.resize(num_verts);
			if (have_Ss) 
				split_frag.m_Ss.resize(num_verts);
			if (have_Ts) 
				split_frag.m_Ts.resize(num_verts);

			for (int v=0; v<num_verts; v++)
			{
				split_frag.m_Vertices[v] = i_FragInfo.m_Vertices[ split_frag.m_RemapArray[v] ];
				split_frag.m_Normals[v] = i_FragInfo.m_Normals[ split_frag.m_RemapArray[v] ];
				if (have_uvs)
					split_frag.m_UVs[v] = i_FragInfo.m_UVs[ split_frag.m_RemapArray[v] ];
				//if (have_colors)
				//	split_frag.m_Colors[v] = i_FragInfo.m_Colors[ split_frag.m_RemapArray[v] ];
				if (have_Ss)
					split_frag.m_Ss[v] = i_FragInfo.m_Ss[ split_frag.m_RemapArray[v] ];
				if (have_Ts)
					split_frag.m_Ts[v] = i_FragInfo.m_Ts[ split_frag.m_RemapArray[v] ];
			}
		}
	}

#define UNIFORM_TANGENT_SPACE_ACROSS_UVSEAM_BUG_1014 //Define this for 1.5
#if defined( UNIFORM_TANGENT_SPACE_ACROSS_UVSEAM_BUG_1014 )
	//Version of CreateBasisVectors with uniform tangent space across uv seam
	//This is commented out for 1.4, but should be put in once we release 1.4


	//------------------------------------------------------------------------
	// Creates basis vectors for the given fragment info
	//------------------------------------------------------------------------
	void CreateBasisVectors(mdlFragInfo &io_FragInfo)
	{
		std::map< int, int> vertexReverseRemap;
		//if the vertex remap is there
		//then build a revese vertex remap
		if( io_FragInfo.m_VertexRemap.size() > 0)
		{
			std::multimap< int, int >::const_iterator mit;
			for( mit = io_FragInfo.m_VertexRemap.begin(); mit != io_FragInfo.m_VertexRemap.end(); ++mit )
			{
				DBG_ASSERT( (vertexReverseRemap.find( mit->second ) == vertexReverseRemap.end() ), "reverse vertex remap should be a unique map" );
				std::pair< std::map<int, int>::iterator, bool > insertRes = vertexReverseRemap.insert( std::pair<int, int>( mit->second, mit->first ) );
				DBG_ASSERT( insertRes.second == true, "reverse vertex remap should b a unique map " );
			}
		}
		int numVertices = io_FragInfo.m_Vertices.size();

		// Need UVs to generate basis vectors
		if (io_FragInfo.m_UVs.size() != numVertices)
			return;

		// Resize and clear basis vectors
		io_FragInfo.m_Ss.resize(numVertices);
		io_FragInfo.m_Ts.resize(numVertices);

		// Clear the basis vectors
		int i;
		for (i = 0; i < numVertices; i++)
		{
			io_FragInfo.m_Ss[i] = maPoint3d(0.0f, 0.0f, 0.0f);
			io_FragInfo.m_Ts[i] = maPoint3d(0.0f, 0.0f, 0.0f);
		}

		int numIndices = io_FragInfo.m_Indices.size();
		if (numIndices %3 != 0)
			return;

		// Walk through the triangle list and calculate gradiants for each triangle.
		// Sum the results into the S and T components.
		maVector3d S, T, edge01, edge02, cp;
		envType::Int32 ind0, ind1, ind2;
		for ( i = 0; i < numIndices; i += 3 )
		{
			ind0 = io_FragInfo.m_Indices[i];
			ind1 = io_FragInfo.m_Indices[i+1];
			ind2 = io_FragInfo.m_Indices[i+2];
			const maPoint3d& v0 = io_FragInfo.m_Vertices[ind0];
			const maPoint2d& t0 = io_FragInfo.m_UVs[ind0];
			const maPoint3d& v1 = io_FragInfo.m_Vertices[ind1];
			const maPoint2d& t1 = io_FragInfo.m_UVs[ind1];
			const maPoint3d& v2 = io_FragInfo.m_Vertices[ind2];
			const maPoint2d& t2 = io_FragInfo.m_UVs[ind2];

			S.Set(0,0,0);
			T.Set(0,0,0);

			// x, s, t
			edge01.Set( v1.m_X - v0.m_X, t1.m_X - t0.m_X, t1.m_Y - t0.m_Y );
			edge02.Set( v2.m_X - v0.m_X, t2.m_X - t0.m_X, t2.m_Y - t0.m_Y );

			cp = edge01 / edge02;
			if ( fabs(cp.m_X) > maConstants::c_fEpsilon*maConstants::c_fEpsilon )
			{
				S.m_X = -cp.m_Y / cp.m_X;
				T.m_X = -cp.m_Z / cp.m_X;
			}

			// y, s, t
			edge01.Set( v1.m_Y - v0.m_Y, t1.m_X - t0.m_X, t1.m_Y - t0.m_Y );
			edge02.Set( v2.m_Y - v0.m_Y, t2.m_X - t0.m_X, t2.m_Y - t0.m_Y );

			cp = edge01 / edge02;
			if ( fabs(cp.m_X) > maConstants::c_fEpsilon*maConstants::c_fEpsilon )
			{
				S.m_Y = -cp.m_Y / cp.m_X;
				T.m_Y = -cp.m_Z / cp.m_X;
			}

			// z, s, t
			edge01.Set( v1.m_Z - v0.m_Z, t1.m_X - t0.m_X, t1.m_Y - t0.m_Y );
			edge02.Set( v2.m_Z - v0.m_Z, t2.m_X - t0.m_X, t2.m_Y - t0.m_Y );

			cp = edge01 / edge02;
			if ( fabs(cp.m_X) > maConstants::c_fEpsilon*maConstants::c_fEpsilon )
			{
				S.m_Z = -cp.m_Y / cp.m_X;
				T.m_Z = -cp.m_Z / cp.m_X;
			}

			S.Normalize();
			T.Normalize();

			// Now add normalized vector to actual vertex
			//If remapping is present, add the normalized S and T vectors
			//to the final S and T of any other unqiue vertex that is related to the current vertex
			//through re-mapping
			if( vertexReverseRemap.size() > 0)
			{
				std::map< int, int>::const_iterator mrit0 = vertexReverseRemap.find( ind0 );
				std::map< int, int>::const_iterator mrit1 = vertexReverseRemap.find( ind1 );
				std::map< int, int>::const_iterator mrit2 = vertexReverseRemap.find( ind2 );
				DBG_ASSERT( mrit0 != vertexReverseRemap.end(), "vertex should be there" );
				DBG_ASSERT( mrit1 != vertexReverseRemap.end(), "vertex should be there" );
				DBG_ASSERT( mrit2 != vertexReverseRemap.end(), "vertex should be there" );
				std::pair< std::multimap<int, int>::const_iterator, std::multimap<int, int>::const_iterator > itPair0 = io_FragInfo.m_VertexRemap.equal_range( mrit0->second );
				std::pair< std::multimap<int, int>::const_iterator, std::multimap<int, int>::const_iterator > itPair1 = io_FragInfo.m_VertexRemap.equal_range( mrit1->second );
				std::pair< std::multimap<int, int>::const_iterator, std::multimap<int, int>::const_iterator > itPair2 = io_FragInfo.m_VertexRemap.equal_range( mrit2->second );
				DBG_ASSERT( itPair0.first != itPair0.second , "original vertex should be in the vertexMap" );
				DBG_ASSERT( itPair1.first != itPair1.second , "original vertex should be in the vertexMap" );
				DBG_ASSERT( itPair2.first != itPair2.second , "original vertex should be in the vertexMap" );
				std::size_t dist0 = std::distance( itPair0.first, itPair0.second );
				std::size_t dist1 = std::distance( itPair1.first, itPair1.second );
				std::size_t dist2 = std::distance( itPair2.first, itPair2.second );

				std::multimap< int, int >::const_iterator mit0;
				//If there are too many vertoces to share
				//then just dont do this sharing
				if( dist0 <= 10 )
				{
					for( mit0 = itPair0.first; mit0 != itPair0.second; ++ mit0 )
					{
						int sharedVertex = mit0->second;
						io_FragInfo.m_Ss[ sharedVertex ] += S;
						io_FragInfo.m_Ts[ sharedVertex ] += T;
					}
				} else
				{

					io_FragInfo.m_Ss[ ind0 ] += S;
					io_FragInfo.m_Ts[ ind0 ] += T;
				}
				std::multimap< int, int >::const_iterator mit1;

				//If there are too many vertoces to share
				//then just dont do this sharing
				if( dist1 <= 10 )
				{
					for( mit1 = itPair1.first; mit1 != itPair1.second; ++ mit1 )
					{
						int sharedVertex = mit1->second;
						io_FragInfo.m_Ss[ sharedVertex ] += S;
						io_FragInfo.m_Ts[ sharedVertex ] += T;
					}
				} else
				{
					io_FragInfo.m_Ss[ ind1 ] += S;
					io_FragInfo.m_Ts[ ind1 ] += T;
				}
				std::multimap< int, int >::const_iterator mit2;

				//If there are too many vertoces to share
				//then just dont do this sharing
				if( dist2 <= 10 )
				{
					for( mit2 = itPair2.first; mit2 != itPair2.second; ++ mit2 )
					{
						int sharedVertex = mit2->second;
						io_FragInfo.m_Ss[ sharedVertex ] += S;
						io_FragInfo.m_Ts[ sharedVertex ] += T;
					}
				} else
				{
					io_FragInfo.m_Ss[ ind2 ] += S;
					io_FragInfo.m_Ts[ ind2 ] += T;
				}
			} else
			{
				io_FragInfo.m_Ss[ind0] += S;
				io_FragInfo.m_Ts[ind0] += T;
				io_FragInfo.m_Ss[ind1] += S;
				io_FragInfo.m_Ts[ind1] += T;
				io_FragInfo.m_Ss[ind2] += S;
				io_FragInfo.m_Ts[ind2] += T;
			}
		}

		// Calculate the SxT vector
		maVector3d vecSxT;
		for (i = 0; i < numVertices; i++)
		{
			// Normalize the S, T vectors
			io_FragInfo.m_Ss[i].Normalize();
			io_FragInfo.m_Ts[i].Normalize();
		} 
	}

#else //Revert back to the original implementation of CreateBasisVectors

	//------------------------------------------------------------------------
	// Creates basis vectors for the given fragment info
	//------------------------------------------------------------------------
	void CreateBasisVectors(mdlFragInfo &io_FragInfo)
	{
		int numVertices = io_FragInfo.m_Vertices.size();
		
		// Need UVs to generate basis vectors
		if (io_FragInfo.m_UVs.size() != numVertices)
			return;

		// Resize and clear basis vectors
		io_FragInfo.m_Ss.resize(numVertices);
		io_FragInfo.m_Ts.resize(numVertices);

		// Clear the basis vectors
		int i;
		for (i = 0; i < numVertices; i++)
		{
			io_FragInfo.m_Ss[i] = maPoint3d(0.0f, 0.0f, 0.0f);
			io_FragInfo.m_Ts[i] = maPoint3d(0.0f, 0.0f, 0.0f);
		}

		int numIndices = io_FragInfo.m_Indices.size();
		if (numIndices %3 != 0)
			return;

		// Walk through the triangle list and calculate gradiants for each triangle.
		// Sum the results into the S and T components.
		maVector3d S, T, edge01, edge02, cp;
		envType::Int32 ind0, ind1, ind2;
		for ( i = 0; i < numIndices; i += 3 )
		{
			ind0 = io_FragInfo.m_Indices[i];
			ind1 = io_FragInfo.m_Indices[i+1];
			ind2 = io_FragInfo.m_Indices[i+2];
			const maPoint3d& v0 = io_FragInfo.m_Vertices[ind0];
			const maPoint2d& t0 = io_FragInfo.m_UVs[ind0];
			const maPoint3d& v1 = io_FragInfo.m_Vertices[ind1];
			const maPoint2d& t1 = io_FragInfo.m_UVs[ind1];
			const maPoint3d& v2 = io_FragInfo.m_Vertices[ind2];
			const maPoint2d& t2 = io_FragInfo.m_UVs[ind2];

			S.Set(0,0,0);
			T.Set(0,0,0);

			// x, s, t
			edge01.Set( v1.m_X - v0.m_X, t1.m_X - t0.m_X, t1.m_Y - t0.m_Y );
			edge02.Set( v2.m_X - v0.m_X, t2.m_X - t0.m_X, t2.m_Y - t0.m_Y );

			cp = edge01 / edge02;
			if ( fabs(cp.m_X) > maConstants::c_fEpsilon*maConstants::c_fEpsilon )
			{
				S.m_X = -cp.m_Y / cp.m_X;
				T.m_X = -cp.m_Z / cp.m_X;
			}

			// y, s, t
			edge01.Set( v1.m_Y - v0.m_Y, t1.m_X - t0.m_X, t1.m_Y - t0.m_Y );
			edge02.Set( v2.m_Y - v0.m_Y, t2.m_X - t0.m_X, t2.m_Y - t0.m_Y );

			cp = edge01 / edge02;
			if ( fabs(cp.m_X) > maConstants::c_fEpsilon*maConstants::c_fEpsilon )
			{
				S.m_Y = -cp.m_Y / cp.m_X;
				T.m_Y = -cp.m_Z / cp.m_X;
			}

			// z, s, t
			edge01.Set( v1.m_Z - v0.m_Z, t1.m_X - t0.m_X, t1.m_Y - t0.m_Y );
			edge02.Set( v2.m_Z - v0.m_Z, t2.m_X - t0.m_X, t2.m_Y - t0.m_Y );

			cp = edge01 / edge02;
			if ( fabs(cp.m_X) > maConstants::c_fEpsilon*maConstants::c_fEpsilon )
			{
				S.m_Z = -cp.m_Y / cp.m_X;
				T.m_Z = -cp.m_Z / cp.m_X;
			}

			S.Normalize();
			T.Normalize();

			// Now add normalized vector to actual vertex
			io_FragInfo.m_Ss[ind0] += S;
			io_FragInfo.m_Ts[ind0] += T;
			io_FragInfo.m_Ss[ind1] += S;
			io_FragInfo.m_Ts[ind1] += T;
			io_FragInfo.m_Ss[ind2] += S;
			io_FragInfo.m_Ts[ind2] += T;
		}

		// Calculate the SxT vector
		maVector3d vecSxT;
		for (i = 0; i < numVertices; i++)
		{
			// Normalize the S, T vectors
			io_FragInfo.m_Ss[i].Normalize();
			io_FragInfo.m_Ts[i].Normalize();
		} 
	}
#endif
	//------------------------------------------------------------------------
	// Convert remapping from "old index->new index" map to a vector
	//	which maps "new index->old index"
	//------------------------------------------------------------------------
	void ConvertRemapping(const std::multimap<int, int>& i_OrgRemapping,
		std::vector<int>& o_NewRemapping,
		int i_NumExpandedVertices)
	{
		if (i_OrgRemapping.empty())
		{
			// Empty remapping array, so need to generate a simple one
			o_NewRemapping.resize( i_NumExpandedVertices );
			for (int i=0; i<i_NumExpandedVertices; ++i)
				o_NewRemapping[i] = i;
		}
		else
		{
			// I am not sure if it is necessary for the number of expanded 
			// vertices to equal the size of the original remapping multimap
			//DBG_ASSERT(i_OrgRemapping.size() == i_NumExpandedVertices, "Not all vertices are remapped, %d of %d", i_OrgRemapping.size(), i_NumExpandedVertices);
			o_NewRemapping.resize( i_NumExpandedVertices, 0 );
			std::multimap<int, int>::const_iterator it, end = i_OrgRemapping.end();
			for ( it = i_OrgRemapping.begin(); it != end; ++it )
			{
				int nOldVertexIndex = it->first;
				int nNewVertexIndex = it->second;
				o_NewRemapping[nNewVertexIndex] = nOldVertexIndex;
			}
		}
	}

	//------------------------------------------------------------------------
	// Combine the remappings in the original MayFragInfo with the 
	//	remapping arrays from the split fragments generated in order
	//	to create new arrays which go all the way back to the Maya arrays.
	//------------------------------------------------------------------------
	void GenerateFullRemappings(const mdlFragInfo& i_FragInfo,
		const std::vector<mdlSplitFragInfo>& i_SplitFrags,
		std::vector< std::vector<int> >& o_FullVertexRemapping,
		std::vector< std::vector<int> >& o_FullNormalRemapping)
	{
		// Expand out remapping of vertices:
		int num_verts = i_FragInfo.m_Vertices.size();;
		//int num_orig_verts = i_FragInfo.m_VertexRemap.size();
		std::vector<int> primary_vremap;
		ConvertRemapping(i_FragInfo.m_VertexRemap, primary_vremap, num_verts);

		//int num_orig_norms = i_FragInfo.m_NormalRemap.size();
		std::vector<int> primary_nremap;
		ConvertRemapping(i_FragInfo.m_NormalRemap, primary_nremap, num_verts);

		int num_split_frags = i_SplitFrags.size();
		o_FullVertexRemapping.resize(num_split_frags);
		o_FullNormalRemapping.resize(num_split_frags);
		for (int j=0; j<num_split_frags; j++)
		{
			const mdlSplitFragInfo &info = i_SplitFrags[j];

			// There are two remappings, 
			// 1 - remapping from Maya indexing to Direct3D expanded vertices
			// 2 - remapping from when fragments was split from multiple materials
			// Join them all into one remapping here.
			const int num_remap = info.m_RemapArray.size();
			std::vector<int> &vremapping = o_FullVertexRemapping[j];
			vremapping.resize( num_remap );
			std::vector<int> &nremapping = o_FullNormalRemapping[j];
			nremapping.resize( num_remap );
			for (int r=0; r<num_remap; ++r)
			{
				vremapping[r] = primary_vremap[info.m_RemapArray[r]];
				nremapping[r] = primary_nremap[info.m_RemapArray[r]];
			}
		}
	}

	//------------------------------------------------------------------------
	//	Spatially partition the triangles in this fragment so that
	//	no fragment has more than i_MaxNumTris number of triangles.
	//------------------------------------------------------------------------
	void SpatiallyPartition(const mdlSplitFragInfo &i_FragInfo,
		std::vector<mdlSplitFragInfo> &o_SplitFrags,
		int i_MaxNumTris)
	{
		int num_tris = i_FragInfo.m_Indices.size() / 3;
		int old_num_verts = i_FragInfo.m_Vertices.size();
		//DBG_ASSERT(num_tris>i_MaxNumTris, "Num tris: %d is already less than threshold %d, don't call SpatiallyPartition", num_tris, i_MaxNumTris); 

		// If already under limit, just return it.
		if (num_tris <= i_MaxNumTris)
		{
			o_SplitFrags.push_back(i_FragInfo);
			return;
		}

		geoKDTree kd_tree;
		kd_tree.SetNumDesiredPolysInLeaves(i_MaxNumTris);
		kd_tree.AddTriangles(&i_FragInfo.m_Vertices[0], i_FragInfo.m_Vertices.size(),
			&i_FragInfo.m_Indices[0], i_FragInfo.m_Indices.size());
		kd_tree.Create();

		std::vector< std::vector< int > > tri_groups;
		kd_tree.GetGroups( tri_groups );

		const int num_split_frags = tri_groups.size();
		//o_SplitFrags.resize( num_split_frags );
		for (int i=0; i<num_split_frags; i++)
		{
			mdlSplitFragInfo split_frag;
			split_frag.m_Material = i_FragInfo.m_Material;
			split_frag.m_Flags.m_bCastsShadow = i_FragInfo.m_Flags.m_bCastsShadow;
			split_frag.m_Flags.m_bReceivesShadow = i_FragInfo.m_Flags.m_bReceivesShadow;
			split_frag.m_Flags.m_bShadowHull = i_FragInfo.m_Flags.m_bShadowHull;
			split_frag.m_Flags.m_bComponentSort = i_FragInfo.m_Flags.m_bComponentSort;
			split_frag.m_Flags.m_bDoubleSided = i_FragInfo.m_Flags.m_bDoubleSided;
			split_frag.m_Flags.m_bBumpMap = i_FragInfo.m_Flags.m_bBumpMap;

			std::vector< int > &tri_group = tri_groups[i];
			int num_new_indices = tri_group.size() * 3;
			std::vector<envType::UInt32> new_indices(num_new_indices);
			int tri_ind, ind = 0;
			for (int j=0; j<tri_group.size(); ++j)
			{
				tri_ind = tri_group[j] * 3;
				new_indices[ind++] = i_FragInfo.m_Indices[tri_ind];
				new_indices[ind++] = i_FragInfo.m_Indices[tri_ind+1];
				new_indices[ind++] = i_FragInfo.m_Indices[tri_ind+2];

			}

			// keep track of number of non-welding indices
			split_frag.m_WeldIndex = new_indices.size();

			// Find only used vertices
			int num_verts = find_used_vertices( new_indices,
				split_frag.m_Indices,
				split_frag.m_RemapArray);

			bool have_uvs = (!i_FragInfo.m_UVs.empty());
			//bool have_colors = (!i_FragInfo.m_Colors.empty());

			split_frag.m_Vertices.resize(num_verts);
			split_frag.m_Normals.resize(num_verts);

			if (have_uvs) 
				split_frag.m_UVs.resize(num_verts);
			//if (have_colors) 
			//	split_frag.m_Colors.resize(num_verts);

			for (int v=0; v<num_verts; v++)
			{
				split_frag.m_Vertices[v] = i_FragInfo.m_Vertices[ split_frag.m_RemapArray[v] ];
				split_frag.m_Normals[v] = i_FragInfo.m_Normals[ split_frag.m_RemapArray[v] ];
				if (have_uvs)
					split_frag.m_UVs[v] = i_FragInfo.m_UVs[ split_frag.m_RemapArray[v] ];
				//if (have_colors)
				//	split_frag.m_Colors[v] = i_FragInfo.m_Colors[ split_frag.m_RemapArray[v] ];
			}

			o_SplitFrags.push_back( split_frag );
		}
	}

}	// end of namespace

