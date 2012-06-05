/*****************************************************************************
**	cam3dManipMaya.hpp
**
**		cam3dManipMaya allows user to freely move camera
**	by orbiting around target point and translating target.
**	Uses a maya style config
**
**	StudioGPU
**	Copyright(C) 2000-2002 - All Rights Reserved
\****************************************************************************/
#ifdef CAM3D_CAMERAMANIP_MAYA_HPP
#error cam3dManipMaya.hpp multiply included
#endif
#define CAM3D_CAMERAMANIP_MAYA_HPP

#ifndef CAM_CAMERAMANIP_ORBIT_HPP
#include "Graphics/cam/camCameraManipOrbit.hpp"
#endif

#ifndef CAM3D_MANIP_UTIL_HPP
#include "Tool/cam3d/cam3dManipUtil.hpp"
#endif


//============================================================================
//============================================================================
namespace g3dPrefs
{
	struct g3dRenderPrefs;
};


//============================================================================
//============================================================================
class cam3dManipMaya : public camCameraManipOrbit
{
	public:

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cam3dManipMaya();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~cam3dManipMaya();

		//--------------------------------------------------------------------
		//	Think should be called to update camera and push
		//	changes into g3d system.
		//--------------------------------------------------------------------
		virtual void	Think();

		//--------------------------------------------------------------------
		// SetPanRate sets the speed of camera panning (tracking)
		//--------------------------------------------------------------------
		static void SetPanRate( float i_Rate );

	private:
		//--------------------------------------------------------------------
		//	SafeSetTarget
		//--------------------------------------------------------------------
		void	SafeSetTarget(const maPoint3d &i_Target);

		//--------------------------------------------------------------------
		//	IncrementPitch
		//--------------------------------------------------------------------
		void	IncrementPitch(float i_Radians);

		//--------------------------------------------------------------------
		//	IncrementYaw
		//--------------------------------------------------------------------
		void	IncrementYaw(float i_Radians);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void IncrementRadius(float i_Radius);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void zoom(float i_ZoomDiff, float i_RadModifier, float i_TargetModifier);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void GetRadiusMod(float* o_PositionMod, float* o_TargetMod);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		maVector3d get_shift_from_cursor_drag(const camCameraManipTarget &i_Camera,
											  const maPoint3d& i_Target,
											  int i_DeltaX,
											  int i_DeltaY,
											  float i_Depth);

private:	
	g3dPrefs::g3dRenderPrefs* m_ManipPrefs;
	g3dPrefs::g3dRenderPrefs* m_RestorePrefs;
	cam3dManipUtil::CameraControl m_ManipControls;
	void SetManipPrefs();
};

