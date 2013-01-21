/*****************************************************************************
**  g3dSceneGlobal.hpp
**
**
**
** Area17
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "Area18/g3d/g3dSceneGlobal.hpp"

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------

g3dSceneGlobal::g3dSceneGlobal()
{
	m_FrameTime = 0.0f;
	m_ClipPlane = maVector4d(0,0,0,1);
}

void g3dSceneGlobal::SetTransforms(const maPoint3d& i_CameraPos,
	const maMatrix4x4& i_Camera,
	const maMatrix4x4& i_Projection)
{
	m_CameraPos = i_CameraPos;
	m_Camera = i_Camera;
	m_Projection = i_Projection;
	
	m_CameraProjection = m_Camera * m_Projection;

	m_CameraIT = m_Camera;
	m_CameraIT.Invert();
	m_CameraInverse = m_CameraIT;
	m_CameraIT.Transpose();
}
const maPoint3d& g3dSceneGlobal::GetCameraPos() const {return m_CameraPos;}
const maMatrix4x4& g3dSceneGlobal::GetCameraTransform() const {return m_Camera;}
const maMatrix4x4& g3dSceneGlobal::GetCameraInverseTransform() const {return m_CameraInverse;}
const maMatrix4x4& g3dSceneGlobal::GetCameraITTransform() const {return m_CameraIT;}
const maMatrix4x4& g3dSceneGlobal::GetProjectionTransform() const {return m_Projection;}
const maMatrix4x4& g3dSceneGlobal::GetCameraProjectionTransform() const {return m_CameraProjection;}
