/*****************************************************************************
**	mspCamAnim.cpp
**
**	 mspCamAnim represents the camera animation.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "mspCamAnim.hpp"
#include "mspViewSettings.hpp"

#include "Core/App/appSimTime.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/G3d/g3dConstants.hpp"
#include "Tool/cam3d/cam3dAnimKeys.hpp"
#include "Tool/cam3d/cam3dImport.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"

namespace
{
	cam3dAnimKeys*	l_pAnimKeys = NULL;

	
	//--------------------------------------------------------------------
	// Compute frame of animation to use based on frame rate, 
	//	start/end frame, etc.
	//--------------------------------------------------------------------
	//float compute_frame(float i_Time)
	//{
	//	float elapsed = i_Time - this->GetBeginTime();
	//	float start_frame = (m_StartFrame.GetValue() < 0) ? 0.0f : m_StartFrame.GetValue();
	//	float cur_frame = elapsed * m_FrameRate.GetValue() + start_frame;
	//
	//	float end_frame = (m_EndFrame.GetValue() < 0) ? m_NumFrames : m_EndFrame.GetValue();
	//	if (cur_frame > end_frame)
	//	{
	//		if (m_bLooping.GetValue())
	//		{
	//			float anim_len = end_frame - start_frame;
	//			if (anim_len <= 0)
	//				return end_frame;
	//
	//			int num_cycles = int ((cur_frame - start_frame) / anim_len);
	//			float loop_frame = (cur_frame - (num_cycles * anim_len));
	//			return loop_frame;
	//		}
	//		else
	//		{
	//			// stopped at last frame
	//			return end_frame;
	//		}
	//	}
	//
	//	return cur_frame;
	//}

}

//--------------------------------------------------------------------
// Clear
//--------------------------------------------------------------------
void  mspCamAnim::Clear()
{
	mspViewSettings::sm_CameraAnimFilename.SetValue( itString() );
	delete l_pAnimKeys;
	l_pAnimKeys = NULL;
}

//--------------------------------------------------------------------
// LoadAnimation
//--------------------------------------------------------------------
bool  mspCamAnim::LoadAnimation(const fsLocator &i_AnimLocator )
{
	if (l_pAnimKeys)
	{
		delete l_pAnimKeys;
		l_pAnimKeys = NULL;
	}

	float anim_fps = g3dConstants::c_fDefaultFrameRate;
	float begin_frame = 0;
	l_pAnimKeys = cam3dImport::LoadAnimation( i_AnimLocator, anim_fps, begin_frame );

	if (l_pAnimKeys)
	{
		mspViewSettings::sm_CameraAnimFilename.SetValue(i_AnimLocator.GetLastName());
		mspViewSettings::sm_CameraAnimNumFrames.SetValue((float)l_pAnimKeys->GetNumFrames());
	}
	else
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_AnimLocator, filename);
		DBG_WARNING("NULL object returned from LoadAnimation: " << filename );
		std::string msg = "File is not a supported camera animation file format: " + filename;
		guiMessageBox::Show(msg.c_str(), "Error");
	}

	return false;
}

//--------------------------------------------------------------------
// Returns true if animation is loaded
//--------------------------------------------------------------------
bool mspCamAnim::HasAnimation()
{
	return (l_pAnimKeys != NULL);
}


//--------------------------------------------------------------------
// Alters camera based on animation for the given frame
//--------------------------------------------------------------------
void mspCamAnim::Animate(float i_Frame)
{
	// Execute camera script
	if (l_pAnimKeys)
	{
		// calculate the frame
		float frame = i_Frame; //compute_frame(i_Time);

		// Test for camera cut. 
		const float c_LargeMoveThreshold = 1.0f;
		if (l_pAnimKeys->TestForCut(frame, c_LargeMoveThreshold))
		{
			// Clamp down to previous frame, making a "step"
			int stepped_frame = (int) frame;
			frame = (float) stepped_frame;
		}

		maPoint3d pos = l_pAnimKeys->GetPosition(frame);
		maVector3d view = l_pAnimKeys->GetViewVector(frame);
		// Position manipulator and camera
		cam3dMgr::SetManipPositionAndTarget(pos, pos+view);

		//	tilt 
		if (l_pAnimKeys->HasTiltAnimation())
		{
			float tilt = l_pAnimKeys->GetTilt(frame);

			maRotation rot(view, maConstants::c_fAngleToRad * tilt);
			maVector3d up(0,1,0);
			rot.RotateVector(up);

			// set scripted camera directly
			cam3dMgr::GetCamera().LookAt(pos, pos+view, up);
		}
		else
		{
			cam3dMgr::GetCamera().LookAt(pos,pos+view, maVector3d(0,1,0));
		}

		//	field of view
		if (l_pAnimKeys->HasFieldOfViewAnimation())
		{
			float fov = l_pAnimKeys->GetFieldOfView(frame);
			cam3dMgr::GetCamera().SetFOV(fov);
		}
	}
}
