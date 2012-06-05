/*****************************************************************************
**	gpxProjectedLight.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxProjectedLight.hpp"

#include "Core/Ma/maConstants.hpp"
#include "Core/Ma/maRotation.hpp"
#include "Tool/gpx/gpxCamera.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
// Constructor takes reference to light it will control
//--------------------------------------------------------------------
gpxProjectedLight::gpxProjectedLight(g3dProjectedLight &i_Light)
:	gpxLight(i_Light),
	m_Light(i_Light)
{
#if USE_PROXIES
	m_Position = i_Light.GetPosition();
	m_Target = i_Light.GetTarget();
	m_Range = i_Light.GetRange();
	m_bDirectional = i_Light.GetIsDirectional();
	m_Angle = i_Light.GetAngle();
	m_InnerAngle = i_Light.GetInnerAngle();
	m_Scale = i_Light.GetScale();
	m_Aspect = i_Light.GetAspect();
	m_Tilt = i_Light.GetTilt();
	m_LightSize = i_Light.GetLightSize();
	m_PCSSAdjust = i_Light.GetPCSSAdjust();
	m_ShadowQuality = i_Light.GetShadowQuality();
	m_ShadowIntensity = i_Light.GetShadowIntensity();
	m_ShadowColor = i_Light.GetShadowColor();
	m_f0 = i_Light.GetFalloff0();
	m_f1 = i_Light.GetFalloff1();
	m_f2 = i_Light.GetFalloff2();
	m_f3 = i_Light.GetFalloff3();
	m_FalloffStart = i_Light.GetFalloffStart();
	m_DepthBias = i_Light.GetDepthBias();
	m_HairMinBound = i_Light.GetHairMinBound();
	m_HairMaxBound = i_Light.GetHairMaxBound();
	m_HairShadowType = i_Light.GetHairShadowType();
	m_bGIEnabled = i_Light.GetGIEnabled();
#endif

	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxProjectedLight::~gpxProjectedLight()
{
	PROXY_REMOVE();
}

//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the light when "Update()" is called.
//--------------------------------------------------------------------
void gpxProjectedLight::SetPosition(const maVector3d& i_Position)
{
	PROXY_SET_OR_STORE(m_Light, SetPosition, m_Position, i_Position);
}
void gpxProjectedLight::SetTarget(const maVector3d& i_Target)
{
	PROXY_SET_OR_STORE(m_Light, SetTarget, m_Target, i_Target);
}
void gpxProjectedLight::SetRange(float i_Range)
{
	PROXY_SET_OR_STORE(m_Light, SetRange, m_Range, i_Range);
}
void gpxProjectedLight::SetIsDirectional(bool i_bDirectional)
{
	PROXY_SET_OR_STORE(m_Light, SetIsDirectional, m_bDirectional, i_bDirectional);
}
void gpxProjectedLight::SetAngle(float i_Angle)
{
	PROXY_SET_OR_STORE(m_Light, SetAngle, m_Angle, i_Angle);
}
void gpxProjectedLight::SetInnerAngle(float i_Angle)
{
	PROXY_SET_OR_STORE(m_Light, SetInnerAngle, m_InnerAngle, i_Angle);
}
void gpxProjectedLight::SetScale(float i_Scale)
{
	PROXY_SET_OR_STORE(m_Light, SetScale, m_Scale, i_Scale);
}
void gpxProjectedLight::SetLightSize(float i_LightSize)
{
	PROXY_SET_OR_STORE(m_Light, SetLightSize, m_LightSize, i_LightSize);
}
void gpxProjectedLight::SetPCSSAdjust(float i_PCSSAdjust)
{
	PROXY_SET_OR_STORE(m_Light, SetPCSSAdjust, m_PCSSAdjust, i_PCSSAdjust);
}
void gpxProjectedLight::SetShadowQuality(g3dProjectedLight::ShadowQuality i_ShadowQuality)
{
	PROXY_SET_OR_STORE(m_Light, SetShadowQuality, m_ShadowQuality, i_ShadowQuality);
}
void gpxProjectedLight::SetShadowIntensity(float i_ShadowIntensity)
{
	PROXY_SET_OR_STORE(m_Light, SetShadowIntensity, m_ShadowIntensity, i_ShadowIntensity);
}
void gpxProjectedLight::SetShadowColor(const maFloatRGBA& i_ShadowColor)
{
	PROXY_SET_OR_STORE(m_Light, SetShadowColor, m_ShadowColor, i_ShadowColor);
}
void gpxProjectedLight::SetDepthBias(float i_DepthBias)
{
	PROXY_SET_OR_STORE(m_Light, SetDepthBias, m_DepthBias, i_DepthBias);
}
void gpxProjectedLight::SetAspect(float i_Aspect)
{
	PROXY_SET_OR_STORE(m_Light, SetAspect, m_Aspect, i_Aspect);
}
void gpxProjectedLight::SetTilt(float i_Tilt)
{
	PROXY_SET_OR_STORE(m_Light, SetTilt, m_Tilt, i_Tilt);
}
void gpxProjectedLight::SetFalloff0(float i_Val)
{
	PROXY_SET_OR_STORE(m_Light, SetFalloff0, m_f0, i_Val);
}
void gpxProjectedLight::SetFalloff1(float i_Val)
{
	PROXY_SET_OR_STORE(m_Light, SetFalloff1, m_f1, i_Val);
}
void gpxProjectedLight::SetFalloff2(float i_Val)
{
	PROXY_SET_OR_STORE(m_Light, SetFalloff2, m_f2, i_Val);
}
void gpxProjectedLight::SetFalloff3(float i_Val)
{
	PROXY_SET_OR_STORE(m_Light, SetFalloff3, m_f3, i_Val);
}
void gpxProjectedLight::SetFalloffStart(float i_Val)
{
	PROXY_SET_OR_STORE(m_Light, SetFalloffStart, m_FalloffStart, i_Val);
}
void gpxProjectedLight::SetGIEnabled(bool i_Val)
{
	PROXY_SET_OR_STORE(m_Light, SetGIEnabled, m_bGIEnabled, i_Val);
}
void gpxProjectedLight::SetHairMinBound( float i_Val )
{
	PROXY_SET_OR_STORE(m_Light, SetHairMinBound, m_HairMinBound, i_Val);
}
void gpxProjectedLight::SetHairMaxBound( float i_Val )
{
	PROXY_SET_OR_STORE(m_Light, SetHairMaxBound, m_HairMaxBound, i_Val);
}
void gpxProjectedLight::SetHairShadowType( HAIR_SHADOW_TYPE i_Val )
{
	PROXY_SET_OR_STORE(m_Light, SetHairShadowType, m_HairShadowType, i_Val);
}

//--------------------------------------------------------------------
//	Get functions just return the data internally based on
//	the "Set" calls earlier.
//--------------------------------------------------------------------
float gpxProjectedLight::GetScale() const
{
	return PROXY_GET(m_Light, GetScale, m_Scale);
}
float gpxProjectedLight::GetAngle() const
{
	return PROXY_GET(m_Light, GetAngle, m_Angle);
}
float gpxProjectedLight::GetInnerAngle() const
{
	return PROXY_GET(m_Light, GetInnerAngle, m_InnerAngle);
}
const maPoint3d& gpxProjectedLight::GetPosition() const
{
	return PROXY_GET(m_Light, GetPosition, m_Position);
}
const maPoint3d& gpxProjectedLight::GetTarget() const
{
	return PROXY_GET(m_Light, GetTarget, m_Target);
}

//--------------------------------------------------------------------
//	Orient the given camera proxy to the direction of this light,
//	used for rendering depth maps. 
//--------------------------------------------------------------------
void gpxProjectedLight::OrientCamera(gpxCamera& o_Camera) const
{
#if USE_PROXIES
	// Projection:
	o_Camera.SetOrthographic(m_bDirectional);
	o_Camera.SetAspect(m_Aspect);
	o_Camera.SetClip(m_Scale, m_Scale+m_Range);
	o_Camera.SetFOV(m_Angle);
	o_Camera.SetOrthoWidth(m_Scale);

	// direction
	maVector3d direction = m_Target - m_Position;
	bool valid_len = direction.Normalize();
	if (!valid_len)
		direction.Set(0,0,1);

	// Look At:
	maPoint3d cam_pos = m_Position - direction * m_Scale;
	maVector3d up_vec(0,1,0);
	if (m_Tilt != 0)
	{
		// rotate up vector through tilt angle
		maRotation rot(direction, maConstants::c_fAngleToRad * m_Tilt);
		rot.RotateVector(up_vec);
	}

	o_Camera.LookAt(cam_pos, m_Position, up_vec);
#else
	return m_Light.OrientCamera(o_Camera.GetCamera());
#endif
}

//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxProjectedLight::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	// The base class will not ask for a lock
	gpxLight::Update();

	m_Light.SetPosition( m_Position );
	m_Light.SetTarget( m_Target );
	m_Light.SetRange( m_Range );
	m_Light.SetIsDirectional( m_bDirectional );
	m_Light.SetAngle( m_Angle );
	m_Light.SetInnerAngle( m_InnerAngle );
	m_Light.SetScale( m_Scale );
	m_Light.SetLightSize( m_LightSize );
	m_Light.SetPCSSAdjust( m_PCSSAdjust );
	m_Light.SetShadowQuality( m_ShadowQuality );
	m_Light.SetShadowIntensity( m_ShadowIntensity );
	m_Light.SetShadowColor( m_ShadowColor );
	m_Light.SetDepthBias( m_DepthBias );
	m_Light.SetAspect( m_Aspect );
	m_Light.SetTilt( m_Tilt );
	m_Light.SetFalloff0( m_f0 );
	m_Light.SetFalloff1( m_f1 );
	m_Light.SetFalloff2( m_f2 );
	m_Light.SetFalloff3( m_f3 );
	m_Light.SetFalloffStart( m_FalloffStart );
	m_Light.SetGIEnabled( m_bGIEnabled );
	m_Light.SetHairMinBound( m_HairMinBound );
	m_Light.SetHairMaxBound( m_HairMaxBound );
	m_Light.SetHairShadowType( m_HairShadowType );

	this->SetNeedsUpdate(false);
#endif // USE_PROXIES

	return true;
}

