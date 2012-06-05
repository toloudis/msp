/*****************************************************************************
**  bumpBumpRenderer.hpp
**
**      bumpBumpRenderer renders tri mesh fragments with extra data
**	for texture space at each vertex in order to render per-pixel effects
**  like bump mapping.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef BUMP_BUMPRENDERER_HPP
#error bumpBumpRenderer.hpp multiply included
#endif
#define BUMP_BUMPRENDERER_HPP

#ifndef G3D_BASERENDERER_HPP
#include "Graphics/g3d/g3dBaseRenderer.hpp"
#endif

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class maMatrix4x4;
class matMaterial;
class matShaderEffect;
class tmeshFrag;

class bumpBumpRenderer : public g3dBaseRenderer
{
	public:

		//--------------------------------------------------------------------
		// Constructor
		//--------------------------------------------------------------------
		bumpBumpRenderer();

		//--------------------------------------------------------------------
		// Destructor
		//--------------------------------------------------------------------
		virtual ~bumpBumpRenderer();

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
		// Render
		//--------------------------------------------------------------------
		virtual int Render( const g3dSceneNode* i_pNode, const matMaterial* i_pMaterial, matShaderEffect* i_pEffect );

		//--------------------------------------------------------------------
		// draw a limited number of triangles per draw call.  
		// this is to mitigate the windows TDR (timeout detection response)
		// for expensive calls (e.g. high tessellation + GS amplification)
		// i_NumIndices should be a multiple of 2,3 and 4 ideally.
		// -1 means use the D3D limit.
		//--------------------------------------------------------------------
		static void SetDrawLimit(int i_NumIndices);
	private:
};
