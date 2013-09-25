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

	bool g_AdditiveMode;
	 bool g_CubeMode;
	 int g_CubeStage;

	 maMatrix4x4 g_Identity;
	// used by animated materials
	 float g_FrameTime;
	 bool g_ScreenSpaceViewport;
	 bool g_TextureTransformIsDisabled[8];
	 int g_QuadrantDivision;

	// xres, yres, 1/xres, 1/yres
	 maVector4d g_TargetRes;

	struct dofParams
	{
		float m_NearBlurDist;
		float m_NearFocalDist;
		float m_FarFocalDist;
		float m_FarBlurDist;
		float m_MaxFarBlur;
		float m_MaxCoC;
	};
	 dofParams g_DOFParams;

	 float g_AlphaTestRef;

	 maVector4d g_ClipPlane;

	void SetTransforms(const maPoint3d& i_CameraPos,
		const maMatrix4x4& i_Camera,
		const maMatrix4x4& i_Projection);
	const maPoint3d& GetCameraPos() const;
	const maMatrix4x4& GetCameraTransform() const;
	const maMatrix4x4& GetCameraInverseTransform() const;
	const maMatrix4x4& GetCameraITTransform() const;
	const maMatrix4x4& GetProjectionTransform() const;
	const maMatrix4x4& GetCameraProjectionTransform() const;

private:
	maMatrix4x4 g_Camera;
	maMatrix4x4 g_CameraInverse;
	maMatrix4x4 g_CameraIT;
	maMatrix4x4 g_Projection;
	maMatrix4x4 g_CameraProjection;
	maPoint3d g_CameraPos;

};

