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
	g_AdditiveMode = false;
	g_CubeMode = false;
	g_CubeStage = 0;

	// xres, yres, 1/xres, 1/yres
	//maVector4d g_TargetRes;
	
	//maMatrix4x4 g_Identity;
	g_FrameTime = 0.0f;
	g_QuadrantDivision = 1;

	g_ScreenSpaceViewport = false;

	//bool g_TextureTransformIsDisabled[8];

	//dofParams g_DOFParams;

	g_AlphaTestRef = 0.0f;

	g_ClipPlane = maVector4d(0,0,0,1);
}



void g3dSceneGlobal::SetTransforms(const maPoint3d& i_CameraPos,
	const maMatrix4x4& i_Camera,
	const maMatrix4x4& i_Projection)
{
	g_CameraPos = i_CameraPos;
	g_Camera = i_Camera;
	g_Projection = i_Projection;
	
	g_CameraProjection = g_Camera * g_Projection;

	g_CameraIT = g_Camera;
	g_CameraIT.Invert();
	g_CameraInverse = g_CameraIT;
	g_CameraIT.Transpose();
}
const maPoint3d& g3dSceneGlobal::GetCameraPos() const {return g_CameraPos;}
const maMatrix4x4& g3dSceneGlobal::GetCameraTransform() const {return g_Camera;}
const maMatrix4x4& g3dSceneGlobal::GetCameraInverseTransform() const {return g_CameraInverse;}
const maMatrix4x4& g3dSceneGlobal::GetCameraITTransform() const {return g_CameraIT;}
const maMatrix4x4& g3dSceneGlobal::GetProjectionTransform() const {return g_Projection;}
const maMatrix4x4& g3dSceneGlobal::GetCameraProjectionTransform() const {return g_CameraProjection;}
