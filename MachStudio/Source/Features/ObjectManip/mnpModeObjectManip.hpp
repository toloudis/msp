/*****************************************************************************
**  mnpModeObjectManip.hpp
**
**      The object manipulation mode
**
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef MNP_MODEOBJECTMANIP_HPP
#error mnpModeObjectManip.hpp multiply included
#endif
#define MNP_MODEOBJECTMANIP_HPP

#ifndef MODE_MODETIME_HPP
#include "Support/mode/modeModeTime.hpp"
#endif
#ifndef DM_FSM_HPP
#include "Core/dm/dmFSM.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif

//----------------------------------------------------------------------------
//	forward references
//----------------------------------------------------------------------------
class mnpInteraction;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class mnpModeObjectManip : public modeModeTime,
						      public dmFSM
{
	public:
		enum ObjectManipStates
		{
			e_StateSelect = 0,
			e_StateTranslate,
			e_StateRotate,
			e_StateScale,
			e_StatePlacement,
		};
		enum PickMode
		{
			e_PickAll = -1,
			e_PickObjects = 0,
			e_PickMaterials,
			e_PickSurfaces
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		mnpModeObjectManip();

		//--------------------------------------------------------------------
		// pure-virtual, children must be created
		//--------------------------------------------------------------------
		virtual ~mnpModeObjectManip();

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
		// PickMode - does a mouse click pick the object, material or surface
		//--------------------------------------------------------------------
		PickMode GetPickMode() const;
		void SetPickMode(PickMode i_PickMode);

		//--------------------------------------------------------------------
		// Pick Mode Toolbar - Sets boolean for toolbar pick mode state change
		//--------------------------------------------------------------------
		void SetPickModeChanged(bool i_bPickChanged);
		bool GetPickModeChanged();

		//--------------------------------------------------------------------
		//	SetCompassParts sets the parts of the compass that we are currently
		//	using
		//--------------------------------------------------------------------
		void SetCompassPart(const int i_Parts);

		//--------------------------------------------------------------------
		//	GetCompassParts gets the cparts of the compass that we are currently
		//	using
		//--------------------------------------------------------------------
		inline int GetCompassPart();

		//--------------------------------------------------------------------
		//	change the scale of the compass
		//--------------------------------------------------------------------
		void IncrementScaleOfCompass();
		void DecrementScaleOfCompass();

		//--------------------------------------------------------------------
		//	Flip the compass
		//--------------------------------------------------------------------
		void ToggleCompassFlipState();

		//--------------------------------------------------------------------
		//	Local space translation
		//--------------------------------------------------------------------
		static void SetLocalSpaceTranslation(bool i_bLocalSpace);
		static bool IsLocalSpaceTranslation();

		//--------------------------------------------------------------------
		//	move the timeline
		//--------------------------------------------------------------------
		//void TimelineIncrementFrame();
		//void TimelineDecrementFrame();

		//--------------------------------------------------------------------
		//	SetObject sets the currently selected object.  NULL is valid
		//--------------------------------------------------------------------
		void SetObjectIndex(int i_Object);

		//--------------------------------------------------------------------
		//	GetObject gets the currently selected object
		//--------------------------------------------------------------------
		inline int GetObjectIndex();

		//--------------------------------------------------------------------
		//	Set States (externally)
		//--------------------------------------------------------------------
		void SetStateTranslateFreeForm();
		void SetStateTranslate();
		void SetStateRotate();
		void SetStateScale();
		void SetStateSelect( bool i_bUpdateCurrentState = true );
		void SetStateLockSelect();
		void SetStatePlacement();

		//--------------------------------------------------------------------
		// select the next object in the selected list
		//--------------------------------------------------------------------
		//void SelectNextObjectInPickList();

private:
		//--------------------------------------------------------------------
		//	set the current state based on the last state of the object
		//--------------------------------------------------------------------
		void set_current_state();

		//--------------------------------------------------------------------
		//	set the current text based on the current mode
		//--------------------------------------------------------------------
		void set_current_mode_text();

		//--------------------------------------------------------------------
		//	Configure compass for a given state. This is called each time
		//	the selected object changes (but the state doesn't)
		//--------------------------------------------------------------------
		void SetCompassTranslateFreeForm();
		void SetCompassTranslate();
		void SetCompassRotate();
		void SetCompassScale();
		void SetCompassSelect();
		void SetCompassPlacement();

public:
		//
		//	states
		//

		//--------------------------------------------------------------------
		//	BeginStateTranslate - is executed ONCE at the start of the state
		//--------------------------------------------------------------------
		virtual void BeginStateTranslate();

		//--------------------------------------------------------------------
		//	OnStateTranslate - is executed every frame while in this state
		//--------------------------------------------------------------------
		virtual void OnStateTranslate();

		//--------------------------------------------------------------------
		//	EndStateTranslate - is executed ONCE on ending the state
		//--------------------------------------------------------------------
		virtual void EndStateTranslate();

		//--------------------------------------------------------------------
		//	BeginStateTranslateFreeForm - is executed ONCE at the start of the state
		//--------------------------------------------------------------------
		virtual void BeginStateTranslateFreeForm();

		//--------------------------------------------------------------------
		//	OnStateTranslateFreeForm - is executed every frame while in this state
		//--------------------------------------------------------------------
		virtual void OnStateTranslateFreeForm();

		//--------------------------------------------------------------------
		//	EndStateTranslateFreeForm - is executed ONCE on ending the state
		//--------------------------------------------------------------------
		virtual void EndStateTranslateFreeForm();

		//--------------------------------------------------------------------
		//	BeginStateRotate - is executed ONCE at the start of the state
		//--------------------------------------------------------------------
		virtual void BeginStateRotate();

		//--------------------------------------------------------------------
		//	OnStateRotate - is executed every frame while in this state
		//--------------------------------------------------------------------
		virtual void OnStateRotate();

		//--------------------------------------------------------------------
		//	EndStateRotate - is executed ONCE on ending the state
		//--------------------------------------------------------------------
		virtual void EndStateRotate();

		//--------------------------------------------------------------------
		//	BeginStateScale - is executed ONCE at the start of the state
		//--------------------------------------------------------------------
		virtual void BeginStateScale();

		//--------------------------------------------------------------------
		//	OnStateScale - is executed every frame while in this state
		//--------------------------------------------------------------------
		virtual void OnStateScale();

		//--------------------------------------------------------------------
		//	EndStateScale - is executed ONCE on ending the state
		//--------------------------------------------------------------------
		virtual void EndStateScale();

		//--------------------------------------------------------------------
		//	BeginStateSelect - is executed ONCE at the start of the state
		//--------------------------------------------------------------------
		virtual void BeginStateSelect();

		//--------------------------------------------------------------------
		//	OnStateSelect - is executed every frame while in this state
		//--------------------------------------------------------------------
		virtual void OnStateSelect();

		//--------------------------------------------------------------------
		//	EndStateSelect - is executed ONCE on ending the state
		//--------------------------------------------------------------------
		virtual void EndStateSelect();

		//--------------------------------------------------------------------
		//	BeginStatePlacement - is executed ONCE at the start of the state
		//--------------------------------------------------------------------
		virtual void BeginStatePlacement();

		//--------------------------------------------------------------------
		//	OnStatePlacement - is executed every frame while in this state
		//--------------------------------------------------------------------
		virtual void OnStatePlacement();

		//--------------------------------------------------------------------
		//	EndStatePlacement - is executed ONCE on ending the state
		//--------------------------------------------------------------------
		virtual void EndStatePlacement();

	private:
		//--------------------------------------------------------------------
		//	SetSavedCompassParts sets the parts of the compass that we are currently
		//	using
		//--------------------------------------------------------------------
		inline void SetSavedCompassParts(int i_Part);

		//--------------------------------------------------------------------
		//	GetSavedCompassParts gets the parts of the compass that we are currently
		//	using
		//--------------------------------------------------------------------
		inline int GetSavedCompassParts();

	private:
		//state machine declarations
		dmState<mnpModeObjectManip>				m_StateTranslate;
		dmState<mnpModeObjectManip>				m_StateTranslateFreeForm;
		dmState<mnpModeObjectManip>				m_StateRotate;
		dmState<mnpModeObjectManip>				m_StateScale;
		dmState<mnpModeObjectManip>				m_StateSelect;
		dmState<mnpModeObjectManip>				m_StatePlacement;

		bool m_bInitialized;
		bool m_bAdded;

		int	m_nCompassPart;
		int	m_nSavedCompassParts;
		int m_ObjectIndex;

		bool m_bClickStarted;
		PickMode m_PickMode;
		int m_MouseDownX, m_MouseDownY;

		// Interaction objects handle translation of mouse motion 
		// into movement of selected objects
		shared_ptr<mnpInteraction>	m_CurrentInteraction;
};

//--------------------------------------------------------------------
//	GetCompassParts gets the cparts of the compass that we are currently
//	using
//--------------------------------------------------------------------
inline int mnpModeObjectManip::GetCompassPart()
{
	return m_nCompassPart;
}

//--------------------------------------------------------------------
//	GetObject gets the currently selected object
//--------------------------------------------------------------------
inline int mnpModeObjectManip::GetObjectIndex()
{
	return m_ObjectIndex;
}


//--------------------------------------------------------------------
//	SetSavedCompassPartss
//--------------------------------------------------------------------
inline void mnpModeObjectManip::SetSavedCompassParts(int i_Parts)
{
	m_nSavedCompassParts = i_Parts;
}

//--------------------------------------------------------------------
//	GetSavedCompassPartss
//--------------------------------------------------------------------
inline int mnpModeObjectManip::GetSavedCompassParts()
{
	return m_nSavedCompassParts;
}
