/*****************************************************************************
**  matStubsVk.hpp
**
**      Vulkan implementations of the material hooks that aren't ported yet
**	(see g2dStubsVk.hpp for the rules): the textures matTextureMgrVk doesn't
**	make yet keep their sizes and names but have no GPU image; shader effects
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
#ifndef GVK_MATTEXTUREVK_HPP
#include "GraphicsVulkan/Mat/matTextureVk.hpp"
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
	//	The texture shadow maps, reflective shadow maps and cube render
	//	targets get until Plan 5 ports them (matTextureMgrVk makes them): a
	//	size and a pixel format, no image
	class matStubTextureVk : public matTextureVk
	{
		public:
			matStubTextureVk(int i_Width, int i_Height, const g2dPFD* i_pPFD, const std::string& i_Name);
			virtual ~matStubTextureVk();
			virtual vkdImage* GetImage() const { return NULL; }
			const std::string& Name() const { return m_Name; }

		private:
			std::string m_Name;
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
