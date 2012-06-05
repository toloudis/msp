/*****************************************************************************
**  bumpBumpRenderer.hpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/bump/bumpBumpRenderer.hpp"

#include "GraphicsDX11/bump/bumpTriMeshBumpFrag.hpp"
#include "GraphicsDX11/bump/private/bumpVertexDecl.hpp"

#include "Core/fs/fsLocator.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dLight.hpp"
#include "Graphics/g3d/g3dLightMgr.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matMaterial.hpp"
//#include "Graphics/mat/matTextureMgr.hpp"
#include "GraphicsDX11/Eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
//#include "GraphicsDX11/g3d/g3dFVFWin.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/g3d/g3dStateMgr.hpp"
#include "GraphicsDX11/mat/matPlainTexture.hpp"
//#include "Graphics/g3d/g3dSceneNode.hpp"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
namespace
{
	UINT l_DrawCallLimit = 1 << D3D11_WHQL_DRAWINDEXED_INDEX_COUNT_2_TO_EXP;
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

	// Set material blending mode
	bool last_additive_mode = g3dSceneGlobal::g_AdditiveMode;

	bool bDoSkinning = false;//pFrag->GetHasSkinning() && i_pEffect->GetHasSkinning();
	bool bTessellate = false;//only tessellate if we have a displacement map

	bool bVelocityMaps = pFrag->GetHasVelocityBuffer();

	const matMaterial* pMtl = pFrag->GetMaterial();	//acquire material from fragment
	if( pMtl )
	{
		bTessellate = pMtl->GetHasDisplacement();	//use original material 
	}

	bTessellate &= i_pEffect->HasHardwareTessellation() && g3dPrefs::CurrentPrefs().m_bUseHardwareTessellation;

	// Set up D3D device
	g2dDX11Global::g_pDeviceContext->IASetInputLayout(bumpVertexDecl::GetBumpMeshDeclaration(bDoSkinning, bVelocityMaps));

	ID3D11Buffer* buf[2] = {
		pFrag->GetVertexBuffer()->GetVertexBuffer(), 
		//bDoSkinning ? pFrag->GetSkinningBuffer()->GetVertexBuffer() : NULL
		bVelocityMaps ? pFrag->GetVertexBuffer_Old()->GetVertexBuffer() : NULL
	};
	UINT strides[2] = {
		pFrag->GetVertexStride(), 
		//bDoSkinning ? sizeof(g3dType::SkinVertex) : 0
		bVelocityMaps ? sizeof(g3dType::NonTexVertex) : 0
	};
	UINT offsets[2] = {0,0};

	g2dDX11Global::g_pDeviceContext->IASetVertexBuffers(0, 2, buf, strides, offsets);
	g2dDX11Global::g_pDeviceContext->IASetIndexBuffer(pFrag->GetIndexBuffer()->GetIndexBuffer(), 
		(pFrag->GetIndexBuffer()->GetSizeOfIndex() == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);

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

	g3dStateMgrDX11::SetDeviceStates();

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
			g2dDX11Global::g_pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST );
			passType = PassTess;
		}
		else
		{
			g2dDX11Global::g_pDeviceContext->IASetPrimitiveTopology( (D3D11_PRIMITIVE_TOPOLOGY)pFrag->GetPrimitiveType());
		}

		i_pEffect->BeginPass( passType );


		// ability to limit to a certain number of primitives per draw call:

		int drawCallSize = l_DrawCallLimit;// multiple of 3 and of 2 for tris and lines!!
		if (drawCallSize > numInds) 
			drawCallSize = numInds;
		int i = 0;
		// draw in increments of drawCallSize indices.
		while (i <= numInds-drawCallSize)
		{
			g2dDX11Global::g_pDeviceContext->DrawIndexed(drawCallSize, i+startIndex, 0);
			i += drawCallSize;
		}
		// draw any remaining.
		if (i < numInds)
		{
			g2dDX11Global::g_pDeviceContext->DrawIndexed(numInds-i, i+startIndex, 0);
		}

		
		i_pEffect->EndPass();

		if( passType == PassTess )
		{
			g2dDX11Global::g_pDeviceContext->HSSetShader( NULL, NULL, 0 );
			g2dDX11Global::g_pDeviceContext->DSSetShader( NULL, NULL, 0 );
		}
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
		l_DrawCallLimit = 1 << D3D11_WHQL_DRAWINDEXED_INDEX_COUNT_2_TO_EXP;
	else
		l_DrawCallLimit = i_NumIndices;
}
