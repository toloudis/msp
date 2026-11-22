/****************************************************************************\
**  effShaderArray.hpp
**
**      effShaderArray manages an array of shaders that chooses the 
**	appropriate shader based on the combination of texture layers.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_EFF_SHADERARRAY_HPP
#include "GraphicsVulkan/Eff/effShaderArray.hpp"
#endif
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#ifndef FS_ABSOLUTEPATHMGR_HPP
#include "Core/Fs/fsAbsolutePathMgr.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef FS_FILEX_HPP
#include "Core/fs/fsFileX.hpp"
#endif
#ifndef FS_RESOURCETRACKER_HPP
#include "Core/fs/fsResourceTracker.hpp"
#endif
#ifndef GF_PATHS_HPP
#include "Core/gf/gfPaths.hpp"
#endif
#ifndef EFF_BAKEDATA_HPP
#include "Graphics/eff/effBakeData.hpp"
#endif
#ifndef EFF_BILLBOARDDATA_HPP
#include "Graphics/eff/effBillboardData.hpp"
#endif
#ifndef EFF_BLINNDATA_HPP
#include "Graphics/eff/effBlinnData.hpp"
#endif
#ifndef EFF_BLURDATA_HPP
#include "Graphics/eff/effBlurData.hpp"
#endif
#ifndef EFF_DOFDATA_HPP
#include "Graphics/eff/effDOFData.hpp"
#endif
#ifndef EFF_NORMALSDATA_HPP
#include "Graphics/eff/effNormalsData.hpp"
#endif
#ifndef EFF_RENDERMANOVERRIDEDATA_HPP
#include "Graphics/eff/effRendermanOverrideData.hpp"
#endif
#ifndef EFF_GLOWDATA_HPP
#include "Graphics/eff/effGlowData.hpp"
#endif
#ifndef EFF_HAIRDATA_HPP
#include "Graphics/eff/effHairData.hpp"
#endif
#ifndef EFF_STRANDHAIRDATA_HPP
#include "Graphics/eff/effStrandHairData.hpp"
#endif
#ifndef EFF_LAMBERTDATA_HPP
#include "Graphics/eff/effLambertData.hpp"
#endif
#ifndef EFF_LIGHTGLOWDATA_HPP
#include "Graphics/eff/effLightGlowData.hpp"
#endif
#ifndef EFF_MASKALPHADATA_HPP
#include "Graphics/eff/effMaskAlphaData.hpp"
#endif
#ifndef EFF_OCCLUSIONDATA_HPP
#include "Graphics/eff/effOcclusionData.hpp"
#endif
#ifndef EFF_PARTICLEDATA_HPP
#include "Graphics/eff/effParticleData.hpp"
#endif
#ifndef EFF_PHONGDATA_HPP
#include "Graphics/eff/effPhongData.hpp"
#endif
#ifndef EFF_RAMPDATA_HPP
#include "Graphics/eff/effRampData.hpp"
#endif
#ifndef EFF_REFLDATA_HPP
#include "Graphics/eff/effReflData.hpp"
#endif
#ifndef EFF_SKINDATA_HPP
#include "Graphics/eff/effSkinData.hpp"
#endif
#ifndef EFF_SOLIDDATA_HPP
#include "Graphics/eff/effSolidData.hpp"
#endif
//#include "Graphics/eff/effTexturedData.hpp"
#ifndef EFF_TOONDATA_HPP
#include "Graphics/eff/effToonData.hpp"
#endif
#ifndef EFF_WATERDATA_HPP
#include "Graphics/eff/effWaterData.hpp"
#endif
#ifndef G3D_EXCEPTIONX_HPP
#include "Graphics/g3d/g3dExceptionX.hpp"
#endif
#ifndef MAT_EXCEPTIONX_HPP
#include "Graphics/mat/matExceptionX.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef GVK_EFF_BAKE_HPP
#include "GraphicsVulkan/Eff/effBake.hpp"
#endif
#ifndef GVK_EFF_BILLBOARD_HPP
#include "GraphicsVulkan/Eff/effBillboard.hpp"
#endif
#ifndef GVK_EFF_BLUR_HPP
#include "GraphicsVulkan/Eff/effBlur.hpp"
#endif
#ifndef GVK_EFF_DOF_HPP
#include "GraphicsVulkan/Eff/effDOF.hpp"
#endif
#ifndef GVK_EFF_GLOW_HPP
#include "GraphicsVulkan/Eff/effGlow.hpp"
#endif
//#include "GraphicsVulkan/Eff/effHair.hpp"
#ifndef GVK_EFF_STRANDHAIR_HPP
#include "GraphicsVulkan/Eff/effStrandHair.hpp"
#endif
//#include "GraphicsVulkan/Eff/effLambert.hpp"
#ifndef GVK_EFF_LIGHTGLOW_HPP
#include "GraphicsVulkan/Eff/effLightGlow.hpp"
#endif
#ifndef GVK_EFF_MASKALPHA_HPP
#include "GraphicsVulkan/Eff/effMaskAlpha.hpp"
#endif
#ifndef GVK_EFF_OCCLUSION_HPP
#include "GraphicsVulkan/Eff/effOcclusion.hpp"
#endif
#ifndef GVK_EFF_OUTLINE_HPP
#include "GraphicsVulkan/Eff/effOutline.hpp"
#endif
#ifndef GVK_EFF_PARTICLE_HPP
#include "GraphicsVulkan/Eff/effParticle.hpp"
#endif
#ifndef GVK_EFF_PHONG_HPP
#include "GraphicsVulkan/Eff/effPhong.hpp"
#endif
#ifndef GVK_EFF_RAMP_HPP
#include "GraphicsVulkan/Eff/effRamp.hpp"
#endif
#ifndef GVK_EFF_SOLID_HPP
#include "GraphicsVulkan/Eff/effSolid.hpp"
#endif
//#include "GraphicsVulkan/Eff/effTextured.hpp"
//#include "GraphicsVulkan/Eff/effToon.hpp"
#ifndef GVK_FXEMBEDDEDSHADERSVK_HPP
#include "GraphicsVulkan/Fx/fxEmbeddedShadersVk.hpp"
#endif
#ifndef GVK_EFFPLAINEFFECTVK_HPP
#include "GraphicsVulkan/Eff/effPlainEffectVk.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif

//#include "Tool/gui/guiMessageBox.hpp"

#include <map>

namespace gvk
{

//------------------------------------------------------------------------
// LoadPlainBuiltIn() - the embedded plain-HLSL effect of a built-in
//	shader: "Phong.fx" or "Phong" loads the embedded "Phong". NULL if there
//	is none or it fails to build.
//------------------------------------------------------------------------
static std::unique_ptr<fxEffect> LoadPlainBuiltIn(const std::string& i_Name)
{
	std::string name = i_Name;
	size_t dot = name.rfind('.');
	if (dot != std::string::npos && _stricmp(name.c_str() + dot, ".fx") == 0)
		name.erase(dot);

	const fxEmbeddedEffect* embedded = fxEmbeddedShadersVk::Find(name);
	if (!embedded)
		return nullptr;

	std::string error;
	std::unique_ptr<fxEffectVk> effect = fxEmbeddedShadersVk::CreateVk(*embedded, &vkdGlobal::Device(), error);
	if (!effect)
	{
		DBG_ERROR("Built in shader " << name.c_str() << " failed to load: " << error.c_str());
		return nullptr;
	}
	return std::unique_ptr<fxEffect>(new effPlainEffectVk(std::move(effect), embedded->m_Source));
}

//------------------------------------------------------------------------
// ~effShaderArray()
//------------------------------------------------------------------------
effShaderArray::~effShaderArray()
{
}
//------------------------------------------------------------------------
// RegisterShader() - built-in shader registered under its old file name
//	("Phong.fx"), built from the embedded effect of the same name
//------------------------------------------------------------------------
template<class EFF_TYPE> void RegisterShader(std::string i_Name,
											 effShaderData* i_pDataTemplate,
											 std::map<std::string, matShaderInfo>& io_ShaderMap)
{
	std::unique_ptr<fxEffect> effect = LoadPlainBuiltIn(i_Name);
	if (!effect)
	{
		DBG_ERROR("Built in shader " <<  i_Name.c_str() << " load FAILED");
		throw g3dShaderLoadX(i_Name);
	}

	matShaderInfo info;
	info.m_Name = i_Name.c_str();
	info.m_DataTemplate = i_pDataTemplate;
	info.m_pEffect = new EFF_TYPE(fsLocator(), std::move(effect), i_Name);
	io_ShaderMap[i_Name] = info;
}

//------------------------------------------------------------------------
// RegisterPlainShader() - built-in effect registered under its effect name
//	("Blur"), embedded at build time (see fxEmbeddedShaders)
//------------------------------------------------------------------------
template<class EFF_TYPE> void RegisterPlainShader(std::string i_Name,
											 effShaderData* i_pDataTemplate,
											 std::map<std::string, matShaderInfo>& io_ShaderMap)
{
	const fxEmbeddedEffect* embedded = fxEmbeddedShadersVk::Find(i_Name);
	std::string error = "not embedded";
	std::unique_ptr<fxEffectVk> effect;
	if (embedded)
		effect = fxEmbeddedShadersVk::CreateVk(*embedded, &vkdGlobal::Device(), error);
	if (!effect)
	{
		DBG_ERROR("Built in shader " << i_Name.c_str() << " load FAILED: " << error.c_str());
		throw g3dShaderLoadX(i_Name);
	}

	matShaderInfo info;
	info.m_Name = i_Name.c_str();
	info.m_DataTemplate = i_pDataTemplate;
	info.m_pEffect = new EFF_TYPE(std::move(effect), i_Name);
	io_ShaderMap[i_Name] = info;
}

//------------------------------------------------------------------------
// get_sas_string() - a string annotation on the effect's SasGlobal
//	variable ("SasEffectDescription" etc.); empty if there is none.
//------------------------------------------------------------------------
static std::string get_sas_string(const fxEffectDesc& i_Desc, const char* i_Annotation)
{
	for (size_t v = 0; v < i_Desc.m_Variables.size(); v++)
	{
		const fxVariableDesc& var = i_Desc.m_Variables[v];
		if (var.m_Semantic != "SasGlobal")
			continue;
		for (size_t a = 0; a < var.m_Annotations.size(); a++)
		{
			if (var.m_Annotations[a].m_Name == i_Annotation)
				return var.m_Annotations[a].m_String;
		}
	}
	return std::string();
}

//------------------------------------------------------------------------
// make_shader_info() - picker entry for an embedded effect, named by its
//	old file name ("Phong.fx"), which is what materials store.
//------------------------------------------------------------------------
static matShaderInfo make_shader_info(const fxEmbeddedEffect& i_Effect, const fxEffectDesc& i_Desc)
{
	matShaderInfo info;
	info.m_Name = (std::string(i_Effect.m_Name) + ".fx").c_str();
	info.m_UIName = get_sas_string(i_Desc, "SasEffectDescription");
	info.m_HelpString = get_sas_string(i_Desc, "SasEffectHelp");
	info.m_bSupportsOutline = _stricmp(get_sas_string(i_Desc, "SupportsOutline").c_str(), "true") == 0;
	info.m_DataTemplate = NULL;
	info.m_pEffect = NULL;
	return info;
}

//------------------------------------------------------------------------
// RegisterSelectableShaders() - the embedded material shaders
//	(Materials/*, category "/material") and post effects (PostEffect/*)
//------------------------------------------------------------------------
void effShaderArray::RegisterSelectableShaders(std::vector<matShaderInfo>& o_Materials,
											   std::vector<matShaderInfo>& o_PostEffects)
{
	// material manifests that are not offered: Bake is used by the baking
	// pass only, Skin and BlinnSkin are old names the material parser
	// renames, Default and testtess are not user materials.
	static const char* const c_Hidden[] = { "Bake", "Skin", "BlinnSkin", "Default", "testtess" };

	for (int i = 0; i < fxEmbeddedShadersVk::k_NumEffects; i++)
	{
		const fxEmbeddedEffect& effect = fxEmbeddedShadersVk::k_Effects[i];
		const bool bMaterial = strncmp(effect.m_Source, "Materials/", 10) == 0;
		const bool bPostEffect = strncmp(effect.m_Source, "PostEffect/", 11) == 0;
		if (!bMaterial && !bPostEffect)
			continue;

		bool bHidden = false;
		for (size_t h = 0; h < sizeof(c_Hidden) / sizeof(c_Hidden[0]); h++)
		{
			if (_stricmp(effect.m_Name, c_Hidden[h]) == 0)
				bHidden = true;
		}
		if (bHidden)
			continue;

		fxEffectDesc desc;
		std::string error;
		if (!fxEffectDesc::Parse(effect.m_Manifest, desc, error))
		{
			DBG_ERROR("Built in shader " << effect.m_Source << " has a bad manifest: " << error.c_str());
			continue;
		}

		if (bMaterial)
		{
			if (get_sas_string(desc, "SasEffectCategory") == "/material")
				o_Materials.push_back(make_shader_info(effect, desc));
		}
		else
		{
			o_PostEffects.push_back(make_shader_info(effect, desc));
		}
	}
	DBG_LOG("Found " << (int)o_Materials.size() << " material shaders and " << (int)o_PostEffects.size() << " post effects");
}

//------------------------------------------------------------------------
// RegisterEffects() - Load effects from given directory
//------------------------------------------------------------------------
void effShaderArray::RegisterEffects(const fsLocator &i_ShaderDir,
			std::map<std::string, matShaderInfo>& io_ShaderMap)
{
	// need to reimplement effPhongData etc in terms of effShaderParams.
	// a shader's CreateData could then be creating a effShaderParams derived object.
	
	// default effect uses effPhong and effPhongData
	RegisterShader<effPhong>("default", new effPhongData, io_ShaderMap);
	RegisterShader<effPhong>("testtess.fx", new effPhongData, io_ShaderMap);

	RegisterShader<effBake>("Bake.fx",		new effBakeData, io_ShaderMap);

	// simple.fx could have a simplified shader class but phong works too...
	RegisterShader<effPhong>("Simple.fx",		new effPhongData, io_ShaderMap);
	RegisterShader<effPhong>("Phong.fx",		new effPhongData, io_ShaderMap);
	RegisterShader<effPhong>("Phong_wBump.fx",	new effPhongData, io_ShaderMap);

	RegisterPlainShader<effPlainShaderVk>	("HDRLighting",	NULL,		io_ShaderMap);
	RegisterPlainShader<effBlur>	("Blur",				new effBlurData, io_ShaderMap);
	RegisterPlainShader<effGlow>		("Glow",				new effGlowData, io_ShaderMap);
	RegisterPlainShader<effDOF>		("DOF",				new effDOFData, io_ShaderMap);
	RegisterShader<effLightGlow>	("LightGlow.fx",	new effLightGlowData, io_ShaderMap);
	RegisterShader<effParticle>		("Particle.fx",		new effParticleData, io_ShaderMap);
	RegisterPlainShader<effPlainShaderVk>	("PostAlphaMatte",	NULL,		io_ShaderMap);
	RegisterPlainShader<effPlainShaderVk>	("Fog",				NULL,		io_ShaderMap);

	// Constant solid color
	RegisterShader<effSolid>("Solid.fx", new effSolidData, io_ShaderMap);
	// Constant solid color, but skips values where texture's alpha is below threshold
	RegisterShader<effMaskAlpha>("MaskAlpha.fx", new effMaskAlphaData, io_ShaderMap);
	// depth map shader, pseudorandomly dithers values where texture's alpha is below threshold
	RegisterShader<effMaskAlpha>("DepthMap.fx", new effMaskAlphaData, io_ShaderMap);
	// depth only renderer
	RegisterShader<effMaskAlpha>("DepthRender.fx", new effMaskAlphaData, io_ShaderMap);
	// reflective shadow map shader
	RegisterShader<effMaskAlpha>("ReflectiveShadowMap.fx", new effMaskAlphaData, io_ShaderMap);

	RegisterPlainShader<effOutline>("Outline",	new effOutlineData, io_ShaderMap);

	RegisterPlainShader<effPlainShaderVk>("ssaoBilateralBlurEngine", new effOcclusionData, io_ShaderMap);
	RegisterPlainShader<effPlainShaderVk>("ssaoMultiHorizonBasedAO", new effOcclusionData, io_ShaderMap);
	RegisterPlainShader<effPlainShaderVk>("ssgiMultiHorizonBasedGI", new effOcclusionData, io_ShaderMap);
	RegisterShader<effOcclusion>("AOVolumes.fx", new effOcclusionData, io_ShaderMap);

	RegisterShader<effOcclusion>("GIVolumes.fx", new effOcclusionData, io_ShaderMap);

	RegisterShader<effBillboard>("Billboard.fx", new effBillboardData, io_ShaderMap);
	
	RegisterShader<effTextured>("Brushstroke.fx", new effTexturedData, io_ShaderMap);

	RegisterShader<effTextured>("VelocityRender.fx", new effTexturedData, io_ShaderMap);

	RegisterShader<effTextured>("IlluminationOnly.fx", new effTexturedData, io_ShaderMap);
	RegisterShader<effTextured>("ShadowsOnly.fx", new effTexturedData, io_ShaderMap);

	RegisterShader<effMaskAlpha>("NormalMap.fx", new effMaskAlphaData, io_ShaderMap);

	//RegisterShader<effTextured>("RSMGI.fx", new effTexturedData, io_ShaderMap);

	RegisterPlainShader<effPlainShaderVk>("MotionBlur", new effTexturedData, io_ShaderMap);

	RegisterPlainShader<effPlainShaderVk>("AAEdgeFilter", new effTexturedData, io_ShaderMap);

	RegisterShader<effTextured>("LPV_GI.fx", new effTexturedData, io_ShaderMap);

	RegisterShader<effRamp>("Ramp.fx", new effRampData, io_ShaderMap);

	RegisterShader<effStrandHair>("HairDefault.fx", new effStrandHairData, io_ShaderMap);

	// opacity renderer
	RegisterShader<effStrandHair>("OpacityRender.fx", new effStrandHairData, io_ShaderMap);

	RegisterShader<effTextured>("BlackAndWhite.fx", new effTexturedData, io_ShaderMap);

	RegisterShader<effTextured>("Sepia.fx", new effTexturedData, io_ShaderMap);

	RegisterShader<effTextured>("GradientMap.fx", new effTexturedData, io_ShaderMap);

	RegisterShader<effTextured>("Sketch.fx", new effTexturedData, io_ShaderMap);

	RegisterShader<effTextured>("EnvBackground.fx", new effTexturedData, io_ShaderMap);
}

//------------------------------------------------------------------------
// UnloadEffect()
//------------------------------------------------------------------------
void effShaderArray::UnloadEffect(const fsLocator& i_PathToShader,
								  std::map<fsLocator, matShaderEffect*>& io_ShaderMap)
{
	// Make sure shader is loaded in as a tokenized fsLocator
	std::string itName;
	fsLocator shaderLoc;	
	fsFileUtil::LocatorToUnicodeString( i_PathToShader, itName );	
	fsFileUtil::UnicodeStringToLocator( itName, shaderLoc );

	std::map<fsLocator, matShaderEffect*>::iterator it, end = io_ShaderMap.end();
	it = io_ShaderMap.find(shaderLoc);
	if( it != end )
	{
		delete (*it).second;
		io_ShaderMap.erase(shaderLoc);		
	}

}

//------------------------------------------------------------------------
// LoadPlainMaterial() - the built-in material shader named by an old .fx
//	file name: Lambert.fx loads the embedded Materials/Lambert.effect.json,
//	whatever folder the name is in. NULL if there is none (or it fails to
//	build).
//------------------------------------------------------------------------
static matShaderEffect* LoadPlainMaterial(const fsLocator& i_ShaderLoc, const std::string& i_FileName)
{
	std::string name = i_FileName;
	size_t dot = name.rfind('.');
	if (dot == std::string::npos || _stricmp(name.c_str() + dot, ".fx") != 0)
		return NULL;
	name.erase(dot);

	const fxEmbeddedEffect* embedded = fxEmbeddedShadersVk::Find(name);
	if (!embedded || strncmp(embedded->m_Source, "Materials/", 10) != 0)
		return NULL;

	std::string error;
	std::unique_ptr<fxEffectVk> effect = fxEmbeddedShadersVk::CreateVk(*embedded, &vkdGlobal::Device(), error);
	if (!effect)
	{
		DBG_ERROR("Built in material " << name.c_str() << " failed to load: " << error.c_str());
		return NULL;
	}

	DBG_LOG("Material " << i_FileName.c_str() << " loaded (" << embedded->m_Source << ")");
	fsLocator folder = i_ShaderLoc;
	folder.Pop();
	std::unique_ptr<fxEffect> plain(new effPlainEffectVk(std::move(effect), embedded->m_Source));
	if (plain->GetVariableByName("hasHairSupport")->IsValid())	//this is a hair effect
		return new effStrandHair(folder, std::move(plain), i_FileName);
	return new effShaderBaseVk(folder, std::move(plain), i_FileName);
}

//------------------------------------------------------------------------
// LoadEffect()
//------------------------------------------------------------------------
matShaderEffect* effShaderArray::LoadEffect(const fsLocator& i_PathToShader,
											std::map<fsLocator, matShaderEffect*>& io_ShaderMap)
{
	// Return pointer
	matShaderEffect* pEffect = NULL;

	fsLocator shaderLoc = matShaderMgr::ResolveShaderPath(i_PathToShader);

	// Get the shader name
	std::string shaderFileName = shaderLoc.GetLastName();

	pEffect = LoadPlainMaterial(shaderLoc, shaderFileName);
	if (pEffect)
		io_ShaderMap[i_PathToShader] = pEffect;
	else
	{
		// Only the built-in shaders can be loaded; .fx and .mfx files
		// (Effects 11 format) are not supported any more.
		DBG_WARNING("Shader " << shaderLoc << " is not a built-in shader and cannot be loaded.");
	}

	return pEffect;
}


}
