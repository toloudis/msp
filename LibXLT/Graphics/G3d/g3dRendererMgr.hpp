/*****************************************************************************
**	g3dRendererMgr.hpp
**
**		g3dRendererMgr controls all the renderers
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_RENDERERMGR_HPP
#error g3dRendererMgr.hpp multiply included
#endif
#define G3D_RENDERERMGR_HPP


//============================================================================
//	Forward References
//============================================================================
class g3dBaseRenderer;
class g3dSceneNode;
class matMaterial;
class matShaderEffect;


//============================================================================
//============================================================================
namespace g3dRendererMgr
{
	//------------------------------------------------------------------------
	//	Initialize - must be called before using any other function
	//------------------------------------------------------------------------
	void Initialize();

	//------------------------------------------------------------------------
	//	DeInitialize - cleans up the renderers
	//------------------------------------------------------------------------
	void DeInitialize();

	//------------------------------------------------------------------------
	//	AddRenderer - adds a renderer.  Returns the render mode associated
	//	with the renderer.  Assumes ownership of the renderer.
	//------------------------------------------------------------------------
	int AddRenderer( g3dBaseRenderer* i_pRenderer );

	//--------------------------------------------------------------------
	//  Render - calls the appropriate renderer for the scene node
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	int Render( const g3dSceneNode* i_pNode, const matMaterial* i_pMaterial, matShaderEffect* i_pShader );
}

