/*****************************************************************************
**  HairRendererVk.cpp
**
**	GraphicsVulkan port of GraphicsDX11/hair/private/HairRendererDX11.cpp
**	(namespace gvk).
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_HAIR_RENDERERVK_HPP
#include "GraphicsVulkan/hair/HairRendererVk.hpp"
#endif
#ifndef GVK_HAIR_MODELFRAGVK_HPP
#include "GraphicsVulkan/hair/hairModelFragVk.hpp"
#endif
#ifndef GVK_EFF_STRANDHAIR_HPP
#include "GraphicsVulkan/Eff/effStrandHair.hpp"
#endif
#ifndef GVK_FXEFFECTAPI_HPP
#include "GraphicsVulkan/Fx/fxEffectApi.hpp"
#endif
#ifndef GVK_G3D_VKUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkUtil.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef G3D_SCENENODE_HPP
#include "Graphics/g3d/g3dSceneNode.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/G3d/g3dPrefs.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

namespace gvk
{

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
HairRendererVk::HairRendererVk()
{
}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
HairRendererVk::~HairRendererVk()
{
}

//------------------------------------------------------------------------
//	Deallocate - called when all device dependent resources should be
//	released.
//------------------------------------------------------------------------
void HairRendererVk::Deallocate()
{
	DBG_ASSERT(false, "Deallocate not implemented for HairRendererVk");
}

//------------------------------------------------------------------------
//	Reallocate - called when the device has been Reset and resources can
//	be reloaded again.
//------------------------------------------------------------------------
void HairRendererVk::Reallocate()
{
	DBG_ASSERT(false, "Reallocate not implemented for HairRendererVk");
}

//--------------------------------------------------------------------
// Render
//--------------------------------------------------------------------
int HairRendererVk::Render( const g3dSceneNode* i_pNode, const matMaterial* i_pMaterial, matShaderEffect* i_pEffect )
{
	// look at the fragment
	hairModelFragVk* pFrag = dynamic_cast<hairModelFragVk*>( const_cast<g3dFragment*>(i_pNode->GetFragment()) );	//acquire and unconst the fragment, it will be changing
	DBG_ASSERT( pFrag, "A hair fragment expected" );
	if( !pFrag ) return 0;

	// a hair object without strands has no buffers (DX11 can't make it)
	if( !pFrag->GetHairMaterials() || !pFrag->GetHairGeometry() ) return 0;

	// quick kill
	if (g3dVkUtil::CanSkipRender(*i_pMaterial, i_pEffect))
	{
		return 0;
	}

	// look at the effect
	effStrandHair* pEffect = dynamic_cast<effStrandHair*>(i_pEffect);
//	DBG_ASSERT( pEffect, "A hair effect expected" );
	if( !pEffect ) return 0;

	//Set all fragment states for further batch rendering calls
	pFrag->PreBatch();

	bool bTessellate = pFrag->IsHardwareTessellated() && i_pEffect->HasHardwareTessellation() && g3dPrefs::CurrentPrefs().m_bUseHardwareTessellation;
	bool bLines = g3dPrefs::CurrentPrefs().m_bHairLines;

	fxEffect* pFxEffect = pEffect->GetFxEffect();
	pFxEffect->GetVariableByName("g_HairMaterials")->AsShaderResource()->SetResource( pFrag->GetHairMaterials() );
	pFxEffect->GetVariableByName("g_HairGeometry")->AsShaderResource()->SetResource( pFrag->GetHairGeometry() );
	pFxEffect->GetVariableByName("g_nStrandCPs")->AsScalar()->SetInt( pFrag->GetVertsPerStrand() );

	//no vertex geometry, the data is passed in the structured buffers
	vkdContext& context = vkdGlobal::Context();
	context.SetVertexLayout( NULL );
	const VkPrimitiveTopology topology = pFrag->GetPrimitiveType();
	context.SetTopology( topology, topology == VK_PRIMITIVE_TOPOLOGY_PATCH_LIST ? 1 : 0 );

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
		context.Draw( pFrag->GetNumVertices(), 0 );
		G3D_PROFILE_DRAWS(1);
		i_pEffect->EndPass();

		// DX11 unbinds the hull, domain and geometry shaders here;
		// vkdContext::SetShaders sets every stage, so the next shaders set
		// replace them
	}
	i_pEffect->End();

	return pFrag->GetNumHairTriangles();
}

}
