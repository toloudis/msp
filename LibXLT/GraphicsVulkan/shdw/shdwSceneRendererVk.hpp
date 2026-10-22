/****************************************************************************\
**	shdwSceneRendererVk.hpp
**
**	intermediate base class to handle common code among all dx11 renderers
**	(GraphicsVulkan port of GraphicsDX11's shdwSceneRendererVk)
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_SHDW_SCENERENDERERVK_HPP
#error shdwSceneRendererVk.hpp multiply included
#endif
#define GVK_SHDW_SCENERENDERERVK_HPP

#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/g3d/g3dSceneRenderer.hpp" 
#endif

#ifndef ENV_BOOST_HPP
#include "Core/env/envBoost.hpp"
#endif

#include <vector>

namespace gvk
{

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class shdwFrameBufferMgr;

//--------------------------------------------------------------------
// intermediate base class to handle common code among all dx11 renderers
//--------------------------------------------------------------------
class shdwSceneRendererVk : public g3dSceneRenderer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwSceneRendererVk();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~shdwSceneRendererVk();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetFrameBuffer(shared_ptr<shdwFrameBufferMgr> i_FrameBuffer);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shared_ptr<shdwFrameBufferMgr> GetFrameBuffer();

	//--------------------------------------------------------------------
	//	Tell this renderer to share resources with another.
	//	The partner's resources will be replaced. 
	//--------------------------------------------------------------------
	virtual void ShareBuffers(g3dSceneRenderer* i_Partner);

protected:

	shared_ptr<shdwFrameBufferMgr> m_pFrameBuffer;
};

}
