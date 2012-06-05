/*****************************************************************************
**	scRotateControl.hpp
**
**		scRotateControl defines a base class for animations that give
**	programmatic control over nodes in an object.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef SC_ROTATECONTROL_HPP
#error scRotateControl.hpp multiply included
#endif
#define SC_ROTATECONTROL_HPP

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
class scRotateControl : public scControlAnim
{
	public:
		//--------------------------------------------------------------------
		//  Constructor - pass in node to control
		//--------------------------------------------------------------------
		scRotateControl(g3dSceneNode *i_pControlNode);

		//--------------------------------------------------------------------
		//	Destructor
		//--------------------------------------------------------------------
		virtual ~scRotateControl();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetRotation(const maRotation& i_Rotation);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		const maRotation& GetRotation() const;

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
		bool m_bDirty;

};


//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline g3dSceneNode* scRotateControl::GetSceneNode()
{
	return m_pControlNode;
}

