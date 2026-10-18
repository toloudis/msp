/*****************************************************************************
**  matStubsVk.hpp
**
**      Vulkan implementations of the material hooks that aren't ported yet
**	(see g2dStubsVk.hpp for the rules): textures and the texture manager
**	keep their sizes and names but have no GPU image yet; shader effects
**	have no passes (renderers draw nothing with them); the shader manager
**	registers no effects yet.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_MATSTUBSVK_HPP
#error matStubsVk.hpp multiply included
#endif
#define GVK_MATSTUBSVK_HPP

#ifndef MAT_TEXTURE_HPP
#include "Graphics/Mat/matTexture.hpp"
#endif
#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/Mat/matTextureMgr.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/Mat/matShaderEffect.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/Mat/matShaderMgr.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#include <string>
#include <vector>

namespace gvk
{
	class matTextureVk : public matTexture
	{
		public:
			matTextureVk(int i_Width, int i_Height, const g2dPFD* i_pPFD, const std::string& i_Name);
			virtual ~matTextureVk();
			virtual char* MakeTransparencyMap(float i_Bias, const g2dPFD& i_PFD, int i_Width, int i_Height);
			const std::string& Name() const { return m_Name; }

		private:
			std::string m_Name;
	};

	class matTextureMgrVk : public matTextureMgrImpl
	{
		public:
			virtual ~matTextureMgrVk();
			virtual unsigned int GetTotalTextureMemory();
			virtual matTexture* LoadTexture(const fsLocator& i_Directory, const itString& i_Name, std::vector<itString>& o_UVANames,
											TEXTURE_TYPE i_Type = TEXTURE_TYPE_2D, bool i_bIgnoreTUVSuffix = false, bool mipmap_if_2D = true);
			virtual matTexture* CreateShadowMap(int i_nTextureWidth, int i_nTextureHeight);
			virtual matTexture* CreateReflectiveShadowMap(int i_nTextureWidth, int i_nTextureHeight);
			virtual matTexture* CreateRenderTargetTexture(int i_nTextureWidth, int i_nTextureHeight, bool i_bFloatingPoint,
														  const g2dPFD* i_PFD, bool i_bAutoGenMipmap, bool i_bAllocDepthBuffer,
														  matTextureMgr::eResourceCategory i_ResourceCategory, bool i_bFloatDepth,
														  bool i_bAntiAlias);
			virtual matTexture* CreateRenderTargetTexture(matTexture* i_Texture, bool i_bAutoGenMipmap,
														  matTextureMgr::eResourceCategory i_ResourceCategory);
			virtual matTexture* CreateCubeRenderTargetTexture(int i_nTextureWidth, int i_nTextureHeight, g2dPFD* i_PFD);
			virtual void DestroyTexture(matTexture* i_Texture);
			virtual void DestroyAllTextures();
			virtual void ReloadTexture(const fsLocator& i_Locator, bool i_bIsMipMap = true);
			virtual void UnloadTexture(const fsLocator& i_Locator);
			virtual void SaveTextureToRgbaTiff(matTexture* i_pTexture, const fsLocator& i_FilePathLocator);
			virtual void SaveTextureToRgbaPNG(matTexture* i_pTexture, const fsLocator& i_FilePathLocator);
			virtual void SaveTextureToFile(matTexture* i_pTexture, const fsLocator& i_FilePathLocator);
			virtual void FillTexture(matTexture* i_pTexture, const maFloatRGBA& i_Color);
			virtual void FillTexture(matTexture* i_pTexture, void* i_PixelData, int i_nByte);
			virtual void UpdateTexture(matTexture* i_pTexture, unsigned char* i_Data, int i_Size, int nMipLevel = 0);
			virtual matTexture* CreateTexture(int i_nTextureWidth, int i_nTextureHeight, const g2dPFD* i_PFD, bool i_bMipmap);
			virtual matTexture* CreateTexture(int i_nTextureWidth, int i_nTextureHeight, const g2dPFD* i_PFD, bool i_bMipmap,
											  void* i_PixelData);
			virtual matTexture* CreateTexture3D(int i_nTextureWidth, int i_nTextureHeight, int i_nTextureDepth,
												const g2dPFD* i_PFD, BIND_TYPE i_Bindings);
			virtual matTexture* CreateTexture3D(int i_nTextureWidth, int i_nTextureHeight, int i_nTextureDepth,
												const g2dPFD* i_PFD, BIND_TYPE i_Bindings, void* i_PixelData);
			virtual void CopyTexture(matTexture* i_SrcTex, matTexture* i_DstTex);
			virtual void MergeTransparentTextures(matTexture* i_pTexO, matTexture* i_pTexI, g2dRenderTarget* io_pTarget);

		private:
			matTexture* add(matTextureVk* i_pTexture);
			std::vector<matTexture*> m_Textures;
	};

	class matShaderEffectVk : public matShaderEffect
	{
		public:
			explicit matShaderEffectVk(const std::string& i_Name);

			//	effShaderEffect
			virtual int Begin() const;
			virtual void BeginPass(int i_Pass) const;
			virtual void EndPass() const;
			virtual void End() const;
			virtual int GetParamIndex(const std::string& i_name) const;
			virtual bool GetParamUI(const std::string& i_name, matShaderParamUI& o_paramUI) const;
			virtual void GetAllParamUIs(std::list<matShaderParamUI>& o_paramUI) const;
			virtual void SetMatrix(int i_param, const maMatrix4x4& i_matrix);
			virtual void SetFloat(int i_param, float i_float);
			virtual void SetVector(int i_param, const maVector4d& i_vector);
			virtual void SetData(int i_param, void* i_data, unsigned int i_nbytes);
			virtual void SetBool(int i_param, bool i_bool);
			virtual void SetString(int i_param, std::string i_string);
			virtual void SetTexture(int i_param, const matTexture* i_pTexture) const;
			virtual std::string GetName() const;
			virtual void SetName(std::string i_name);

			//	matShaderEffect
			virtual void SetTechnique(Technique i_Technique) const;
			virtual void SetTechnique(const std::string& i_Technique) const;
			virtual void SetupMatrices(const maMatrix4x4& i_WorldMat, const maMatrix4x4& i_CameraMat,
									   const maMatrix4x4& i_ProjMat, const maPoint3d& i_CameraPos) const;
			virtual bool GetHasSkinning() const;
			virtual void SetupSkinningMatrices(const std::vector<maMatrix4x4>& i_MatrixPalette) const;
			virtual void SetupAmbientPass(const g3dAmbientEnvState& i_AmbientEnvState) const;
			virtual void SetupDOFPrep() const;
			virtual void SetupLighting(const maAxisBox& i_BBox) const;
			virtual void SetupAmbientLighting(const maAxisBox& i_BBox) const;
			virtual void SetupSingleLight(const g3dLight* i_pLight, const g3dProjectedLight* i_pProjLight, bool i_bAllowShadows) const;
			virtual void SetupMaterial(const matMaterial* i_Material, int i_MaterialLayerIndex = 0) const;
			virtual void SetupParams(const effShaderData* i_Data) const;
			virtual void SetIsolateReflections(bool i_bIsolateReflections) const;
			virtual void SetTime(float i_Time) const;
			virtual void SetVertexUVBakeMode(bool i_bDoBaking) const;
			virtual void SetIsDoubleSided(bool i_IsDoubleSided) const;
			virtual void SetupGlowPass(const effGlowData& i_GlowData) const;
			virtual void SetupOutlinePass(const effOutlineData& i_OutlineData) const;
			virtual void SetBakingFactors(const maPoint2d& i_Scale, const maPoint2d& i_Translate) const;
			virtual int BuildPrtyObject(effShaderParams* o_pParams) const;
			virtual bool HasReflectionMap() const;
			virtual bool HasHardwareTessellation() const;
			virtual void SetupTessellatorMeshTexture(const matTexture* i_pTexture);
			virtual void SetupDisplacementMap(const matTexture* i_pTexture, float i_Scale, float i_Bias, float i_Blur,
											  const maVector2d& i_ObjUVScale);
			virtual void SetupUVTransform(const maMatrix4x4& i_WorldMat);
			virtual void SetupPaintOverlay(matRenderTargetTexture* i_pRenderTexture) const;
			virtual void SetupNormalMap(const matTexture* i_pNormalMap, float i_BumpScale);
			virtual bool GetSupportOutline() const;

		private:
			std::string m_Name;
	};

	class matShaderMgrVk : public matShaderMgrImpl
	{
		public:
			virtual void RegisterEffects(const fsLocator& i_ShaderDir, std::map<std::string, matShaderInfo>& io_ShaderMap);
			virtual void RegisterSelectableShaders(std::vector<matShaderInfo>& o_Materials, std::vector<matShaderInfo>& o_PostEffects);
			virtual void UnloadEffect(const fsLocator& i_PathToShader, std::map<fsLocator, matShaderEffect*>& io_ShaderMap);
			virtual matShaderEffect* LoadEffect(const fsLocator& i_PathToShader, std::map<fsLocator, matShaderEffect*>& io_ShaderMap);
	};
}
