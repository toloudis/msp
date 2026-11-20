/****************************************************************************\
**	g3dLight.hpp
**
**		g3dLight is the base for all lights in the g3d package.  All g3dLights
**	have a color (intensity) property and can be enabled or disabled.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_LIGHT_HPP
#error g3dLight.hpp already included
#endif
#define G3D_LIGHT_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif


//============================================================================
//============================================================================
class g3dLight
{
	public:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		g3dLight();

		//----------------------------------------------------------------------------
		//	pure virtual destructor (this class cannot be instantiated directly)
		//----------------------------------------------------------------------------
		virtual ~g3dLight() = 0;

		//----------------------------------------------------------------------------
		//	IsEnabled returns true if the light should affect the scene.  On
		//	creation, lights default to a disabled state.
		//----------------------------------------------------------------------------
		bool IsEnabled() const;

		//----------------------------------------------------------------------------
		//	same as Enable() + Disable, but allows a single interface for doing
		//	the same thing.
		//----------------------------------------------------------------------------
		void SetEnable( bool i_bEnable );

		//----------------------------------------------------------------------------
		//	Enable will cause the effects of the light to be visible in the scene.
		//----------------------------------------------------------------------------
		void Enable();

		//----------------------------------------------------------------------------
		//	Disable will cause the effects of the light to be invisible in the
		//	scene.
		//----------------------------------------------------------------------------
		void Disable();

		//----------------------------------------------------------------------------
		//	GetIntensity returns the intensity value (color) of the light.  Although
		//	all lights have this property, it may be interpreted differently for
		//	different types of lights; for instance, a spotlight may not make
		//	make a strong contribution to the lighting at a distant point, but a
		//	directional light may.
		//----------------------------------------------------------------------------
		const maFloatRGBA& GetIntensity() const;

		//----------------------------------------------------------------------------
		//	SetIntensity sets the color value of the light.
		//----------------------------------------------------------------------------
		void SetIntensity(const maFloatRGBA& i_Intensity);

		//----------------------------------------------------------------------------
		//	GetIntensityFactor returns the intensity value of the light.
		//  This number is multiplied by the light color to intensify the brightness.
		//----------------------------------------------------------------------------
		float GetIntensityFactor() const;

		//----------------------------------------------------------------------------
		//	SetIntensityFactor sets the intensity value of the light.
		//----------------------------------------------------------------------------
		void SetIntensityFactor(float i_Intensity);

		//----------------------------------------------------------------------------
		//	SetFadeFactor sets a Scalar the is multiplied by the intensity
		//  Useful for animating lights or fading a light in and out.
		//	Must be a number in the range 0 - 1
		//----------------------------------------------------------------------------
		void SetFadeFactor( float i_fScalar );

		//----------------------------------------------------------------------------
		//	GetFadeFactor returns the Scalar of the intensity
		//  Useful for animating lights or fading a light in and out.
		//	Will be in the range of 0 - 1
		//----------------------------------------------------------------------------
		float GetFadeFactor() const;

		//----------------------------------------------------------------------------
		//	GetScaledIntensity returns the color multiplied by the Scalar
		//----------------------------------------------------------------------------
		const maFloatRGBA& GetScaledIntensity() const;

		//----------------------------------------------------------------------------
		//	For the rasterizers only: whether the light is drawn in a pass of its
		//	own (g3dSingleLightRendering: the lit pass, Illumination Only, Shadow
		//	Mask, the per-light hair and transparency loops) rather than with the
		//	ambient pass's forward lighting. It doesn't say whether the light
		//	casts shadows: a rasterized light is shadowed when it has a shadow
		//	map. False by default; MachStudio sets it on every light.
		//----------------------------------------------------------------------------
		inline bool GetIsSinglePassLight() const;
		inline void SetIsSinglePassLight(bool i_bSinglePass);

		//----------------------------------------------------------------------------
		//	Whether the light casts shadows (its owner's "Shadow Source"). A
		//	backend that traces shadows (GraphicsOptiX) traces shadow rays for a
		//	light only when this is true; the rasterizers decide by whether the
		//	light has a shadow map, which its owner makes only when this is
		//	true. True by default.
		//----------------------------------------------------------------------------
		inline bool GetShadowSource() const;
		inline void SetShadowSource(bool i_bShadowSource);

		//----------------------------------------------------------------------------
		// Lights can give just diffuse or specular light contributions
		//  by setting these flags.
		//----------------------------------------------------------------------------
		inline bool IsDiffuseEnabled() const;
		inline bool IsSpecularEnabled() const;
		inline void SetDiffuseEnabled(bool i_bEnabled);
		inline void SetSpecularEnabled(bool i_bEnabled);

		inline void SetAffectsGlow(bool i_bAffectsGlow);
		inline bool GetAffectsGlow() const;
	private:

		bool m_Enabled;
		bool m_bSinglePass;
		bool m_bShadowSource;
		maFloatRGBA m_Color;
		maFloatRGBA m_ScaledIntensity;
		float m_FadeFactor;
		float m_Intensity;
		bool m_DiffuseEnabled;
		bool m_SpecularEnabled;
		bool m_bAffectsGlow;
};


//----------------------------------------------------------------------------
//	Whether the rasterizers draw the light in a pass of its own
//----------------------------------------------------------------------------
inline bool g3dLight::GetIsSinglePassLight() const
{
	return m_bSinglePass;
}

inline void g3dLight::SetIsSinglePassLight(bool i_bSinglePass)
{
	m_bSinglePass = i_bSinglePass;
}

//----------------------------------------------------------------------------
//	Whether the light casts shadows
//----------------------------------------------------------------------------
inline bool g3dLight::GetShadowSource() const
{
	return m_bShadowSource;
}

inline void g3dLight::SetShadowSource(bool i_bShadowSource)
{
	m_bShadowSource = i_bShadowSource;
}

//----------------------------------------------------------------------------
// Lights can give just diffuse or specular light contributions
//  by setting these flags.
//----------------------------------------------------------------------------
inline bool g3dLight::IsDiffuseEnabled() const
{
	return m_DiffuseEnabled;
}
inline void g3dLight::SetDiffuseEnabled(bool i_bEnabled)
{
	m_DiffuseEnabled = i_bEnabled;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inline bool g3dLight::IsSpecularEnabled() const
{
	return m_SpecularEnabled;
}
inline void g3dLight::SetSpecularEnabled(bool i_bEnabled)
{
	m_SpecularEnabled = i_bEnabled;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inline void g3dLight::SetAffectsGlow(bool i_bAffectsGlow)
{
	m_bAffectsGlow = i_bAffectsGlow;
}

inline bool g3dLight::GetAffectsGlow() const
{
	return m_bAffectsGlow;
}
