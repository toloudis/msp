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
//	A partner that isn't a shdw renderer is left alone: the creator still
//	answers the types not ported yet with the forward renderer, which has
//	no frame buffer (DX11 asserts here; all its types are shdw renderers).
//--------------------------------------------------------------------
void shdwSceneRendererVk::ShareBuffers(g3dSceneRenderer* i_Partner)
{
	shdwSceneRendererVk* partner = dynamic_cast<shdwSceneRendererVk*>(i_Partner);
	if (partner)
		partner->SetFrameBuffer(this->m_pFrameBuffer);
}

}
