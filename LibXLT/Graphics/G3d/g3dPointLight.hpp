/****************************************************************************\
**	g3dPointLight.hpp
**
**		A g3dPointLight represents a light which radiates equal energy in all
**	directions.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_POINTLIGHT_HPP
#error g3dPointLight.hpp already included
#endif
#define G3D_POINTLIGHT_HPP

#ifndef G3D_LIGHT_HPP
#include "Graphics/g3d/g3dLight.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


//============================================================================
//============================================================================
class g3dPointLight : public g3dLight
{
	public:
		//--------------------------------------------------------------------
		//	The default constructor places the light at the origin.
		//--------------------------------------------------------------------
		g3dPointLight();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~g3dPointLight();

		//--------------------------------------------------------------------
		//	GetPosition returns the position of the light.
		//--------------------------------------------------------------------
		const maPoint3d& GetPosition() const;

		//--------------------------------------------------------------------
		//	GetPosition returns the position of the light after jittering
		//--------------------------------------------------------------------
		maPoint3d GetJitteredPosition(int i_Pass) const;

		//--------------------------------------------------------------------
		//	SetPosition sets the position of the light.
		//--------------------------------------------------------------------
		void SetPosition(const maPoint3d& i_Position);

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
		//	GetRange returns the range of the light.
		//--------------------------------------------------------------------
		float GetRange() const;

		//--------------------------------------------------------------------
		//	SetRange sets the the range of the light.  If the range is set to
		//	a value less than zero, a range will be computed for the light
		//	based on its attenuation values.
		//--------------------------------------------------------------------
		void SetRange(float i_Range);

	private:
		maPoint3d m_Position;
		float m_f0, m_f1, m_f2, m_f3;
		float m_fStart;
		float m_fRange;
};

