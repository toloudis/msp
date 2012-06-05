/*****************************************************************************
**  HairRendererDX11.hpp
**
**      
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/hair/HairRendererDX11.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/eff/effParticleData.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g3d/g3dDX11BufferUtil.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dHelpersWin.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matUVATexture.hpp"
#include "GraphicsDX11/hair/HairModelFrag.hpp"
#include "GraphicsDX11/Eff/effStrandHair.hpp"
#include "GraphicsDX11/G3d/g3dDX11TextureUtil.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "Graphics/G3d/g3dPrefs.hpp"

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
HairRendererDX11::HairRendererDX11()
{
}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
HairRendererDX11::~HairRendererDX11()
{
}

//------------------------------------------------------------------------
//	Deallocate - called when all device dependent resources should be
//	released.
//------------------------------------------------------------------------
void HairRendererDX11::Deallocate()
{
	DBG_ASSERT(false, "Deallocate not implemented for HairRendererDX11");
}

//------------------------------------------------------------------------
//	Reallocate - called when the device has been Reset and resources can
//	be reloaded again.
//------------------------------------------------------------------------
void HairRendererDX11::Reallocate()
{
	DBG_ASSERT(false, "Reallocate not implemented for HairRendererDX11");
}

//--------------------------------------------------------------------
// Render
//--------------------------------------------------------------------
int HairRendererDX11::Render( const g3dSceneNode* i_pNode, const matMaterial* i_pMaterial, matShaderEffect* i_pEffect )
{
	int num_tris = 0;

	// look at the fragment
	hairModelFrag* pFrag = dynamic_cast<hairModelFrag*>( const_cast<g3dFragment*>(i_pNode->GetFragment()) );	//acquire and unconst the fragment, it will be changing
	DBG_ASSERT( pFrag, "A hair fragment expected" );
	if( !pFrag ) return 0;

	// quick kill
	if (g3dDX11Util::CanSkipRender(*i_pMaterial, i_pEffect))
	{
		return 0;
	}

	// look at the effect
	effStrandHair* pEffect = dynamic_cast<effStrandHair*>(i_pEffect);
//	DBG_ASSERT( pEffect, "A hair effect expected" );
	if( !pEffect ) return 0;

	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"HairRendererDX11::Render" );

	//Set all fragment states for further batch rendering calls
	pFrag->PreBatch();

	bool bTessellate = pFrag->IsHardwareTessellated() && i_pEffect->HasHardwareTessellation() && g3dPrefs::CurrentPrefs().m_bUseHardwareTessellation;
	bool bLines = g3dPrefs::CurrentPrefs().m_bHairLines;

	ID3DX11Effect* pD3DEffect = pEffect->GetD3DXEffect();
	pD3DEffect->GetVariableByName("g_HairMaterials")->AsShaderResource()->SetResource( pFrag->GetHairMaterialsView() );
	pD3DEffect->GetVariableByName("g_HairGeometry")->AsShaderResource()->SetResource( pFrag->GetHairGeometryView() );
	pD3DEffect->GetVariableByName("g_nStrandCPs")->AsScalar()->SetInt( pFrag->GetVertsPerStrand() );

	//no vertex geometry, the data is passed in the structured buffers
	g2dDX11Global::g_pDeviceContext->IASetInputLayout( NULL );
	g2dDX11Global::g_pDeviceContext->IASetPrimitiveTopology( (D3D11_PRIMITIVE_TOPOLOGY)pFrag->GetPrimitiveType());

	// NOTE THIS MUST MATCH THE ORDER IN THE FX FILE!
	enum Passes {PassDefault=0,PassTess=1,PassLines=2,PassLinesTess=3};
	Passes passType = PassDefault;

	int PassCount = i_pEffect->Begin();
	if( PassCount > 0 )
	{
		if( PassCount > 1 && bTessellate ) passType = PassTess;
		if( PassCount > 2 && bLines ) passType = PassLines; 
		if( PassCount > 3 && bLines && bTessellate ) passType = PassLinesTess; 

		i_pEffect->BeginPass( passType );
		g2dDX11Global::g_pDeviceContext->Draw( pFrag->GetNumVertices(), 0);
		i_pEffect->EndPass();

		g2dDX11Global::g_pDeviceContext->HSSetShader( NULL, NULL, 0 );
		g2dDX11Global::g_pDeviceContext->DSSetShader( NULL, NULL, 0 );
		g2dDX11Global::g_pDeviceContext->GSSetShader( NULL, NULL, 0 );
	}
	i_pEffect->End();

	D3DPERF_EndEvent();
	return pFrag->GetNumHairTriangles();
}
