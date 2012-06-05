/*****************************************************************************
**  lodModeLODTest.hpp
**
**      The LOD test mode
**
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef LOD_MODELODTEST_HPP
#error lodModeLODTest.hpp multiply included
#endif
#define LOD_MODELODTEST_HPP

#ifndef LOD_MODE_HPP
#include "lodMode.hpp"
#endif
#ifndef DM_FSM_HPP
#include "dmFSM.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "maPoint3d.hpp"
#endif
#ifndef MA_VECTOR3D_HPP
#include "maVector3d.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "maRotation.hpp"
#endif

//----------------------------------------------------------------------------
//	forward references
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class lodModeLODTest : public lodMode,
						public dmFSM
{
	public:
		enum LODTestStates
		{
			e_StateMovement = 0,
			e_StateTestLOD,
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		lodModeLODTest();

		//--------------------------------------------------------------------
		// pure-virtual, children must be created
		//--------------------------------------------------------------------
		virtual ~lodModeLODTest();

		//--------------------------------------------------------------------
		//	Initialize will be called before the first call of Think after
		//	the object is first created or DeInitialized.  During the
		//	lifetime of a mode, Initialize and DeInitialize may be called
		//	several times.  Children of appMode should remember to call
		//	appMode::Initialize() at the beginning of their Initialize
		//	function.
		//--------------------------------------------------------------------
		virtual void Initialize();

		//--------------------------------------------------------------------
		//	The object should clean up things that are not needed while the
		//	mode is not running in the DeInitialize function.
		//--------------------------------------------------------------------
		virtual void DeInitialize();

		//--------------------------------------------------------------------
		//	The mode should do it's per frame "work" in the Think function.
		// This simply calls the base class and handles screen captures.
		//--------------------------------------------------------------------
		virtual void Think();

		//--------------------------------------------------------------------
		//	Set States (externally)
		//--------------------------------------------------------------------
		void SetStateMovement();
		void SetStateTestLOD();

private:
		//--------------------------------------------------------------------
		//	set the current state based on the last state of the object
		//--------------------------------------------------------------------
		void set_current_state();

public:
		//
		//	states
		//

		//--------------------------------------------------------------------
		//	BeginStateMovement - is executed ONCE at the start of the state
		//--------------------------------------------------------------------
		virtual void BeginStateMovement();

		//--------------------------------------------------------------------
		//	OnStateMovement - is executed every frame while in this state
		//--------------------------------------------------------------------
		virtual void OnStateMovement();

		//--------------------------------------------------------------------
		//	EndStateMovement - is executed ONCE on ending the state
		//--------------------------------------------------------------------
		virtual void EndStateMovement();

		//--------------------------------------------------------------------
		//	BeginStateTestLOD - is executed ONCE at the start of the state
		//--------------------------------------------------------------------
		virtual void BeginStateTestLOD();

		//--------------------------------------------------------------------
		//	OnStateTestLOD - is executed every frame while in this state
		//--------------------------------------------------------------------
		virtual void OnStateTestLOD();

		//--------------------------------------------------------------------
		//	EndStateTestLOD - is executed ONCE on ending the state
		//--------------------------------------------------------------------
		virtual void EndStateTestLOD();

	private:
		//state machine declarations
		dmState<lodModeLODTest>				m_StateMovement;
		dmState<lodModeLODTest>				m_StateTestLOD;

		bool m_bInitialized;
		bool m_bAdded;
};

