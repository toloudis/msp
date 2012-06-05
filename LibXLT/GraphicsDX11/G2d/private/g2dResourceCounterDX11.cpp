/****************************************************************************\
**  g2dResourceCounterDX11.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/g2d/g2dResourceCounterDX11.hpp"

#include "Core/Dbg/dbgMsg.hpp"
#include "GraphicsDX11/g3d/g3dDX11BufferUtil.hpp"
#include "GraphicsDX11/mat/matTextureMgrDX11.hpp"

namespace
{
	float l_TotalWindowMemory = 0;
	unsigned int l_NumWindows = 0;

	float l_TotalRenderTargetMemory = 0;
	unsigned int l_NumRenderTargets = 0;

	float l_TotalShadowBufferMemory = 0;
	unsigned int l_NumShadowBuffers = 0;

	float l_TotalTextureMemory = 0;
	unsigned int l_NumTextures = 0;
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float g2dResourceCounterDX11::GetTotalVideoMemory()
{
	return g3dDX11BufferUtil::GetTotalVertexBufferMemory() +
		g3dDX11BufferUtil::GetTotalIndexBufferMemory() +
		l_TotalWindowMemory + 
		l_TotalRenderTargetMemory +
		l_TotalShadowBufferMemory + 
		+ l_TotalTextureMemory;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float g2dResourceCounterDX11::GetTotalSceneTextureMemory()
{
	return l_TotalTextureMemory;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int g2dResourceCounterDX11::GetNumSceneTextures()
{
	return l_NumTextures;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float g2dResourceCounterDX11::GetTotalFramebufferMemory()
{
	return l_TotalWindowMemory
		+ l_TotalRenderTargetMemory;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int g2dResourceCounterDX11::GetNumFramebufferTextures()
{
	return l_NumWindows
		+ l_NumRenderTargets;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float g2dResourceCounterDX11::GetTotalShadowMapMemory()
{
	return l_TotalShadowBufferMemory;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int g2dResourceCounterDX11::GetNumShadowMapTextures()
{
	return matTextureMgrDX11::GetNumShadowMaps();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float g2dResourceCounterDX11::GetTotalVertexBufferMemory()
{
	return g3dDX11BufferUtil::GetTotalVertexBufferMemory();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int g2dResourceCounterDX11::GetNumVertexBuffers()
{
	return g3dDX11BufferUtil::GetNumVertexBuffers();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float g2dResourceCounterDX11::GetTotalIndexBufferMemory()
{
	return g3dDX11BufferUtil::GetTotalIndexBufferMemory();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int g2dResourceCounterDX11::GetNumIndexBuffers()
{
	return g3dDX11BufferUtil::GetNumIndexBuffers();
}

//--------------------------------------------------------------------
// add a resource not managed by matTextureMgr
//--------------------------------------------------------------------
void g2dResourceCounterDX11::AddResource(float i_SizeKBytes, eResourceCategory i_Category)
{
	switch(i_Category)
	{
	case eTexture:
		l_TotalTextureMemory += i_SizeKBytes;
		l_NumTextures++;
		break;
	case eShadow:
		l_TotalShadowBufferMemory += i_SizeKBytes;
		l_NumShadowBuffers++;
		break;
	case eRenderTarget:
		l_TotalRenderTargetMemory += i_SizeKBytes;
		l_NumRenderTargets++;
		break;
	case eWindow:
		l_TotalWindowMemory += i_SizeKBytes;
		l_NumWindows++;
		break;
	}
}
void g2dResourceCounterDX11::RemoveResource(float i_SizeKBytes, eResourceCategory i_Category)
{
	switch(i_Category)
	{
	case eTexture:
		l_TotalTextureMemory -= i_SizeKBytes;
		l_NumTextures--;
		break;
	case eShadow:
		l_TotalShadowBufferMemory -= i_SizeKBytes;
		l_NumShadowBuffers--;
		break;
	case eRenderTarget:
		l_TotalRenderTargetMemory -= i_SizeKBytes;
		l_NumRenderTargets--;
		break;
	case eWindow:
		l_TotalWindowMemory -= i_SizeKBytes;
		l_NumWindows--;
		break;
	}
}
