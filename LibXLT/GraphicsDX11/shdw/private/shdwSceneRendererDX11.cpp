/****************************************************************************\
**	shdwSceneRendererDX11.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/shdwSceneRendererDX11.hpp"

#include "GraphicsDX11/shdw/shdwFrameBufferMgr.hpp"

#include "Core/dbg/dbgMsg.hpp"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwSceneRendererDX11::shdwSceneRendererDX11()
{
	// create local framebuffer
	m_pFrameBuffer.reset(new shdwFrameBufferMgr);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwSceneRendererDX11::~shdwSceneRendererDX11()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwSceneRendererDX11::SetFrameBuffer(shared_ptr<shdwFrameBufferMgr> i_FrameBuffer)
{
	m_pFrameBuffer = i_FrameBuffer;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shared_ptr<shdwFrameBufferMgr> shdwSceneRendererDX11::GetFrameBuffer()
{
	return m_pFrameBuffer;
}

//--------------------------------------------------------------------
//	Tell this renderer to share resources with another.
//	The partner's resources will be replaced.
//--------------------------------------------------------------------
void shdwSceneRendererDX11::ShareBuffers(g3dSceneRenderer* i_Partner)
{
	shdwSceneRendererDX11* partner = dynamic_cast<shdwSceneRendererDX11*>(i_Partner);
	DBG_ASSERT(partner, "ShareBuffers: Bad renderer class type!");

	partner->SetFrameBuffer(this->m_pFrameBuffer);
}
