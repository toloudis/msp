/*****************************************************************************
**	gpxRenderPrefs.hpp
**
**	This class is a thread-safe proxy for a g3dPrefs::g3dRenderPrefs.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_RENDERPREFS_HPP
#error gpxRenderPrefs.hpp multiply included
#endif
#define GPX_RENDERPREFS_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/G3d/g3dPrefs.hpp"
#endif
#ifndef G3D_SCENERENDERENGINECREATE_HPP
#include "Graphics/g3d/g3dSceneRenderEngineCreate.hpp"
#endif

#ifndef G3D_PROJECTEDLIGHT_HPP
#include "Graphics/G3d/g3dProjectedLight.hpp"
#endif


//============================================================================
//============================================================================
class gpxRenderPrefs : public gpxProxyObject
{
public:
	//--------------------------------------------------------------------
	// Constructor takes reference to object it will control
	//--------------------------------------------------------------------
	explicit gpxRenderPrefs(g3dPrefs::g3dRenderPrefs &i_Prefs);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxRenderPrefs();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the effect when "Update()" is called.
	//--------------------------------------------------------------------
	void SetRendererType(g3dSceneRendererTypes::RendererType i_RendererType);
	void SetRendererEngine(g3dSceneRenderEngineCreate::RenderEngine i_RenderEngine);
	void SetEnableLitPass(bool i_bEnable);
	void SetEnableTransparent(bool i_bEnable);
	void SetEnableDOF(bool i_bEnable);
	void SetEnableGlow(bool i_bEnable);
	void SetEnableReflection(bool i_bEnable);
	void SetEnableEnvironment(bool i_bEnable);
	void SetRenderMatte(bool i_bEnable);
	void SetMultipassOn(bool i_bEnable);
	void SetHeadlightOn(bool i_bEnable);
	void SetEnableOutline(bool i_bEnable);
	void SetEnableSpecularLighting(bool i_bEnable);
	void SetEnableDiffuseLighting(bool i_bEnable);
	void SetHDRToneMap(bool i_bEnable);
	void SetHDRDebugMode(int i_Mode);
	void SetLowResolution(bool i_bEnable);
	void SetProjLightFrustumCull(bool i_bEnable);
	void SetProjLightsOn(bool i_bEnable);
	void SetPtLightsOn(bool i_bEnable);
	void SetDoShadowMapGen(bool i_bEnable);
	void SetEnableShadows(bool i_bEnable);
	void SetEnableInvisibleCastShadows(bool i_bEnable);
	void SetEnableInvisibleMaskBlack(bool i_bEnable);
	void SetEnableInvisibleInReflections(bool i_bEnable);
	void SetHDRAA(bool i_bEnable);
	void SetUseAOVolumes(bool i_bEnable);
	void SetEnableSSAO(bool i_bEnable);
	void SetEnableSSAOBlur(bool i_bEnable);
	void SetSSAONumSteps(float i_NumSteps);
	void SetSSAONumDirs(int i_NumDirs);
	void SetEnableSSAODepthPeeling(bool i_bEnable);
	void SetSSAONumLayers(int i_NumLayers);
	void SetAOResolutionReduce(int i_AOResolutionReduce);

	void SetUseLPVGI(bool i_bEnable);
	void SetEnableSSGI(bool i_bEnable);
	void SetEnableSSGIBlur(bool i_bEnable);
	void SetSSGINumSteps(float i_NumSteps);
	void SetSSGINumDirs(int i_NumDirs);
	void SetEnableSSGIDepthPeeling(bool i_bEnable);
	void SetSSGINumLayers(int i_NumLayers);
	void SetGIResolutionReduce(int i_GIResolutionReduce);

	void SetUseHardwareTessellation(bool i_bEnable);
	void SetPixelSubdivLimit(float i_Value);
	void SetRenderWireframe(bool i_bEnable);
	void SetMotionBlurEnable(bool i_bEnable);
	void SetMotionBlurSamples(int i_Num);
	void SetMotionBlurPercent(float i_Percent);
	void SetTransparencyMode(int i_Mode);
	void SetDebugDepthPeel(bool i_bEnable);
	void SetDebugSinglePeel(bool i_bEnable);
	void SetDebugDepthPeelLayers(int i_NumLayers);
	void SetIlluminationUsesNormals(bool i_bEnable);

	void SetEnableHair(bool i_bEnable);
	void SetHairLines(bool i_bEnable);
	void SetHairTransparencyMode( int i_Mode );
	void SetHairShadowType( HAIR_SHADOW_TYPE i_Mode );
	void SetHairShadowRes( int i_Mode );
	void SetHairTessellation( float i_Value );
	void SetHairVertexLimit( unsigned int i_Count );
	void SetHairSkipStrand( unsigned int i_Count );
	void SetHairSubPixelPower( float i_Value );
	void SetHairDepthPeel( bool i_bEnable );
	void SetHairDepthPeelLayer( int i_Value );
	void SetHairInterpolationCount( unsigned int i_Count );
	void SetHairClumpRadius( float i_Value );

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
	g3dPrefs::g3dRenderPrefs &m_Prefs;

#if USE_PROXIES
	g3dPrefs::g3dRenderPrefs m_PrefsCopy;
#endif
};