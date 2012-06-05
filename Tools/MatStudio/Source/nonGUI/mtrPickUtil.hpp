/*****************************************************************************
**  mtrPickUtil.hpp
**
**      mtrPickUtil 
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MTR_PICKUTIL_HPP
#error mtrPickUtil.hpp multiply included
#endif
#define MTR_PICKUTIL_HPP

#ifndef MTR_FACEMESH_HPP
#include "mtrFaceMesh.hpp"
#endif

class g3dFragment;
class matMaterial;
class scObject;

#include <map>

namespace mtrPickUtil 
{
	struct FragmentMaterial
	{
		FragmentMaterial(g3dFragment *i_Frag, matMaterial *i_Mat)
			: m_Fragment(i_Frag), m_Material(i_Mat) {}

		bool operator == (const FragmentMaterial& i_Info) const
		{
			return (this->m_Material == i_Info.m_Material &&
					this->m_Fragment == i_Info.m_Fragment);
		}

		bool operator < (const FragmentMaterial& i_Info) const
		{
			if (i_Info.m_Fragment == this->m_Fragment)
				return (this->m_Material < i_Info.m_Material);
			else
				return (this->m_Fragment < i_Info.m_Fragment);
		}

		g3dFragment *m_Fragment;
		matMaterial *m_Material;
	};

	typedef std::map<FragmentMaterial, mtrFaceMesh> FaceMap;

	//========================================================================
	// PickMaterial()
	//========================================================================
	matMaterial* PickMaterial( scObject* i_Object,
							   FaceMap& i_FaceMap,
							   const maPoint3d &i_RayStart, 
							   const maVector3d &i_RayDir );

};

