/****************************************************************************\
**	shdwSceneRendererVk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_SCENERENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwSceneRendererVk.hpp"
#endif

#ifndef GVK_SHDW_FRAMEBUFFERMGR_HPP
#include "GraphicsVulkan/shdw/shdwFrameBufferMgr.hpp"
#endif

#ifndef GVK_G3D_SCENERENDERERVK_HPP
#include "GraphicsVulkan/G3d/g3dSceneRendererVk.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

namespace gvk
{

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwSceneRendererVk::shdwSceneRendererVk()
{
	// create local framebuffer
	m_pFrameBuffer.reset(new shdwFrameBufferMgr);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwSceneRendererVk::~shdwSceneRendererVk()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwSceneRendererVk::SetFrameBuffer(shared_ptr<shdwFrameBufferMgr> i_FrameBuffer)
{
	m_pFrameBuffer = i_FrameBuffer;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shared_ptr<shdwFrameBufferMgr> shdwSceneRendererVk::GetFrameBuffer()
{
	return m_pFrameBuffer;
}

//--------------------------------------------------------------------
//	Tell this renderer to share resources with another.
//	The partner's resources will be replaced.
//	The forward renderer standing in for the opacity map renderer (Plan 7)
//	has no frame buffer and is left alone; on DX11 that is a shdw renderer.
//--------------------------------------------------------------------
void shdwSceneRendererVk::ShareBuffers(g3dSceneRenderer* i_Partner)
{
	if (dynamic_cast<g3dSceneRendererVk*>(i_Partner))
		return;

	shdwSceneRendererVk* partner = dynamic_cast<shdwSceneRendererVk*>(i_Partner);
	DBG_ASSERT(partner, "ShareBuffers: Bad renderer class type!");

	if (partner)
		partner->SetFrameBuffer(this->m_pFrameBuffer);
}

}
