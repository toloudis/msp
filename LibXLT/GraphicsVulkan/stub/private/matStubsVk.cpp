/*****************************************************************************
**  matStubsVk.cpp
**
**      See matStubsVk.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_MATSTUBSVK_HPP
#include "GraphicsVulkan/stub/matStubsVk.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/G2d/g2dPFD.hpp"
#endif
#ifndef IT_STRINGUTIL_HPP
#include "Core/It/itStringUtil.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#ifndef EFF_SHADERPARAMS_HPP
#include "Graphics/Eff/effShaderParams.hpp"
#endif
#ifndef EFF_BAKEDATA_HPP
#include "Graphics/Eff/effBakeData.hpp"
#endif
#ifndef EFF_BILLBOARDDATA_HPP
#include "Graphics/Eff/effBillboardData.hpp"
#endif
#ifndef EFF_BLURDATA_HPP
#include "Graphics/Eff/effBlurData.hpp"
#endif
#ifndef EFF_DOFDATA_HPP
#include "Graphics/Eff/effDOFData.hpp"
#endif
#ifndef EFF_GLOWDATA_HPP
#include "Graphics/Eff/effGlowData.hpp"
#endif
#ifndef EFF_LIGHTGLOWDATA_HPP
#include "Graphics/Eff/effLightGlowData.hpp"
#endif
#ifndef EFF_MASKALPHADATA_HPP
#include "Graphics/Eff/effMaskAlphaData.hpp"
#endif
#ifndef EFF_OCCLUSIONDATA_HPP
#include "Graphics/Eff/effOcclusionData.hpp"
#endif
#ifndef EFF_OUTLINEDATA_HPP
#include "Graphics/Eff/effOutlineData.hpp"
#endif
#ifndef EFF_PARTICLEDATA_HPP
#include "Graphics/Eff/effParticleData.hpp"
#endif
#ifndef EFF_PHONGDATA_HPP
#include "Graphics/Eff/effPhongData.hpp"
#endif
#ifndef EFF_RAMPDATA_HPP
#include "Graphics/Eff/effRampData.hpp"
#endif
#ifndef EFF_SOLIDDATA_HPP
#include "Graphics/Eff/effSolidData.hpp"
#endif
#ifndef EFF_STRANDHAIRDATA_HPP
#include "Graphics/Eff/effStrandHairData.hpp"
#endif
#ifndef EFF_TEXTUREDDATA_HPP
#include "Graphics/Eff/effTexturedData.hpp"
#endif

#include <algorithm>

namespace gvk
{
	//------------------------------------------------------------------------
	//	matTextureVk
	//------------------------------------------------------------------------
	matTextureVk::matTextureVk(int i_Width, int i_Height, const g2dPFD* i_pPFD, const std::string& i_Name)
		: m_Name(i_Name)
	{
		SetWidth(i_Width);
		SetHeight(i_Height);
		SetPixelFormat(i_pPFD ? *i_pPFD : g2dPFD(0, 8, 8, 8, 16, 8, 24, 8, 32));
	}

	matTextureVk::~matTextureVk()
	{
	}

	char* matTextureVk::MakeTransparencyMap(float, const g2dPFD&, int, int)
	{
		GVK_NOT_YET("matTextureVk::MakeTransparencyMap");
		return NULL;
	}

	//------------------------------------------------------------------------
	//	matTextureMgrVk - textures are tracked (and deleted) but have no GPU
	//	image yet
	//------------------------------------------------------------------------
	matTextureMgrVk::~matTextureMgrVk()
	{
		DestroyAllTextures();
	}

	matTexture* matTextureMgrVk::add(matTextureVk* i_pTexture)
	{
		m_Textures.push_back(i_pTexture);
		return i_pTexture;
	}

	unsigned int matTextureMgrVk::GetTotalTextureMemory()
	{
		GVK_NOT_YET("matTextureMgrVk::GetTotalTextureMemory");
		return 0;
	}

	matTexture* matTextureMgrVk::LoadTexture(const fsLocator& i_Directory, const itString& i_Name, std::vector<itString>&,
											 TEXTURE_TYPE, bool, bool)
	{
		GVK_NOT_YET("matTextureMgrVk::LoadTexture");
		std::string directory;
		fsFileUtil::LocatorToANSIFilename(i_Directory, directory);
		return add(new matTextureVk(1, 1, NULL, directory + "/" + itStringUtil::GetStdString(i_Name)));
	}

	matTexture* matTextureMgrVk::CreateShadowMap(int i_Width, int i_Height)
	{
		GVK_NOT_YET("matTextureMgrVk::CreateShadowMap");
		return add(new matTextureVk(i_Width, i_Height, NULL, "shadow map"));
	}

	matTexture* matTextureMgrVk::CreateReflectiveShadowMap(int i_Width, int i_Height)
	{
		GVK_NOT_YET("matTextureMgrVk::CreateReflectiveShadowMap");
		return add(new matTextureVk(i_Width, i_Height, NULL, "reflective shadow map"));
	}

	matTexture* matTextureMgrVk::CreateRenderTargetTexture(int i_Width, int i_Height, bool, const g2dPFD* i_PFD, bool, bool,
														   matTextureMgr::eResourceCategory, bool, bool)
	{
		GVK_NOT_YET("matTextureMgrVk::CreateRenderTargetTexture");
		return add(new matTextureVk(i_Width, i_Height, i_PFD, "render target"));
	}

	matTexture* matTextureMgrVk::CreateRenderTargetTexture(matTexture* i_Texture, bool, matTextureMgr::eResourceCategory)
	{
		GVK_NOT_YET("matTextureMgrVk::CreateRenderTargetTexture (from a texture)");
		return add(new matTextureVk(i_Texture ? i_Texture->GetWidth() : 1, i_Texture ? i_Texture->GetHeight() : 1,
									NULL, "render target"));
	}

	matTexture* matTextureMgrVk::CreateCubeRenderTargetTexture(int i_Width, int i_Height, g2dPFD* i_PFD)
	{
		GVK_NOT_YET("matTextureMgrVk::CreateCubeRenderTargetTexture");
		return add(new matTextureVk(i_Width, i_Height, i_PFD, "cube render target"));
	}

	void matTextureMgrVk::DestroyTexture(matTexture* i_Texture)
	{
		auto found = std::find(m_Textures.begin(), m_Textures.end(), i_Texture);
		if (found != m_Textures.end())
		{
			m_Textures.erase(found);
			delete i_Texture;
		}
	}

	void matTextureMgrVk::DestroyAllTextures()
	{
		for (matTexture* texture : m_Textures)
		{
			delete texture;
		}
		m_Textures.clear();
	}

	void matTextureMgrVk::ReloadTexture(const fsLocator&, bool)						{ GVK_NOT_YET("matTextureMgrVk::ReloadTexture"); }
	void matTextureMgrVk::UnloadTexture(const fsLocator&)							{ GVK_NOT_YET("matTextureMgrVk::UnloadTexture"); }
	void matTextureMgrVk::SaveTextureToRgbaTiff(matTexture*, const fsLocator&)		{ GVK_NOT_YET("matTextureMgrVk::SaveTextureToRgbaTiff"); }
	void matTextureMgrVk::SaveTextureToRgbaPNG(matTexture*, const fsLocator&)		{ GVK_NOT_YET("matTextureMgrVk::SaveTextureToRgbaPNG"); }
	void matTextureMgrVk::SaveTextureToFile(matTexture*, const fsLocator&)			{ GVK_NOT_YET("matTextureMgrVk::SaveTextureToFile"); }
	void matTextureMgrVk::FillTexture(matTexture*, const maFloatRGBA&)				{ GVK_NOT_YET("matTextureMgrVk::FillTexture"); }
	void matTextureMgrVk::FillTexture(matTexture*, void*, int)						{ GVK_NOT_YET("matTextureMgrVk::FillTexture"); }
	void matTextureMgrVk::UpdateTexture(matTexture*, unsigned char*, int, int)		{ GVK_NOT_YET("matTextureMgrVk::UpdateTexture"); }

	matTexture* matTextureMgrVk::CreateTexture(int i_Width, int i_Height, const g2dPFD* i_PFD, bool)
	{
		GVK_NOT_YET("matTextureMgrVk::CreateTexture");
		return add(new matTextureVk(i_Width, i_Height, i_PFD, "texture"));
	}

	matTexture* matTextureMgrVk::CreateTexture(int i_Width, int i_Height, const g2dPFD* i_PFD, bool, void*)
	{
		GVK_NOT_YET("matTextureMgrVk::CreateTexture");
		return add(new matTextureVk(i_Width, i_Height, i_PFD, "texture"));
	}

	matTexture* matTextureMgrVk::CreateTexture3D(int i_Width, int i_Height, int, const g2dPFD* i_PFD, BIND_TYPE)
	{
		GVK_NOT_YET("matTextureMgrVk::CreateTexture3D");
		return add(new matTextureVk(i_Width, i_Height, i_PFD, "texture 3D"));
	}

	matTexture* matTextureMgrVk::CreateTexture3D(int i_Width, int i_Height, int, const g2dPFD* i_PFD, BIND_TYPE, void*)
	{
		GVK_NOT_YET("matTextureMgrVk::CreateTexture3D");
		return add(new matTextureVk(i_Width, i_Height, i_PFD, "texture 3D"));
	}

	void matTextureMgrVk::CopyTexture(matTexture*, matTexture*)						{ GVK_NOT_YET("matTextureMgrVk::CopyTexture"); }
	void matTextureMgrVk::MergeTransparentTextures(matTexture*, matTexture*, g2dRenderTarget*)	{ GVK_NOT_YET("matTextureMgrVk::MergeTransparentTextures"); }

	//------------------------------------------------------------------------
	//	matShaderEffectVk - no passes, so nothing is drawn with it
	//------------------------------------------------------------------------
	matShaderEffectVk::matShaderEffectVk(const std::string& i_Name)
		: m_Name(i_Name)
	{
	}

	int matShaderEffectVk::Begin() const									{ GVK_NOT_YET("matShaderEffectVk"); return 0; }
	void matShaderEffectVk::BeginPass(int) const							{}
	void matShaderEffectVk::EndPass() const									{}
	void matShaderEffectVk::End() const										{}
	int matShaderEffectVk::GetParamIndex(const std::string&) const			{ return -1; }
	bool matShaderEffectVk::GetParamUI(const std::string&, matShaderParamUI&) const	{ return false; }
	void matShaderEffectVk::GetAllParamUIs(std::list<matShaderParamUI>&) const		{}
	void matShaderEffectVk::SetMatrix(int, const maMatrix4x4&)				{}
	void matShaderEffectVk::SetFloat(int, float)							{}
	void matShaderEffectVk::SetVector(int, const maVector4d&)				{}
	void matShaderEffectVk::SetData(int, void*, unsigned int)				{}
	void matShaderEffectVk::SetBool(int, bool)								{}
	void matShaderEffectVk::SetString(int, std::string)						{}
	void matShaderEffectVk::SetTexture(int, const matTexture*) const		{}
	std::string matShaderEffectVk::GetName() const							{ return m_Name; }
	void matShaderEffectVk::SetName(std::string i_name)						{ m_Name = i_name; }
	void matShaderEffectVk::SetTechnique(Technique) const					{}
	void matShaderEffectVk::SetTechnique(const std::string&) const			{}
	void matShaderEffectVk::SetupMatrices(const maMatrix4x4&, const maMatrix4x4&, const maMatrix4x4&, const maPoint3d&) const {}
	bool matShaderEffectVk::GetHasSkinning() const							{ return false; }
	void matShaderEffectVk::SetupSkinningMatrices(const std::vector<maMatrix4x4>&) const	{}
	void matShaderEffectVk::SetupAmbientPass(const g3dAmbientEnvState&) const	{}
	void matShaderEffectVk::SetupDOFPrep() const							{}
	void matShaderEffectVk::SetupLighting(const maAxisBox&) const			{}
	void matShaderEffectVk::SetupAmbientLighting(const maAxisBox&) const	{}
	void matShaderEffectVk::SetupSingleLight(const g3dLight*, const g3dProjectedLight*, bool) const	{}
	void matShaderEffectVk::SetupMaterial(const matMaterial*, int) const	{}
	void matShaderEffectVk::SetupParams(const effShaderData*) const			{}
	void matShaderEffectVk::SetIsolateReflections(bool) const				{}
	void matShaderEffectVk::SetTime(float) const							{}
	void matShaderEffectVk::SetVertexUVBakeMode(bool) const					{}
	void matShaderEffectVk::SetIsDoubleSided(bool) const					{}
	void matShaderEffectVk::SetupGlowPass(const effGlowData&) const			{}
	void matShaderEffectVk::SetupOutlinePass(const effOutlineData&) const	{}
	void matShaderEffectVk::SetBakingFactors(const maPoint2d&, const maPoint2d&) const	{}
	//	No parameters yet, but the dialogs that list them need the (empty)
	//	property object; effShaderParams deletes it, as with DX11
	int matShaderEffectVk::BuildPrtyObject(effShaderParams* o_pParams) const
	{
		GVK_NOT_YET("matShaderEffectVk::BuildPrtyObject");
		if (o_pParams->m_pPrtyUI == NULL)
		{
			o_pParams->m_pPrtyUI = new prtyObject;
		}
		return 0;
	}
	bool matShaderEffectVk::HasReflectionMap() const						{ return false; }
	bool matShaderEffectVk::HasHardwareTessellation() const					{ return false; }
	void matShaderEffectVk::SetupTessellatorMeshTexture(const matTexture*)	{}
	void matShaderEffectVk::SetupDisplacementMap(const matTexture*, float, float, float, const maVector2d&)	{}
	void matShaderEffectVk::SetupUVTransform(const maMatrix4x4&)			{}
	void matShaderEffectVk::SetupPaintOverlay(matRenderTargetTexture*) const	{}
	void matShaderEffectVk::SetupNormalMap(const matTexture*, float)		{}
	bool matShaderEffectVk::GetSupportOutline() const						{ return false; }

	//------------------------------------------------------------------------
	//	matShaderMgrVk - registers no effects yet; an effect asked for by
	//	path gets a pass-less stand-in
	//------------------------------------------------------------------------
	namespace
	{
		template<class DATA> effShaderData* make() { return new DATA; }

		//	The built-in shaders GraphicsDX11 registers (effShaderArray::
		//	RegisterEffects), with the same names and data types: materials
		//	take their data type from here (matMaterial("Billboard.fx")
		//	gets effBillboardData).
		struct built_in_shader
		{
			const char* m_Name;
			effShaderData* (*m_MakeData)();		// NULL: no data template
		};
		const built_in_shader c_BuiltInShaders[] =
		{
			{ "default",                  make<effPhongData> },
			{ "testtess.fx",              make<effPhongData> },
			{ "Bake.fx",                  make<effBakeData> },
			{ "Simple.fx",                make<effPhongData> },
			{ "Phong.fx",                 make<effPhongData> },
			{ "Phong_wBump.fx",           make<effPhongData> },
			{ "HDRLighting",              NULL },
			{ "Blur",                     make<effBlurData> },
			{ "Glow",                     make<effGlowData> },
			{ "DOF",                      make<effDOFData> },
			{ "LightGlow.fx",             make<effLightGlowData> },
			{ "Particle.fx",              make<effParticleData> },
			{ "PostAlphaMatte",           NULL },
			{ "Fog",                      NULL },
			{ "Solid.fx",                 make<effSolidData> },
			{ "MaskAlpha.fx",             make<effMaskAlphaData> },
			{ "DepthMap.fx",              make<effMaskAlphaData> },
			{ "DepthRender.fx",           make<effMaskAlphaData> },
			{ "ReflectiveShadowMap.fx",   make<effMaskAlphaData> },
			{ "Outline",                  make<effOutlineData> },
			{ "ssaoBilateralBlurEngine",  make<effOcclusionData> },
			{ "ssaoMultiHorizonBasedAO",  make<effOcclusionData> },
			{ "ssgiMultiHorizonBasedGI",  make<effOcclusionData> },
			{ "AOVolumes.fx",             make<effOcclusionData> },
			{ "GIVolumes.fx",             make<effOcclusionData> },
			{ "Billboard.fx",             make<effBillboardData> },
			{ "Brushstroke.fx",           make<effTexturedData> },
			{ "VelocityRender.fx",        make<effTexturedData> },
			{ "IlluminationOnly.fx",      make<effTexturedData> },
			{ "ShadowsOnly.fx",           make<effTexturedData> },
			{ "NormalMap.fx",             make<effMaskAlphaData> },
			{ "MotionBlur",               make<effTexturedData> },
			{ "AAEdgeFilter",             make<effTexturedData> },
			{ "LPV_GI.fx",                make<effTexturedData> },
			{ "Ramp.fx",                  make<effRampData> },
			{ "HairDefault.fx",           make<effStrandHairData> },
			{ "OpacityRender.fx",         make<effStrandHairData> },
			{ "BlackAndWhite.fx",         make<effTexturedData> },
			{ "Sepia.fx",                 make<effTexturedData> },
			{ "GradientMap.fx",           make<effTexturedData> },
			{ "Sketch.fx",                make<effTexturedData> },
			{ "EnvBackground.fx",         make<effTexturedData> },
		};
	}

	//	Each gets a matShaderEffectVk (no passes yet); matShaderMgr owns and
	//	deletes the data templates and effects, as with DX11
	void matShaderMgrVk::RegisterEffects(const fsLocator&, std::map<std::string, matShaderInfo>& io_ShaderMap)
	{
		GVK_NOT_YET("matShaderMgrVk::RegisterEffects (effects draw nothing)");
		for (const built_in_shader& shader : c_BuiltInShaders)
		{
			matShaderInfo info;
			info.m_Name = shader.m_Name;
			info.m_DataTemplate = shader.m_MakeData ? shader.m_MakeData() : NULL;
			info.m_pEffect = new matShaderEffectVk(shader.m_Name);
			io_ShaderMap[shader.m_Name] = info;
		}
	}

	void matShaderMgrVk::RegisterSelectableShaders(std::vector<matShaderInfo>&, std::vector<matShaderInfo>&)
	{
		GVK_NOT_YET("matShaderMgrVk::RegisterSelectableShaders");
	}

	void matShaderMgrVk::UnloadEffect(const fsLocator& i_PathToShader, std::map<fsLocator, matShaderEffect*>& io_ShaderMap)
	{
		auto found = io_ShaderMap.find(i_PathToShader);
		if (found != io_ShaderMap.end())
		{
			delete found->second;
			io_ShaderMap.erase(found);
		}
	}

	matShaderEffect* matShaderMgrVk::LoadEffect(const fsLocator& i_PathToShader, std::map<fsLocator, matShaderEffect*>& io_ShaderMap)
	{
		GVK_NOT_YET("matShaderMgrVk::LoadEffect");
		auto found = io_ShaderMap.find(i_PathToShader);
		if (found != io_ShaderMap.end())
		{
			return found->second;
		}
		std::string name;
		fsFileUtil::LocatorToANSIFilename(i_PathToShader, name);
		matShaderEffect* effect = new matShaderEffectVk(name);
		io_ShaderMap[i_PathToShader] = effect;
		return effect;
	}
}
