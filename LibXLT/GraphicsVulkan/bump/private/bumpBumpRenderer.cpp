/*****************************************************************************
**  bumpBumpRenderer.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_BUMP_BUMPRENDERER_HPP
#include "GraphicsVulkan/bump/bumpBumpRenderer.hpp"
#endif

#ifndef GVK_BUMP_TRIMESHBUMPFRAG_HPP
#include "GraphicsVulkan/bump/bumpTriMeshBumpFrag.hpp"
#endif
#ifndef GVK_BUMP_VERTEXDECL_HPP
#include "GraphicsVulkan/bump/private/bumpVertexDecl.hpp"
#endif

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif
#ifndef G3D_SCENENODE_HPP
#include "Graphics/g3d/g3dSceneNode.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif
#ifndef GVK_G3DSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dStateMgr.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

namespace gvk
{

//--------------------------------------------------------------------
//--------------------------------------------------------------------
namespace
{
	// D3D11_WHQL_DRAWINDEXED_INDEX_COUNT_2_TO_EXP
	const int c_WhqlDrawIndexedIndexCount2ToExp = 25;

	uint32_t l_DrawCallLimit = 1 << c_WhqlDrawIndexedIndexCount2ToExp;

	// the fragment's PrimitiveType (D3D11 topology numbers) as Vulkan's
	VkPrimitiveTopology to_topology(tmeshFrag::PrimitiveType i_Type)
	{
		switch (i_Type)
		{
			case tmeshFrag::e_LineList:			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			case tmeshFrag::e_TriangleStrip:	return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
			case tmeshFrag::e_TriangleList:		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			default:
				// D3D11 draws nothing with an undefined topology (7 is not one)
				DBG_ASSERT(false, "bumpBumpRenderer: unsupported primitive type " << (int)i_Type);
				return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		}
	}
};

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
bumpBumpRenderer::bumpBumpRenderer()
{
}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
bumpBumpRenderer::~bumpBumpRenderer()
{
}

//------------------------------------------------------------------------
//	Deallocate - called when all device dependent resources should be
//	released.
//------------------------------------------------------------------------
void bumpBumpRenderer::Deallocate()
{
//	bumpVertexDecl::DeInitialize();
}

//------------------------------------------------------------------------
//	Reallocate - called when the device has been Reset and resources can
//	be reloaded again.
//------------------------------------------------------------------------
void bumpBumpRenderer::Reallocate()
{
//	bumpVertexDecl::Initialize();
}

int bumpBumpRenderer::Render( const g3dSceneNode* i_pNode, const matMaterial* i_pMaterial, matShaderEffect* i_pEffect )
{
	const tmeshFrag* pFrag = dynamic_cast<const tmeshFrag*>( i_pNode->GetFragment() );
	DBG_ASSERT( pFrag, "A triangle mesh fragment expected" );

	bool bDoSkinning = false;//pFrag->GetHasSkinning() && i_pEffect->GetHasSkinning();
	bool bTessellate = false;//only tessellate if we have a displacement map

	bool bVelocityMaps = pFrag->GetHasVelocityBuffer();

	const matMaterial* pMtl = pFrag->GetMaterial();	//acquire material from fragment
	if( pMtl )
	{
		bTessellate = pMtl->GetHasDisplacement();	//use original material 
	}

	bTessellate &= i_pEffect->HasHardwareTessellation() && g3dPrefs::CurrentPrefs().m_bUseHardwareTessellation;

	// Set up the context
	vkdContext& context = vkdGlobal::Context();
	context.SetVertexLayout(bumpVertexDecl::GetBumpMeshDeclaration(bDoSkinning, bVelocityMaps));

	// (the strides are the layout's: Vulkan bakes them into the pipeline)
	VkBuffer buffer = VK_NULL_HANDLE;
	VkDeviceSize offset = 0;
	pFrag->GetVertexBuffer()->GetVertexBuffer()->GetBinding(context, buffer, offset);
	context.SetVertexBuffer(0, buffer, offset);
	buffer = VK_NULL_HANDLE;
	offset = 0;
	if (bVelocityMaps)
		pFrag->GetVertexBuffer_Old()->GetVertexBuffer()->GetBinding(context, buffer, offset);
	context.SetVertexBuffer(1, buffer, offset);
	pFrag->GetIndexBuffer()->GetIndexBuffer()->GetBinding(context, buffer, offset);
	context.SetIndexBuffer(buffer, offset,
		(pFrag->GetIndexBuffer()->GetSizeOfIndex() == 2) ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);

	bool bDoubleSided = pFrag->GetDoubleSided();
	
	int numInds = 0;
	if( pFrag->GetPrimitiveType() == tmeshFrag::e_TriangleList )
	{
		numInds = pFrag->GetNumNonShadowIndices();
	}
	else
	{
		numInds = pFrag->GetNumIndices();
	}
	int startIndex = 0;
	if (pFrag->GetSubFragments().size() > 0)
	{
		startIndex = pFrag->GetSubFragments()[i_pNode->GetSubFragment()].m_startIndex;
		numInds = pFrag->GetSubFragments()[i_pNode->GetSubFragment()].m_numTris * 3; // assumes tri list.
	}

	g3dStateMgrVk::SetDeviceStates();

//	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, D3D11_FILL_WIREFRAME );

	// Draw the triangles
	//

	// NOTE THIS MUST MATCH THE ORDER IN THE FX FILE!
	enum Passes {PassDefault=0,PassTess=1,PassSkin=2};
	Passes passType = PassDefault;

	int PassCount = i_pEffect->Begin();
	if( PassCount > 0 )
	{
		if( PassCount > 1 && bTessellate && g3dPrefs::CurrentPrefs().m_bUseHardwareTessellation )
		{
			context.SetTopology( VK_PRIMITIVE_TOPOLOGY_PATCH_LIST, 3 );
			passType = PassTess;
		}
		else
		{
			context.SetTopology( to_topology(pFrag->GetPrimitiveType()) );
		}

		i_pEffect->BeginPass( passType );


		// ability to limit to a certain number of primitives per draw call:

		int drawCallSize = l_DrawCallLimit;// multiple of 3 and of 2 for tris and lines!!
		if (drawCallSize > numInds) 
			drawCallSize = numInds;
		int i = 0;
		// draw in increments of drawCallSize indices.
		// (DX11 loops forever on a fragment with no indices; nothing is drawn here)
		while (drawCallSize > 0 && i <= numInds-drawCallSize)
		{
			context.DrawIndexed(drawCallSize, i+startIndex, 0);
			G3D_PROFILE_DRAWS(1);
			i += drawCallSize;
		}
		// draw any remaining.
		if (i < numInds)
		{
			context.DrawIndexed(numInds-i, i+startIndex, 0);
			G3D_PROFILE_DRAWS(1);
		}

		
		i_pEffect->EndPass();

		// DX11 unbinds the hull and domain shaders after the tessellation
		// pass; vkdContext::SetShaders sets every stage, so the next
		// shaders set replace them
	}
	i_pEffect->End();

	int nPrimitives = 0;
	if( pFrag->GetPrimitiveType() == tmeshFrag::e_TriangleList )
	{
		// tri list
		nPrimitives = numInds / 3;
	}
	else
	{
		// line list
		nPrimitives = numInds / 2;
	}
	return nPrimitives;
}

//--------------------------------------------------------------------
// draw a limited number of triangles per draw call.
// this is to mitigate the windows TDR (timeout detection response)
// for expensive calls (e.g. high tessellation + GS amplification)
// -1 means use the D3D limit.
//--------------------------------------------------------------------
void bumpBumpRenderer::SetDrawLimit(int i_NumIndices)
{
	if (i_NumIndices == -1)
		l_DrawCallLimit = 1 << c_WhqlDrawIndexedIndexCount2ToExp;
	else
		l_DrawCallLimit = i_NumIndices;
}

}
