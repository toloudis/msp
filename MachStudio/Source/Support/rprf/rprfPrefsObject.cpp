/*****************************************************************************
**	rprfPrefsObject.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Support/rprf/rprfPrefsObject.hpp"

#include "Support/mnm/mnmConstants.hpp"
#include "Support/rman/rmanMgr.hpp"
#include "Support/rlyr/rlyrRenderLayer.hpp"
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"

#include "Support/rprf/rprfPrefsUtil.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gpx/gpxRenderPrefs.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "Tool/gui/guiXMLTextReader.hpp"
#include "Tool/gui/guiXMLTextWriter.hpp"

#include <string>
#include <vector>


//------------------------------------------------------------------------
//------------------------------------------------------------------------
rprfPrefsObject::rprfPrefsObject()
{
	Init();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
rprfPrefsObject::rprfPrefsObject(const rprfPrefsData& i_Data, const g3dPrefs::g3dRenderPrefs& i_RenderPrefs)
{
	m_Data = i_Data;
	m_ActualPrefs = i_RenderPrefs;
	Init();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
rprfPrefsObject::rprfPrefsObject(const g3dPrefs::g3dRenderPrefs& i_RenderPrefs)
	: m_ActualPrefs(i_RenderPrefs)
{
	Init();
}


void rprfPrefsObject::Init() 
{
	// create proxy for buffering changes from GUI thread
	m_PrefsProxy.reset(new gpxRenderPrefs(m_ActualPrefs));
	
	//RegisterProperties();
	rprfPrefsUtil::RegisterPrefProperties(this, m_Data, false);
	rprfPrefsUtil::RegisterSSAOEnable(this, m_Data);
	rprfPrefsUtil::RegisterSSAOSampling(this, m_Data);
	rprfPrefsUtil::RegisterSSAOgui(this, m_Data);

	rprfPrefsUtil::RegisterSSGIEnable(this, m_Data);
	rprfPrefsUtil::RegisterSSGISampling(this, m_Data);
	rprfPrefsUtil::RegisterSSGIgui(this, m_Data);

	AddCallbacks();
	rprfPrefsUtil::UpdateGUICategories(m_Data, false);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void rprfPrefsObject::Apply()
{
	// stop any render threads
	gpxRenderControl::ConfirmSingleThread();

	g3dPrefs::SetPrefs(&m_ActualPrefs);
}

void rprfPrefsObject::AddCallbacks() 
{
	// TODO : split these into their own atomic update functions, and have them update the "real" application data!
	m_Data.m_bMultipassOn.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateShadows));
	m_Data.m_bEnableDOF.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateDOF));
	m_Data.m_bEnableGlow.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateGlow));
	m_Data.m_bEnableOutline.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateOutline));
//			m_Data.m_bEnableAmbientPass.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdatePasses));
	m_Data.m_bEnableLitPass.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdatePasses));
	m_Data.m_bEnableDiffuseLighting.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdatePasses));
	m_Data.m_bEnableSpecularLighting.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdatePasses));
	m_Data.m_bEnableShadows.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdatePasses));
	m_Data.m_bEnableInvisibleCastShadows.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdatePasses));
	m_Data.m_bEnableInvisibleMaskBlack.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdatePasses));
	m_Data.m_bEnableInvisibleInReflections.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdatePasses));
	m_Data.m_bEnableTransparent.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdatePasses));
	m_Data.m_bMatteMode.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateMatte));
//			m_Data.m_bHeadlightOn.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateHeadlight));
//	m_Data.m_RendererType.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderType));
	m_Data.m_RendererEngine.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderEngine));
	m_Data.m_SSAOQuality.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateSSAOQuality));
	m_Data.m_SSGIQuality.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateSSGIQuality));

#ifdef _DEBUG
	m_Data.m_bToneMap.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateHDR));
#endif
//			m_Data.m_bBlueShift.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateHDR));
#ifdef _DEBUG
	m_Data.m_HDRDebugMode.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateHDR));
#endif
	m_Data.m_bHDRAA.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateHDR));
	m_Data.m_bCaptureToneMapped.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateHDR));

	m_Data.m_bEnableReflection.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateReflection));
	m_Data.m_bEnableEnvironment.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateEnvironment));
	m_Data.m_bLowResolution.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateResolution));

//			m_Data.m_bEnableAO.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateAO));
//			m_Data.m_bRecalcAOPerFrame.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateAO));

#ifdef _DEBUG
	m_Data.m_bProjLightFrustumCull.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateProjLtFrustumCull));
	m_Data.m_bProjLightsOn.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateProjLtFrustumCull));
	m_Data.m_bPtLightsOn.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateProjLtFrustumCull));
	m_Data.m_bDoShadowMapGen.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateProjLtFrustumCull));
//			m_Data.m_bEnableDeferredTransparency.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdatePasses));
#endif

	m_Data.m_bUseAOVolumes.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateSSAO));
	m_Data.m_bEnableSSAO.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateSSAO));
	m_Data.m_SSAONumSteps.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateSSAOSampling));
	m_Data.m_SSAONumDirs.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateSSAOSampling));
	m_Data.m_SSAOEnableBlur.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateSSAO));
	m_Data.m_bEnableSSAODepthPeeling.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateSSAO));
	m_Data.m_SSAONumLayers.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateSSAOSampling));

	m_Data.m_bUseLPVGI.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateSSGI));
	m_Data.m_bEnableSSGI.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateSSGI));
	m_Data.m_SSGINumSteps.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateSSGISampling));
	m_Data.m_SSGINumDirs.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateSSGISampling));
	m_Data.m_SSGIEnableBlur.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateSSGI));
	m_Data.m_bEnableSSGIDepthPeeling.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateSSGI));
	m_Data.m_SSGINumLayers.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateSSGISampling));

	m_Data.m_bUseHardwareTessellation.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateHardwareTessellation));	
	m_Data.m_PixelSubdivLimit.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateHardwareTessellation));	
	m_Data.m_bRenderWireframe.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderWireframe));
#ifdef ENABLE_MOTIONBLUR
	m_Data.m_bMotionBlurEnable.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateMotionBlur));
	m_Data.m_MotionBlurSamples.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateMotionBlur));
	m_Data.m_MotionBlurPercent.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateMotionBlur));
#endif

	m_Data.m_TransparencyMode.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateTransparency));	
	m_Data.m_bDebugDepthPeel.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateTransparency));	
	m_Data.m_nDebugDepthPeelLayers.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateTransparency));	
#ifdef _DEBUG
	m_Data.m_bDebugSinglePeel.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateTransparency));	
#endif

//	m_Data.m_bIlluminationUsesNormals.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateIlluminationRender));

#ifdef HAIR_SUPPORTED
	m_Data.m_bEnableHair.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateHair));
	m_Data.m_bHairLines.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateHair));
	m_Data.m_HairTransparencyMode.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateHair));
//	m_Data.m_HairShadowRes.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateHair));
//	m_Data.m_HairShadowType.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateHair));
	m_Data.m_HairTessellation.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateHair));
//	m_Data.m_HairVertexLimit.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateHair));
//	m_Data.m_HairStrandSkip.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateHair));
	m_Data.m_HairSubPixelPower.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateHair));
	m_Data.m_bHairDepthPeel.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateHair));
	m_Data.m_nHairDepthPeelLayers.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateHair));
	m_Data.m_HairInterpolationCount.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateHair));
	m_Data.m_HairClumpRadius.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateHair));
#endif

	m_Data.m_bRmanRenderRIB.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_bRmanGenShadowMaps.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_bRmanGenReflectionMaps.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_nRmanAArate.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanShadingRate.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanOutType.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanBatchContent.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_bRmanTextureBatch.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanAOsamples.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_bRmanAOEnable.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanAOMaxDist.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanAOMaxVariation.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanAOConeAngle.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanReflType.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_bRmanReflEnable.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_bRmanShadowEnable.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanShadowType.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanShadowMinSamples.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanShadowSamples.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanShadowBias.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanShadowSoftness.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_bRmanGIEnable.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanGIsamples.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanGIMaxDist.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanGIMaxVariation.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanGIConeAngle.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanFilterType.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanFilterWidth.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_bRmanCacheTextures.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_bRmanDisableWarnings.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanNumCores.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanTexMemory.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanBucketSize.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanBucketOrder.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanGridSize.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
	m_Data.m_RmanRayDepth.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateRenderManOptions));
}

//--------------------------------------------------------------------
// Convert UI specific combo box index to a known enum type
//--------------------------------------------------------------------
g3dSceneRendererTypes::RendererType rprfPrefsObject::GetRendererType(int i_ComboBoxIndex)
{
	switch(i_ComboBoxIndex)
	{
	case 0:
		return g3dSceneRendererTypes::e_Default;
	case 1:
		return g3dSceneRendererTypes::e_AmbientOcclusion;
	case 2:
		return g3dSceneRendererTypes::e_Depth;
	case 3:
		return g3dSceneRendererTypes::e_ShadowMask;
	case 4:
		return g3dSceneRendererTypes::e_IlluminationOnly;
	case 5:
		return g3dSceneRendererTypes::e_Normals;
	case 6:
		return g3dSceneRendererTypes::e_DirtyMatte;
	case 7:
		return g3dSceneRendererTypes::e_Wireframe;
	case 8:
		return g3dSceneRendererTypes::e_Materials;
	case 9:
		return g3dSceneRendererTypes::e_ReflectionOnly;
	case 10:
		return g3dSceneRendererTypes::e_GlobalIllumination;
	case 11:
		return g3dSceneRendererTypes::e_VelocityMap;
	case 12:
		return g3dSceneRendererTypes::e_Glow;
	default:
		return g3dSceneRendererTypes::e_Default;
	}
}

//--------------------------------------------------------------------
// Convert UI specific combo box index to a known enum type
//--------------------------------------------------------------------
g3dSceneRenderEngineCreate::RenderEngine rprfPrefsObject::GetRenderEngine(int i_ComboBoxIndex)
{
	switch(i_ComboBoxIndex)
	{
	case 0:
		return g3dSceneRenderEngineCreate::e_Default;
	case 1:
		return g3dSceneRenderEngineCreate::e_RmanPrman;
	case 2:
		return g3dSceneRenderEngineCreate::e_MentalRay;
	default:
		return g3dSceneRenderEngineCreate::e_Default;
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void rprfPrefsObject::UpdateShadows(prtyProperty *i_pProperty, bool i_bDirty)
{
	if( m_Data.m_bRenderWireframe.GetValue() )
	{
		m_PrefsProxy->SetMultipassOn( false );
		m_PrefsProxy->SetHeadlightOn( true );
	}
	else
	{
		this->m_PrefsProxy->SetMultipassOn( m_Data.m_bMultipassOn.GetValue() );
		this->m_PrefsProxy->SetHeadlightOn(!m_Data.m_bMultipassOn.GetValue() );
	}
	//rndrPrefsUtil::SetMultipassRendering(this->m_PrefsProxy->SetMultipassOn);
	//WritePrefsToConfigFile();
	//update_statusbar_renderprefs();
}

void rprfPrefsObject::UpdateDOF(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->m_PrefsProxy->SetEnableDOF( m_Data.m_bEnableDOF.GetValue() );
	//WritePrefsToConfigFile();
	//update_statusbar_renderprefs();
}
void rprfPrefsObject::UpdateGlow(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->m_PrefsProxy->SetEnableGlow( m_Data.m_bEnableGlow.GetValue() );
	//WritePrefsToConfigFile();
	//update_statusbar_renderprefs();
}
void rprfPrefsObject::UpdateMatte(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->m_PrefsProxy->SetRenderMatte( m_Data.m_bMatteMode.GetValue() );
	//WritePrefsToConfigFile();
	//update_statusbar_renderprefs();
}
void rprfPrefsObject::UpdateOutline(prtyProperty *i_pProperty, bool i_bDirty)
{	
	m_PrefsProxy->SetEnableOutline( m_Data.m_bEnableOutline.GetValue() );
}
void rprfPrefsObject::UpdateMotionBlur(prtyProperty *i_pProperty, bool i_bDirty)
{	
	m_PrefsProxy->SetMotionBlurEnable( m_Data.m_bMotionBlurEnable.GetValue() );
	m_PrefsProxy->SetMotionBlurSamples( m_Data.m_MotionBlurSamples.GetValue() );
	m_PrefsProxy->SetMotionBlurPercent( m_Data.m_MotionBlurPercent.GetValue() );
}
void rprfPrefsObject::UpdatePasses(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->m_PrefsProxy->SetEnableLitPass( m_Data.m_bEnableLitPass.GetValue() );
	this->m_PrefsProxy->SetEnableDiffuseLighting( m_Data.m_bEnableDiffuseLighting.GetValue() );
	this->m_PrefsProxy->SetEnableSpecularLighting( m_Data.m_bEnableSpecularLighting.GetValue() );
	this->m_PrefsProxy->SetEnableShadows( m_Data.m_bEnableShadows.GetValue() );
	this->m_PrefsProxy->SetEnableInvisibleCastShadows( m_Data.m_bEnableInvisibleCastShadows.GetValue() );
	this->m_PrefsProxy->SetEnableInvisibleMaskBlack( m_Data.m_bEnableInvisibleMaskBlack.GetValue() );
	this->m_PrefsProxy->SetEnableInvisibleInReflections( m_Data.m_bEnableInvisibleInReflections.GetValue() );
	this->m_PrefsProxy->SetEnableTransparent( m_Data.m_bEnableTransparent.GetValue() );
}
void rprfPrefsObject::UpdateIlluminationRender(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->m_PrefsProxy->SetIlluminationUsesNormals( m_Data.m_bIlluminationUsesNormals.GetValue() );
}
void rprfPrefsObject::UpdateRenderType(prtyProperty *i_pProperty, bool i_bDirty)
{	
	if(!rprfPrefsUtil::RenderTypeEnabled( m_Data.m_RendererType.GetValue() ))
	{
		//if the user chose a disabled render type, set it back to default
		m_Data.m_RendererType.SetValue(g3dSceneRendererTypes::e_Default);
		return;
	}

	g3dSceneRendererTypes::RendererType render_type = GetRendererType(m_Data.m_RendererType.GetValue());
	this->m_PrefsProxy->SetRendererType( render_type );

	if ( render_type == g3dSceneRendererTypes::e_AmbientOcclusion ) 
	{
		m_Data.m_bEnableSSAO.SetValue(true);
		this->m_PrefsProxy->SetEnableSSAO( m_Data.m_bEnableSSAO.GetValue() );
	}

	if ( render_type == g3dSceneRendererTypes::e_GlobalIllumination ) 
	{
		m_Data.m_bEnableSSGI.SetValue(true);
		this->m_PrefsProxy->SetEnableSSGI( m_Data.m_bEnableSSGI.GetValue() );
	}

	if ( render_type == g3dSceneRendererTypes::e_DirtyMatte )
	{
		m_Data.m_bMatteMode.SetValue(true);
	}
	else
	{
		m_Data.m_bMatteMode.SetValue(false);
	}

	if ( render_type == g3dSceneRendererTypes::e_Wireframe )
	{
		m_Data.m_bRenderWireframe.SetValue(true);
	}
	else
	{
		m_Data.m_bRenderWireframe.SetValue(false);
	}

	if ( render_type == g3dSceneRendererTypes::e_IlluminationOnly )
	{
		m_Data.m_bMultipassOn.SetValue(true);
		m_Data.m_bEnableDiffuseLighting.SetValue(true);
		m_Data.m_bEnableShadows.SetValue(true);
	}

	if ( render_type == g3dSceneRendererTypes::e_ShadowMask )
	{
		m_Data.m_bEnableShadows.SetValue(true);
	}

	//update the category listings in the dialog
	rprfPrefsUtil::UpdateGUICategories(this->m_Data, false);
}

void rprfPrefsObject::UpdateRenderEngine(prtyProperty *i_pProperty, bool i_bDirty)
{	
	
	g3dSceneRenderEngineCreate::RenderEngine render_engine = GetRenderEngine(m_Data.m_RendererEngine.GetValue());

	g3dPrefs::CurrentPrefs().m_RendererEngine = GetRenderEngine(m_Data.m_RendererEngine.GetValue());

	this->m_PrefsProxy->SetRendererEngine( render_engine );

	//update the category listings in the dialog
	rprfPrefsUtil::UpdateGUICategories(this->m_Data, false);

	std::vector<rlyrRenderLayer*> layers = rlyrRenderLayerMgr::GetRenderLayers();
	for ( int i = 0 ; i < layers.size() ; i++ )
	{
		layers[i]->UpdateRenderPassesVisbility();
	}

}

void rprfPrefsObject::UpdateHDR(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->m_PrefsProxy->SetHDRToneMap( m_Data.m_bToneMap.GetValue() );
	if(!rprfPrefsUtil::HDRLayerEnabled( m_Data.m_HDRDebugMode.GetValue() ))
	{
		//update the category listings in the dialog
		rprfPrefsUtil::UpdateGUICategories(this->m_Data, false);
		m_Data.m_HDRDebugMode.SetValue(0);
	}
	else
	{
		this->m_PrefsProxy->SetHDRDebugMode( m_Data.m_HDRDebugMode.GetValue() );
	}
	this->m_PrefsProxy->SetHDRAA( m_Data.m_bHDRAA.GetValue() );
}
void rprfPrefsObject::UpdateReflection(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->m_PrefsProxy->SetEnableReflection( m_Data.m_bEnableReflection.GetValue() );
}
void rprfPrefsObject::UpdateEnvironment(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->m_PrefsProxy->SetEnableEnvironment( m_Data.m_bEnableEnvironment.GetValue() );
}
void rprfPrefsObject::UpdateResolution(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->m_PrefsProxy->SetLowResolution( m_Data.m_bLowResolution.GetValue() );
}

void rprfPrefsObject::UpdateProjLtFrustumCull(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->m_PrefsProxy->SetProjLightFrustumCull( m_Data.m_bProjLightFrustumCull.GetValue() );
	this->m_PrefsProxy->SetProjLightsOn( m_Data.m_bProjLightsOn.GetValue() );
	this->m_PrefsProxy->SetPtLightsOn( m_Data.m_bPtLightsOn.GetValue() );
	this->m_PrefsProxy->SetDoShadowMapGen( m_Data.m_bDoShadowMapGen.GetValue() );
}
void rprfPrefsObject::UpdateSSAO(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->m_PrefsProxy->SetUseAOVolumes( m_Data.m_bUseAOVolumes.GetValue() );
	this->m_PrefsProxy->SetEnableSSAO( m_Data.m_bEnableSSAO.GetValue() );
	this->m_PrefsProxy->SetEnableSSAOBlur( m_Data.m_SSAOEnableBlur.GetValue() );
	m_PrefsProxy->SetEnableSSAODepthPeeling( m_Data.m_bEnableSSAODepthPeeling.GetValue() );
}
void rprfPrefsObject::UpdateSSAOSampling(prtyProperty *i_pProperty, bool i_bDirty)
{				
	int preset = MatchSSAOStateToPreset();
	m_Data.m_SSAOQuality.SetValue(preset);
	this->m_PrefsProxy->SetSSAONumSteps( m_Data.m_SSAONumSteps.GetValue() );
	this->m_PrefsProxy->SetSSAONumDirs( m_Data.m_SSAONumDirs.GetValue() );
	this->m_PrefsProxy->SetSSAONumLayers( m_Data.m_SSAONumLayers.GetValue() );
}
void rprfPrefsObject::UpdateSSAOQuality(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// We only want to do something if the property changed because of
	//	the user interface combo box. 
	switch(m_Data.m_SSAOQuality.GetValue())
		{
		case rprfPrefsData::e_SSAOLow:
			m_Data.m_SSAONumSteps.SetValue(AO_LOW_STEPS);
			m_Data.m_SSAONumDirs.SetValue(AO_LOW_DIRS);
			this->m_PrefsProxy->SetAOResolutionReduce(2);
			break;
		case rprfPrefsData::e_SSAOMed:
			m_Data.m_SSAONumSteps.SetValue(AO_MED_STEPS);
			m_Data.m_SSAONumDirs.SetValue(AO_MED_DIRS);
			this->m_PrefsProxy->SetAOResolutionReduce(1);
			break;
		case rprfPrefsData::e_SSAOHigh:
			m_Data.m_SSAONumSteps.SetValue(AO_HIGH_STEPS);
			m_Data.m_SSAONumDirs.SetValue(AO_HIGH_DIRS);
			this->m_PrefsProxy->SetAOResolutionReduce(0);
			break;
	}
}

void rprfPrefsObject::UpdateSSGI(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->m_PrefsProxy->SetUseLPVGI(m_Data.m_bUseLPVGI.GetValue());
	this->m_PrefsProxy->SetEnableSSGI( m_Data.m_bEnableSSGI.GetValue() );
	this->m_PrefsProxy->SetEnableSSGIBlur( m_Data.m_SSGIEnableBlur.GetValue() );
	m_PrefsProxy->SetEnableSSGIDepthPeeling( m_Data.m_bEnableSSGIDepthPeeling.GetValue() );
}
void rprfPrefsObject::UpdateSSGISampling(prtyProperty *i_pProperty, bool i_bDirty)
{				
	int preset = MatchSSGIStateToPreset();
	m_Data.m_SSGIQuality.SetValue(preset);
	this->m_PrefsProxy->SetSSGINumSteps( m_Data.m_SSGINumSteps.GetValue() );
	this->m_PrefsProxy->SetSSGINumDirs( m_Data.m_SSGINumDirs.GetValue() );
	this->m_PrefsProxy->SetSSGINumLayers( m_Data.m_SSGINumLayers.GetValue() );
}
void rprfPrefsObject::UpdateSSGIQuality(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// We only want to do something if the property changed because of
	//	the user interface combo box. 
	switch(m_Data.m_SSGIQuality.GetValue())
		{
		case rprfPrefsData::e_SSGILow:
			m_Data.m_SSGINumSteps.SetValue(GI_LOW_STEPS);
			m_Data.m_SSGINumDirs.SetValue(GI_LOW_DIRS);
			this->m_PrefsProxy->SetGIResolutionReduce(2);
			break;
		case rprfPrefsData::e_SSGIMed:
			m_Data.m_SSGINumSteps.SetValue(GI_MED_STEPS);
			m_Data.m_SSGINumDirs.SetValue(GI_MED_DIRS);
			this->m_PrefsProxy->SetGIResolutionReduce(1);
			break;
		case rprfPrefsData::e_SSGIHigh:
			m_Data.m_SSGINumSteps.SetValue(GI_HIGH_STEPS);
			m_Data.m_SSGINumDirs.SetValue(GI_HIGH_DIRS);
			this->m_PrefsProxy->SetGIResolutionReduce(0);
			break;
	}
}

void rprfPrefsObject::UpdateHardwareTessellation(prtyProperty *i_pProperty, bool i_bDirty)
{
	// This probably should be in g3d somewhere so that multiple
	// classes throughout the library can use the flag.
#if(SGPU_APP == MS_CORE)
	m_PrefsProxy->SetUseHardwareTessellation( false );
#else
	m_PrefsProxy->SetUseHardwareTessellation( m_Data.m_bUseHardwareTessellation.GetValue() );
	m_PrefsProxy->SetPixelSubdivLimit( m_Data.m_PixelSubdivLimit.GetValue() );
#endif
}

void rprfPrefsObject::UpdateRenderWireframe(prtyProperty *i_pProperty, bool i_bDirty)
{
	m_PrefsProxy->SetRenderWireframe( m_Data.m_bRenderWireframe.GetValue() );
	UpdateShadows( i_pProperty, i_bDirty );
}

// note this must match its values with the values in UpdateSSAOQuality
int rprfPrefsObject::MatchSSAOStateToPreset()
{	
	// simple round off, only works for positive ints.
	int steps = (int)(m_Data.m_SSAONumSteps.GetValue() + 0.5);
	int dirs = (int)(m_Data.m_SSAONumDirs.GetValue() + 0.5);
	if (steps == AO_LOW_STEPS && dirs == AO_LOW_DIRS)
		return 0;
	else if (steps == AO_MED_STEPS && dirs == AO_MED_DIRS)
		return 1;
	else if (steps == AO_HIGH_STEPS && dirs == AO_HIGH_DIRS)
		return 2;
	else
		return 3;
}

// note this must match its values with the values in UpdateSSGIQuality
int rprfPrefsObject::MatchSSGIStateToPreset()
{	
	// simple round off, only works for positive ints.
	int steps = (int)(m_Data.m_SSGINumSteps.GetValue() + 0.5);
	int dirs = (int)(m_Data.m_SSGINumDirs.GetValue() + 0.5);
	if (steps == GI_LOW_STEPS && dirs == GI_LOW_DIRS)
		return 0;
	else if (steps == GI_MED_STEPS && dirs == GI_MED_DIRS)
		return 1;
	else if (steps == GI_HIGH_STEPS && dirs == GI_HIGH_DIRS)
		return 2;
	else
		return 3;
}

//void rprfPrefsObject::RegisterSSAOSampling() 
//{
//	prtyComboBoxUIInfo* pCBUII;		
//	pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_SSAOQuality), "Ambient Occlusion", "Sampling quality presets");
//	AddProperty( pCBUII );
//	m_Data.m_SSAOQuality.AddCallback(new prtyCallbackWrapper<rprfPrefsObject>(this, &rprfPrefsObject::UpdateSSAOQuality));
//}

void rprfPrefsObject::UpdateTransparency(prtyProperty *i_pProperty, bool i_bDirty)
{
	m_PrefsProxy->SetTransparencyMode( m_Data.m_TransparencyMode.GetValue() );
	m_PrefsProxy->SetDebugDepthPeel( m_Data.m_bDebugDepthPeel.GetValue() );
	m_PrefsProxy->SetDebugSinglePeel( m_Data.m_bDebugSinglePeel.GetValue() );
	m_PrefsProxy->SetDebugDepthPeelLayers( m_Data.m_nDebugDepthPeelLayers.GetValue() );
}

void rprfPrefsObject::UpdateHair(prtyProperty *i_pProperty, bool i_bDirty)
{	
	m_PrefsProxy->SetEnableHair( m_Data.m_bEnableHair.GetValue() );
	m_PrefsProxy->SetHairLines( m_Data.m_bHairLines.GetValue() );
	m_PrefsProxy->SetHairTransparencyMode( m_Data.m_HairTransparencyMode.GetValue() );
//	m_PrefsProxy->SetHairShadowType( m_Data.m_HairShadowType.GetValue() );
//	m_PrefsProxy->SetHairShadowRes( m_Data.m_HairShadowRes.GetValue() );
	m_PrefsProxy->SetHairTessellation( m_Data.m_HairTessellation.GetValue() );
//	m_PrefsProxy->SetHairVertexLimit( (unsigned int)m_Data.m_HairVertexLimit.GetValue() );
//	m_PrefsProxy->SetHairSkipStrand( (unsigned int)m_Data.m_HairStrandSkip.GetValue() );
	m_PrefsProxy->SetHairSubPixelPower( m_Data.m_HairSubPixelPower.GetValue() );
	m_PrefsProxy->SetHairDepthPeel( m_Data.m_bHairDepthPeel.GetValue() );
	m_PrefsProxy->SetHairDepthPeelLayer( m_Data.m_nHairDepthPeelLayers.GetValue() );
	m_PrefsProxy->SetHairInterpolationCount( (unsigned int)m_Data.m_HairInterpolationCount.GetValue() );
	m_PrefsProxy->SetHairClumpRadius( m_Data.m_HairClumpRadius.GetValue() );
}

void rprfPrefsObject::UpdateRenderManOptions(prtyProperty *i_pProperty, bool i_bDirty)
{	
	//if ( m_Data.m_RmanBatchContent.GetValue() == 0 || m_Data.m_RmanBatchContent.GetValue() == 2 )
	//{
	//	m_Data.m_bRmanCacheTextures.SetValue(true);
	//}

	// Safeguard max distances
	//if ( m_Data.m_RmanAOMaxDist.GetValue() <= 0 )
	//{
	//	m_Data.m_RmanAOMaxDist.SetValue(0.001);
	//}
	//if ( m_Data.m_RmanGIMaxDist.GetValue() <= 0 )
	//{
	//	m_Data.m_RmanGIMaxDist.SetValue(0.001);
	//}
}
