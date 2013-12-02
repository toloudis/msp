#include "matShaderMgrGL.h"

#include "Area18/mat/matShaderBaseGL.hpp"
#include "Area18/shdr/shdrPipeline.hpp"
#include "Area18/shdr/shdrShader.hpp"
#include "Area18/shdr/shdrUtil.hpp"
#include "Graphics/Eff/effSolidData.hpp"
#include "Graphics/Eff/effPhongData.hpp"
#include "Graphics/Eff/effTexturedData.hpp"

matShaderMgrGL::matShaderMgrGL(void)
{
}


matShaderMgrGL::~matShaderMgrGL(void)
{
}

matShaderInfo matShaderMgrGL::registerShader(const std::string& shaderName, 
											 effShaderData* i_pDataTemplate,
											 shdrShaderPtr iVs,
											 shdrShaderPtr iFs)
{
	matShaderInfo info;
	info.m_Name = shaderName.c_str();
	info.m_DataTemplate = i_pDataTemplate;
	info.m_pEffect = NULL;

//	BinaryShaderLoader<EFF_TYPE>* loader = new BinaryShaderLoader<EFF_TYPE>(i_ShaderBits, i_ShaderSizeBytes);
//	loader->LoadShader(i_Name, io_ShaderMap);
//	delete loader;

	shdrPipeline* effect = new shdrPipeline();//NULL, shaderName);
	effect->AttachVS(iVs);
	effect->AttachPS(iFs);

	matShaderEffect* pEffect = NULL;
	if (effect != NULL)
	{
		pEffect = new matShaderBaseGL(fsLocator(), effect, shaderName);
		//DBG_LOG( "Built in shader = " << i_Name.c_str());
		info.m_pEffect = pEffect;
	}
	return info;
}

void matShaderMgrGL::RegisterEffects(const fsLocator &i_ShaderDir,
			std::map<std::string, matShaderInfo>& io_ShaderMap)
{

	// TODO: compile on a particular device?

	std::vector<const fsLocator*> vshaders;
	fsLocator loc1(itString("Globals.h"));
	vshaders.push_back(&loc1);
	fsLocator loc2(itString("vsDefault.glsl"));
	vshaders.push_back(&loc2);
	GLuint vsp = shdrUtil::CompileShaderFromFiles( vshaders, GL_VERTEX_SHADER);
	mVsGeneric.reset(new shdrShader(vsp));

	std::vector<const fsLocator*> fshaders;
	fsLocator loc3(itString("fsDefault.glsl"));
	fshaders.push_back(&loc3);
	GLuint fsp = shdrUtil::CompileShaderFromFiles( fshaders, GL_FRAGMENT_SHADER);
	mFsWhite.reset(new shdrShader(fsp));

	fshaders.clear();
	fshaders.push_back(&loc1);
	fsLocator loc4(itString("fsColorTexture.glsl"));
	fshaders.push_back(&loc4);
	GLuint fspt = shdrUtil::CompileShaderFromFiles( fshaders, GL_FRAGMENT_SHADER);
	shdrShader* s = new shdrShader(fspt);
	s->LoadMetadata(fsLocator(itString("fsColorTexture.xml")));
	mFsColorTexture.reset(s);

	//// default effect uses effPhong and effPhongData
	//RegisterShader<effPhong>("default", new effPhongData, (BYTE*)g_ShaderDefault, sizeof(g_ShaderDefault), io_ShaderMap);
	//RegisterShader<effPhong>("testtess.fx", new effPhongData, (BYTE*)g_Shadertesttess, sizeof(g_Shadertesttess), io_ShaderMap);
	//RegisterShader<effBake>("Bake.fx",		new effBakeData, g_ShaderBake, sizeof(g_ShaderBake), io_ShaderMap);
	//// simple.fx could have a simplified shader class but phong works too...
	//RegisterShader<effPhong>("Simple.fx",		new effPhongData, g_ShaderSimple, sizeof(g_ShaderSimple), io_ShaderMap);
	//RegisterShader<effPhong>("Phong.fx",		new effPhongData, g_ShaderPhong, sizeof(g_ShaderPhong), io_ShaderMap);
	//RegisterShader<effPhong>("Phong_wBump.fx",	new effPhongData, g_ShaderPhong_wBump, sizeof(g_ShaderPhong_wBump), io_ShaderMap);
	//RegisterShader<effHDRLighting>	("HDRLighting.fx",	NULL,			 g_ShaderHDRLighting, sizeof(g_ShaderHDRLighting), io_ShaderMap);
	//RegisterShader<effBlur>			("Blur.fx",			new effBlurData, g_ShaderBlur, sizeof(g_ShaderBlur), io_ShaderMap);
	//RegisterShader<effGlow>			("Glow.fx",			new effGlowData, g_ShaderGlow, sizeof(g_ShaderGlow), io_ShaderMap);
	//RegisterShader<effDOF>			("DOF.fx",			new effDOFData, g_ShaderDOF, sizeof(g_ShaderDOF), io_ShaderMap);
	//RegisterShader<effLightGlow>	("LightGlow.fx",	new effLightGlowData, g_ShaderLightGlow, sizeof(g_ShaderLightGlow), io_ShaderMap);
	//RegisterShader<effParticle>		("Particle.fx",		new effParticleData, g_ShaderParticle, sizeof(g_ShaderParticle), io_ShaderMap);
	//RegisterShader<effPostMatte>	("PostAlphaMatte.fx",	NULL,			 g_ShaderPostAlphaMatte, sizeof(g_ShaderPostAlphaMatte), io_ShaderMap);
	//RegisterShader<effHDRLighting>	("Fog.fx",			NULL,			 g_ShaderFog, sizeof(g_ShaderFog), io_ShaderMap);
	//// Constant solid color
	//RegisterShader<effSolid>("Solid.fx", new effSolidData, g_ShaderSolid, sizeof(g_ShaderSolid), io_ShaderMap);
	//// Constant solid color, but skips values where texture's alpha is below threshold
	//RegisterShader<effMaskAlpha>("MaskAlpha.fx", new effMaskAlphaData, g_ShaderMaskAlpha, sizeof(g_ShaderMaskAlpha), io_ShaderMap);
	//// depth map shader, pseudorandomly dithers values where texture's alpha is below threshold
	//RegisterShader<effMaskAlpha>("DepthMap.fx", new effMaskAlphaData, g_ShaderDepthMap, sizeof(g_ShaderDepthMap), io_ShaderMap);
	//// depth only renderer
	//RegisterShader<effMaskAlpha>("DepthRender.fx", new effMaskAlphaData, g_ShaderDepthRender, sizeof(g_ShaderDepthRender), io_ShaderMap);
	//// reflective shadow map shader
	//RegisterShader<effMaskAlpha>("ReflectiveShadowMap.fx", new effMaskAlphaData, g_ShaderReflectiveShadowMap, sizeof(g_ShaderReflectiveShadowMap), io_ShaderMap);
	//RegisterShader<effOutline>("Outline.fx",	new effOutlineData, g_ShaderOutline, sizeof(g_ShaderOutline), io_ShaderMap);
	//RegisterShader<effOcclusion>("ssaoBilateralBlurEngine.fx", new effOcclusionData, g_ShaderssaoBilateralBlurEngine, sizeof(g_ShaderssaoBilateralBlurEngine), io_ShaderMap);
	//RegisterShader<effOcclusion>("ssaoMultiHorizonBasedAO.fx", new effOcclusionData, g_ShaderssaoMultiHorizonBasedAO, sizeof(g_ShaderssaoMultiHorizonBasedAO), io_ShaderMap);
	//RegisterShader<effOcclusion>("ssgiMultiHorizonBasedGI.fx", new effOcclusionData, g_ShaderssgiMultiHorizonBasedGI, sizeof(g_ShaderssgiMultiHorizonBasedGI), io_ShaderMap);
	//RegisterShader<effOcclusion>("AOVolumes.fx", new effOcclusionData, g_ShaderAOVolumes, sizeof(g_ShaderAOVolumes), io_ShaderMap);
	//RegisterShader<effOcclusion>("GIVolumes.fx", new effOcclusionData, g_ShaderGIVolumes, sizeof(g_ShaderGIVolumes), io_ShaderMap);
	//RegisterShader<effBillboard>("Billboard.fx", new effBillboardData, g_ShaderBillboard, sizeof(g_ShaderBillboard), io_ShaderMap);
	//RegisterShader<effTextured>("Brushstroke.fx", new effTexturedData, g_ShaderBrushstroke, sizeof(g_ShaderBrushstroke), io_ShaderMap);
	//RegisterShader<effTextured>("VelocityRender.fx", new effTexturedData, g_ShaderVelocityRender, sizeof(g_ShaderVelocityRender), io_ShaderMap);
	//RegisterShader<effTextured>("IlluminationOnly.fx", new effTexturedData, g_ShaderIlluminationOnly, sizeof(g_ShaderIlluminationOnly), io_ShaderMap);
	//RegisterShader<effTextured>("ShadowsOnly.fx", new effTexturedData, g_ShaderShadowsOnly, sizeof(g_ShaderShadowsOnly), io_ShaderMap);
	//RegisterShader<effMaskAlpha>("NormalMap.fx", new effMaskAlphaData, g_ShaderNormalMap, sizeof(g_ShaderNormalMap), io_ShaderMap);
	////RegisterShader<effTextured>("RSMGI.fx", new effTexturedData, g_ShaderRSMGI, sizeof(g_ShaderRSMGI), io_ShaderMap);
	//RegisterShader<effTextured>("MotionBlur.fx", new effTexturedData, g_ShaderMotionBlur, sizeof(g_ShaderMotionBlur), io_ShaderMap);
	//RegisterShader<effTextured>("AAEdgeFilter.fx", new effTexturedData, g_ShaderAAEdgeFilter, sizeof(g_ShaderAAEdgeFilter), io_ShaderMap);
	//RegisterShader<effTextured>("LPV_GI.fx", new effTexturedData, g_ShaderLPV_GI, sizeof(g_ShaderLPV_GI), io_ShaderMap);
	//RegisterShader<effRamp>("Ramp.fx", new effRampData, g_ShaderRamp, sizeof(g_ShaderRamp), io_ShaderMap);
	//RegisterShader<effStrandHair>("HairDefault.fx", new effStrandHairData, g_ShaderHairDefault, sizeof(g_ShaderHairDefault), io_ShaderMap);
	//// opacity renderer
	//RegisterShader<effStrandHair>("OpacityRender.fx", new effStrandHairData, g_ShaderOpacityRender, sizeof(g_ShaderOpacityRender), io_ShaderMap);
	//RegisterShader<effTextured>("BlackAndWhite.fx", new effTexturedData, g_ShaderBlackAndWhite, sizeof(g_ShaderBlackAndWhite), io_ShaderMap);
	//RegisterShader<effTextured>("Sepia.fx", new effTexturedData, g_ShaderSepia, sizeof(g_ShaderSepia), io_ShaderMap);
	//RegisterShader<effTextured>("GradientMap.fx", new effTexturedData, g_ShaderGradientMap, sizeof(g_ShaderGradientMap), io_ShaderMap);
	//RegisterShader<effTextured>("Sketch.fx", new effTexturedData, g_ShaderSketch, sizeof(g_ShaderSketch), io_ShaderMap);
	//RegisterShader<effTextured>("EnvBackground.fx", new effTexturedData, g_ShaderEnvBackground, sizeof(g_ShaderEnvBackground), io_ShaderMap);

	struct shdrEntry {
		char* name;
		shdrShaderPtr pVs;
		shdrShaderPtr pFs;
		effShaderData* pData;
	};

	shdrEntry names[] = {
		{"Simple.fx", mVsGeneric, mFsWhite, new effPhongData},
		{"Solid.fx", mVsGeneric, mFsWhite, new effSolidData},
		{"Billboard.fx", mVsGeneric, mFsColorTexture, new effTexturedData},
		{"Phong.fx", mVsGeneric, mFsColorTexture, new effPhongData},
		{"Blinn.fx", mVsGeneric, mFsColorTexture, new effPhongData},
		{NULL, shdrShaderPtr(), shdrShaderPtr(), NULL}
	};
	
	int i = 0;
	shdrEntry n = names[i];
	while (n.name != NULL) {
		io_ShaderMap[n.name] = registerShader(n.name, n.pData, n.pVs, n.pFs);
		n = names[i++];
	}

//	io_ShaderMap["Simple.fx"] = registerShader("Simple.fx");
//	io_ShaderMap["Solid.fx"] = registerShader("Solid.fx");
//	io_ShaderMap["Phong.fx"] = registerShader("Phong.fx");
//	io_ShaderMap["Blinn.fx"] = registerShader("Blinn.fx");
}

void matShaderMgrGL::RegisterUserShaders(const fsLocator& i_ShaderDir, 
			std::vector<matShaderInfo>& o_Shaders)
{
}

void matShaderMgrGL::UnloadEffect(const fsLocator& i_PathToShader,
		std::map<fsLocator, matShaderEffect*>& io_ShaderMap)
{
}

matShaderEffect* matShaderMgrGL::LoadEffect(const fsLocator& i_PathToShader,
		std::map<fsLocator, matShaderEffect*>& io_ShaderMap)
{
	return NULL;
}

