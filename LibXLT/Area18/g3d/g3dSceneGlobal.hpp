/*****************************************************************************
**  g3dSceneGlobal.hpp
**
**      Contains data common to all renderers.
**
** Area17
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_SCENEGLOBAL_HPP
#error g3dSceneGlobal.hpp multiply included
#endif
#define G3D_SCENEGLOBAL_HPP

#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------

class g3dSceneGlobal
{
public:
	g3dSceneGlobal();

	// used by animated materials
	float m_FrameTime;

	maVector4d m_ClipPlane;

	maMatrix4x4 m_Camera;
	maMatrix4x4 m_CameraInverse;
	maMatrix4x4 m_CameraIT;
	maMatrix4x4 m_Projection;
	maMatrix4x4 m_CameraProjection;
	maPoint3d m_CameraPos;

	void SetTransforms(const maPoint3d& i_CameraPos,
		const maMatrix4x4& i_Camera,
		const maMatrix4x4& i_Projection);
	const maPoint3d& GetCameraPos() const;
	const maMatrix4x4& GetCameraTransform() const;
	const maMatrix4x4& GetCameraInverseTransform() const;
	const maMatrix4x4& GetCameraITTransform() const;
	const maMatrix4x4& GetProjectionTransform() const;
	const maMatrix4x4& GetCameraProjectionTransform() const;
};

