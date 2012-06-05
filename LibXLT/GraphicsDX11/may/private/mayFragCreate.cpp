/****************************************************************************\
**  mayFragCreate.cpp
**
**      mayFragCreate.hpp supplies functions for controlling what
**	types of fragments are created by the mayPackage. In the
**	future, this may provide convience functions for loading
**	textures and creating fragments from mdlFragInfo structs.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/may/mayFragCreate.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlHairInfo.hpp"
#include "Graphics/mdl/mdlSplitFragInfo.hpp"
#include "GraphicsDX11/bump/bumpBufferUtil.hpp"
#include "GraphicsDX11/bump/bumpTriMeshBumpFrag.hpp"
#include "GraphicsDX11/bump/bumpVertexBuffer.hpp"
#include "GraphicsDX11/hair/hairModelFrag.hpp"
//#include "GraphicsDX11/tmesh/tmeshRegFrag.hpp"

//----------------------------------------------------------------------------
// Static data
//----------------------------------------------------------------------------
mayFragCreate::Choice mayFragCreate::sm_Choice = e_Always;

namespace
{
	//--------------------------------------------------------------------
	// Alter order of vertex data based on remapping array returned
	//	from D3DXOptimizeVertices
	//--------------------------------------------------------------------
	template <class T>
	void Remap( std::vector<T> &io_Verts,
				const std::vector<DWORD>& i_Remap)
	{
		if (io_Verts.empty()) return;

		std::vector<T> temp = io_Verts;

		const T *pTemp = &temp[0];
		const DWORD *pRemap = &i_Remap[0];

		DWORD new_ind;
		const int num_remap = i_Remap.size();
		for (int i=0; i<num_remap; ++i)
		{
			new_ind = *(pRemap++);
			if (new_ind == 0xFFFFFFFF)
			{
				// reached premature end. This means that there
				// were some unused vertices, so resize the array down smaller
				io_Verts.resize(i);
				break;
			}
			else
				io_Verts[new_ind] = *(pTemp++);
		}
	}


}

//------------------------------------------------------------------------
// Controls when to use bump fragments.  Some apps may want to use
// bump fragments always and some may never want to use them.
// This is "e_WhenAppropriate" by default.
//------------------------------------------------------------------------
mayFragCreate::Choice mayFragCreate::GetBumpFragChoice()
{
	return mayFragCreate::sm_Choice;
}

void mayFragCreate::SetBumpFragChoice(Choice i_Choice)
{
	mayFragCreate::sm_Choice = i_Choice;
}


//------------------------------------------------------------------------
//	Returns true if this fragment should use a bumpTriMeshBumpFrag
//  instead of a g3dFragment
//------------------------------------------------------------------------
bool mayFragCreate::UseBumpFrag(int i_NumTexCoords, bool i_EffectQualified)
{
	if (sm_Choice == e_Never)
		return false;
	else if (sm_Choice == e_Always)
		return ( i_NumTexCoords == 1 );
	else
	{
		// when appropriate:
		// use special effect of material to determine
		return ( i_EffectQualified && i_NumTexCoords == 1 );
	}
}

//------------------------------------------------------------------------
//	Create a single fragment from a mdlSplitFragInfo struct
//------------------------------------------------------------------------
g3dFragment* mayFragCreate::CreateFragment(const mdlSplitFragInfo & i_FragInfo,
							   bool i_Morphable)
{
	tmeshFrag* ret_val = NULL;
	if (i_FragInfo.m_Vertices.empty())
		return NULL;

	DBG_ASSERT(i_FragInfo.m_Material, "Attempting to create fragment without material.");

	//const maFloatRGBA* pColors = (i_FragInfo.m_Colors.empty()) ? NULL : (&i_FragInfo.m_Colors[0]);
	const maVector2d* pUVs = (i_FragInfo.m_UVs.empty()) ? NULL : (&i_FragInfo.m_UVs[0]);
	const maVector3d* pSs = (i_FragInfo.m_Ss.empty()) ? NULL : (&i_FragInfo.m_Ss[0]);
	const maVector3d* pTs = (i_FragInfo.m_Ts.empty()) ? NULL : (&i_FragInfo.m_Ts[0]);
	ret_val = new bumpTriMeshBumpFrag(	&i_FragInfo.m_Vertices[0],
										&i_FragInfo.m_Normals[0],
										pUVs,
										//pColors,
										pSs,
										pTs,
										i_FragInfo.m_Vertices.size(),
										&i_FragInfo.m_Indices[0],
										i_FragInfo.m_Indices.size(),
										i_FragInfo.m_WeldIndex,
										i_FragInfo.m_Material,
										i_Morphable,
										i_FragInfo.m_Flags.m_bComponentSort);

	// Flags
	ret_val->SetCastsShadow(i_FragInfo.m_Flags.m_bCastsShadow);
	ret_val->SetReceivesShadow(i_FragInfo.m_Flags.m_bReceivesShadow);
	ret_val->SetShadowHull(i_FragInfo.m_Flags.m_bShadowHull);
	ret_val->SetDoubleSided(i_FragInfo.m_Flags.m_bDoubleSided);

	return ret_val;
}

//------------------------------------------------------------------------
//	Create a hair fragment from a mdlHairInfo struct
//------------------------------------------------------------------------
g3dFragment* mayFragCreate::CreateHairFragment(const mdlHairInfo &i_HairInfo,
											   matMaterial *i_pMaterial)
{
	matMaterial *pMaterial = (i_pMaterial) ? i_pMaterial : i_HairInfo.m_Material->m_pMaterial;
	return new hairModelFrag( i_HairInfo, pMaterial );
}

//------------------------------------------------------------------------
// Alter the triangle and vertex ordering to best utilize the
//	vertex cache in the GPU
//------------------------------------------------------------------------
void mayFragCreate::OptimizeFragment(mdlSplitFragInfo &io_Info)
{
	return;
#if 0
	// Leaving in this assert because I want to know if this happens,
	// but the assert itself isn't necessary.
	//DBG_ASSERT(io_Info.m_WeldIndex == io_Info.m_Indices.size(), "Cannot optimize fragment info with shadow welding.");
	if (io_Info.m_WeldIndex != io_Info.m_Indices.size())
	{
		DBG_WARNING("Cannot optimize fragment info with shadow welding.");
		return;
	}

	// D3DXOptimizeFaces sorts the triangle ordering so that the
	// vertex cache is used best.
	const int num_faces = (io_Info.m_Indices.size() / 3); // always just triangles
	const int num_verts = io_Info.m_Vertices.size();
	const bool b32BitIndices = true;
	std::vector<DWORD> remapping(num_faces);
	HRESULT hr = D3DXOptimizeFaces( &io_Info.m_Indices[0],
									num_faces,
									num_verts,
									b32BitIndices,
									&remapping[0]);

	// If error, return without changing info structure
	if (hr != D3D_OK)
		return;

	// Apply remap, reordering faces better for vertex cache
	std::vector<envType::UInt32> org_indices = io_Info.m_Indices;
	envType::UInt32 *pOldFace, *pNewFace = &io_Info.m_Indices[0];
	for (int fi=0; fi<num_faces; fi++)
	{
		pOldFace = &org_indices[ remapping[fi] * 3 ];
		*(pNewFace++) = *(pOldFace++);
		*(pNewFace++) = *(pOldFace++);
		*(pNewFace++) = *(pOldFace++);
	}

	// Now reorganize the vertex ordering
	remapping.resize(num_verts);
	hr = D3DXOptimizeVertices( &io_Info.m_Indices[0],
									num_faces,
									num_verts,
									b32BitIndices,
									&remapping[0]);

	// If error, return without changing vertex ordering
	if (hr != D3D_OK)
		return;

	// Remap the arrays of vertex info
	Remap<maPoint3d>(io_Info.m_Vertices, remapping);
	Remap<maVector3d>(io_Info.m_Normals, remapping);
	Remap<maPoint2d>(io_Info.m_UVs, remapping);
	//Remap<maFloatRGBA>(io_Info.m_Colors, remapping);
	Remap<maVector3d>(io_Info.m_Ss, remapping);
	Remap<maVector3d>(io_Info.m_Ts, remapping);
	Remap<int>(io_Info.m_RemapArray, remapping);

	// Remap indices also
	org_indices = io_Info.m_Indices;
	for (int i=0;i<io_Info.m_Indices.size();i++)
		io_Info.m_Indices[i] = remapping[ org_indices[i] ];

#endif
}

//------------------------------------------------------------------------
// Create multiple fragments from a multiple material surface 
//	structure such that they share the same vertex buffer.
//	Returns pointer to shared vertex buffer, owned by the caller.
//------------------------------------------------------------------------
g3dVertexBuffer* mayFragCreate::CreateFragmentGroup(const mdlFragInfo &i_FragInfo,
								 std::vector<g3dFragment*> &o_Fragments,
								 bool i_Morphable)
{	
	if (i_FragInfo.m_Vertices.empty())
		return NULL;

	int num_materials = i_FragInfo.m_Materials.size();
	DBG_ASSERT(num_materials>0, "Attempting to create fragment group without material.");
	DBG_ASSERT(i_FragInfo.m_MaterialChanges.size() == num_materials-1, "Incorrect number of material changes.");

	const maVector2d* pUVs = (i_FragInfo.m_UVs.empty()) ? NULL : (&i_FragInfo.m_UVs[0]);
	const maVector3d* pSs = (i_FragInfo.m_Ss.empty()) ? NULL : (&i_FragInfo.m_Ss[0]);
	const maVector3d* pTs = (i_FragInfo.m_Ts.empty()) ? NULL : (&i_FragInfo.m_Ts[0]);

	const bool bCreateBasisVectors = true;
	shared_ptr<tmeshVertexBuffer> shared_vertex_buffer =
		bumpBufferUtil::CreateVertexBuffer( &i_FragInfo.m_Vertices[0],
											&i_FragInfo.m_Normals[0],
											pUVs,
											pSs,
											pTs,
											i_FragInfo.m_Vertices.size(),
											&i_FragInfo.m_Indices[0],
											i_FragInfo.m_Indices.size(),
											i_Morphable,
											i_FragInfo.m_Flags.m_bTriangleSort,
											bCreateBasisVectors);

	std::auto_ptr<bumpVertexBuffer> pBumpBuffer( new bumpVertexBuffer(shared_vertex_buffer) );

	o_Fragments.resize( num_materials, NULL );
	for (int mi=0; mi<num_materials; mi++)
	{
		// Get indices for just the mi'th material
		int first_index = 0, num_indices = i_FragInfo.m_Indices.size();
		i_FragInfo.GetIndexRange(mi, first_index, num_indices);

		shared_ptr<tmeshIndexBuffer> individual_index_buffer =
			bumpBufferUtil::CreateIndexBuffer(&i_FragInfo.m_Indices[first_index],
											num_indices,
											num_indices,
											i_FragInfo.m_Vertices.size(),
											i_FragInfo.m_Flags.m_bTriangleSort);


		// Create fragment that uses the buffers we just created
		std::auto_ptr<tmeshFrag> pFragment( new bumpTriMeshBumpFrag(	shared_vertex_buffer,
														pBumpBuffer->GetVertexBuffer_Old(),
														individual_index_buffer,
														i_FragInfo.m_Materials[mi]->m_pMaterial) );

		// Flags
		pFragment->SetCastsShadow(i_FragInfo.m_Flags.m_bCastsShadow);
		pFragment->SetReceivesShadow(i_FragInfo.m_Flags.m_bReceivesShadow);
		pFragment->SetShadowHull(i_FragInfo.m_Flags.m_bShadowHull);
		pFragment->SetDoubleSided(i_FragInfo.m_Flags.m_bDoubleSided);
		o_Fragments[mi] = pFragment.release();
	}

	return pBumpBuffer.release();
}

//------------------------------------------------------------------------
// Update an existing multiple fragment group based on a new topology.
// The assumption is that the number of vertices and indices has changed
// (i.e. through a subdivision level change). 
//------------------------------------------------------------------------
void mayFragCreate::UpdateFragmentGroup(const mdlFragInfo &i_FragInfo,
									 g3dVertexBuffer &io_SharedVertexBuffer,
									 std::vector<g3dFragment*> &io_Fragments,
									 bool i_Morphable)
{
	if (i_FragInfo.m_Vertices.empty())
		return;

	int num_materials = i_FragInfo.m_Materials.size();
	DBG_ASSERT(num_materials>0, "Attempting to create fragment group without material.");
	DBG_ASSERT(i_FragInfo.m_MaterialChanges.size() == num_materials-1, "Incorrect number of material changes.");

	const maVector2d* pUVs = (i_FragInfo.m_UVs.empty()) ? NULL : (&i_FragInfo.m_UVs[0]);
	const maVector3d* pSs = (i_FragInfo.m_Ss.empty()) ? NULL : (&i_FragInfo.m_Ss[0]);
	const maVector3d* pTs = (i_FragInfo.m_Ts.empty()) ? NULL : (&i_FragInfo.m_Ts[0]);

	const bool bCreateBasisVectors = true;
	shared_ptr<tmeshVertexBuffer> shared_vertex_buffer =
		bumpBufferUtil::CreateVertexBuffer( &i_FragInfo.m_Vertices[0],
											&i_FragInfo.m_Normals[0],
											pUVs,
											pSs,
											pTs,
											i_FragInfo.m_Vertices.size(),
											&i_FragInfo.m_Indices[0],
											i_FragInfo.m_Indices.size(),
											i_Morphable,
											i_FragInfo.m_Flags.m_bTriangleSort,
											bCreateBasisVectors);


	DBG_ASSERT(num_materials==io_Fragments.size(), "Incorrect number of fragments in group for updating.");
	std::vector< shared_ptr<tmeshIndexBuffer> > index_buffers(num_materials);
	for (int mi=0; mi<num_materials; mi++)
	{
		// Get indices for just the mi'th material
		int first_index = 0, num_indices = i_FragInfo.m_Indices.size();
		i_FragInfo.GetIndexRange(mi, first_index, num_indices);

		index_buffers[mi] = bumpBufferUtil::CreateIndexBuffer(&i_FragInfo.m_Indices[first_index],
											num_indices,
											num_indices,
											i_FragInfo.m_Vertices.size(),
											i_FragInfo.m_Flags.m_bTriangleSort);
	}

	// Now that we know that all allocations succeeded, we can assign these
	// new buffers into the fragment group. (Exceptions could have been thrown earlier).
	bumpVertexBuffer* pVertexBuffer = dynamic_cast<bumpVertexBuffer*>(&io_SharedVertexBuffer);
	if (pVertexBuffer)
	{
		pVertexBuffer->Update(shared_vertex_buffer);

		for (int mi=0; mi<num_materials; mi++)
		{

			// Update fragment with these new buffers
			bumpTriMeshBumpFrag* pFragment = dynamic_cast<bumpTriMeshBumpFrag*>(io_Fragments[mi]);
			pFragment->UpdateVertexBuffer(pVertexBuffer->GetVertexBuffer(), 
										  pVertexBuffer->GetVertexBuffer_Old());
			pFragment->UpdateIndexBuffer(index_buffers[mi]);
		}
	}

}
