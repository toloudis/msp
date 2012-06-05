/*****************************************************************************
**  g3dSceneGlobal.hpp
**
**      Contains data common to all renderers.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_SCENEGLOBAL_HPP
#error g3dSceneGlobal.hpp multiply included
#endif
#define G3D_SCENEGLOBAL_HPP

#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif

#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------

namespace g3dSceneGlobal
{
	extern bool g_AdditiveMode;
	extern bool g_CubeMode;
	extern int g_CubeStage;

	extern maMatrix4x4 g_Identity;
	// used by animated materials
	extern float g_FrameTime;
	extern bool g_ScreenSpaceViewport;
	extern bool g_TextureTransformIsDisabled[8];
	extern int g_QuadrantDivision;

	// xres, yres, 1/xres, 1/yres
	extern maVector4d g_TargetRes;

	struct dofParams
	{
		float m_NearBlurDist;
		float m_NearFocalDist;
		float m_FarFocalDist;
		float m_FarBlurDist;
		float m_MaxFarBlur;
		float m_MaxCoC;
	};
	extern dofParams g_DOFParams;

	extern float g_AlphaTestRef;

	extern maVector4d g_ClipPlane;

	void SetTransforms(const maPoint3d& i_CameraPos,
		const maMatrix4x4& i_Camera,
		const maMatrix4x4& i_Projection);
	const maPoint3d& GetCameraPos();
	const maMatrix4x4& GetCameraTransform();
	const maMatrix4x4& GetCameraInverseTransform();
	const maMatrix4x4& GetCameraITTransform();
	const maMatrix4x4& GetProjectionTransform();
	const maMatrix4x4& GetCameraProjectionTransform();
}
