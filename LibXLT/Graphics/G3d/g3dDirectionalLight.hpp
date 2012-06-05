/****************************************************************************\
**	g3dDirectionalLight.hpp
**
**		A g3dDirectionalLight represents a light which radiates in one direction
**	and does not attenuate.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_DIRECTIONALLIGHT_HPP
#error g3dDirectionalLight.hpp already included
#endif
#define G3D_DIRECTIONALLIGHT_HPP

#ifndef G3D_LIGHT_HPP
#include "Graphics/g3d/g3dLight.hpp"
#endif
#ifndef MA_VECTOR3D_HPP
#include "Core/ma/maVector3d.hpp"
#endif


//============================================================================
//============================================================================
class g3dDirectionalLight : public g3dLight
{
	public:
		//--------------------------------------------------------------------
		//	The default constructor faces the light towards -y (straight
		//	down).
		//--------------------------------------------------------------------
		g3dDirectionalLight();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~g3dDirectionalLight();

		//--------------------------------------------------------------------
		//	GetDirection returns the direction of the light.
		//--------------------------------------------------------------------
		const maVector3d& GetDirection() const;

		//--------------------------------------------------------------------
		//	GetJitteredDirection returns the direction of the
		//	light after jittering
		//--------------------------------------------------------------------
		maVector3d GetJitteredDirection(int i_Pass) const;

		//--------------------------------------------------------------------
		//	SetDirection sets the direction of the light.  i_Direction does
		//	not need to be normalized.
		//--------------------------------------------------------------------
		void SetDirection(const maVector3d& i_Direction);

	private:
		maVector3d m_Direction;
};

