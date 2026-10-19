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
#ifndef GVK_MAY_FRAGCREATE_HPP
#include "GraphicsVulkan/may/mayFragCreate.hpp"
#endif

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef MDL_FRAGINFO_HPP
#include "Graphics/mdl/mdlFragInfo.hpp"
#endif
#ifndef MDL_HAIRINFO_HPP
#include "Graphics/mdl/mdlHairInfo.hpp"
#endif
#ifndef MDL_SPLITFRAGINFO_HPP
#include "Graphics/mdl/mdlSplitFragInfo.hpp"
#endif
#ifndef GVK_BUMP_BUFFERUTIL_HPP
#include "GraphicsVulkan/bump/bumpBufferUtil.hpp"
#endif
#ifndef GVK_BUMP_TRIMESHBUMPFRAG_HPP
#include "GraphicsVulkan/bump/bumpTriMeshBumpFrag.hpp"
#endif
#ifndef GVK_BUMP_VERTEXBUFFER_HPP
#include "GraphicsVulkan/bump/bumpVertexBuffer.hpp"
#endif
#ifndef GVK_G3DSTUBSVK_HPP
#include "GraphicsVulkan/stub/g3dStubsVk.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif

namespace gvk
{

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
				const std::vector<uint32_t>& i_Remap)
	{
		if (io_Verts.empty()) return;

		std::vector<T> temp = io_Verts;

		const T *pTemp = &temp[0];
		const uint32_t *pRemap = &i_Remap[0];

		uint32_t new_ind;
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
	// hairModelFrag comes with the hair port (Plan 7): an empty stand-in
	GVK_NOT_YET("mayFragCreate::CreateHairFragment (hair is Plan 7)");
	return new hairModelFragVk( NULL, NULL, NULL, 0, NULL, 0, pMaterial, false );
}

//------------------------------------------------------------------------
// Alter the triangle and vertex ordering to best utilize the
//	vertex cache in the GPU
//------------------------------------------------------------------------
void mayFragCreate::OptimizeFragment(mdlSplitFragInfo &io_Info)
{
	// DX11 returns here: its reordering with D3DXOptimizeFaces and
	// D3DXOptimizeVertices is compiled out (#if 0), so it isn't ported
	return;
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

	std::unique_ptr<bumpVertexBuffer> pBumpBuffer( new bumpVertexBuffer(shared_vertex_buffer) );

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
		std::unique_ptr<tmeshFrag> pFragment( new bumpTriMeshBumpFrag(	shared_vertex_buffer,
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

}
