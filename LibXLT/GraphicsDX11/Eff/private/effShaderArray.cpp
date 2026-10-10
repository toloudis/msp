/****************************************************************************\
**  effShaderArray.hpp
**
**      effShaderArray manages an array of shaders that chooses the 
**	appropriate shader based on the combination of texture layers.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/eff/effShaderArray.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Core/Fs/fsAbsolutePathMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Graphics/eff/effBakeData.hpp"
#include "Graphics/eff/effBillboardData.hpp"
#include "Graphics/eff/effBlinnData.hpp"
#include "Graphics/eff/effBlurData.hpp"
#include "Graphics/eff/effDOFData.hpp"
#include "Graphics/eff/effNormalsData.hpp"
#include "Graphics/eff/effRendermanOverrideData.hpp"
#include "Graphics/eff/effGlowData.hpp"
#include "Graphics/eff/effHairData.hpp"
#include "Graphics/eff/effStrandHairData.hpp"
#include "Graphics/eff/effLambertData.hpp"
#include "Graphics/eff/effLightGlowData.hpp"
#include "Graphics/eff/effMaskAlphaData.hpp"
#include "Graphics/eff/effOcclusionData.hpp"
#include "Graphics/eff/effParticleData.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/eff/effRampData.hpp"
#include "Graphics/eff/effReflData.hpp"
#include "Graphics/eff/effSkinData.hpp"
#include "Graphics/eff/effSolidData.hpp"
//#include "Graphics/eff/effTexturedData.hpp"
#include "Graphics/eff/effToonData.hpp"
#include "Graphics/eff/effWaterData.hpp"
#include "Graphics/g3d/g3dExceptionX.hpp"
#include "Graphics/mat/matExceptionX.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "GraphicsDX11/eff/effBake.hpp"
#include "GraphicsDX11/eff/effBillboard.hpp"
#include "GraphicsDX11/eff/effBlur.hpp"
#include "GraphicsDX11/eff/effDOF.hpp"
#include "GraphicsDX11/eff/effGlow.hpp"
//#include "GraphicsDX11/eff/effHair.hpp"
#include "GraphicsDX11/eff/effStrandHair.hpp"
//#include "GraphicsDX11/eff/effLambert.hpp"
#include "GraphicsDX11/eff/effLightGlow.hpp"
#include "GraphicsDX11/eff/effMaskAlpha.hpp"
#include "GraphicsDX11/eff/effOcclusion.hpp"
#include "GraphicsDX11/eff/effOutline.hpp"
#include "GraphicsDX11/eff/effParticle.hpp"
#include "GraphicsDX11/eff/effPhong.hpp"
#include "GraphicsDX11/eff/effRamp.hpp"
#include "GraphicsDX11/eff/effShaderUtilWin.hpp"
#include "GraphicsDX11/eff/effSolid.hpp"
//#include "GraphicsDX11/eff/effTextured.hpp"
//#include "GraphicsDX11/eff/effToon.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/Fx/fxEmbeddedShaders.hpp"
#include "GraphicsDX11/eff/effPlainEffect.hpp"

//#include "Tool/gui/guiMessageBox.hpp"
#include "GraphicsDX11/eff/private/PSMapNormalsToScreen.hpp"

#include <map>

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

	const fxEmbeddedEffect* embedded = fxEmbeddedShaders::Find(name);
	if (!embedded)
		return nullptr;

	std::string error;
	std::unique_ptr<fxEffectDX11> effect = fxEmbeddedShaders::CreateDX11(*embedded, g2dDX11Global::g_pDevice, error);
	if (!effect)
	{
		DBG_ERROR("Built in shader " << name.c_str() << " failed to load: " << error.c_str());
		return nullptr;
	}
	return std::unique_ptr<fxEffect>(new effPlainEffect(std::move(effect), embedded->m_Source));
}

namespace
{
	//ID3DBlob* l_DefaultVS = NULL;
	void* l_DefaultVSData = NULL;
	SIZE_T l_DefaultVSSize = 0;
	ID3DBlob* l_DefaultPS = NULL;
	ID3D11PixelShader *l_DefaultPSShader = NULL;
	ID3D11VertexShader *l_DefaultVSShader = NULL;
	ID3D11PixelShader *l_DefaultPhongPS = NULL;

	ID3D11PixelShader* l_PSMapNormalsToScreen = NULL;

	effShaderBaseDX11* l_pDefaultEffect = NULL;
}	// end of namespace

//------------------------------------------------------------------------
// ~effShaderArray()
//------------------------------------------------------------------------
effShaderArray::~effShaderArray()
{
	// assumes singleton.
	l_pDefaultEffect = NULL;

	if (l_DefaultVSShader)
	{
		delete [] l_DefaultVSData;
		l_DefaultVSData= NULL;

		l_DefaultVSShader->Release();
		l_DefaultVSShader = NULL;
	}
	if (l_DefaultPS)
	{
		l_DefaultPS->Release();
		l_DefaultPS = NULL;
		l_DefaultPSShader->Release();
		l_DefaultPSShader = NULL;
	}
	if (l_DefaultPhongPS)
	{
		l_DefaultPhongPS->Release();
		l_DefaultPhongPS = NULL;
	}

	if (l_PSMapNormalsToScreen)
		l_PSMapNormalsToScreen->Release();

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
	const fxEmbeddedEffect* embedded = fxEmbeddedShaders::Find(i_Name);
	std::string error = "not embedded";
	std::unique_ptr<fxEffectDX11> effect;
	if (embedded)
		effect = fxEmbeddedShaders::CreateDX11(*embedded, g2dDX11Global::g_pDevice, error);
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

	for (int i = 0; i < fxEmbeddedShaders::k_NumEffects; i++)
	{
		const fxEmbeddedEffect& effect = fxEmbeddedShaders::k_Effects[i];
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

	RegisterPlainShader<effPlainShaderDX11>	("HDRLighting",	NULL,		io_ShaderMap);
	RegisterPlainShader<effBlur>	("Blur",				new effBlurData, io_ShaderMap);
	RegisterPlainShader<effGlow>		("Glow",				new effGlowData, io_ShaderMap);
	RegisterPlainShader<effDOF>		("DOF",				new effDOFData, io_ShaderMap);
	RegisterShader<effLightGlow>	("LightGlow.fx",	new effLightGlowData, io_ShaderMap);
	RegisterShader<effParticle>		("Particle.fx",		new effParticleData, io_ShaderMap);
	RegisterPlainShader<effPlainShaderDX11>	("PostAlphaMatte",	NULL,		io_ShaderMap);
	RegisterPlainShader<effPlainShaderDX11>	("Fog",				NULL,		io_ShaderMap);

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

	RegisterPlainShader<effPlainShaderDX11>("ssaoBilateralBlurEngine", new effOcclusionData, io_ShaderMap);
	RegisterPlainShader<effPlainShaderDX11>("ssaoMultiHorizonBasedAO", new effOcclusionData, io_ShaderMap);
	RegisterPlainShader<effPlainShaderDX11>("ssgiMultiHorizonBasedGI", new effOcclusionData, io_ShaderMap);
	RegisterShader<effOcclusion>("AOVolumes.fx", new effOcclusionData, io_ShaderMap);

	RegisterShader<effOcclusion>("GIVolumes.fx", new effOcclusionData, io_ShaderMap);

	RegisterShader<effBillboard>("Billboard.fx", new effBillboardData, io_ShaderMap);
	
	RegisterShader<effTextured>("Brushstroke.fx", new effTexturedData, io_ShaderMap);

	RegisterShader<effTextured>("VelocityRender.fx", new effTexturedData, io_ShaderMap);

	RegisterShader<effTextured>("IlluminationOnly.fx", new effTexturedData, io_ShaderMap);
	RegisterShader<effTextured>("ShadowsOnly.fx", new effTexturedData, io_ShaderMap);

	RegisterShader<effMaskAlpha>("NormalMap.fx", new effMaskAlphaData, io_ShaderMap);

	//RegisterShader<effTextured>("RSMGI.fx", new effTexturedData, io_ShaderMap);

	RegisterPlainShader<effPlainShaderDX11>("MotionBlur", new effTexturedData, io_ShaderMap);

	RegisterPlainShader<effPlainShaderDX11>("AAEdgeFilter", new effTexturedData, io_ShaderMap);

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
	itString itName;
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

	const fxEmbeddedEffect* embedded = fxEmbeddedShaders::Find(name);
	if (!embedded || strncmp(embedded->m_Source, "Materials/", 10) != 0)
		return NULL;

	std::string error;
	std::unique_ptr<fxEffectDX11> effect = fxEmbeddedShaders::CreateDX11(*embedded, g2dDX11Global::g_pDevice, error);
	if (!effect)
	{
		DBG_ERROR("Built in material " << name.c_str() << " failed to load: " << error.c_str());
		return NULL;
	}

	DBG_LOG("Material " << i_FileName.c_str() << " loaded (" << embedded->m_Source << ")");
	fsLocator folder = i_ShaderLoc;
	folder.Pop();
	std::unique_ptr<fxEffect> plain(new effPlainEffect(std::move(effect), embedded->m_Source));
	if (plain->GetVariableByName("hasHairSupport")->IsValid())	//this is a hair effect
		return new effStrandHair(folder, std::move(plain), i_FileName);
	return new effShaderBaseDX11(folder, std::move(plain), i_FileName);
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
	std::string shaderFileName = itStringUtil::GetStdString(shaderLoc.GetLastName());

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

//effShaderBaseDX11* effShaderArray::GetDefaultEffect()
//{
//	return l_pDefaultEffect;
//}

void effShaderArray::GetDefaultEffect(void** o_pShaderBytecode, unsigned long* o_bytecodeLength,
									  ID3D11VertexShader** o_pVS, ID3D11PixelShader** o_pPS )
{
	fsLocator f(itString("C:\\Projects\\SourceCode - SIGGRAPH09\\LibXLT\\GraphicsDX11\\Eff\\private\\Shaders\\default.fx"));

	if (!l_DefaultPSShader)
	{
		void* pBytes = NULL;
		SIZE_T numBytes = 0;
		HRESULT hr = effShaderUtilWin::LoadShaderFromFile( L"testPS.o", &numBytes, &pBytes );
		hr = ( g2dDX11Global::g_pDevice->CreatePixelShader( pBytes, numBytes, NULL, &l_DefaultPSShader ) );
		delete [] pBytes;

	}

	if (!l_DefaultPhongPS)
	{
		void* pBytes = NULL;
		SIZE_T numBytes = 0;
		HRESULT hr = effShaderUtilWin::LoadShaderFromFile( L"singleLightPS.o", &numBytes, &pBytes );
		hr = ( g2dDX11Global::g_pDevice->CreatePixelShader( pBytes, numBytes, NULL, &l_DefaultPhongPS ) );
		delete [] pBytes;
	}

	if (!l_DefaultVSShader)
	{
		HRESULT hr = effShaderUtilWin::LoadShaderFromFile( L"VS_Default.o", &l_DefaultVSSize, &l_DefaultVSData );
		hr = ( g2dDX11Global::g_pDevice->CreateVertexShader( l_DefaultVSData, l_DefaultVSSize, NULL, &l_DefaultVSShader ) );
	}
	*o_pShaderBytecode = l_DefaultVSData;
	*o_bytecodeLength = (unsigned long)l_DefaultVSSize;
	*o_pVS = l_DefaultVSShader;
	*o_pPS = l_DefaultPSShader;
}

void effShaderArray::GetDefaultLightingEffect(ID3D11VertexShader** o_pVS, ID3D11PixelShader** o_pPS)
{
	*o_pVS = l_DefaultVSShader;
	*o_pPS = l_DefaultPhongPS;
}

ID3D11PixelShader* effShaderArray::GetMapNormalsToScreen()
{
	// lazy init?
	if (l_PSMapNormalsToScreen == NULL)
	{
		HRESULT hr = ( g2dDX11Global::g_pDevice->CreatePixelShader( 
			g_PSMapNormalsToScreen, sizeof(g_PSMapNormalsToScreen), 
			NULL, &l_PSMapNormalsToScreen) );
	}
	return l_PSMapNormalsToScreen;
}
