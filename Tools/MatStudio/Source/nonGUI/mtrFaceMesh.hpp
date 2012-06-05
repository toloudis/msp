/****************************************************************************\
**	mtrFaceMesh.hpp
**
**	A mtrFaceMesh keeps track of faces in mesh for picking
**	materials
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MTR_FACEMESH_HPP
#error mtrFaceMesh.hpp multiply included
#endif
#define MTR_FACEMESH_HPP

#ifndef MA_POINT3D_HPP
#include "maPoint3d.hpp"
#endif

#ifndef ENV_TYPE_HPP
#include "envType.hpp"
#endif

class maMatrix4x4;

#include <vector>

class mtrFaceMesh
{
	public:

		//====================================================================
		//====================================================================
		mtrFaceMesh();

		//====================================================================
		//====================================================================
		mtrFaceMesh(const maPoint3d* i_Points, 
				 int i_NumPoints,
				 const envType::UInt32* i_Indices, 
				 int i_NumIndices);

		//============================================================================
		//============================================================================
		int PickFace(  const maPoint3d &i_RayStart, 
					   const maVector3d &i_RayDir,
					   float &o_Tval);


	private:

		//====================================================================
		//====================================================================
		struct FaceRef
		{
			FaceRef() {};
			FaceRef(envType::UInt32 i0, envType::UInt32 i1, envType::UInt32 i2);
			envType::UInt32 index[3];
			//maPoint3d face_normal;
		};

		std::vector<maPoint3d>	m_Points;
		std::vector<FaceRef>	m_Faces;

};
