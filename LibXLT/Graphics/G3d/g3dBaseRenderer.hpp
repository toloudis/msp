/*****************************************************************************
**	g3dBaseRenderer.hpp
**
**		g3dBaseRenderer is the base class for all renderers
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_BASERENDERER_HPP
#error g3dBaseRenderer.hpp multiply included
#endif
#define G3D_BASERENDERER_HPP


//============================================================================
//	Forward References
//============================================================================
class g3dSceneNode;
class matMaterial;
class matShaderEffect;


//============================================================================
//============================================================================
class g3dBaseRenderer
{
	public:
		//--------------------------------------------------------------------
		// Constructor
		//--------------------------------------------------------------------
		g3dBaseRenderer(){};

		//--------------------------------------------------------------------
		// Destructor
		//--------------------------------------------------------------------
		virtual ~g3dBaseRenderer() = 0 {};

		//------------------------------------------------------------------------
		//	Deallocate - called when all device dependent resources should be
		//	released.
		//------------------------------------------------------------------------
		virtual void Deallocate() = 0;

		//------------------------------------------------------------------------
		//	Reallocate - called when the device has been Reset and resources can
		//	be reloaded again.
		//------------------------------------------------------------------------
		virtual void Reallocate() = 0;

		//--------------------------------------------------------------------
		// Render - returns how many triangles rendered
		//--------------------------------------------------------------------
		virtual int Render( const g3dSceneNode* i_pNode, const matMaterial* i_pMaterial, matShaderEffect* i_pEffect ) = 0;

};

