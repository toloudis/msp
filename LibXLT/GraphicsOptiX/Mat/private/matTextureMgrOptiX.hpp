/*****************************************************************************
**  matTextureMgrOptiX.hpp
**
**      matTextureMgrOptiX loads, creates and destroys the OptiX backend's
**	textures (matTextureMgr).
**
**	Stub: loads and creates no textures (NULL).
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_MATTEXTUREMGROPTIX_HPP
#error matTextureMgrOptiX.hpp multiply included
#endif
#define GOX_MATTEXTUREMGROPTIX_HPP

#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/Mat/matTextureMgr.hpp"
#endif

#include <string>
#include <vector>

namespace gox
{
	class matTextureMgrOptiX : public matTextureMgrImpl
	{
		public:
			virtual unsigned int GetTotalTextureMemory();
			virtual matTexture* LoadTexture(const fsLocator& i_Directory,
											const std::string& i_Name,
											std::vector<std::string>& o_UVANames,
											TEXTURE_TYPE i_Type = TEXTURE_TYPE_2D,
											bool i_bIgnoreTUVSuffix = false,
											bool mipmap_if_2D = true);
			virtual matTexture* CreateShadowMap(int i_nTextureWidth, int i_nTextureHeight);
			virtual matTexture* CreateReflectiveShadowMap(int i_nTextureWidth, int i_nTextureHeight);
			virtual matTexture* CreateRenderTargetTexture(int i_nTextureWidth,
														  int i_nTextureHeight,
														  bool i_bFloatingPoint,
														  const g2dPFD* i_PFD,
														  bool i_bAutoGenMipmap,
														  bool i_bAllocDepthBuffer,
														  matTextureMgr::eResourceCategory i_ResourceCategory,
														  bool i_bFloatDepth,
														  bool i_bAntiAlias);
			virtual matTexture* CreateRenderTargetTexture(matTexture* i_Texture,
														  bool i_bAutoGenMipmap,
														  matTextureMgr::eResourceCategory i_ResourceCategory);
			virtual matTexture* CreateCubeRenderTargetTexture(int i_nTextureWidth,
															  int i_nTextureHeight,
															  g2dPFD* i_PFD);
			virtual void DestroyTexture(matTexture* i_Texture);
			virtual void DestroyAllTextures();
			virtual void ReloadTexture(const fsLocator& i_Locator, bool i_bIsMipMap = true);
			virtual void UnloadTexture(const fsLocator& i_Locator);
			virtual void FillTexture(matTexture* i_pTexture, const maFloatRGBA& i_Color);
			virtual void FillTexture(matTexture* i_pTexture, void* i_PixelData, int i_nByte);
			virtual void UpdateTexture(matTexture* i_pTexture, unsigned char* i_Data, int i_Size, int nMipLevel = 0);
			virtual matTexture* CreateTexture(int i_nTextureWidth,
											  int i_nTextureHeight,
											  const g2dPFD* i_PFD,
											  bool i_bMipmap);
			virtual matTexture* CreateTexture(int i_nTextureWidth,
											  int i_nTextureHeight,
											  const g2dPFD* i_PFD,
											  bool i_bMipmap,
											  void* i_PixelData);
			virtual matTexture* CreateTexture3D(int i_nTextureWidth,
												int i_nTextureHeight,
												int i_nTextureDepth,
												const g2dPFD* i_PFD,
												BIND_TYPE i_Bindings);
			virtual matTexture* CreateTexture3D(int i_nTextureWidth,
												int i_nTextureHeight,
												int i_nTextureDepth,
												const g2dPFD* i_PFD,
												BIND_TYPE i_Bindings,
												void* i_PixelData);
			virtual void CopyTexture(matTexture* i_SrcTex, matTexture* i_DstTex);
			virtual void MergeTransparentTextures(matTexture* i_pTexO, matTexture* i_pTexI, g2dRenderTarget* io_pTarget);
	};
}
