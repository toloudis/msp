/****************************************************************************\
**	g3dProjectedLight.hpp
**
**		A g3dProjectedLight represents a light which projects a texture.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_PROJECTEDLIGHT_HPP
#error g3dProjectedLight.hpp already included
#endif
#define G3D_PROJECTEDLIGHT_HPP

#ifndef G3D_LIGHT_HPP
#include "Graphics/g3d/g3dLight.hpp"
#endif
#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

enum HAIR_SHADOW_TYPE
{
	HAIR_SHADOW_OSM4,
	HAIR_SHADOW_OSM16,
	HAIR_SHADOW_OSM32,
	HAIR_SHADOW_DOSM4,
	HAIR_SHADOW_DOSM16,
	HAIR_SHADOW_DOSM32,
	HAIR_SHADOW_VOLUME4,
	HAIR_SHADOW_MAXINT = 0xFFFFFFFF,
};

#define MAX_OPACITY_MAPS 8

//============================================================================
//============================================================================
class camCamera;
class matTexture;


//============================================================================
//============================================================================
class g3dProjectedLight : public g3dLight
{
	public:
		//--------------------------------------------------------------------
		//	The default constructor places the light at the origin.
		//--------------------------------------------------------------------
		g3dProjectedLight();

		//--------------------------------------------------------------------
		// same texture pointers - be careful with managing.
		//--------------------------------------------------------------------
		g3dProjectedLight(const g3dProjectedLight& i_Copy);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~g3dProjectedLight();

	//--------------------------------------------------------------------
	// Attributes
	//--------------------------------------------------------------------
		enum ShadowQuality
		{
			SQ_LOW, SQ_MED, SQ_HIGH
		};

		//--------------------------------------------------------------------
		//	Position of the light. 
		//	As opposed to cameras, this position is on the near plane 
		//	of the projection.
		//--------------------------------------------------------------------
		const maVector3d& GetPosition() const;
		void SetPosition(const maVector3d& i_Position);

		//--------------------------------------------------------------------
		//	Target of the light. This is the way to set the direction
		//  of the light. In rendering, only the direction will be used.
		//--------------------------------------------------------------------
		const maVector3d& GetTarget() const;
		void SetTarget(const maVector3d& i_Target);

		//--------------------------------------------------------------------
		//	Direction of the projection of the light, defined by
		//  position and target
		//--------------------------------------------------------------------
		const maVector3d& GetDirection() const;

		//--------------------------------------------------------------------
		//	Range of the light, counting from the position of the light
		//		along the direction vector.
		//--------------------------------------------------------------------
		float GetRange() const;
		void SetRange(float i_Range);

		//--------------------------------------------------------------------
		// If the Directional flag is true, then the projected light behaves 
		// like a directional light. It uses an orthographic camera matrix
		// and the lighting algorithm treats the light source as an infinite
		// light soure instead of a point light source.
		// When in directional mode, the angle and inner angle fields 
		// are ignored.
		// Default value is false, which means "spot light mode".
		//--------------------------------------------------------------------
		bool GetIsDirectional() const;
		void SetIsDirectional(bool i_bDirectional);

		//--------------------------------------------------------------------
		//	Angle of the light, in degrees. This represents the angle of 
		//	the "spotlight"	effect. This is the same as "FOV" in a camera.
		//--------------------------------------------------------------------
		float GetAngle() const;
		void SetAngle(float i_Angle);

		//--------------------------------------------------------------------
		//	Inner Angle of the light, in degrees. This represents the angle 
		//	where the spotlight is at full intensity. From this inner angle
		//  out to the "Angle" value, the light's intensity should falloff.
		//  Default is 180 degrees, which results in no falloff.
		//--------------------------------------------------------------------
		float GetInnerAngle() const;
		void SetInnerAngle(float i_Angle);

		//--------------------------------------------------------------------
		//	Scale of the light.  This increases the overall size of the 
		//	light, causing it to affect a larger area.
		//--------------------------------------------------------------------
		float GetScale() const;
		void SetScale(float i_Scale);

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		float GetLightSize() const;
		void SetLightSize(float i_LightSize);

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		float GetPCSSAdjust() const;
		void SetPCSSAdjust(float i_PCSSAdjust);

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		float GetSceneScale() const;
		void SetSceneScale(float i_SceneScale);

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		ShadowQuality GetShadowQuality() const;
		void SetShadowQuality(ShadowQuality i_ShadowQuality);

		//--------------------------------------------------------------------
		//	How dark are shadows? 1.0 is full darkness, 0.0 is not darkened at all.
		//--------------------------------------------------------------------
		float GetShadowIntensity() const;
		void SetShadowIntensity(float i_ShadowIntensity);

		//--------------------------------------------------------------------
		//	What color should the shadows be tinted?
		//--------------------------------------------------------------------
		maFloatRGBA GetShadowColor() const;
		void SetShadowColor(const maFloatRGBA& i_ShadowColor);

		//--------------------------------------------------------------------
		//	Shadow mapping depth bias (applied at render time, not shadow map generation)
		//--------------------------------------------------------------------
		float GetDepthBias() const;
		void SetDepthBias(float i_DepthBias);

		//--------------------------------------------------------------------
		//	Aspect ratio of the light, (width / height)
		//--------------------------------------------------------------------
		float GetAspect() const;
		void SetAspect(float i_Aspect);

		//--------------------------------------------------------------------
		//	Tilt rotation around the view direction. In degrees.
		//--------------------------------------------------------------------
		float GetTilt() const;
		void SetTilt(float i_Tilt);

		//--------------------------------------------------------------------
		//	GetFalloff0, GetFalloff1, and GetFalloff2 all return
		//	coefficients that describe the falloff of the pointlight.
		//	A point light is attenuated according to this formula:
		//
		//		A =				1
		//			-------------------------
		//			a0 + a1 * D + a2 * D^2,
		//
		//	where D is the distance from the light to the surface it is
		//	illuminating and a0, a1, and a2 are the falloff coefficients.
		//	Some (but not all) of the coefficients may be zero.
		//--------------------------------------------------------------------
		float GetFalloff0() const;
		float GetFalloff1() const;
		float GetFalloff2() const;
		float GetFalloff3() const;

		float GetFalloffStart() const;

		//--------------------------------------------------------------------
		//	The SetFalloff0-2 functions allow the user to set the falloff
		//	curve.
		//--------------------------------------------------------------------
		void SetFalloff0(float i_Val);
		void SetFalloff1(float i_Val);
		void SetFalloff2(float i_Val);
		void SetFalloff3(float i_Val);

		void SetFalloffStart(float i_Val);

		//--------------------------------------------------------------------
		//	GI Properties
		//--------------------------------------------------------------------
		bool GetGIEnabled() const;
		void SetGIEnabled( bool i_Val );

		//--------------------------------------------------------------------
		//	Hair Properties
		//--------------------------------------------------------------------
		float GetHairMinBound() const;
		void SetHairMinBound( float i_Val );
		
		float GetHairMaxBound() const;
		void SetHairMaxBound( float i_Val );

		HAIR_SHADOW_TYPE GetHairShadowType() const;
		void SetHairShadowType( HAIR_SHADOW_TYPE i_Val );

	//--------------------------------------------------------------------
	//	Textures
	//--------------------------------------------------------------------

		//--------------------------------------------------------------------
		// This is the texture that is projected as the color of the light.
		//	The texture is not owned by the light.
		//--------------------------------------------------------------------
		void SetTexture(matTexture* i_pTexture);
		matTexture* GetTexture();
		const matTexture* GetTexture() const;

		//--------------------------------------------------------------------
		// This is the texture that is used to determine the shadows
		//	for this light.  This may need to be updated each frame.
		//	The texture is not owned by the light and it is not 
		//	automatically updated by the light.
		//--------------------------------------------------------------------
		void SetShadowMap(matTexture* i_pShadowMap);
		matTexture* GetShadowMap();
		const matTexture* GetShadowMap() const;

		//--------------------------------------------------------------------
		// This is the texture that is used to determine the gi contribution
		//	from this light.  This may need to be updated each frame.
		//	The texture is not owned by the light and it is not 
		//	automatically updated by the light.
		//--------------------------------------------------------------------
		void SetReflectiveMap(matTexture* i_pShadowMap);
		matTexture* GetReflectiveMap();
		const matTexture* GetReflectiveMap() const;

		//--------------------------------------------------------------------
		// This is the texture that is used to determine the Opacity shadows
		//	for this light. This is used for strand hair objects. This may need
		//  to be updated each frame. The texture is not owned by the light and
		//  it is not automatically updated by the light.
		//--------------------------------------------------------------------
		void SetOpacityShadowMap(matTexture* i_pShadowMap, int i_Index = 0 );
		matTexture* GetOpacityShadowMap( int i_Index = 0 );
		const matTexture* GetOpacityShadowMap( int i_Index = 0 ) const;
		void SetOpacityVolume( matTexture* i_pShadowVolume );
		matTexture* GetOpacityVolume();
		const matTexture* GetOpacityVolume() const;

	//--------------------------------------------------------------------
	// Functions
	//--------------------------------------------------------------------

		//--------------------------------------------------------------------
		//	Orient the given camera to the direction of this light,
		//	used for rendering depth maps. This sets the view and 
		//	projection matrices of the camera.
		//--------------------------------------------------------------------
		void OrientCamera(camCamera& o_Camera) const;

		//--------------------------------------------------------------------
		// Return World to Texture coordinate matrix for this light, used
		//	in the vertex shader.
		//--------------------------------------------------------------------
		const maMatrix4x4& GetTotalMatrix() const;

		//--------------------------------------------------------------------
		// Return World to Camera View matrix for this light, used
		//	in the vertex shader.
		//--------------------------------------------------------------------
		const maMatrix4x4& GetCameraMatrix() const;


	private:
		//--------------------------------------------------------------------
		// update matrices when dirty
		//--------------------------------------------------------------------
		void update_projmatx() const;
		void update_cammatx() const;
		void update_shiftmatx() const;

		//--------------------------------------------------------------------
		// internal set of direction, normalizes vector
		//--------------------------------------------------------------------
		void set_direction(const maVector3d& i_Direction);

		//--------------------------------------------------------------------
		// compute up vector based on direction and tilt values
		//--------------------------------------------------------------------
		maVector3d compute_up_vector() const;

	private:
		maPoint3d m_Position;
		maVector3d m_Direction;
		maPoint3d m_Target;
		bool m_bDirectional;
		float m_Range, m_Angle, m_InnerAngle, m_Scale, m_Aspect, m_Tilt;
		float m_LightSize, m_PCSSAdjust, m_SceneScale;
		ShadowQuality m_ShadowQuality;
		float m_shadowIntensity;
		maFloatRGBA m_ShadowColor;
		float m_f0, m_f1, m_f2, m_f3;
		float m_falloffStart;
		float m_DepthBias;
		float m_HairMinBound;
		float m_HairMaxBound;
		HAIR_SHADOW_TYPE m_HairShadowType;
		
		matTexture* m_pTexture;
		matTexture* m_pShadowMap;
		matTexture* m_pReflectiveMap;
		matTexture* m_pOpacityShadowMaps[MAX_OPACITY_MAPS];
		matTexture* m_pOpacityVolume;

		mutable maMatrix4x4 m_TotalMatx;
		// The total matrix for this light projection is the 
		//	combination of these 3 matrices.
		mutable maMatrix4x4 m_ProjMatx, m_CamMatx, m_ShiftMatx; 
		mutable bool m_bProjDirty, m_bCamDirty, m_bShiftDirty;

		bool m_bGIEnabled;
};

