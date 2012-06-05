/****************************************************************************\
**	g3dSpotLight.hpp
**
**		A g3dSpotLight represents a light which radiates equal energy in all
**	directions.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_SPOTLIGHT_HPP
#error g3dSpotLight.hpp already included
#endif
#define G3D_SPOTLIGHT_HPP

#ifndef G3D_POINTLIGHT_HPP
#include "Graphics/g3d/g3dPointLight.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maVector3d.hpp"
#endif


//============================================================================
//============================================================================
class g3dSpotLight : public g3dPointLight
{
	public:
		//--------------------------------------------------------------------
		//	The default constructor places the light at the origin.
		//--------------------------------------------------------------------
		g3dSpotLight();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~g3dSpotLight();

		//--------------------------------------------------------------------
		//	GetDirection returns the position of the light.
		//--------------------------------------------------------------------
		const maVector3d& GetDirection() const;

		//--------------------------------------------------------------------
		//	SetDirection sets the position of the light.
		//--------------------------------------------------------------------
		void SetDirection(const maVector3d& i_Direction);

		//--------------------------------------------------------------------
		//	GetInnerAngle returns the inner angle of the light.
		//--------------------------------------------------------------------
		float GetInnerAngle() const;

		//--------------------------------------------------------------------
		//	SetInnerAngle sets the the inner angle of the light.  Must be
		//	between 0 and the outer angle.
		//--------------------------------------------------------------------
		void SetInnerAngle(float i_InnerAngle);

		//--------------------------------------------------------------------
		//	GetOuterAngle returns the outer angle of the light.
		//--------------------------------------------------------------------
		float GetOuterAngle() const;

		//--------------------------------------------------------------------
		//	SetOuterAngle sets the the outer angle of the light.  Must be
		//  greater than the inner angle
		//--------------------------------------------------------------------
		void SetOuterAngle(float i_OuterAngle);

	private:
		maVector3d m_Direction;
		float m_fInnerAngle;
		float m_fOuterAngle;
};
