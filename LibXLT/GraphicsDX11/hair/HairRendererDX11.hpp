/*****************************************************************************
**  HairRendererDX11.hpp
**
**      HairRendererDX11 renders particles
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef HAIR_RENDERERDX11_HPP
#error HairRendererDX11.hpp multiply included
#endif
#define HAIR_RENDERERDX11_HPP

#ifndef G3D_BASERENDERER_HPP
#include "Graphics/g3d/g3dBaseRenderer.hpp"
#endif

#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif


//============================================================================
//============================================================================
class HairRendererDX11 : public g3dBaseRenderer
{
	public:
		//--------------------------------------------------------------------
		// Constructor
		//--------------------------------------------------------------------
		HairRendererDX11();

		//--------------------------------------------------------------------
		// Destructor
		//--------------------------------------------------------------------
		virtual ~HairRendererDX11();

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
