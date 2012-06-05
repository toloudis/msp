/*****************************************************************************
**	gpxRenderPrefs.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxRenderPrefs.hpp"

#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
// Constructor takes reference to object it will control
//--------------------------------------------------------------------
gpxRenderPrefs::gpxRenderPrefs(g3dPrefs::g3dRenderPrefs &i_Prefs)
:	m_Prefs(i_Prefs)
{
#if USE_PROXIES
	// Make copy of whole structure
	m_PrefsCopy = i_Prefs;
#endif

	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxRenderPrefs::~gpxRenderPrefs()
{
	PROXY_REMOVE();
}


//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the light when "Update()" is called.
//--------------------------------------------------------------------
void gpxRenderPrefs::SetRendererType(g3dSceneRendererTypes::RendererType i_RendererType)
{
	PROXY_SET_VARIABLE(m_Prefs.m_RendererType, m_PrefsCopy.m_RendererType, i_RendererType);
}	
void gpxRenderPrefs::SetRendererEngine(g3dSceneRenderEngineCreate::RenderEngine i_RendererEngine)
{
	PROXY_SET_VARIABLE(m_Prefs.m_RendererEngine, m_PrefsCopy.m_RendererEngine, i_RendererEngine);
}	
void gpxRenderPrefs::SetEnableLitPass(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bEnableLitPass, m_PrefsCopy.m_bEnableLitPass, i_bEnable);
}				
void gpxRenderPrefs::SetEnableTransparent(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bEnableTransparent, m_PrefsCopy.m_bEnableTransparent, i_bEnable);
}							
void gpxRenderPrefs::SetEnableDOF(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bEnableDOF, m_PrefsCopy.m_bEnableDOF, i_bEnable);
}							
void gpxRenderPrefs::SetEnableGlow(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bEnableGlow, m_PrefsCopy.m_bEnableGlow, i_bEnable);
}							
void gpxRenderPrefs::SetEnableReflection(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bEnableReflection, m_PrefsCopy.m_bEnableReflection, i_bEnable);
}							
void gpxRenderPrefs::SetEnableEnvironment(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bEnableEnvironment, m_PrefsCopy.m_bEnableEnvironment, i_bEnable);
}							
void gpxRenderPrefs::SetRenderMatte(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bRenderMatte, m_PrefsCopy.m_bRenderMatte, i_bEnable);
}								
void gpxRenderPrefs::SetMultipassOn(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bMultipassOn, m_PrefsCopy.m_bMultipassOn, i_bEnable);
}								
void gpxRenderPrefs::SetHeadlightOn(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bHeadlightOn, m_PrefsCopy.m_bHeadlightOn, i_bEnable);
}								
void gpxRenderPrefs::SetEnableOutline(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bEnableOutline, m_PrefsCopy.m_bEnableOutline, i_bEnable);
}					
void gpxRenderPrefs::SetEnableSpecularLighting(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bEnableSpecularLighting, m_PrefsCopy.m_bEnableSpecularLighting, i_bEnable);
}							
void gpxRenderPrefs::SetEnableDiffuseLighting(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bEnableDiffuseLighting, m_PrefsCopy.m_bEnableDiffuseLighting, i_bEnable);
}							
void gpxRenderPrefs::SetHDRToneMap(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_HDRToneMap, m_PrefsCopy.m_HDRToneMap, i_bEnable);
}									
void gpxRenderPrefs::SetHDRDebugMode(int i_Mode)
{
	PROXY_SET_VARIABLE(m_Prefs.m_HDRDebugMode, m_PrefsCopy.m_HDRDebugMode, i_Mode);
}									
void gpxRenderPrefs::SetLowResolution(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bLowResolution, m_PrefsCopy.m_bLowResolution, i_bEnable);
}										
void gpxRenderPrefs::SetProjLightFrustumCull(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bProjLightFrustumCull, m_PrefsCopy.m_bProjLightFrustumCull, i_bEnable);
}										
void gpxRenderPrefs::SetProjLightsOn(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bProjLightsOn, m_PrefsCopy.m_bProjLightsOn, i_bEnable);
}										
void gpxRenderPrefs::SetPtLightsOn(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bPtLightsOn, m_PrefsCopy.m_bPtLightsOn, i_bEnable);
}										
void gpxRenderPrefs::SetDoShadowMapGen(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bDoShadowMapGen, m_PrefsCopy.m_bDoShadowMapGen, i_bEnable);
}										
void gpxRenderPrefs::SetEnableShadows(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bEnableShadows, m_PrefsCopy.m_bEnableShadows, i_bEnable);
}							
void gpxRenderPrefs::SetEnableInvisibleCastShadows(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bEnableInvisibleCastShadows, m_PrefsCopy.m_bEnableInvisibleCastShadows, i_bEnable);
}
void gpxRenderPrefs::SetEnableInvisibleMaskBlack(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bEnableInvisibleMaskBlack, m_PrefsCopy.m_bEnableInvisibleMaskBlack, i_bEnable);
}
void gpxRenderPrefs::SetEnableInvisibleInReflections(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bEnableInvisibleInReflections, m_PrefsCopy.m_bEnableInvisibleInReflections, i_bEnable);
}
void gpxRenderPrefs::SetHDRAA(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bHDRAA, m_PrefsCopy.m_bHDRAA, i_bEnable);
}										
void gpxRenderPrefs::SetUseAOVolumes(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bAOInvalid, m_PrefsCopy.m_bAOInvalid, i_bEnable);
}									
void gpxRenderPrefs::SetEnableSSAO(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bEnableSSAO, m_PrefsCopy.m_bEnableSSAO, i_bEnable);
}									
void gpxRenderPrefs::SetEnableSSAOBlur(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bEnableSSAOBlur, m_PrefsCopy.m_bEnableSSAOBlur, i_bEnable);
}								
void gpxRenderPrefs::SetSSAONumSteps(float i_NumSteps)
{
	PROXY_SET_VARIABLE(m_Prefs.m_SSAONumSteps, m_PrefsCopy.m_SSAONumSteps, i_NumSteps);
}								
void gpxRenderPrefs::SetSSAONumDirs(int i_NumDirs)
{
	PROXY_SET_VARIABLE(m_Prefs.m_SSAONumDirs, m_PrefsCopy.m_SSAONumDirs, i_NumDirs);
}									
void gpxRenderPrefs::SetEnableSSAODepthPeeling(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bEnableSSAODepthPeeling, m_PrefsCopy.m_bEnableSSAODepthPeeling, i_bEnable);
}									
void gpxRenderPrefs::SetSSAONumLayers(int i_NumLayers)
{
	PROXY_SET_VARIABLE(m_Prefs.m_SSAONumLayers, m_PrefsCopy.m_SSAONumLayers, i_NumLayers);
}									
void gpxRenderPrefs::SetAOResolutionReduce(int i_AOResolutionReduce)
{
	PROXY_SET_VARIABLE(m_Prefs.m_AOResolutionReduce, m_PrefsCopy.m_AOResolutionReduce, i_AOResolutionReduce);
}									

void gpxRenderPrefs::SetUseLPVGI(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bGIInvalid, m_PrefsCopy.m_bGIInvalid, i_bEnable);
}
void gpxRenderPrefs::SetEnableSSGI(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bEnableSSGI, m_PrefsCopy.m_bEnableSSGI, i_bEnable);
}									
void gpxRenderPrefs::SetEnableSSGIBlur(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bEnableSSGIBlur, m_PrefsCopy.m_bEnableSSGIBlur, i_bEnable);
}								
void gpxRenderPrefs::SetSSGINumSteps(float i_NumSteps)
{
	PROXY_SET_VARIABLE(m_Prefs.m_SSGINumSteps, m_PrefsCopy.m_SSGINumSteps, i_NumSteps);
}								
void gpxRenderPrefs::SetSSGINumDirs(int i_NumDirs)
{
	PROXY_SET_VARIABLE(m_Prefs.m_SSGINumDirs, m_PrefsCopy.m_SSGINumDirs, i_NumDirs);
}									
void gpxRenderPrefs::SetEnableSSGIDepthPeeling(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bEnableSSGIDepthPeeling, m_PrefsCopy.m_bEnableSSGIDepthPeeling, i_bEnable);
}									
void gpxRenderPrefs::SetSSGINumLayers(int i_NumLayers)
{
	PROXY_SET_VARIABLE(m_Prefs.m_SSGINumLayers, m_PrefsCopy.m_SSGINumLayers, i_NumLayers);
}
void gpxRenderPrefs::SetGIResolutionReduce(int i_GIResolutionReduce)
{
	PROXY_SET_VARIABLE(m_Prefs.m_GIResolutionReduce, m_PrefsCopy.m_GIResolutionReduce, i_GIResolutionReduce);
}

void gpxRenderPrefs::SetUseHardwareTessellation(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bUseHardwareTessellation, m_PrefsCopy.m_bUseHardwareTessellation, i_bEnable);
}											
void gpxRenderPrefs::SetPixelSubdivLimit(float i_Value)
{
	PROXY_SET_VARIABLE(m_Prefs.m_PixelSubdivLimit, m_PrefsCopy.m_PixelSubdivLimit, i_Value);
}											
void gpxRenderPrefs::SetRenderWireframe(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bRenderWireframe, m_PrefsCopy.m_bRenderWireframe, i_bEnable);
}									
void gpxRenderPrefs::SetMotionBlurEnable(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bMotionBlurEnable, m_PrefsCopy.m_bMotionBlurEnable, i_bEnable);
}								
void gpxRenderPrefs::SetMotionBlurSamples(int i_Num)
{
	PROXY_SET_VARIABLE(m_Prefs.m_MotionBlurSamples, m_PrefsCopy.m_MotionBlurSamples, i_Num);
}									
void gpxRenderPrefs::SetMotionBlurPercent(float i_Percent)
{
	PROXY_SET_VARIABLE(m_Prefs.m_MotionBlurPercent, m_PrefsCopy.m_MotionBlurPercent, i_Percent);
}								
void gpxRenderPrefs::SetTransparencyMode(int i_Mode)
{
	PROXY_SET_VARIABLE(m_Prefs.m_TransparencyMode, m_PrefsCopy.m_TransparencyMode, i_Mode);
}												
void gpxRenderPrefs::SetDebugDepthPeel(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bDebugDepthPeel, m_PrefsCopy.m_bDebugDepthPeel, i_bEnable);
}											
void gpxRenderPrefs::SetDebugSinglePeel(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bDebugSinglePeel, m_PrefsCopy.m_bDebugSinglePeel, i_bEnable);
}										
void gpxRenderPrefs::SetDebugDepthPeelLayers(int i_NumLayers)
{
	PROXY_SET_VARIABLE(m_Prefs.m_nDebugDepthPeelLayers, m_PrefsCopy.m_nDebugDepthPeelLayers, i_NumLayers);
}							
void gpxRenderPrefs::SetIlluminationUsesNormals(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bIlluminationUsesNormals, m_PrefsCopy.m_bIlluminationUsesNormals, i_bEnable);
}							
void gpxRenderPrefs::SetEnableHair(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bEnableHair, m_PrefsCopy.m_bEnableHair, i_bEnable);
}							
void gpxRenderPrefs::SetHairLines(bool i_bEnable)
{
	PROXY_SET_VARIABLE(m_Prefs.m_bHairLines, m_PrefsCopy.m_bHairLines, i_bEnable);
}							
void gpxRenderPrefs::SetHairTransparencyMode( int i_Mode )
{
	PROXY_SET_VARIABLE(m_Prefs.m_HairTransparencyMode, m_PrefsCopy.m_HairTransparencyMode, i_Mode);
}							
void gpxRenderPrefs::SetHairShadowType( HAIR_SHADOW_TYPE i_Mode )
{
	PROXY_SET_VARIABLE(m_Prefs.m_HairShadowType, m_PrefsCopy.m_HairShadowType, i_Mode);
}
void gpxRenderPrefs::SetHairShadowRes( int i_Mode )
{
	PROXY_SET_VARIABLE(m_Prefs.m_HairShadowRes, m_PrefsCopy.m_HairShadowRes, i_Mode);
}
void gpxRenderPrefs::SetHairTessellation( float i_Value )
{
	PROXY_SET_VARIABLE(m_Prefs.m_HairTessellation, m_PrefsCopy.m_HairTessellation, i_Value);
}
void gpxRenderPrefs::SetHairVertexLimit( unsigned int i_Count )
{
	PROXY_SET_VARIABLE(m_Prefs.m_HairVertexLimit, m_PrefsCopy.m_HairVertexLimit, i_Count);
}
void gpxRenderPrefs::SetHairSkipStrand( unsigned int i_Count )
{
	PROXY_SET_VARIABLE(m_Prefs.m_HairStrandSkip, m_PrefsCopy.m_HairStrandSkip, i_Count);
}
void gpxRenderPrefs::SetHairSubPixelPower( float i_Value )
{
	PROXY_SET_VARIABLE(m_Prefs.m_HairSubPixelPower, m_PrefsCopy.m_HairSubPixelPower, i_Value);
}
void gpxRenderPrefs::SetHairDepthPeel( bool i_bEnable )
{
	PROXY_SET_VARIABLE(m_Prefs.m_bHairDepthPeel, m_PrefsCopy.m_bHairDepthPeel, i_bEnable);
}
void gpxRenderPrefs::SetHairDepthPeelLayer( int i_Value )
{
	PROXY_SET_VARIABLE(m_Prefs.m_nHairDepthPeelLayers, m_PrefsCopy.m_nHairDepthPeelLayers, i_Value);
}
void gpxRenderPrefs::SetHairInterpolationCount( unsigned int i_Count )
{
	PROXY_SET_VARIABLE(m_Prefs.m_HairInterpolationCount, m_PrefsCopy.m_HairInterpolationCount, i_Count);
}
void gpxRenderPrefs::SetHairClumpRadius( float i_Value )
{
	PROXY_SET_VARIABLE(m_Prefs.m_HairClumpRadius, m_PrefsCopy.m_HairClumpRadius, i_Value);
}


//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxRenderPrefs::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	// Copy whole structure at once instead of individual values
	m_Prefs = m_PrefsCopy;

	this->SetNeedsUpdate(false);
#endif

	return true;
}