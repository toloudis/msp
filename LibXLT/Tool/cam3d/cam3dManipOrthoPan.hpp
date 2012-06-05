/*****************************************************************************
**	cam3dManipOrthoPan.hpp
**
**		cam3dManipOrthoPan allows user to translate camera without changing 
**	the view direction
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef CAM3D_CAMERAMANIPORTHOPAN_HPP
#error cam3dManipOrthoPan.hpp multiply included
#endif
#define CAM3D_CAMERAMANIPORTHOPAN_HPP

#ifndef CAM_CAMERAMANIP_HPP
#include "Graphics/cam/camCameraManip.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef CAM3D_MANIP_UTIL_HPP
#include "Tool/cam3d/cam3dManipUtil.hpp"
#endif


//============================================================================
//============================================================================
class cam3dManipOrthoPan : public camCameraManip
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cam3dManipOrthoPan();

		//--------------------------------------------------------------------
		//	Think should be called to update camera and push
		//	changes into g3d system.
		//--------------------------------------------------------------------
		virtual void	Think();

		//--------------------------------------------------------------------
		// Focus_Camera centers camera with respect to the point
		//--------------------------------------------------------------------
		virtual void FocusCamera(const maPoint3d& i_Focus, float i_Radius, maAxisBox i_Box);

		//--------------------------------------------------------------------
		// SetPanRate sets the speed of camera panning (tracking)
		//--------------------------------------------------------------------
		static void SetPanRate( float i_Rate );

	private:
		//--------------------------------------------------------------------
		// Shift position and target by given amounts along left and
		//	up direction of camera.
		//--------------------------------------------------------------------
		void IncrementPan(float i_LeftDelta, float i_UpDelta);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void IncrementWidth(float i_ZoomDiff, float i_RadModifier);

		//--------------------------------------------------------------------
		// Move camera forward/back on view direction
		//--------------------------------------------------------------------
		void IncrementDolly(float i_Motion);

	private:
		maVector3d m_MouseDownPosition;
		cam3dManipUtil::CameraControl m_ManipControls;
};

