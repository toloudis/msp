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
		//	These functions control if the light casts a shadow.  To see a shadow
		//	you must also set the shadow layer range in the g3dPackage.
		//----------------------------------------------------------------------------
		inline bool GetCastsShadow() const;
		inline void SetCastsShadow(bool i_bShadow);

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
		bool m_bShadow;
		maFloatRGBA m_Color;
		maFloatRGBA m_ScaledIntensity;
		float m_FadeFactor;
		float m_Intensity;
		bool m_DiffuseEnabled;
		bool m_SpecularEnabled;
		bool m_bAffectsGlow;
};


//----------------------------------------------------------------------------
//	These functions control if the light casts a shadow.  To see a shadow
//	you must also set the shadow layer range in the g3dPackage.
//----------------------------------------------------------------------------
inline bool g3dLight::GetCastsShadow() const
{
	return m_bShadow;
}

inline void g3dLight::SetCastsShadow(bool i_bShadow)
{
	m_bShadow = i_bShadow;
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
