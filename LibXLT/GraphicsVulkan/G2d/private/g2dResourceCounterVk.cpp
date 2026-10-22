/****************************************************************************\
**  g2dResourceCounterVk.cpp
**
**	See g2dResourceCounterVk.hpp.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_G2DRESOURCECOUNTERVK_HPP
#include "GraphicsVulkan/G2d/g2dResourceCounterVk.hpp"
#endif
#ifndef GVK_G3DVKBUFFERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkBufferUtil.hpp"
#endif
#ifndef GVK_MATTEXTUREMGRVK_HPP
#include "GraphicsVulkan/Mat/matTextureMgrVk.hpp"
#endif

namespace gvk
{
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
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	float g2dResourceCounterVk::GetTotalVideoMemory()
	{
		return g3dVkBufferUtil::GetTotalVertexBufferMemory() +
			g3dVkBufferUtil::GetTotalIndexBufferMemory() +
			l_TotalWindowMemory +
			l_TotalRenderTargetMemory +
			l_TotalShadowBufferMemory +
			+ l_TotalTextureMemory;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	float g2dResourceCounterVk::GetTotalSceneTextureMemory()
	{
		return l_TotalTextureMemory;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	int g2dResourceCounterVk::GetNumSceneTextures()
	{
		return l_NumTextures;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	float g2dResourceCounterVk::GetTotalFramebufferMemory()
	{
		return l_TotalWindowMemory
			+ l_TotalRenderTargetMemory;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	int g2dResourceCounterVk::GetNumFramebufferTextures()
	{
		return l_NumWindows
			+ l_NumRenderTargets;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	float g2dResourceCounterVk::GetTotalShadowMapMemory()
	{
		return l_TotalShadowBufferMemory;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	int g2dResourceCounterVk::GetNumShadowMapTextures()
	{
		return matTextureMgrVk::GetNumShadowMaps();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	float g2dResourceCounterVk::GetTotalVertexBufferMemory()
	{
		return g3dVkBufferUtil::GetTotalVertexBufferMemory();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	int g2dResourceCounterVk::GetNumVertexBuffers()
	{
		return g3dVkBufferUtil::GetNumVertexBuffers();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	float g2dResourceCounterVk::GetTotalIndexBufferMemory()
	{
		return g3dVkBufferUtil::GetTotalIndexBufferMemory();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	int g2dResourceCounterVk::GetNumIndexBuffers()
	{
		return g3dVkBufferUtil::GetNumIndexBuffers();
	}

	//--------------------------------------------------------------------
	// add a resource not managed by matTextureMgr
	//--------------------------------------------------------------------
	void g2dResourceCounterVk::AddResource(float i_SizeKBytes, eResourceCategory i_Category)
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
	void g2dResourceCounterVk::RemoveResource(float i_SizeKBytes, eResourceCategory i_Category)
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
}
