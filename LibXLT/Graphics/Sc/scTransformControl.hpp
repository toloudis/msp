/*****************************************************************************
**	scTransformControl.hpp
**
**		scTransformControl defines a base class for animations that give
**	programmatic control over nodes in an object.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef SC_TRANSFORMCONTROL_HPP
#error scTransformControl.hpp multiply included
#endif
#define SC_TRANSFORMCONTROL_HPP

#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif
#ifndef SC_CONTROLANIM_HPP
#include "Graphics/sc/scControlAnim.hpp"
#endif


//============================================================================
//============================================================================
class g3dSceneNode;


//============================================================================
//============================================================================
class scTransformControl : public scControlAnim
{
	public:
		//--------------------------------------------------------------------
		//  Constructor - pass in node to control
		//--------------------------------------------------------------------
		scTransformControl(g3dSceneNode *i_pControlNode);

		//--------------------------------------------------------------------
		//	Destructor
		//--------------------------------------------------------------------
		virtual ~scTransformControl();

		//--------------------------------------------------------------------
		// Rotation, expressed as quaternions
		//--------------------------------------------------------------------
		void SetRotation(const maRotation& i_Rotation);
		const maRotation& GetRotation() const;

		//--------------------------------------------------------------------
		//  Rotation value, stored as Euler angles (X,Y,Z) in degrees
		//--------------------------------------------------------------------
		void  SetEulerAngles(const maVector3d &i_Angles);
		const maVector3d &  GetEulerAngles() const;

		//--------------------------------------------------------------------
		// Translation
		//--------------------------------------------------------------------
		void SetTranslation(const maVector3d& i_Translation);
		const maVector3d& GetTranslation() const;

		//--------------------------------------------------------------------
		// Scale
		//--------------------------------------------------------------------
		void SetScale(const maVector3d& i_Scale);
		const maVector3d& GetScale() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		inline g3dSceneNode* GetSceneNode();

		//--------------------------------------------------------------------
		//	Animate is called by the scObject after it has done its base
		//		animation
		//--------------------------------------------------------------------
		virtual void Animate(float i_SimulationTime);

		//--------------------------------------------------------------------
		// CheckDirty - return true if the object needs to animate the model.
		//		Clears the dirty bit so that if the time is different
		//		it will be dirty next call.
		//--------------------------------------------------------------------
		virtual bool CheckDirty(float i_SimTime);

	private:
		g3dSceneNode *m_pControlNode;
		maRotation m_Rotation;
		maVector3d m_EulerAngles;
		maVector3d m_Translation;
		maVector3d m_Scale;
		bool m_bDirty;
};


//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline g3dSceneNode* scTransformControl::GetSceneNode()
{
	return m_pControlNode;
}

