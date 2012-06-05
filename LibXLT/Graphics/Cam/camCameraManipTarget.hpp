/*****************************************************************************
**  camCameraManipTarget.hpp
**
**      camCameraManipTarget is a intermediary class that is controlled 
**	by a camCameraManip. It provides an interface like a camCamera,
**	but it also allows other classes to be controlled by the camera manip.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef CAM_CAMERAMANIPTARGET_HPP
#error camCameraManipTarget.hpp multiply included
#endif
#define CAM_CAMERAMANIPTARGET_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


//============================================================================
//============================================================================
class camCameraManipTarget
{
	public:
		//--------------------------------------------------------------------
		//	Set should be called when you are done setting up the camera and
		//	you are ready for the camCameraManipTarget to set the Terawatt transforms.
		//--------------------------------------------------------------------
		virtual void Set() const = 0;

		//--------------------------------------------------------------------
		//	GetPosition returns the position of the camCameraManipTarget
		//--------------------------------------------------------------------
		virtual const maPoint3d& GetPosition() const = 0;

		//--------------------------------------------------------------------
		//	GetTarget returns the target position of the camCameraManipTarget
		//--------------------------------------------------------------------
		virtual const maPoint3d& GetTarget() const = 0;

		//--------------------------------------------------------------------
		//	GetDirection returns the direction that the camera is facing.
		//--------------------------------------------------------------------
		virtual maVector3d GetDirection() const = 0;

		//--------------------------------------------------------------------
		//	GetLeft returns a normalized vector representing the left of the
		//	camera.
		//--------------------------------------------------------------------
		virtual maVector3d GetLeft() const = 0;

		//--------------------------------------------------------------------
		//	GetUp returns a normalized vector representing the upwards
		//	direction of the camera.
		//--------------------------------------------------------------------
		virtual maVector3d GetUp() const = 0;

		//--------------------------------------------------------------------
		//	GetFarClip gets the distance from the camera position to the far
		//	clip plane.
		//--------------------------------------------------------------------
		virtual float GetFarClip() const = 0;

		//--------------------------------------------------------------------
		//	GetNearClip gets the distance from the camera position to the near
		//	clip plane.
		//--------------------------------------------------------------------
		virtual float GetNearClip() const = 0;

		//--------------------------------------------------------------------
		// Orthographic width controls the size of the orthographic 
		//	view plane
		//--------------------------------------------------------------------
		virtual void SetOrthoWidth(float i_Width) = 0;
		virtual float GetOrthoWidth() const = 0;

		//--------------------------------------------------------------------
		//	LookAt sets a camera matrix from position, target and up vector
		//--------------------------------------------------------------------
		virtual void LookAt(const maPoint3d &i_Pos, const maPoint3d &i_Target,
						 const maVector3d &i_Up) = 0;
};


//============================================================================
// Template implementation to make this target class easier to use.
//============================================================================
template <class TargetType>
class camCameraManipTargetWrapper : public camCameraManipTarget
{
	public:
		//--------------------------------------------------------------------
		// Give initial object to wrap
		//--------------------------------------------------------------------
		camCameraManipTargetWrapper(TargetType* i_pTarget = NULL)
			: m_pTarget(i_pTarget)
		{
		}

		//--------------------------------------------------------------------
		// Redirect this wrapper to a different object
		//--------------------------------------------------------------------
		void ChangeManipTarget(TargetType* i_pTarget)
		{
			m_pTarget = i_pTarget;
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Set() const
		{
			DBG_ASSERT(m_pTarget!=NULL, "Need camera manip target for wrapper class.");
			if (!m_pTarget)
				return;
			m_pTarget->Set();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual const maPoint3d& GetPosition() const
		{
			DBG_ASSERT(m_pTarget!=NULL, "Need camera manip target for wrapper class.");
			return m_pTarget->GetPosition();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual const maPoint3d& GetTarget() const
		{
			DBG_ASSERT(m_pTarget!=NULL, "Need camera manip target for wrapper class.");
			return m_pTarget->GetTarget();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual maVector3d GetDirection() const
		{
			DBG_ASSERT(m_pTarget!=NULL, "Need camera manip target for wrapper class.");
			if (!m_pTarget)
				return maVector3d();
			return m_pTarget->GetDirection();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual maVector3d GetLeft() const
		{
			DBG_ASSERT(m_pTarget!=NULL, "Need camera manip target for wrapper class.");
			if (!m_pTarget)
				return maVector3d();
			return m_pTarget->GetLeft();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual maVector3d GetUp() const
		{
			DBG_ASSERT(m_pTarget!=NULL, "Need camera manip target for wrapper class.");
			if (!m_pTarget)
				return maVector3d();
			return m_pTarget->GetUp();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual float GetFarClip() const
		{
			DBG_ASSERT(m_pTarget!=NULL, "Need camera manip target for wrapper class.");
			if (!m_pTarget)
				return 0.0f;
			return m_pTarget->GetFarClip();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual float GetNearClip() const
		{
			DBG_ASSERT(m_pTarget!=NULL, "Need camera manip target for wrapper class.");
			if (!m_pTarget)
				return 0.0f;
			return m_pTarget->GetNearClip();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void SetOrthoWidth(float i_Width)
		{
			DBG_ASSERT(m_pTarget!=NULL, "Need camera manip target for wrapper class.");
			if (!m_pTarget)
				return;
			m_pTarget->SetOrthoWidth(i_Width);
		}
		virtual float GetOrthoWidth() const
		{
			DBG_ASSERT(m_pTarget!=NULL, "Need camera manip target for wrapper class.");
			if (!m_pTarget)
				return 0.0f;
			return m_pTarget->GetOrthoWidth();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void LookAt(const maPoint3d &i_Pos, const maPoint3d &i_Target,
						 const maVector3d &i_Up)
		{
			DBG_ASSERT(m_pTarget!=NULL, "Need camera manip target for wrapper class.");
			if (!m_pTarget)
				return;
			m_pTarget->LookAt(i_Pos, i_Target, i_Up);

			// This separate call to notify callbacks helps track changes from the
			// manipulator versus changes from other sources.
			m_pTarget->NotifyCallbacks();
		}

	private:
		TargetType* m_pTarget;
};
