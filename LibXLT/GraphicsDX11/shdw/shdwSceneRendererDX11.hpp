/****************************************************************************\
**	shdwSceneRendererDX11.hpp
**
**	intermediate base class to handle common code among all dx11 renderers
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef SHDW_SCENERENDERERDX11_HPP
#error shdwSceneRendererDX11.hpp multiply included
#endif
#define SHDW_SCENERENDERERDX11_HPP

#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/g3d/g3dSceneRenderer.hpp" 
#endif

#ifndef ENV_BOOST_HPP
#include "Core/env/envBoost.hpp"
#endif

#include <vector>

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class shdwFrameBufferMgr;

//--------------------------------------------------------------------
// intermediate base class to handle common code among all dx11 renderers
//--------------------------------------------------------------------
class shdwSceneRendererDX11 : public g3dSceneRenderer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwSceneRendererDX11();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~shdwSceneRendererDX11();

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


