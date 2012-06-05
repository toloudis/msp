/*****************************************************************************
**  orthoModeObjectManip.hpp
**
**      The object manipulation mode
**
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef ORTHO_MODEOBJECTMANIP_HPP
#error orthoModeObjectManip.hpp multiply included
#endif
#define ORTHO_MODEOBJECTMANIP_HPP

#ifndef MODE_MODETIME_HPP
#include "Support/mode/modeModeTime.hpp"
#endif
#ifndef DM_FSM_HPP
#include "Core/dm/dmFSM.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/Ma/maPoint3d.hpp"
#endif
#ifndef MA_VECTOR3D_HPP
#include "Core/Ma/maVector3d.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/Ma/maRotation.hpp"
#endif

//============================================================================
//============================================================================
class orthoModeObjectManip : public modeModeTime,
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
		orthoModeObjectManip();

		//--------------------------------------------------------------------
		// pure-virtual, children must be created
		//--------------------------------------------------------------------
		virtual ~orthoModeObjectManip();

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
		//	move the timeline
		//--------------------------------------------------------------------
		void TimelineIncrementFrame();
		void TimelineDecrementFrame();

		//--------------------------------------------------------------------
		//	SetObject sets the currently selected object.  NULL is valid
		//--------------------------------------------------------------------
		void SetObjectIndex(int i_Object);

		//--------------------------------------------------------------------
		//	GetObject gets the currently selected object
		//--------------------------------------------------------------------
		inline int GetObjectIndex();

		//--------------------------------------------------------------------
		//	SetMouseDownPoint sets the point on the object where the click
		//	took place
		//--------------------------------------------------------------------
		inline void SetMouseDownPoint(const maPoint3d& i_Point);

		//--------------------------------------------------------------------
		//	GetMouseDownPoint gets the point on the object where the click
		//	took place
		//--------------------------------------------------------------------
		inline maPoint3d GetMouseDownPoint();

		//--------------------------------------------------------------------
		//	Set States (externally)
		//--------------------------------------------------------------------
		void SetStateTranslateFreeForm();
		void SetStateTranslate();
		void SetStateRotate();
		void SetStateScale();
		void SetStateSelect( bool i_bUpdateCurrentState = true );
		void SetStatePlacement();

		//--------------------------------------------------------------------
		// select the next object in the selected list
		//--------------------------------------------------------------------
		void SelectNextObjectInPickList();

private:
		//--------------------------------------------------------------------
		//	set the current state based on the last state of the object
		//--------------------------------------------------------------------
		void set_current_state();

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
		//	SetOriginalPosition
		//--------------------------------------------------------------------
		inline void SetOriginalPosition(const maPoint3d& i_Point);

		//--------------------------------------------------------------------
		//	GetOriginalPosition
		//--------------------------------------------------------------------
		inline maPoint3d GetOriginalPosition();

		//--------------------------------------------------------------------
		//	SetSelectionOffset
		//--------------------------------------------------------------------
		inline void SetSelectionOffset(const maPoint3d& i_Point);

		//--------------------------------------------------------------------
		//	GetSelectionOffset
		//--------------------------------------------------------------------
		inline maPoint3d GetSelectionOffset();

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
		dmState<orthoModeObjectManip>				m_StateTranslate;
		dmState<orthoModeObjectManip>				m_StateTranslateFreeForm;
		dmState<orthoModeObjectManip>				m_StateRotate;
		dmState<orthoModeObjectManip>				m_StateScale;
		dmState<orthoModeObjectManip>				m_StateSelect;
		dmState<orthoModeObjectManip>				m_StatePlacement;

		bool m_bInitialized;
		bool m_bAdded;

		int	m_nCompassPart;
		int	m_nSavedCompassParts;
		int m_ObjectIndex;
		int m_StartRotationValue;
		float m_fOriginalSize;

		maPoint3d m_MouseDownPoint;
		maVector3d m_OriginalPosition;
		maVector3d m_SelectionOffset;
		maVector3d m_OriginalScale;

		maRotation m_OriginalOrientation;
		maPoint3d m_PivotPoint;
		maVector3d m_RotationAxis;
};


//--------------------------------------------------------------------
//	GetCompassParts gets the cparts of the compass that we are currently
//	using
//--------------------------------------------------------------------
inline int orthoModeObjectManip::GetCompassPart()
{
	return m_nCompassPart;
}

//--------------------------------------------------------------------
//	GetObject gets the currently selected object
//--------------------------------------------------------------------
inline int orthoModeObjectManip::GetObjectIndex()
{
	return m_ObjectIndex;
}

//--------------------------------------------------------------------
//	SetMouseDownPoint sets the point on the object where the click
//	took place
//--------------------------------------------------------------------
inline void orthoModeObjectManip::SetMouseDownPoint(const maPoint3d& i_Point)
{
	m_MouseDownPoint = i_Point;
}

//--------------------------------------------------------------------
//	GetMouseDownPoint gets the point on the object where the click
//	took place
//--------------------------------------------------------------------
inline maPoint3d orthoModeObjectManip::GetMouseDownPoint()
{
	return m_MouseDownPoint;
}

//--------------------------------------------------------------------
//	SetOriginalPosition
//--------------------------------------------------------------------
inline void orthoModeObjectManip::SetOriginalPosition(const maPoint3d& i_Point)
{
	m_OriginalPosition = i_Point;
}

//--------------------------------------------------------------------
//	GetOriginalPosition
//--------------------------------------------------------------------
inline maPoint3d orthoModeObjectManip::GetOriginalPosition()
{
	return m_OriginalPosition;
}

//--------------------------------------------------------------------
//	SetSelectionOffset
//--------------------------------------------------------------------
inline void orthoModeObjectManip::SetSelectionOffset(const maPoint3d& i_Point)
{
	m_SelectionOffset = i_Point;
}

//--------------------------------------------------------------------
//	GetSelectionOffset
//--------------------------------------------------------------------
inline maPoint3d orthoModeObjectManip::GetSelectionOffset()
{
	return m_SelectionOffset;
}

//--------------------------------------------------------------------
//	SetSavedCompassPartss
//--------------------------------------------------------------------
inline void orthoModeObjectManip::SetSavedCompassParts(int i_Parts)
{
	m_nSavedCompassParts = i_Parts;
}

//--------------------------------------------------------------------
//	GetSavedCompassPartss
//--------------------------------------------------------------------
inline int orthoModeObjectManip::GetSavedCompassParts()
{
	return m_nSavedCompassParts;
}
