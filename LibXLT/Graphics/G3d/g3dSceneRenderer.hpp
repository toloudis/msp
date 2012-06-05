/****************************************************************************\
**	g3dSceneRenderer.hpp
**
**	The g3dSceneRenderer renders the g3dSceneNodes which describe the
**	graphical scene.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_SCENERENDERER_HPP
#error g3dSceneRenderer.hpp multiply included
#endif
#define G3D_SCENERENDERER_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/g2d/g2dPFD.hpp"
#endif

#include <vector>


//============================================================================
//	Forward References
//============================================================================
class g2dRenderTarget;
class g2dWindow;
class g3dPickInfo;
class g3dLayer;
class g3dScene;
class camCamera;


//============================================================================
//============================================================================
class g3dSceneRenderer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	g3dSceneRenderer() {};

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~g3dSceneRenderer() {};

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy plus additional 
	//	viewer specific layers.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	virtual int Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
						 const g3dScene &i_Scene,
						 const std::vector<g3dLayer*>& i_ViewerLayers,
						 float i_fSimTime ) = 0;

	//--------------------------------------------------------------------
	//	Let a renderer free up any memory it is holding on to 
	//--------------------------------------------------------------------
	virtual void ReleaseResources() = 0;

	//--------------------------------------------------------------------
	//	Tell this renderer to share resources with another.
	//	The partner's resources will be replaced. 
	//--------------------------------------------------------------------
	virtual void ShareBuffers(g3dSceneRenderer* i_Partner) {}

	//--------------------------------------------------------------------
	//	A renderer can be enabled or disabled from having Render() called.
	//--------------------------------------------------------------------
	virtual bool IsEnabled() {return true;}

	//--------------------------------------------------------------------
	//	Sometimes we need to access the pre-buffer pixels that don't get 
	//	drawn to a window.
	//--------------------------------------------------------------------
	virtual g2dRenderTarget* GetRawBuffer() {return NULL;}

	//--------------------------------------------------------------------
	//	Sometimes we need to access the buffer's pfd type
	//--------------------------------------------------------------------
	virtual g2dPFD::PixelFormat GetRawBufferPFD() {return g2dPFD::e_Unknown;}
};


//============================================================================
//============================================================================
class g3dPickRenderer : public g3dSceneRenderer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	g3dPickRenderer() : m_PickMask(0) {}

	//--------------------------------------------------------------------
	//	set a 1x1 viewport before clearing.
	//--------------------------------------------------------------------
	virtual void SetPickViewport(g2dRenderTarget* i_pTarget) = 0;

	//--------------------------------------------------------------------
	//	restore viewport
	//--------------------------------------------------------------------
	virtual void RestoreViewport() = 0;

	//--------------------------------------------------------------------
	//	return info about the picked object.
	//--------------------------------------------------------------------
	virtual void GetPickInfo(g2dRenderTarget* i_pWindow, g3dPickInfo& o_PickInfo) = 0;

	//----------------------------------------------------------------------------
	// PickMask is a user defined bit mask that can be used to filter
	// the pickable objects. The default value of "0" means "do not filter".
	//----------------------------------------------------------------------------
	envType::UInt32 GetPickMask() const 
	{ return m_PickMask; }
	void SetPickMask(envType::UInt32 i_PickMask)
	{ m_PickMask = i_PickMask; }

private:
	envType::UInt32 m_PickMask;
};

