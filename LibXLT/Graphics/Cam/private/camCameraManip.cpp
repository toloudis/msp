/*****************************************************************************
**  camCameraManip.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Graphics/cam/camCameraManip.hpp"
#include "Graphics/cam/camCameraManipTarget.hpp"

#include "Core/ma/maConstants.hpp"

//--------------------------------------------------------------------
//	The constructor makes a camera facing towards positive z with
//	a 90 degree FOV and 4/3 aspect ratio.
//--------------------------------------------------------------------
camCameraManip::camCameraManip()
:	m_pCamera(NULL), 
	m_bManipFastRender(false)
{
}

//--------------------------------------------------------------------
// destructor for class derivation
//--------------------------------------------------------------------
// virtual
camCameraManip::~camCameraManip()
{
}

//--------------------------------------------------------------------
//	Attach is called to connect this manipulator to control
//  the given camera. If i_bPreserveCamera is true, the manipulator
//  should set its data to try to preserve the camera's position
//  and orientation.
//--------------------------------------------------------------------
void
camCameraManip::Attach(camCameraManipTarget* i_pCamera, bool i_bPreserveCamera)
{
	DBG_ASSERT(i_pCamera, "Null camera passed to camCameraManip::Attach");
	if (!i_pCamera)
		return;
	m_pCamera = i_pCamera;
}

//--------------------------------------------------------------------
//	Detach is called to disconnect this manipulator from
//	its camera.
//--------------------------------------------------------------------
void
camCameraManip::Detach()
{
	m_pCamera = NULL;
}

//--------------------------------------------------------------------
//	GetCamera returns attached camera
//--------------------------------------------------------------------
camCameraManipTarget*
camCameraManip::GetCamera() const
{
	return m_pCamera;
}

//--------------------------------------------------------------------
//	Think should be called to update camera and push
//	changes into g3d system.
//--------------------------------------------------------------------
void
camCameraManip::Think()
{
	if (m_pCamera)
	{
		m_pCamera->Set();
	}
}

//--------------------------------------------------------------------
// SetManipFastRender makes things render faster when manipulating
//	the camera.
//--------------------------------------------------------------------
void camCameraManip::SetManipFastRender(bool i_bFastRender)
{
	m_bManipFastRender = i_bFastRender;
}

//--------------------------------------------------------------------
// SetManipDepth tells the camera manipulator at what depth the latest 
//	mouse click happened.
//--------------------------------------------------------------------
void camCameraManip::SetManipDepth( float i_Depth )
{
	m_ClickDepth = i_Depth;
}

//--------------------------------------------------------------------
// SetManipDepth tells the camera manipulator at what depth the latest 
//	mouse click happened.
//--------------------------------------------------------------------
void camCameraManip::SetManipBox( maAxisBox i_Box )
{
	m_CurrBox = i_Box;
}
