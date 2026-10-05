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
#include "Core/gf/gfFileEnum.hpp"
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
#include "Graphics/Eff/effShaderSDK.hpp"
#include "Graphics/eff/effSkinData.hpp"
#include "Graphics/eff/effSolidData.hpp"
//#include "Graphics/eff/effTexturedData.hpp"
#include "Graphics/eff/effToonData.hpp"
#include "Graphics/eff/effWaterData.hpp"
#include "Graphics/g3d/g3dExceptionX.hpp"
#include "Graphics/mat/matExceptionX.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/Mat/matMetaFX.hpp"
#include "Graphics/Mat/matMetaFXParser.hpp"
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
#include "GraphicsDX11/eff/effFX11Effect.hpp"
#include "GraphicsDX11/eff/effPlainEffect.hpp"

// built-in shaders:
#include "GraphicsDX11/eff/private/ShaderBake.hpp"
#include "GraphicsDX11/eff/private/ShaderDefault.hpp"
#include "GraphicsDX11/eff/private/ShaderPhong.hpp"
#include "GraphicsDX11/eff/private/ShaderSimple.hpp"
#include "GraphicsDX11/eff/private/ShaderPhong_wBump.hpp"
#include "GraphicsDX11/eff/private/ShaderLightGlow.hpp"
#include "GraphicsDX11/eff/private/ShaderParticle.hpp"
#include "GraphicsDX11/eff/private/ShaderSolid.hpp"
#include "GraphicsDX11/eff/private/ShaderMaskAlpha.hpp"
#include "GraphicsDX11/eff/private/ShaderDepthMap.hpp"
#include "GraphicsDX11/eff/private/ShaderDepthRender.hpp"
#include "GraphicsDX11/eff/private/ShaderBillboard.hpp"
#include "GraphicsDX11/eff/private/ShaderBrushstroke.hpp"
#include "GraphicsDX11/eff/private/ShaderVelocityRender.hpp"
#include "GraphicsDX11/eff/private/ShaderIlluminationOnly.hpp"
#include "GraphicsDX11/eff/private/ShaderShadowsOnly.hpp"
#include "GraphicsDX11/eff/private/ShaderNormalMap.hpp"
#include "GraphicsDX11/eff/private/ShaderRamp.hpp"
#include "GraphicsDX11/eff/private/ShaderReflectiveShadowMap.hpp"
//#include "GraphicsDX11/eff/private/ShaderRSMGI.hpp"
#include "GraphicsDX11/eff/private/ShaderOpacityRender.hpp"
#include "GraphicsDX11/eff/private/ShaderHairDefault.hpp"
#include "GraphicsDX11/eff/private/Shadertesttess.hpp"
#include "GraphicsDX11/eff/private/ShaderAOVolumes.hpp"
#include "GraphicsDX11/eff/private/ShaderGIVolumes.hpp"
#include "GraphicsDX11/eff/private/ShaderLPV_GI.hpp"
#include "GraphicsDX11/eff/private/ShaderBlackAndWhite.hpp"
#include "GraphicsDX11/eff/private/ShaderSepia.hpp"
#include "GraphicsDX11/eff/private/ShaderGradientMap.hpp"
#include "GraphicsDX11/eff/private/ShaderSketch.hpp"
#include "GraphicsDX11/eff/private/ShaderEnvBackground.hpp"

//#include "Tool/gui/guiMessageBox.hpp"
#include "GraphicsDX11/eff/private/PSMapNormalsToScreen.hpp"

#include <map>

//------------------------------------------------------------------------
// OwnedFX11() - an Effects (.fx) effect behind the fxEffect interface; the
//	wrapper takes over the caller's reference.
//------------------------------------------------------------------------
static std::unique_ptr<fxEffect> OwnedFX11(ID3DX11Effect* i_pEffect)
{
	return std::unique_ptr<fxEffect>(new effFX11Effect(i_pEffect, true));
}

//------------------------------------------------------------------------
// LoadPlainBuiltIn() - the embedded plain-HLSL conversion of a built-in
//	effect: "Phong.fx" or "Phong" loads the embedded "Phong". NULL if there
//	is none or it fails to build, so the caller falls back to the .fx.
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
		DBG_ERROR("Built in shader " << name.c_str() << " failed to load as plain HLSL, using the .fx: " << error.c_str());
		return nullptr;
	}
	return std::unique_ptr<fxEffect>(new effPlainEffect(std::move(effect)));
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

	fsLocator l_ShaderDir;

	//------------------------------------------------------------------------
	// class ShaderLoader
	//------------------------------------------------------------------------
	class ShaderLoader 
	{
	public:
		virtual matShaderEffect* LoadShader(const std::string& i_Name,
			std::map<std::string, matShaderInfo>& io_ShaderMap) = 0;

		virtual ~ShaderLoader() {}
	};

	//------------------------------------------------------------------------
	// class TypedShaderLoader
	//------------------------------------------------------------------------
	template <class EFF_TYPE> class TypedShaderLoader : public ShaderLoader
	{
	public:
		TypedShaderLoader(const fsLocator& i_Locator)
		{
			m_Locator = i_Locator;
		}
		fsLocator m_Locator;

		//------------------------------------------------------------------------
		// LoadShader()
		//------------------------------------------------------------------------
		matShaderEffect* LoadShader(const std::string& i_Name,
			std::map<std::string, matShaderInfo>& io_ShaderMap)
		{
			m_Locator.Push(i_Name.c_str());

			fsResourceTracker::MarkBegin(m_Locator);

			LPD3DXEFFECT effect = effShaderUtilWin::LoadEffectDX11(m_Locator);

			fsResourceTracker::MarkEnd(m_Locator);
			m_Locator.Pop();

			matShaderEffect* pEffect = NULL;
			if (effect != NULL)
			{
				pEffect = new EFF_TYPE(m_Locator, OwnedFX11(effect), i_Name);
				//DBG_LOG( "Built in shader = " << i_Name.c_str());
				io_ShaderMap[i_Name].m_pEffect = pEffect;
			}
			else
			{
				DBG_ERROR("Built in shader " <<  i_Name.c_str() << " load FAILED using path: " << m_Locator);
				throw g3dShaderLoadX(i_Name);
			}
			return pEffect;
		}
	};

	//------------------------------------------------------------------------
	// class BinaryShaderLoader
	//------------------------------------------------------------------------
	template <class EFF_TYPE> class BinaryShaderLoader : public ShaderLoader
	{
	public:
		BinaryShaderLoader(const BYTE* i_Bits, DWORD i_Size)
		{
			m_Bits = i_Bits;
			m_Size = i_Size;
		}
		const BYTE* m_Bits;
		DWORD m_Size;

		//------------------------------------------------------------------------
		// LoadShader()
		//------------------------------------------------------------------------
		matShaderEffect* LoadShader(const std::string& i_Name,
			std::map<std::string, matShaderInfo>& io_ShaderMap)
		{

			LPD3DXEFFECT effect = effShaderUtilWin::LoadEffectData((void*)m_Bits, m_Size);

			matShaderEffect* pEffect = NULL;
			if (effect != NULL)
			{
				pEffect = new EFF_TYPE(fsLocator(), OwnedFX11(effect), i_Name);
				//DBG_LOG( "Built in shader = " << i_Name.c_str());
				io_ShaderMap[i_Name].m_pEffect = pEffect;
			}
			else
			{
				DBG_ERROR("Built in shader " <<  i_Name.c_str() << " load FAILED");
				throw g3dShaderLoadX(i_Name);
			}
			return pEffect;
		}
	};
	std::map<std::string, ShaderLoader*> l_ShaderRegistry;

	effShaderBaseDX11* l_pDefaultEffect = NULL;
}	// end of namespace

//------------------------------------------------------------------------
// ~effShaderArray()
//------------------------------------------------------------------------
effShaderArray::~effShaderArray()
{
	// Note: the l_ShaderRegistry variable really should be a member of the class
	std::map<std::string, ShaderLoader*>::iterator it = l_ShaderRegistry.begin();
	while (it != l_ShaderRegistry.end())
	{
		delete it->second;
		++it;
	}
	l_ShaderRegistry.clear();

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
// RegisterShader()
//------------------------------------------------------------------------
template<class EFF_TYPE> void RegisterShader(std::string i_Name, 
											 effShaderData* i_pDataTemplate, const BYTE* i_ShaderBits,
											 int i_ShaderSizeBytes,
											 std::map<std::string, matShaderInfo>& io_ShaderMap)
{
	matShaderInfo info;
	info.m_Name = i_Name.c_str();
	info.m_DataTemplate = i_pDataTemplate;
	info.m_pEffect = NULL;

//	BinaryShaderLoader<EFF_TYPE>* loader = new BinaryShaderLoader<EFF_TYPE>(i_ShaderBits, i_ShaderSizeBytes);
//	loader->LoadShader(i_Name, io_ShaderMap);
//	delete loader;

	// prefer the plain-HLSL conversion when one is built in
	std::unique_ptr<fxEffect> effect = LoadPlainBuiltIn(i_Name);
	if (effect)
		DBG_LOG("Built in shader " << i_Name.c_str() << " loaded as plain HLSL");
	else
	{
		ID3DX11Effect* pD3DXEffect = effShaderUtilWin::LoadEffectData((void*)i_ShaderBits, i_ShaderSizeBytes, i_Name );
		if (pD3DXEffect != NULL)
			effect = OwnedFX11(pD3DXEffect);
	}
	matShaderEffect* pEffect = NULL;
	if (effect)
	{
		pEffect = new EFF_TYPE(fsLocator(), std::move(effect), i_Name);
		//DBG_LOG( "Built in shader = " << i_Name.c_str());
		info.m_pEffect = pEffect;

		// successful load:
		// add to shader map
		io_ShaderMap[i_Name] = info;
	}
	else
	{
		DBG_ERROR("Built in shader " <<  i_Name.c_str() << " load FAILED");
		throw g3dShaderLoadX(i_Name);
	}
}

//------------------------------------------------------------------------
// RegisterPlainShader() - built-in effect converted from .fx to plain HLSL,
//	embedded at build time (see fxEmbeddedShaders)
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
// RegisterSingleUserShader()
//------------------------------------------------------------------------
void RegisterSingleUserShader(fsLocator i_ShaderName, std::vector<matShaderInfo>& o_Shaders)
{
	ID3DX11Effect* effect = effShaderUtilWin::LoadEffectDX11(i_ShaderName);
	if (effect)
	{
		ID3DX11EffectVariable* hGlobal = effect->GetVariableBySemantic("SasGlobal");
		if (hGlobal != NULL)
		{
			matShaderInfo info;
			info.m_Name = i_ShaderName.GetLastName();

			ID3DX11EffectVariable* hAnnot = NULL;
			hAnnot = hGlobal->GetAnnotationByName("SasEffectDescription");
			if (hAnnot)
			{
			    LPCSTR pstrName = NULL;
				ID3DX11EffectStringVariable* hString = hAnnot->AsString();
				if (hString)
					hString->GetString( &pstrName );
				info.m_UIName = pstrName;
			}
			hAnnot = hGlobal->GetAnnotationByName("SasEffectHelp");
			if (hAnnot)
			{
			    LPCSTR pstrName = NULL;
				ID3DX11EffectStringVariable* hString = hAnnot->AsString();
				if (hString)
					hString->GetString( &pstrName );
				info.m_HelpString = pstrName;
			}
			hAnnot = hGlobal->GetAnnotationByName("SupportsOutline");
			if (hAnnot)
			{
			    LPCSTR pstrName = NULL;
				ID3DX11EffectStringVariable* hString = hAnnot->AsString();
				if (hString)
					hString->GetString( &pstrName );
				info.m_bSupportsOutline = _stricmp( pstrName, "true" ) == 0 ? true : false;
			}

			info.m_DataTemplate = NULL;
			info.m_pEffect = NULL;

			o_Shaders.push_back(info);
			//DBG_LOG("Found User Shader: " << info.m_Name << " ; " << info.m_UIName);
		}
		effect->Release();
		effect = NULL;
	}
	else
	{
		DBG_WARNING("Error trying to load " << i_ShaderName << " as a shader.");
	}
}

//------------------------------------------------------------------------
// RegisterUserShaders()
//------------------------------------------------------------------------
void effShaderArray::RegisterUserShaders(const fsLocator& i_ShaderDir, std::vector<matShaderInfo>& o_Shaders)
{
	DBG_LOG( "Loading user shaders from directory " << i_ShaderDir );
	l_ShaderDir = i_ShaderDir;

	// flist is a list of locators.
	fsFileEnum::fsFileList flist;
	std::vector<itString> searchStrings;

	searchStrings.push_back(itString(".fx"));
	bool ok = fsFileEnum::EnumerateFiles(i_ShaderDir, flist, searchStrings);

	for (int i = 0; i < flist.size(); i++)
	{
		DBG_LOG( "Loading user shader " << flist[i] );
		RegisterSingleUserShader( flist[i] , o_Shaders );		
	}
	//	o_Shaders.sort();

//	LoadAllShaders(o_Shaders);

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
	RegisterShader<effPhong>("default", new effPhongData, (BYTE*)g_ShaderDefault, sizeof(g_ShaderDefault), io_ShaderMap);
	RegisterShader<effPhong>("testtess.fx", new effPhongData, (BYTE*)g_Shadertesttess, sizeof(g_Shadertesttess), io_ShaderMap);

	RegisterShader<effBake>("Bake.fx",		new effBakeData, g_ShaderBake, sizeof(g_ShaderBake), io_ShaderMap);

	// simple.fx could have a simplified shader class but phong works too...
	RegisterShader<effPhong>("Simple.fx",		new effPhongData, g_ShaderSimple, sizeof(g_ShaderSimple), io_ShaderMap);
	RegisterShader<effPhong>("Phong.fx",		new effPhongData, g_ShaderPhong, sizeof(g_ShaderPhong), io_ShaderMap);
	RegisterShader<effPhong>("Phong_wBump.fx",	new effPhongData, g_ShaderPhong_wBump, sizeof(g_ShaderPhong_wBump), io_ShaderMap);

	RegisterPlainShader<effPlainShaderDX11>	("HDRLighting",	NULL,		io_ShaderMap);
	RegisterPlainShader<effBlur>	("Blur",				new effBlurData, io_ShaderMap);
	RegisterPlainShader<effGlow>		("Glow",				new effGlowData, io_ShaderMap);
	RegisterPlainShader<effDOF>		("DOF",				new effDOFData, io_ShaderMap);
	RegisterShader<effLightGlow>	("LightGlow.fx",	new effLightGlowData, g_ShaderLightGlow, sizeof(g_ShaderLightGlow), io_ShaderMap);
	RegisterShader<effParticle>		("Particle.fx",		new effParticleData, g_ShaderParticle, sizeof(g_ShaderParticle), io_ShaderMap);
	RegisterPlainShader<effPlainShaderDX11>	("PostAlphaMatte",	NULL,		io_ShaderMap);
	RegisterPlainShader<effPlainShaderDX11>	("Fog",				NULL,		io_ShaderMap);

	// Constant solid color
	RegisterShader<effSolid>("Solid.fx", new effSolidData, g_ShaderSolid, sizeof(g_ShaderSolid), io_ShaderMap);
	// Constant solid color, but skips values where texture's alpha is below threshold
	RegisterShader<effMaskAlpha>("MaskAlpha.fx", new effMaskAlphaData, g_ShaderMaskAlpha, sizeof(g_ShaderMaskAlpha), io_ShaderMap);
	// depth map shader, pseudorandomly dithers values where texture's alpha is below threshold
	RegisterShader<effMaskAlpha>("DepthMap.fx", new effMaskAlphaData, g_ShaderDepthMap, sizeof(g_ShaderDepthMap), io_ShaderMap);
	// depth only renderer
	RegisterShader<effMaskAlpha>("DepthRender.fx", new effMaskAlphaData, g_ShaderDepthRender, sizeof(g_ShaderDepthRender), io_ShaderMap);
	// reflective shadow map shader
	RegisterShader<effMaskAlpha>("ReflectiveShadowMap.fx", new effMaskAlphaData, g_ShaderReflectiveShadowMap, sizeof(g_ShaderReflectiveShadowMap), io_ShaderMap);

	RegisterPlainShader<effOutline>("Outline",	new effOutlineData, io_ShaderMap);

	RegisterPlainShader<effPlainShaderDX11>("ssaoBilateralBlurEngine", new effOcclusionData, io_ShaderMap);
	RegisterPlainShader<effPlainShaderDX11>("ssaoMultiHorizonBasedAO", new effOcclusionData, io_ShaderMap);
	RegisterPlainShader<effPlainShaderDX11>("ssgiMultiHorizonBasedGI", new effOcclusionData, io_ShaderMap);
	RegisterShader<effOcclusion>("AOVolumes.fx", new effOcclusionData, g_ShaderAOVolumes, sizeof(g_ShaderAOVolumes), io_ShaderMap);

	RegisterShader<effOcclusion>("GIVolumes.fx", new effOcclusionData, g_ShaderGIVolumes, sizeof(g_ShaderGIVolumes), io_ShaderMap);

	RegisterShader<effBillboard>("Billboard.fx", new effBillboardData, g_ShaderBillboard, sizeof(g_ShaderBillboard), io_ShaderMap);
	
	RegisterShader<effTextured>("Brushstroke.fx", new effTexturedData, g_ShaderBrushstroke, sizeof(g_ShaderBrushstroke), io_ShaderMap);

	RegisterShader<effTextured>("VelocityRender.fx", new effTexturedData, g_ShaderVelocityRender, sizeof(g_ShaderVelocityRender), io_ShaderMap);

	RegisterShader<effTextured>("IlluminationOnly.fx", new effTexturedData, g_ShaderIlluminationOnly, sizeof(g_ShaderIlluminationOnly), io_ShaderMap);
	RegisterShader<effTextured>("ShadowsOnly.fx", new effTexturedData, g_ShaderShadowsOnly, sizeof(g_ShaderShadowsOnly), io_ShaderMap);

	RegisterShader<effMaskAlpha>("NormalMap.fx", new effMaskAlphaData, g_ShaderNormalMap, sizeof(g_ShaderNormalMap), io_ShaderMap);

	//RegisterShader<effTextured>("RSMGI.fx", new effTexturedData, g_ShaderRSMGI, sizeof(g_ShaderRSMGI), io_ShaderMap);

	RegisterPlainShader<effPlainShaderDX11>("MotionBlur", new effTexturedData, io_ShaderMap);

	RegisterPlainShader<effPlainShaderDX11>("AAEdgeFilter", new effTexturedData, io_ShaderMap);

	RegisterShader<effTextured>("LPV_GI.fx", new effTexturedData, g_ShaderLPV_GI, sizeof(g_ShaderLPV_GI), io_ShaderMap);

	RegisterShader<effRamp>("Ramp.fx", new effRampData, g_ShaderRamp, sizeof(g_ShaderRamp), io_ShaderMap);

	RegisterShader<effStrandHair>("HairDefault.fx", new effStrandHairData, g_ShaderHairDefault, sizeof(g_ShaderHairDefault), io_ShaderMap);

	// opacity renderer
	RegisterShader<effStrandHair>("OpacityRender.fx", new effStrandHairData, g_ShaderOpacityRender, sizeof(g_ShaderOpacityRender), io_ShaderMap);

	RegisterShader<effTextured>("BlackAndWhite.fx", new effTexturedData, g_ShaderBlackAndWhite, sizeof(g_ShaderBlackAndWhite), io_ShaderMap);

	RegisterShader<effTextured>("Sepia.fx", new effTexturedData, g_ShaderSepia, sizeof(g_ShaderSepia), io_ShaderMap);

	RegisterShader<effTextured>("GradientMap.fx", new effTexturedData, g_ShaderGradientMap, sizeof(g_ShaderGradientMap), io_ShaderMap);

	RegisterShader<effTextured>("Sketch.fx", new effTexturedData, g_ShaderSketch, sizeof(g_ShaderSketch), io_ShaderMap);

	RegisterShader<effTextured>("EnvBackground.fx", new effTexturedData, g_ShaderEnvBackground, sizeof(g_ShaderEnvBackground), io_ShaderMap);
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
// LoadPlainMaterial() - a material shader converted to plain HLSL is built
//	in, and replaces the .fx of the same name: Lambert.fx loads the
//	embedded Materials/Lambert.effect.json. NULL if there is none (or it
//	fails to build), so the .fx is loaded instead.
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
		DBG_ERROR("Built in material " << name.c_str() << " failed to load, using " << i_FileName.c_str() << ": " << error.c_str());
		return NULL;
	}

	DBG_LOG("Material " << i_FileName.c_str() << " loaded as plain HLSL (" << embedded->m_Source << ")");
	fsLocator folder = i_ShaderLoc;
	folder.Pop();
	std::unique_ptr<fxEffect> plain(new effPlainEffect(std::move(effect)));
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
	{
		io_ShaderMap[i_PathToShader] = pEffect;
		return pEffect;
	}

	ID3DX11Effect* effect = NULL;

	itString ext;
	shaderLoc.GetLastName().GetExtension(ext);
	itStringUtil::ToLower(ext);
	if (ext == itString("mfx"))
	{
		// Load shader from multiple format effect file (MFX)
		matMetaFX shader_data;
		try
		{
			matMetaFXParser::ReadMetaFX( shaderLoc, shader_data );
		}
		catch (fsFileExceptionX& i_Ex)
		{
			DBG_ERROR(i_Ex.GetErrorMessage());
			// continue with unloaded shader which will return NULL.
		}
		//DBG_LOG("MFX Shader name: " << shader_data.m_ShaderName);

		// If we have compiiled FX data (have to check for correct version),
		// then load that data as the shader.
		bool bUseCompiledFX = (shader_data.m_CompiledFX.m_bHasData &&
			matMetaFX::CompareVersion(shader_data.m_CompiledFX.m_Version, matMetaFX::GetCurrentFXVersion()));
		if (bUseCompiledFX)
		{
			// Load shader from binary data
			effect =  effShaderUtilWin::LoadEffectData(&shader_data.m_CompiledFX.m_Data[0], 
														shader_data.m_CompiledFX.m_Data.size(), 
														shaderFileName);
		}
		else
		{
			bool bUseHLSLSource = (shader_data.m_HLSLSource.m_bHasData &&
				matMetaFX::CompareVersion(shader_data.m_HLSLSource.m_Version, matMetaFX::GetCurrentHLSLVersion()));
			if (bUseHLSLSource)
			{	
				// Try to compile shader from HLSL source, adding in the most recent
				// header and footer.
				shared_ptr<effShaderSDKData> compiledShaderData;
				std::vector<std::string> errors;

				// Compile stitched shader in memory.
				// In this case, the string we are giving is a section of HLSL code that does not
				// contain fragment and pixel shader hooks. This code has a function with the
				// signature "float4 sgpu_shader_main(State state, Light_iterator light)"
				// that will be called from the header and footer code.
				bool success = effShaderSDK::CompileCustomShadeFunction( shader_data.m_HLSLSource.m_Data.c_str(), 
												 compiledShaderData, 
												 errors, shaderFileName );

				if (success && compiledShaderData)
				{
					DBG_LOG("Recompiled user shader " << shaderLoc << ", load time could be improved by generating a new shader file with the latest version.");

					// Get shader data as pointer. Ownership maintained by effShaderSDKData class above.
					int shaderSize = 0;
					BYTE * pCompiledShader = NULL;
					compiledShaderData->GetData(&pCompiledShader, &shaderSize);

					// Compile as DX11 effect
					effect =  effShaderUtilWin::LoadEffectData(pCompiledShader, shaderSize, shaderFileName);
				}
				else
				{
					DBG_ERROR("Could not compile user shader " << shaderLoc << " with current lighting engine.");
				}
			}
			else
			{
				// If we had MetaSL linked with MSP, then we could recompile the MetaSL code 
				// into a new FX shader. But since we don't have that now, throw an exception
				// about not being able to load the shader.
				//throw matIncorrectShaderVersionX(shaderLoc);
				// Or maybe we should log an error and then return NULL?
				DBG_ERROR("Shader file: " << shaderLoc << " is from an unsupported version.");

			}
		}
	}
	else
	{
		// Load shader as compiled FX
		effect = effShaderUtilWin::LoadEffectDX11(shaderLoc);
	}

	// If shader loaded successfully
	if ( effect != NULL )
	{
		fsLocator folder = shaderLoc;
		folder.Pop();

		ID3DX11EffectVariable* hParm = effect->GetVariableByName( "hasHairSupport" );
		if( hParm->IsValid())	//this is a hair effect 
		{
			pEffect = new effStrandHair(folder, OwnedFX11(effect), shaderFileName);
		}
		else
		{
			pEffect = new effShaderBaseDX11(folder, effect, shaderFileName);
		}
		io_ShaderMap[i_PathToShader] = pEffect;		
	}

	return pEffect;
}

//------------------------------------------------------------------------
// Load registered (internally known) shaders
//------------------------------------------------------------------------
void effShaderArray::LoadAllShaders(std::map<std::string, matShaderInfo>& io_ShaderMap)
{
	std::map<std::string, ShaderLoader*>::iterator it = l_ShaderRegistry.begin();
	while (it != l_ShaderRegistry.end())
	{
		it->second->LoadShader(it->first, io_ShaderMap);
		++it;
	}
	std::map<fsLocator, matShaderEffect*> o_Map;
	LoadEffect(itString("PhongReflection.fx"), o_Map);
	LoadEffect(itString("SpecularFresnel.fx"), o_Map);
	LoadEffect(itString("SubSurfaceScatter.fx"), o_Map);
	LoadEffect(itString("Water.fx"), o_Map);
	LoadEffect(itString("Lambert.fx"), o_Map);
	LoadEffect(itString("Blinn.fx"), o_Map);
	LoadEffect(itString("Cartoon.fx"), o_Map);
	std::map<fsLocator, matShaderEffect*>::iterator it2 = o_Map.begin();
	while (it2 != o_Map.end())
	{
		delete it2->second;
		++it2;
	}
	o_Map.clear();


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
