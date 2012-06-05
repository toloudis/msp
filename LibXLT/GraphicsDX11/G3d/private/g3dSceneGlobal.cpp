/*****************************************************************************
**  g3dSceneGlobal.hpp
**
**
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"

#include "Graphics/g3d/g3dType.hpp"

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------

namespace g3dSceneGlobal
{
	// State
	bool g_ViewIsCurrent = false;
	bool g_ViewIsIdentity = false;
	bool g_ProjectionIsCurrent = false;
	bool g_ProjectionIsIdentity = false;
	bool g_WorldIsIdentity = false;
	bool g_AdditiveMode = false;
	bool g_CubeMode = false;
	int g_CubeStage = 0;

	maMatrix4x4 g_Camera;
	maMatrix4x4 g_CameraInverse;
	maMatrix4x4 g_CameraIT;
	maMatrix4x4 g_Projection;
	maMatrix4x4 g_CameraProjection;
	maPoint3d g_CameraPos;
	// xres, yres, 1/xres, 1/yres
	maVector4d g_TargetRes;
	
	maMatrix4x4 g_Identity;
	float g_FrameTime = 0.0f;
	int g_QuadrantDivision = 1;

	bool g_ScreenSpaceViewport = false;

	bool g_TextureTransformIsDisabled[8];

	dofParams g_DOFParams;

	float g_AlphaTestRef = 0.0f;

	maVector4d g_ClipPlane = maVector4d(0,0,0,1);

	void SetTransforms(const maPoint3d& i_CameraPos,
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
	const maPoint3d& GetCameraPos() {return g_CameraPos;}
	const maMatrix4x4& GetCameraTransform() {return g_Camera;}
	const maMatrix4x4& GetCameraInverseTransform() {return g_CameraInverse;}
	const maMatrix4x4& GetCameraITTransform() {return g_CameraIT;}
	const maMatrix4x4& GetProjectionTransform() {return g_Projection;}
	const maMatrix4x4& GetCameraProjectionTransform() {return g_CameraProjection;}
}
