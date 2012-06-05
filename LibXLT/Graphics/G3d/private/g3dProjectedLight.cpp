/****************************************************************************\
**	g3dProjectedLight.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dProjectedLight.hpp"

#include "Core/ma/maConstants.hpp"
#include "Core/ma/maRotation.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/Mat/matTexture.hpp"


//--------------------------------------------------------------------
//	The default constructor places the light at the origin.
//--------------------------------------------------------------------
g3dProjectedLight::g3dProjectedLight()
:	m_Position(0, 0, 0), m_Direction(0, 0, 1), m_Target(0, 0, -1),
	m_Range(100.0f), m_Angle(90), m_Scale(1.0f), m_InnerAngle(180),
	m_shadowIntensity(1.0f), m_bDirectional(false),
	m_Aspect(1.0f), m_Tilt(0),
	m_LightSize(0.0f), m_PCSSAdjust(0.0f), m_SceneScale(1.0f), m_ShadowQuality(SQ_MED),
	m_f0(1.0f),	m_f1(0.0f),	m_f2(0.0f), m_f3(0.0f), m_falloffStart(0.0f),
	m_pTexture(NULL), m_pShadowMap(NULL),
	m_bProjDirty(true), m_bCamDirty(true), m_bShiftDirty(true),
	m_DepthBias(0.002f), m_bGIEnabled(false), m_pReflectiveMap(NULL)
{
	for( int i = 0; i < MAX_OPACITY_MAPS; i++ )
	{
		m_pOpacityShadowMaps[i] = NULL;
	}
	m_pOpacityVolume = NULL;

	m_HairMaxBound = 0.0f;
	m_HairMaxBound = 1000000.0f;
	m_HairShadowType = HAIR_SHADOW_OSM4;

	this->SetCastsShadow(true);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dProjectedLight::g3dProjectedLight(const g3dProjectedLight& i_Copy)
:	m_Position(i_Copy.m_Position), m_Direction(i_Copy.m_Direction), m_Target(i_Copy.m_Target),
	m_Range(i_Copy.m_Range), m_Angle(i_Copy.m_Angle), m_Scale(i_Copy.m_Scale), 
	m_shadowIntensity(i_Copy.m_shadowIntensity), 
	m_InnerAngle(i_Copy.m_InnerAngle), m_bDirectional(i_Copy.m_bDirectional),
	m_Aspect(i_Copy.m_Aspect), m_Tilt(i_Copy.m_Tilt),
	m_LightSize(i_Copy.m_LightSize), m_PCSSAdjust(i_Copy.m_PCSSAdjust), m_SceneScale(i_Copy.m_SceneScale), m_ShadowQuality(i_Copy.m_ShadowQuality),
	m_f0(i_Copy.m_f0),	m_f1(i_Copy.m_f1),	m_f2(i_Copy.m_f2),m_f3(i_Copy.m_f3), m_falloffStart(i_Copy.m_falloffStart),
	m_pTexture(i_Copy.m_pTexture), m_pShadowMap(i_Copy.m_pShadowMap),
	m_bProjDirty(true), m_bCamDirty(true), m_bShiftDirty(true),
	m_DepthBias(i_Copy.m_DepthBias),
	m_HairMinBound(i_Copy.m_HairMinBound),
	m_HairMaxBound(i_Copy.m_HairMaxBound),
	m_HairShadowType(i_Copy.m_HairShadowType),
	m_bGIEnabled(i_Copy.m_bGIEnabled), m_pReflectiveMap(i_Copy.m_pReflectiveMap)
{
	for( int i = 0; i < MAX_OPACITY_MAPS; i++ )
	{
		m_pOpacityShadowMaps[i] = NULL;
	}
	m_pOpacityVolume = NULL;

	this->SetCastsShadow(i_Copy.GetCastsShadow());
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dProjectedLight::~g3dProjectedLight()
{
}

//--------------------------------------------------------------------
//	Position of the light. 
//	As opposed to cameras, this position is on the near plane 
//	of the projection.
//--------------------------------------------------------------------
const maVector3d& g3dProjectedLight::GetPosition() const
{
	return m_Position;
}
void g3dProjectedLight::SetPosition(const maVector3d& i_Position)
{
	m_Position = i_Position;

	set_direction(m_Target - m_Position);

	m_bCamDirty = true;
}

//--------------------------------------------------------------------
//	Target of the light. This is the way to set the direction
//  of the light. In rendering, only the direction will be used.
//--------------------------------------------------------------------
const maVector3d& g3dProjectedLight::GetTarget() const
{
	return m_Target;
}
void g3dProjectedLight::SetTarget(const maVector3d& i_Target)
{
	m_Target = i_Target;

	set_direction(m_Target - m_Position);

	m_bCamDirty = true;
}

//--------------------------------------------------------------------
//	Direction of the projection of the light, defined by
//  position and target
//--------------------------------------------------------------------
const maVector3d& g3dProjectedLight::GetDirection() const
{
	return m_Direction;
}

//--------------------------------------------------------------------
//	Range of the light, counting from the position of the light
//		along the direction vector.
//--------------------------------------------------------------------
float g3dProjectedLight::GetRange() const
{
	return m_Range;
}
void g3dProjectedLight::SetRange(float i_Range)
{
	DBG_ASSERT(i_Range > 0, "This value can't be negative");
	if (i_Range <= 0)
		m_Range = 1.0f;
	
	m_Range = i_Range;

	m_bProjDirty = true;
}

//--------------------------------------------------------------------
// If the Directional flag is true, then the projected light behaves 
// like a directional light. It uses an orthographic camera matrix
// and the lighting algorithm treats the light source as an infinite
// light soure instead of a point light source.
// When in directional mode, the angle and inner angle fields 
// are ignored.
// Default value is false, which means "spot light mode".
//--------------------------------------------------------------------
bool g3dProjectedLight::GetIsDirectional() const
{
	return m_bDirectional;
}
void g3dProjectedLight::SetIsDirectional(bool i_bDirectional)
{
	m_bDirectional = i_bDirectional;

	m_bProjDirty = true;
}

//--------------------------------------------------------------------
//	Angle of the light, in degrees. This represents the angle of 
//	the "spotlight"	effect. This is the same as "FOV" in a camera.
//--------------------------------------------------------------------
float g3dProjectedLight::GetAngle() const
{
	return m_Angle;
}
void g3dProjectedLight::SetAngle(float i_Angle)
{
	DBG_ASSERT(i_Angle >= 0, "This value can't be negative");
	DBG_ASSERT(i_Angle < 180, "This value must be less than 180 degrees.");
	if (i_Angle < 0 || i_Angle >= 180)
		i_Angle = 0.0f;
	m_Angle = i_Angle;

	m_bProjDirty = true;
}

//--------------------------------------------------------------------
//	Inner Angle of the light, in degrees. This represents the angle 
//	where the spotlight is at full intensity. From this inner angle
//  out to the "Angle" value, the light's intensity should falloff.
//--------------------------------------------------------------------
float g3dProjectedLight::GetInnerAngle() const
{
	return m_InnerAngle;
}
void g3dProjectedLight::SetInnerAngle(float i_Angle)
{
	DBG_ASSERT(i_Angle >= 0, "This value can't be negative");
	DBG_ASSERT(i_Angle <= 180, "This value must be less than or equal to 180 degrees.");
	if (i_Angle < 0)
		m_InnerAngle = 0.0f;
	else if (i_Angle > 180)
		m_InnerAngle = 180;
	else
		m_InnerAngle = i_Angle;
}

//--------------------------------------------------------------------
//	Scale of the light.  This increases the overall size of the 
//	light, causing it to affect a larger area.
//--------------------------------------------------------------------
float g3dProjectedLight::GetScale() const
{
	return m_Scale;
}
void g3dProjectedLight::SetScale(float i_Scale)
{
	DBG_ASSERT(i_Scale >= 0, "This value can't be negative");
	if (i_Scale < 0)
		i_Scale = 0.0f;
	m_Scale = i_Scale;

	// scale alters both matrices
	m_bProjDirty = true;
	m_bCamDirty = true;
}

float g3dProjectedLight::GetLightSize() const
{
	return m_LightSize;
}
void g3dProjectedLight::SetLightSize(float i_LightSize)
{
	DBG_ASSERT(i_LightSize >= 0, "This value can't be negative");
	if (i_LightSize < 0)
		i_LightSize = 0;
	m_LightSize = i_LightSize;
}

float g3dProjectedLight::GetPCSSAdjust() const
{
	return m_PCSSAdjust;
}
void g3dProjectedLight::SetPCSSAdjust(float i_PCSSAdjust)
{
	DBG_ASSERT(i_PCSSAdjust >= 0, "This value can't be negative");
	if (i_PCSSAdjust < 0)
		i_PCSSAdjust = 0.0f;
	m_PCSSAdjust = i_PCSSAdjust;
}

float g3dProjectedLight::GetSceneScale() const
{
	return m_SceneScale;
}
void g3dProjectedLight::SetSceneScale(float i_SceneScale)
{
	DBG_ASSERT(i_SceneScale >= 0, "This value can't be negative");
	if (i_SceneScale < 0)
		i_SceneScale = 0.0f;
	m_SceneScale = i_SceneScale;
}
g3dProjectedLight::ShadowQuality g3dProjectedLight::GetShadowQuality() const
{
	return m_ShadowQuality;
}
void g3dProjectedLight::SetShadowQuality(ShadowQuality i_ShadowQuality)
{
	m_ShadowQuality = i_ShadowQuality;
}

float g3dProjectedLight::GetShadowIntensity() const
{
	return m_shadowIntensity;
}
void g3dProjectedLight::SetShadowIntensity(float i_shadowIntensity)
{
	m_shadowIntensity = i_shadowIntensity;
}

maFloatRGBA g3dProjectedLight::GetShadowColor() const
{
	return m_ShadowColor;
}
void g3dProjectedLight::SetShadowColor(const maFloatRGBA& i_ShadowColor)
{
	m_ShadowColor = i_ShadowColor;
}

//--------------------------------------------------------------------
//	Shadow mapping depth bias (applied at render time, not shadow map generation)
//--------------------------------------------------------------------
float g3dProjectedLight::GetDepthBias() const
{
	return m_DepthBias;
}
void g3dProjectedLight::SetDepthBias(float i_DepthBias)
{
	m_DepthBias = i_DepthBias;
	m_bShiftDirty = true;
}

//--------------------------------------------------------------------
//	Aspect ratio of the light, (width / height)
//--------------------------------------------------------------------
float g3dProjectedLight::GetAspect() const
{
	return m_Aspect;
}
void g3dProjectedLight::SetAspect(float i_Aspect)
{
	DBG_ASSERT(i_Aspect >= 0, "This value can't be negative");
	if (i_Aspect < 0)
		i_Aspect = 0.0f;
	m_Aspect = i_Aspect;

	m_bProjDirty = true;
}

//--------------------------------------------------------------------
//	Tilt rotation around the view direction. In degrees.
//--------------------------------------------------------------------
float g3dProjectedLight::GetTilt() const
{
	return m_Tilt;
}
void g3dProjectedLight::SetTilt(float i_Tilt)
{
	m_Tilt = i_Tilt;
	m_bCamDirty = true;
}

//--------------------------------------------------------------------
//	GetFalloff0, GetFalloff1, and GetFalloff2 all return
//	coefficients that describe the falloff of the pointlight.
//	Some (but not all) of the coefficients may be zero.
//--------------------------------------------------------------------
float g3dProjectedLight::GetFalloff0() const
{
	return m_f0;
}

float g3dProjectedLight::GetFalloff1() const
{
	return m_f1;
}

float g3dProjectedLight::GetFalloff2() const
{
	return m_f2;
}

float g3dProjectedLight::GetFalloff3() const
{
	return m_f3;
}

float g3dProjectedLight::GetFalloffStart() const
{
	return m_falloffStart;
}

//--------------------------------------------------------------------
//	The SetFalloff0-2 functions allow the user to set the falloff
//	curve.
//--------------------------------------------------------------------
void g3dProjectedLight::SetFalloff0(float i_Val)
{
	DBG_ASSERT(i_Val != 0, "This value can't be negative");
	if (i_Val == 0.0f)
		i_Val = 1.0f;
	m_f0 = i_Val;
}

void g3dProjectedLight::SetFalloff1(float i_Val)
{
	DBG_ASSERT(i_Val >= 0, "This value can't be negative");
	if (i_Val < 0)
		i_Val = 0.0f;
	m_f1 = i_Val;
}

void g3dProjectedLight::SetFalloff2(float i_Val)
{
	DBG_ASSERT(i_Val >= 0, "This value can't be negative");
	if (i_Val < 0)
		i_Val = 0.0f;
	m_f2 = i_Val;
}

void g3dProjectedLight::SetFalloff3(float i_Val)
{
	DBG_ASSERT(i_Val >= 0, "This value can't be negative");
	if (i_Val < 0)
		i_Val = 0.0f;
	m_f3 = i_Val;
}

void g3dProjectedLight::SetFalloffStart(float i_Val)
{
	DBG_ASSERT(i_Val >= 0, "This value can't be negative");
	if (i_Val < 0)
		i_Val = 0.0f;
	m_falloffStart = i_Val;
}

//--------------------------------------------------------------------
//	GI Properties
//--------------------------------------------------------------------
bool g3dProjectedLight::GetGIEnabled() const
{
	return m_bGIEnabled;
}

void g3dProjectedLight::SetGIEnabled( bool i_Val )
{
	m_bGIEnabled = i_Val;
}

float g3dProjectedLight::GetHairMinBound() const
{
	return m_HairMinBound;
}

void g3dProjectedLight::SetHairMinBound( float i_Val )
{
	m_HairMinBound = i_Val;
}

float g3dProjectedLight::GetHairMaxBound() const
{
	return m_HairMaxBound;
}

void g3dProjectedLight::SetHairMaxBound( float i_Val )
{
	m_HairMaxBound = i_Val;
}

HAIR_SHADOW_TYPE g3dProjectedLight::GetHairShadowType() const
{
	return m_HairShadowType;
}

void g3dProjectedLight::SetHairShadowType( HAIR_SHADOW_TYPE i_Val )
{
	m_HairShadowType = i_Val;
}

//--------------------------------------------------------------------
// This is the texture that is projected as the color of the light.
//	The texture is not owned by the light.
//--------------------------------------------------------------------
void g3dProjectedLight::SetTexture(matTexture* i_pTexture)
{
	m_pTexture = i_pTexture;
}
matTexture* g3dProjectedLight::GetTexture()
{
	return m_pTexture;
}
const matTexture* g3dProjectedLight::GetTexture() const
{
	return m_pTexture;
}

//--------------------------------------------------------------------
// This is the texture that is used to determine the shadows
//	for this light.  This may need to be updated each frame.
//	The texture is not owned by the light and it is not 
//	automatically updatred by the light.
//--------------------------------------------------------------------
void g3dProjectedLight::SetShadowMap(matTexture* i_pShadowMap)
{
	m_pShadowMap = i_pShadowMap;
#ifdef SHADOW_ALIGNMENT
	m_bShiftDirty = true;			//the dimensions might have changed so update the texture alignment
#endif
}
matTexture* g3dProjectedLight::GetShadowMap()
{
	return m_pShadowMap;
}
const matTexture* g3dProjectedLight::GetShadowMap() const
{
	return m_pShadowMap;
}

//--------------------------------------------------------------------
// This is the texture that is used to determine the gi contribution
//	from this light.  This may need to be updated each frame.
//	The texture is not owned by the light and it is not 
//	automatically updated by the light.
//--------------------------------------------------------------------
void g3dProjectedLight::SetReflectiveMap(matTexture* i_pReflectiveMap)
{
	m_pReflectiveMap = i_pReflectiveMap;
}

matTexture* g3dProjectedLight::GetReflectiveMap()
{
	return m_pReflectiveMap;
}
const matTexture* g3dProjectedLight::GetReflectiveMap() const
{
	return m_pReflectiveMap;
}


//--------------------------------------------------------------------
// This is the texture that is used to determine the Opacity shadows
//	for this light. This is used for strand hair objects. This may need
//  to be updated each frame. The texture is not owned by the light and
//  it is not automatically updated by the light.
//--------------------------------------------------------------------
void g3dProjectedLight::SetOpacityShadowMap(matTexture* i_pShadowMap, int i_Index /* = 0 */ )
{
	DBG_ASSERT( i_Index >= 0 && i_Index < MAX_OPACITY_MAPS, "Index out of range." );
	m_pOpacityShadowMaps[ i_Index ] = i_pShadowMap;
#ifdef SHADOW_ALIGNMENT
	m_bShiftDirty = true;			//the dimensions might have changed so update the texture alignment
#endif
}

matTexture* g3dProjectedLight::GetOpacityShadowMap( int i_Index /* = 0 */ )
{
	DBG_ASSERT( i_Index >= 0 && i_Index < MAX_OPACITY_MAPS, "Index out of range." );
	return m_pOpacityShadowMaps[ i_Index ];
}

const matTexture* g3dProjectedLight::GetOpacityShadowMap( int i_Index /* = 0 */ ) const
{
	DBG_ASSERT( i_Index >= 0 && i_Index < MAX_OPACITY_MAPS, "Index out of range." );
	return m_pOpacityShadowMaps[ i_Index ];
}

void g3dProjectedLight::SetOpacityVolume( matTexture* i_pOpacityVolume )
{
	m_pOpacityVolume = i_pOpacityVolume;

#ifdef SHADOW_ALIGNMENT
	m_bShiftDirty = true;			//the dimensions might have changed so update the texture alignment
#endif
}

matTexture* g3dProjectedLight::GetOpacityVolume()
{
	return m_pOpacityVolume;
}

const matTexture* g3dProjectedLight::GetOpacityVolume() const
{
	return m_pOpacityVolume;
}


//--------------------------------------------------------------------
//	Orient the given camera to the direction of this light,
//	used for rendering depth maps. This sets the view and 
//	projection matrices of the camera.
//--------------------------------------------------------------------
void g3dProjectedLight::OrientCamera(camCamera& o_Camera) const
{
	// Projection:
	o_Camera.SetOrthographic(m_bDirectional);
	o_Camera.SetAspect(m_Aspect);
	o_Camera.SetClip(m_Scale, m_Scale+m_Range);
	o_Camera.SetFOV(m_Angle);
	o_Camera.SetOrthoWidth(m_Scale);

	// Look At:
	maPoint3d cam_pos = m_Position - m_Direction * m_Scale;
	maVector3d up_vec = compute_up_vector();
	o_Camera.LookAt(cam_pos, m_Position, up_vec);

	//if (m_bCamDirty) update_cammatx();
	//o_Camera.SetCameraMatrix(m_CamMatx);
}

//--------------------------------------------------------------------
// Return World to Texture coordinate matrix for this light, used
//	in the vertex shader.
//--------------------------------------------------------------------
const maMatrix4x4& g3dProjectedLight::GetTotalMatrix() const
{
	bool bDirty = (m_bProjDirty || m_bCamDirty || m_bShiftDirty);

	if (bDirty)
	{
		if (m_bProjDirty) update_projmatx();
		if (m_bCamDirty) update_cammatx();
		if (m_bShiftDirty) update_shiftmatx();

		m_TotalMatx = m_CamMatx * m_ProjMatx * m_ShiftMatx; 
	}

	return m_TotalMatx;
}

//--------------------------------------------------------------------
// update matrices when dirty
//--------------------------------------------------------------------
void g3dProjectedLight::update_projmatx() const
{	
	m_ProjMatx.Identity();

	if (this->m_bDirectional)
	{
		// Directional light uses an orthographic frustrum
		float w = (m_Scale > 0) ? (m_Scale / 2.0f) : 1.0f;
		float h = w / m_Aspect;

		//float Q = 1.0f / (m_Far - m_Near);
		float Q = (m_Range > 0) ? (1.0f / m_Range) : 1.0f;

		m_ProjMatx(0, 0) = 1.0f / w;
		m_ProjMatx(1, 1) = 1.0f / h;
		m_ProjMatx(2, 2) = Q;
		m_ProjMatx(3, 0) = 0;
		m_ProjMatx(3, 1) = 0;
		m_ProjMatx(3, 2) = -Q * m_Scale; // ? Q * -m_Near;
	}
	else
	{
		// Spot light frustrum uses angle as field of view
		float w = float(1.0 / tan(m_Angle * maConstants::c_fAngleToRad / 2.0f));
		float h = w * m_Aspect;

		float Q = (m_Range + m_Scale) / m_Range;

		m_ProjMatx(0, 0) = w;
		m_ProjMatx(1, 1) = h;
		m_ProjMatx(2, 2) = Q;
		m_ProjMatx(3, 2) = -Q * m_Scale;
		m_ProjMatx(2, 3) = 1;
		m_ProjMatx(3, 3) = 0;
	}

	m_bProjDirty = false;

}
void g3dProjectedLight::update_cammatx() const
{
	maPoint3d cam_pos = m_Position - m_Direction * m_Scale;
	maVector3d up_vec = compute_up_vector();
//	m_CamMatx.LookAt(cam_pos, m_Position, c_Up);

	maVector3d dir = m_Direction;
	dir.Normalize(); // not really needed
	maVector3d left = up_vec.Cross(dir);

	if( left.LengthSqr() == 0.0f )
		left.Set(1, 0, 0);
	else
		left.Normalize();

	maVector3d camera_up = dir.Cross(left);
	camera_up.Normalize();

	m_CamMatx.Identity();

	m_CamMatx(0, 0) = left.m_X;
	m_CamMatx(1, 0) = left.m_Y;
	m_CamMatx(2, 0) = left.m_Z;

	m_CamMatx(0, 1) = camera_up.m_X;
	m_CamMatx(1, 1) = camera_up.m_Y;
	m_CamMatx(2, 1) = camera_up.m_Z;

	m_CamMatx(0, 2) = dir.m_X;
	m_CamMatx(1, 2) = dir.m_Y;
	m_CamMatx(2, 2) = dir.m_Z;

	maMatrix4x4 translate;
	translate.MakeTranslate(-cam_pos.m_X, -cam_pos.m_Y, -cam_pos.m_Z);
	m_CamMatx = translate * m_CamMatx;

	m_bCamDirty = false;
}
void g3dProjectedLight::update_shiftmatx() const
{
	// takes a [-1..1] post projection space and
	// map it to [0..1] for projective texture lookup.

	m_ShiftMatx.MakeScale(-0.5f, -0.5f, 1.0f);

#ifdef SHADOW_ALIGNMENT
	//adjusted offset (compensate for half texel offset)
	float xo = 0;
	float yo = 0;
	if( m_pShadowMap )	//if there is a shadow map then use it's dimensions
	{
		xo = 0.5f / m_pShadowMap->GetWidth();
		yo = 0.5f / m_pShadowMap->GetHeight();
	}
	m_ShiftMatx.TranslateBy(0.5f+xo, 0.5f+yo, -m_DepthBias);
#else
	// original shadow offset
	m_ShiftMatx.TranslateBy(0.5f, 0.5f, -m_DepthBias);
#endif

	// no shadow offset (z-tearing on surface of models)
	//m_ShiftMatx.TranslateBy(0.5f, 0.5f, 0.0f);
	// smaller shadow offset
	//m_ShiftMatx.TranslateBy(0.5f, 0.5f, -0.0005f);
	m_bShiftDirty = false;
}

//--------------------------------------------------------------------
// internal set of direction, normalizes vector
//--------------------------------------------------------------------
void g3dProjectedLight::set_direction(const maVector3d& i_Direction)
{
	m_Direction = i_Direction;

	bool valid_len = m_Direction.Normalize();
	//DBG_ASSERT(valid_len, "The direction must be non-zero.");
	if (!valid_len)
		m_Direction.Set(0,0,1);

	m_bCamDirty = true;
}

//--------------------------------------------------------------------
// compute up vector based on direction and tilt values
//--------------------------------------------------------------------
maVector3d g3dProjectedLight::compute_up_vector() const
{
	if (m_Tilt == 0)
	{
		return maVector3d(0,1,0);
	}
	else
	{
		// rotate up vector through tilt angle
		maRotation rot(m_Direction, maConstants::c_fAngleToRad * m_Tilt);
		maVector3d up(0,1,0);
		rot.RotateVector(up);
		return up;
	}
}

const maMatrix4x4& g3dProjectedLight::GetCameraMatrix() const
{
	return m_CamMatx;
}
