/****************************************************************************\
**	g3dScene.hpp
**
**		The g3dScene holds the g3dLayers which describe a 3d scene
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_SCENE_HPP
#error g3dScene.hpp multiply included
#endif
#define G3D_SCENE_HPP

#ifndef G3D_LAYERCONTAINER_HPP
#include "Graphics/G3d/g3dLayerContainer.hpp"
#endif 
#ifndef G3D_TYPE_HPP
#include "Graphics/g3d/g3dType.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MA_VECTOR3D_HPP
#include "Core/ma/maVector3d.hpp"
#endif
#ifndef G3D_RENDERSTATE_HPP
#include "Graphics/g3d/g3dRenderState.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================
//class g3dSceneNode;
class g3dLayer;

//============================================================================
//============================================================================
struct ssaoParams
{
	float m_AORadius;
	float m_AORadiusFar;
	float m_AngleBias;
	float m_Contrast;
	float m_Attenuation;

	float m_BlurWidth;
	float m_BlurSharpness;
	int   m_OverscanPixels;

	// ao volumes tweaks
	float m_ClipPlaneEpsilon;
	float m_NoClipPlaneEpsilon;
	float m_AreaRatio;
	float m_BehindPlaneEpsilon;

	maFloatRGBA m_Color;
	ssaoParams()
		:
		m_AORadius(25),
		m_AORadiusFar(50),
		m_AngleBias(0.0f),
		m_Contrast(1.4f),
		m_Attenuation(1.0f),
		m_BlurWidth(7),
		m_BlurSharpness(2),
		m_OverscanPixels(0),
		m_ClipPlaneEpsilon(0.01f),
		m_NoClipPlaneEpsilon(0.01f),
		m_AreaRatio(0.1f),
		m_BehindPlaneEpsilon(0.01f)
	{}
};

//============================================================================
//============================================================================
struct ssgiParams
{
	float m_GIRadius;
	float m_GIRadiusFar;
	float m_AngleBias;
	float m_Contrast;
	float m_Attenuation;

	float m_BlurWidth;
	float m_BlurSharpness;
	int   m_OverscanPixels;

	/*float m_RSMGIScale;
	float m_RSMGISampleRadius;
	int	  m_RSMGISampleNum;
	float m_GILightScale;
	int m_RSMSize;*/

	// LPVGI Variables
	float m_LPVScale;
	int	  m_LPVIteration;
	int	  m_LPVVolumeSize;
	float m_LPVGIFalloff;
	int	  m_LPVRSMSize;

	maFloatRGBA m_Color;
	ssgiParams()
		:
		m_GIRadius(100),
		m_GIRadiusFar(500),
		m_AngleBias(0.0f),
		m_Contrast(1.0f),
		m_Attenuation(1.0f),
		m_BlurWidth(7),
		m_BlurSharpness(2),
		m_OverscanPixels(0),
		m_Color(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f)),
		/*m_RSMGIScale(1.0f),
		m_RSMGISampleRadius(0.8f),
		m_RSMGISampleNum(3),
		m_GILightScale(1.0f),
		m_RSMSize(256),*/
		m_LPVScale(1.0f),
		m_LPVIteration(2),
		m_LPVVolumeSize(32),
		m_LPVGIFalloff(1.0f),
		m_LPVRSMSize(512)
	{}
};

//============================================================================
//============================================================================
struct fogParams
{
	g3dType::FogMode m_nMode;
	maFloatRGBA m_Color;
	maVector3d m_Orientation;
	bool m_bUseWorld;
	float m_fStart;
	float m_fEnd;
	float m_fDensity;
	float m_fAltitudeStart;
	float m_fAltitudeEnd;
	float m_fAltitudeDensity;

	fogParams()
		:
		m_nMode(g3dType::e_FogModeNone),
		m_fStart(1),
		m_fEnd(1000),
		m_fDensity(1),
		m_fAltitudeStart(0),
		m_fAltitudeEnd(10),
		m_fAltitudeDensity(1)
	{}
};

//============================================================================
//============================================================================
struct MotionBlurParams
{
	int m_nSamples;
	float m_fPercent;

	MotionBlurParams() :
		m_nSamples(20),
		m_fPercent(1.0)
	{}
};

//============================================================================
//============================================================================
struct HairParams
{
	int m_nSamples;
	float m_fPercent;

	HairParams() :
	m_nSamples(20),
		m_fPercent(1.0)
	{}
};


//============================================================================
//============================================================================
class g3dScene : public g3dLayerContainer
{
public:
	//--------------------------------------------------------------------
	// default constructor has no layers and no world root
	//--------------------------------------------------------------------
	g3dScene();

	//--------------------------------------------------------------------
	// creates scene with single layer and sets it to be the world root
	//--------------------------------------------------------------------
	g3dScene(g3dLayer* i_Layer);

	//--------------------------------------------------------------------
	// construct scene with given layer scheme.
	//	the scene takes ownership of these layers.
	//--------------------------------------------------------------------
	g3dScene(const std::vector<g3dLayer*> &i_Layers);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~g3dScene();

	//------------------------------------------------------------------------
	//	SetFog sets fog parameters for this scene
	//------------------------------------------------------------------------
	void SetFog(const fogParams& i_FogParams);

	//------------------------------------------------------------------------
	//	Returns true if fog mode is an enabled state
	//------------------------------------------------------------------------
	bool IsFogEnabled() const;

	//------------------------------------------------------------------------
	// Returns color of fog
	//------------------------------------------------------------------------
	const maFloatRGBA& GetFogColor() const;

	//------------------------------------------------------------------------
	// Returns orientation of fog
	//------------------------------------------------------------------------
	const maVector3d& GetFogOrientation() const;

	//------------------------------------------------------------------------
	// Returns orientation of fog
	//------------------------------------------------------------------------
	bool IsFogWorldOriented() const;

	//------------------------------------------------------------------------
	// Returns mode, start, end, density settings of fog
	//------------------------------------------------------------------------
	void GetFogSettings(fogParams& o_FogParams) const;

	//------------------------------------------------------------------------
	// Settings of ssao
	//------------------------------------------------------------------------
	void SetSSAOSettings(const ssaoParams& i_SSAOParams);
	void GetSSAOSettings(ssaoParams& o_SSAOParams) const;

	//------------------------------------------------------------------------
	// Settings of ssgi
	//------------------------------------------------------------------------
	void SetSSGISettings(const ssgiParams& i_SSGIParams);
	void GetSSGISettings(ssgiParams& o_SSGIParams) const;

	//------------------------------------------------------------------------
	// Settings of MotionBlur
	//------------------------------------------------------------------------
	void SetMotionBlurSettings(const MotionBlurParams& i_MotionBlurParams);
	void GetMotionBlurSettings( MotionBlurParams& o_MotionBlurParams) const;

	//------------------------------------------------------------------------
	// Settings of global ambient state
	//------------------------------------------------------------------------
	void SetGlobalAmbient(const g3dAmbientEnvState& i_GlobalAmbient);
	void GetGlobalAmbient(g3dAmbientEnvState& o_GlobalAmbient) const;

private:
	// Fog settings
	fogParams m_FogParams;

	ssaoParams m_SSAOParams;
	ssgiParams m_SSGIParams;

	//Motion Blur Parameters
	MotionBlurParams m_MotionBlurParams;

	g3dAmbientEnvState m_GlobalAmbient;
};
