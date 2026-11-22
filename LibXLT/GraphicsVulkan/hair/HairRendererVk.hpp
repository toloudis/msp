/*****************************************************************************
**  HairRendererVk.hpp
**
**	GraphicsVulkan port of GraphicsDX11/hair/HairRendererDX11.hpp (namespace
**	gvk). Draws a hair fragment (hairModelFragVk) with its effect
**	(effStrandHair): no vertex input, the strands read from the fragment's
**	structured buffers, one vertex per control point.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_HAIR_RENDERERVK_HPP
#error HairRendererVk.hpp multiply included
#endif
#define GVK_HAIR_RENDERERVK_HPP

#ifndef G3D_BASERENDERER_HPP
#include "Graphics/g3d/g3dBaseRenderer.hpp"
#endif

namespace gvk
{

//============================================================================
//============================================================================
class HairRendererVk : public g3dBaseRenderer
{
	public:
		//--------------------------------------------------------------------
		// Constructor
		//--------------------------------------------------------------------
		HairRendererVk();

		//--------------------------------------------------------------------
		// Destructor
		//--------------------------------------------------------------------
		virtual ~HairRendererVk();

		//------------------------------------------------------------------------
		//	Deallocate - called when all device dependent resources should be
		//	released.
		//------------------------------------------------------------------------
		virtual void Deallocate();

		//------------------------------------------------------------------------
		//	Reallocate - called when the device has been Reset and resources can
		//	be reloaded again.
		//------------------------------------------------------------------------
		virtual void Reallocate();

		//--------------------------------------------------------------------
		// Render - returns how many triangles rendered
		//--------------------------------------------------------------------
		virtual int Render( const g3dSceneNode* i_pNode, const matMaterial* i_pMaterial, matShaderEffect* i_pEffect );
};

}
