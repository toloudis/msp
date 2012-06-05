/****************************************************************************\
**	shdwTestShaderRendererD3D.hpp
**
**	The shdwTestShaderRendererD3D is a rendering algorithm that uses a floating 
**  point frame buffer and tone mapping to allow high dynamic range lighting.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef SHDW_TESTSHADERRENDERERD3D_HPP
#error shdwTestShaderRendererD3D.hpp multiply included
#endif
#define SHDW_TESTSHADERRENDERERD3D_HPP

#ifndef DEM_G3DTESTRENDERER_HPP
#include "demG3dTestRenderer.hpp"
#endif

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX9/g3d/g3dRenderPass.hpp"
#endif

#ifndef G3D_RENDERSCREENSPACE_HPP
#include "GraphicsDX9/g3d/g3dRenderScreenSpace.hpp"
#endif

#ifndef G3D_RENDERCAMERASPACE_HPP
#include "GraphicsDX9/g3d/g3dRenderCameraSpace.hpp"
#endif

#ifndef SHDW_PASSTRAVERSAL_HPP
#include "GraphicsDX9/shdw/shdwPassTraversal.hpp"
#endif

#include <vector>

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class g3dLayer;
class camCamera;
class matPlainTexture;
class matRenderTargetTexture;
class shdwTestShaderRendererD3D : public demG3dTestRenderer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwTestShaderRendererD3D();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~shdwTestShaderRendererD3D();

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	int Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
		const g3dScene &i_Scene, float i_fSimTime );

	void ReleaseResources();

	void HandleChar(itString::CharType ch);

protected:
	// lazy evaluate whether to recreate temp surfaces
	// based on whether width/height of window have changed.
	int m_Width, m_Height;
	int m_dwCropWidth, m_dwCropHeight;

	g2dRenderTarget* m_pWindow;

	shdwPassTraversal m_SceneDatabase;

	void CreateSurfaces(g2dRenderTarget* i_pWindow);

	HRESULT ClearTexture( LPDIRECT3DTEXTURE9 pTexture );

	void InitMaterials();
	ID3DXEffect* ShaderSetup(int i);

};

