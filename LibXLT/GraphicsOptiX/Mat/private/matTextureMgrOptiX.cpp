/*****************************************************************************
**  matTextureMgrOptiX.cpp
**
**      See matTextureMgrOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_MATTEXTUREMGROPTIX_HPP
#include "GraphicsOptiX/Mat/private/matTextureMgrOptiX.hpp"
#endif

#include <cstddef>

namespace gox
{
	unsigned int matTextureMgrOptiX::GetTotalTextureMemory()
	{
		return 0;
	}

	matTexture* matTextureMgrOptiX::LoadTexture(const fsLocator& /*i_Directory*/,
												const std::string& /*i_Name*/,
												std::vector<std::string>& /*o_UVANames*/,
												TEXTURE_TYPE /*i_Type*/,
												bool /*i_bIgnoreTUVSuffix*/,
												bool /*mipmap_if_2D*/)
	{
		return NULL;
	}

	matTexture* matTextureMgrOptiX::CreateShadowMap(int /*i_nTextureWidth*/, int /*i_nTextureHeight*/)
	{
		return NULL;
	}

	matTexture* matTextureMgrOptiX::CreateReflectiveShadowMap(int /*i_nTextureWidth*/, int /*i_nTextureHeight*/)
	{
		return NULL;
	}

	matTexture* matTextureMgrOptiX::CreateRenderTargetTexture(int /*i_nTextureWidth*/,
															  int /*i_nTextureHeight*/,
															  bool /*i_bFloatingPoint*/,
															  const g2dPFD* /*i_PFD*/,
															  bool /*i_bAutoGenMipmap*/,
															  bool /*i_bAllocDepthBuffer*/,
															  matTextureMgr::eResourceCategory /*i_ResourceCategory*/,
															  bool /*i_bFloatDepth*/,
															  bool /*i_bAntiAlias*/)
	{
		return NULL;
	}

	matTexture* matTextureMgrOptiX::CreateRenderTargetTexture(matTexture* /*i_Texture*/,
															  bool /*i_bAutoGenMipmap*/,
															  matTextureMgr::eResourceCategory /*i_ResourceCategory*/)
	{
		return NULL;
	}

	matTexture* matTextureMgrOptiX::CreateCubeRenderTargetTexture(int /*i_nTextureWidth*/,
																  int /*i_nTextureHeight*/,
																  g2dPFD* /*i_PFD*/)
	{
		return NULL;
	}

	void matTextureMgrOptiX::DestroyTexture(matTexture* /*i_Texture*/)
	{
	}

	void matTextureMgrOptiX::DestroyAllTextures()
	{
	}

	void matTextureMgrOptiX::ReloadTexture(const fsLocator& /*i_Locator*/, bool /*i_bIsMipMap*/)
	{
	}

	void matTextureMgrOptiX::UnloadTexture(const fsLocator& /*i_Locator*/)
	{
	}

	void matTextureMgrOptiX::FillTexture(matTexture* /*i_pTexture*/, const maFloatRGBA& /*i_Color*/)
	{
	}

	void matTextureMgrOptiX::FillTexture(matTexture* /*i_pTexture*/, void* /*i_PixelData*/, int /*i_nByte*/)
	{
	}

	void matTextureMgrOptiX::UpdateTexture(matTexture* /*i_pTexture*/, unsigned char* /*i_Data*/, int /*i_Size*/, int /*nMipLevel*/)
	{
	}

	matTexture* matTextureMgrOptiX::CreateTexture(int /*i_nTextureWidth*/,
												  int /*i_nTextureHeight*/,
												  const g2dPFD* /*i_PFD*/,
												  bool /*i_bMipmap*/)
	{
		return NULL;
	}

	matTexture* matTextureMgrOptiX::CreateTexture(int /*i_nTextureWidth*/,
												  int /*i_nTextureHeight*/,
												  const g2dPFD* /*i_PFD*/,
												  bool /*i_bMipmap*/,
												  void* /*i_PixelData*/)
	{
		return NULL;
	}

	matTexture* matTextureMgrOptiX::CreateTexture3D(int /*i_nTextureWidth*/,
													int /*i_nTextureHeight*/,
													int /*i_nTextureDepth*/,
													const g2dPFD* /*i_PFD*/,
													BIND_TYPE /*i_Bindings*/)
	{
		return NULL;
	}

	matTexture* matTextureMgrOptiX::CreateTexture3D(int /*i_nTextureWidth*/,
													int /*i_nTextureHeight*/,
													int /*i_nTextureDepth*/,
													const g2dPFD* /*i_PFD*/,
													BIND_TYPE /*i_Bindings*/,
													void* /*i_PixelData*/)
	{
		return NULL;
	}

	void matTextureMgrOptiX::CopyTexture(matTexture* /*i_SrcTex*/, matTexture* /*i_DstTex*/)
	{
	}

	void matTextureMgrOptiX::MergeTransparentTextures(matTexture* /*i_pTexO*/, matTexture* /*i_pTexI*/, g2dRenderTarget* /*io_pTarget*/)
	{
	}
}
