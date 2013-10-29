/****************************************************************************\
**  g2dResourceCounterGL.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Area18/g2d/g2dResourceCounterGL.hpp"

#include "Core/Dbg/dbgMsg.hpp"
#include "Area18/ogl/oglBufferUtil.hpp"
#include "Area18/mat/matTextureMgrGL.hpp"

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
float g2dResourceCounterGL::GetTotalVideoMemory()
{
	return oglBufferUtil::GetTotalVertexBufferMemory() +
		oglBufferUtil::GetTotalIndexBufferMemory() +
		l_TotalWindowMemory + 
		l_TotalRenderTargetMemory +
		l_TotalShadowBufferMemory + 
		+ l_TotalTextureMemory;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float g2dResourceCounterGL::GetTotalSceneTextureMemory()
{
	return l_TotalTextureMemory;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int g2dResourceCounterGL::GetNumSceneTextures()
{
	return l_NumTextures;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float g2dResourceCounterGL::GetTotalFramebufferMemory()
{
	return l_TotalWindowMemory
		+ l_TotalRenderTargetMemory;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int g2dResourceCounterGL::GetNumFramebufferTextures()
{
	return l_NumWindows
		+ l_NumRenderTargets;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float g2dResourceCounterGL::GetTotalShadowMapMemory()
{
	return l_TotalShadowBufferMemory;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int g2dResourceCounterGL::GetNumShadowMapTextures()
{
	return matTextureMgrGL::GetNumShadowMaps();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float g2dResourceCounterGL::GetTotalVertexBufferMemory()
{
	return oglBufferUtil::GetTotalVertexBufferMemory();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int g2dResourceCounterGL::GetNumVertexBuffers()
{
	return oglBufferUtil::GetNumVertexBuffers();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float g2dResourceCounterGL::GetTotalIndexBufferMemory()
{
	return oglBufferUtil::GetTotalIndexBufferMemory();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int g2dResourceCounterGL::GetNumIndexBuffers()
{
	return oglBufferUtil::GetNumIndexBuffers();
}

//--------------------------------------------------------------------
// add a resource not managed by matTextureMgr
//--------------------------------------------------------------------
void g2dResourceCounterGL::AddResource(float i_SizeKBytes, eResourceCategory i_Category)
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
void g2dResourceCounterGL::RemoveResource(float i_SizeKBytes, eResourceCategory i_Category)
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
