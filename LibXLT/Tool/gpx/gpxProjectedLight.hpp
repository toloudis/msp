/*****************************************************************************
**	gpxProjectedLight.hpp
**
**	This class is a thread-safe proxy for a g3dProjectedLight.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_PROJECTEDLIGHT_HPP
#error gpxProjectedLight.hpp multiply included
#endif
#define GPX_PROJECTEDLIGHT_HPP

#ifndef GPX_LIGHT_HPP
#include "Tool/gpx/gpxLight.hpp"
#endif 
#ifndef G3D_PROJECTEDLIGHT_HPP
#include "Graphics/G3d/g3dProjectedLight.hpp"
#endif 


//============================================================================
//	forward references
//============================================================================
class g3dProjectedLight;
class gpxCamera;


//============================================================================
//============================================================================
class gpxProjectedLight : public gpxLight
{
public:
	//--------------------------------------------------------------------
	// Constructor takes reference to light it will control
	//--------------------------------------------------------------------
	explicit gpxProjectedLight(g3dProjectedLight &i_Light);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxProjectedLight();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the light when "Update()" is called.
	//--------------------------------------------------------------------
	void SetPosition(const maVector3d& i_Position);
	void SetTarget(const maVector3d& i_Target);
	void SetRange(float i_Range);
	void SetIsDirectional(bool i_bDirectional);
	void SetAngle(float i_Angle);
	void SetInnerAngle(float i_InnerAngle);
	void SetScale(float i_Scale);
	void SetLightSize(float i_LightSize);
	void SetPCSSAdjust(float i_PCSSAdjust);
	void SetShadowQuality(g3dProjectedLight::ShadowQuality i_ShadowQuality);
	void SetShadowIntensity(float i_ShadowIntensity);
	void SetShadowColor(const maFloatRGBA& i_ShadowColor);
	void SetDepthBias(float i_DepthBias);
	void SetAspect(float i_Aspect);
	void SetTilt(float i_Tilt);
	void SetFalloff0(float i_Val);
	void SetFalloff1(float i_Val);
	void SetFalloff2(float i_Val);
	void SetFalloff3(float i_Val);
	void SetFalloffStart(float i_Val);
	void SetGIEnabled(bool i_Val);

	void SetHairMinBound( float i_Val );
	void SetHairMaxBound( float i_Val );
	void SetHairShadowType( HAIR_SHADOW_TYPE i_Val );

	//--------------------------------------------------------------------
	//	Get functions just return the data internally based on
	//	the "Set" calls earlier.
	//--------------------------------------------------------------------
	float GetScale() const;
	float GetAngle() const;
	float GetInnerAngle() const;
	const maPoint3d& GetPosition() const;
	const maPoint3d& GetTarget() const;


	//--------------------------------------------------------------------
	//	Orient the given camera proxy to the direction of this light,
	//	used for rendering depth maps. 
	//--------------------------------------------------------------------
	void OrientCamera(gpxCamera& o_Camera) const;

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
	g3dProjectedLight &m_Light;

// Extra copy of data if using proxy info
#if USE_PROXIES
	maPoint3d m_Position;
	maPoint3d m_Target;
	bool m_bDirectional;
	float m_Range, m_Angle, m_InnerAngle, m_Scale, m_Aspect, m_Tilt;
	float m_LightSize;
	float m_PCSSAdjust;
	g3dProjectedLight::ShadowQuality m_ShadowQuality;
	float m_ShadowIntensity;
	maFloatRGBA m_ShadowColor;
	float m_f0, m_f1, m_f2, m_f3;
	float m_FalloffStart;
	float m_DepthBias;
	float m_HairMinBound;
	float m_HairMaxBound;
	HAIR_SHADOW_TYPE m_HairShadowType;
	bool m_bGIEnabled;
#endif
};
