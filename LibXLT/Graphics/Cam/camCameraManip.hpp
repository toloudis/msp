/*****************************************************************************
**  camCameraManip.hpp
**
**      camCameraManip is base class for manipulators which control
**	camera orientation.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef CAM_CAMERAMANIP_HPP
#error camCameraManip.hpp multiply included
#endif
#define CAM_CAMERAMANIP_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif

//============================================================================
//============================================================================
class camCameraManipTarget;

//============================================================================
//============================================================================
class camCameraManip
{
	protected:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		camCameraManip();

	public:

		//--------------------------------------------------------------------
		// destructor for class derivation
		//--------------------------------------------------------------------
		virtual ~camCameraManip();

		//--------------------------------------------------------------------
		//	Attach is called to connect this manipulator to control
		//  the given camera. If i_bPreserveCamera is true, the manipulator
		//  should set its data to try to preserve the camera's position
		//  and orientation.
		//--------------------------------------------------------------------
		virtual void Attach(camCameraManipTarget* i_pCamera, bool i_bPreserveCamera = false);

		//--------------------------------------------------------------------
		//	Detach is called to disconnect this manipulator from
		//	its camera.
		//--------------------------------------------------------------------
		virtual void Detach();

		//--------------------------------------------------------------------
		//	GetCamera returns attached camera
		//--------------------------------------------------------------------
		camCameraManipTarget* GetCamera() const;

		//--------------------------------------------------------------------
		//	Think should be called to update camera and push
		//	changes into g3d system.
		//--------------------------------------------------------------------
		virtual void Think();

		//--------------------------------------------------------------------
		// Focus_Camera centers camera with respect to the point
		//--------------------------------------------------------------------
		virtual void FocusCamera(const maPoint3d& i_Focus, float i_Radius, maAxisBox i_Box) = 0;

		//--------------------------------------------------------------------
		// SetManipFastRender makes things render faster when manipulating
		//	the camera.
		//--------------------------------------------------------------------
		virtual void SetManipFastRender(bool i_bFastRender);

		//--------------------------------------------------------------------
		// SetManipDepth tells the camera manipulator at what depth the latest 
		//	mouse click happened.
		//--------------------------------------------------------------------
		virtual void SetManipDepth( float i_Depth );

		//--------------------------------------------------------------------
		// SetBBoxDiagonalLength sets the length of the current bounding box
		// diagonal
		//--------------------------------------------------------------------
		virtual void SetManipBox( maAxisBox i_Box );

protected:
		// should we speed up the render when camera is being manipulated?
		bool m_bManipFastRender;

		float m_ClickDepth;
		maAxisBox m_CurrBox;

private:
		camCameraManipTarget* m_pCamera;

};

