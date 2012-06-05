/*****************************************************************************
**  lodModeLODTest.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "lodModeLODTest.hpp"

#include "lodModeMgr.hpp"
#include "lodLevel.hpp"

//	tools
#include "tma3dCursorMgr.hpp"

//	library
#include "appSimTime.hpp"
#include "appTime.hpp"
#include "cam3dMgr.hpp"
#include "dbgLog.hpp"
#include "fsFileUtil.hpp"
//#include "fsLocator.hpp"
#include "inDeviceMgr.hpp"
#include "inVirtualJoystick.hpp"
#include "itStringUtil.hpp"
#include "maConstants.hpp"
//#include "muiMenuMgr.hpp"
#include "muiMessageBox.hpp"
#include "muiStatusBarMgr.hpp"
#include "muiAppTitleMgr.hpp"


namespace
{
	const char* MENUITEM_LOD			= "&LOD";
	const char* MENUITEM_LOD_TEST		= "&Test";
	const char* MENUITEM_LOD_MOVEMENT	= "&Movement";
	
	const float lc_fRotateIncrement = maConstants::c_fPI / 200.0f;
	const float lc_fFocusRadius	= 10.0f;

	const maVector3d lc_RotateX( 1, 0, 0 );
	const maVector3d lc_RotateY( 0, 1, 0 );
	const maVector3d lc_RotateZ( 0, 0, 1 );

	bool l_bDirty = false;

	lodModeLODTest::LODTestStates l_CurrentManipState;

	//--------------------------------------------------------------------
	// the callback function for menu item: TestLOD
	//--------------------------------------------------------------------
	void menuitem_click_TestLOD( int i_ObjectID )
	{
		lodMode* pMode = lodModeMgr::GetCurrentMode();

		//DBG_LOG2( "mode id %d (%s)", pMode->GetID(), pMode->GetMenuItemName() );

		lodModeLODTest* pModeOM = dynamic_cast<lodModeLODTest*>(pMode);
		if ( pModeOM != 0 )
		{
			pModeOM->SetStateTestLOD();
		}
	}

	//--------------------------------------------------------------------
	// the callback function for menu item: Movement
	//--------------------------------------------------------------------
	void menuitem_click_Movement( int i_ObjectID )
	{
		lodMode* pMode = lodModeMgr::GetCurrentMode();
		lodModeLODTest* pModeOM = dynamic_cast<lodModeLODTest*>(pMode);
		if ( pModeOM != 0 )
		{
			pModeOM->SetStateMovement();
		}
	}
	void menu_items_enable()
	{
		//muiMenuMgr::EnableMenuItem( MENUITEM_LOD, MENUITEM_LOD_TEST, true );
		//muiMenuMgr::EnableMenuItem( MENUITEM_LOD, MENUITEM_LOD_MOVEMENT, true );
	}
	void menu_items_disable()
	{
		//muiMenuMgr::EnableMenuItem( MENUITEM_LOD, MENUITEM_LOD_TEST, false );
		//muiMenuMgr::EnableMenuItem( MENUITEM_LOD, MENUITEM_LOD_MOVEMENT, false );
	}
	void menu_items_add()
	{
		//int objectID;
		//objectID = muiMenuMgr::AddMenuItem( MENUITEM_LOD, MENUITEM_LOD_TEST, true, "Movement.bmp" );
		//muiMenuMgr::AttachEventToMenuObjects( objectID, menuitem_click_Movement );
		//objectID = muiMenuMgr::AddMenuItem( MENUITEM_LOD, MENUITEM_LOD_MOVEMENT, true, "rotate.bmp" );
		//muiMenuMgr::AttachEventToMenuObjects( objectID, menuitem_click_Rotate );
	}
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
lodModeLODTest::lodModeLODTest()
:	m_bInitialized( false ),
	m_bAdded( false )
{
	SetMenuItemName( "LOD" );

	//	set-up the state call-backs
	m_StateMovement.SetState( this, &lodModeLODTest::BeginStateMovement, &lodModeLODTest::OnStateMovement, &lodModeLODTest::EndStateMovement );
	m_StateTestLOD.SetState( this, &lodModeLODTest::BeginStateTestLOD, &lodModeLODTest::OnStateTestLOD, &lodModeLODTest::EndStateTestLOD );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// virtual
lodModeLODTest::~lodModeLODTest()
{
}

//----------------------------------------------------------------------------
//	Initialize will be called before the first call of Think after
//	the object is first created or DeInitialized.  During the
//	lifetime of a mode, Initialize and DeInitialize may be called
//	several times.  Children of appMode should remember to call
//	appMode::Initialize() at the beginning of their Initialize
//	function.
//----------------------------------------------------------------------------
//virtual
void lodModeLODTest::Initialize()
{
	if ( !m_bInitialized )
	{
		if ( m_bAdded )
		{
			menu_items_enable();
		}
		else
		{
			menu_items_add();
			m_bAdded = true;
		}

		SetStateMovement();	// start state
	}

	m_bInitialized = true;

	muiStatusBarMgr::SetText(0,"Mode LOD Test");

	//TEST
	//fsLocator modelpath;
	//fsFileUtil::ANSIFilenameToLocator( std::string( "C:\\projects\\MeatSweater\\Data\\Props\\Models\\Bill_Night.jnx" ), modelpath );
	//lodLevel::Load( modelpath );
}

//----------------------------------------------------------------------------
//	The object should clean up things that are not needed while the
//	mode is not running in the DeInitialize function.
//----------------------------------------------------------------------------
//virtual
void lodModeLODTest::DeInitialize()
{
	if ( m_bInitialized )
	{
		//	on leaving the mode don't remove the menu items for this mode,
		//	just disable them.
		//
		menu_items_disable();
	}

	m_bInitialized = false;
}


//----------------------------------------------------------------------------
//	The mode should do it's per frame "work" in the Think function.
//----------------------------------------------------------------------------
// virtual
void lodModeLODTest::Think()
{
	lodMode::Think();

	// update sim time to match our current value (no time flowing though)
	//appSimTime::SetTime(tmlnTimeLine::GetValue(), 0.0f);

	//	update the state
	//
	UpdateState();

	//
	inVirtualJoystick *pVJoy = inDeviceMgr::GetVirtualJoystick();
	inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();

			// debug
	//char text[64];
	//sprintf(text, "compass: part %d  manip %s", l_nCompassPart, ( l_bCompassManip ? "true":"false" ) );
	//lodDebugInfo::SetDebugInfo(11, text);
	//sprintf(text, "pick size = %d", l_PickList.GetSize() );
	//lodDebugInfo::SetDebugInfo(10, text);
	//sprintf(text, "state = %s", ( (l_CurrentManipState==e_StateSelect)?"Select":"Other") );
	//lodDebugInfo::SetDebugInfo(16, text);

	//if ( !( pKeyboard->IsHeld(inKeys::e_LALT) || pKeyboard->IsHeld(inKeys::e_RALT)) )
	//{
	//	// check to see if there is something we are selecting
	//	if ( is_left_click(pVJoy) )
	//	{
	//		// if the shift key is held, accumulate picks.
	//		//
	//		if ( !( pKeyboard->IsHeld(inKeys::e_LSHIFT) || pKeyboard->IsHeld(inKeys::e_RSHIFT)) )
	//		{
	//			//	remove all the picks so they don't accumulate
	//			//
	//			l_PickList.Clear();
	//		}
	//		else
	//		{
	//			// don't clear the list
	//		}

	//		if ( check_for_picks() )
	//		{
	//			//DBG_LOG1( "pick list size = %d", l_PickList.GetSize() );

	//			sel3dMgr::SetSelectList( l_PickList, false );
	//			//pick3dPickObject* pPObj = sel3dMgr::GetSelected();

	//			set_current_state();
	//		}
	//		else
	//		{
	//			//SetStateSelect(false);
	//		}
	//	}
	//	else
	//	if ( pVJoy->IsPressed( lodVJoystick::e_RightClick ) ) // && !pVJoy->IsPressed( lodVJoystick::e_LeftClick ) )
	//	{
	//		selected_object_clear();
	//		SetStateSelect(false);
	//	}
	//}

	//if ( pKeyboard )
	//{
	//	//
	//	// Check for input
	//	//

	//	//	focus on selected object
	//	if ( pKeyboard->IsReleased(inKeys::e_F) )
	//	{
	//		if ( l_pSelectedObject )
	//		{
	//			cam3dMgr::FocusCamera( l_pSelectedObject->GetPosition(), lc_fFocusRadius );
	//		}
	//		else
	//		{
	//			//	if no object, find the origin
	//			cam3dMgr::FocusCamera( maPoint3d(0.0f,0.0f,0.0f), lc_fFocusRadius );
	//		}
	//	}

	//	if ( pKeyboard->IsReleased(inKeys::e_A) )
	//	{
	//		select_next_object_in_picklist();
	//	}

	//	//	switch states
	//	if ( pKeyboard->IsReleased(inKeys::e_W) )
	//	{
	//		SetStateSelect();
	//	}
	//	else if ( pKeyboard->IsReleased(inKeys::e_E) )
	//	{
	//		SetStateScale();
	//	}
	//	else if ( pKeyboard->IsReleased(inKeys::e_R) )
	//	{
	//		SetStateRotate();
	//	}
	//	else if ( pKeyboard->IsReleased(inKeys::e_T) )
	//	{
	//		SetStateMovement();
	//	}
	//	else if ( pKeyboard->IsReleased(inKeys::e_Y) )
	//	{
	//		SetStatePlacement();
	//	}
	//	else if ( pKeyboard->IsReleased(inKeys::e_U) )
	//	{
	//		SetStateMovementFreeForm();
	//	}
	//}

	////	compass updates
	//lodCompassUtil::UpdateCompass( cmpsCompassMgr::e_World );

	//if ( cmpsCompassMgr::IsSelectedObjectChanged()  ||
	//	( l_pSelectedObjectLast != l_pSelectedObject ) )
	//{
	//	cmpsCompassMgr::SetSelectedObjectChanged( false );

	//	if (l_pSelectedObject != NULL)
	//	{
	//		set_compass_to_object( l_nCompassType, l_pSelectedObject );
	//	}
	//}

	////cmpsCompassMgr::Think( cam3dMgr::GetCamera().GetPosition() );

	////	check if the app title bar needs to be updated.
	////
	//if ( l_bDirty && !docSingleDocumentMgr::NeedsSave() )
	//{
	//	l_bDirty = false;
	//	lodAppUtil::UpdateTitleBar( l_bDirty );
	//}
	//else if ( !l_bDirty && docSingleDocumentMgr::NeedsSave() )
	//{
	//	l_bDirty = true;
	//	lodAppUtil::UpdateTitleBar( l_bDirty );
	//}

	////	AutoSave
	////
	//lodAutoSaveMgr::Think();
}


//----------------------------------------------------------------------------
//	Set States (externally)
//----------------------------------------------------------------------------
void lodModeLODTest::SetStateMovement()
{
	l_CurrentManipState = e_StateMovement;
	GotoState( m_StateMovement );
}
void lodModeLODTest::SetStateTestLOD()
{
	l_CurrentManipState = e_StateTestLOD;
	GotoState( m_StateTestLOD );
}


//--------------------------------------------------------------------
//	set the current state based on the last state of the object
//--------------------------------------------------------------------
void lodModeLODTest::set_current_state()
{
	switch( l_CurrentManipState )
	{
		case lodModeLODTest::e_StateMovement:
			SetStateMovement();
			//DBG_LOG0( "setting...Movement");
			break;
		case lodModeLODTest::e_StateTestLOD:
			SetStateTestLOD();
			//DBG_LOG0( "setting...scale");
			break;
	}
}


//
//	state functions
//

//----------------------------------------------------------------------------
//	BeginStateMovement - is executed ONCE at the start of the state
//----------------------------------------------------------------------------
//virtual
void lodModeLODTest::BeginStateMovement()
{
	//	set the object
	//l_pSelectedObject = cast_selected_to_lodobject();
	//DBG_ASSERT0( l_pSelectedObject != 0, "NULL object in modeLODTest state" );
}

//----------------------------------------------------------------------------
//	OnStateMovement - is executed every frame while in this state
//----------------------------------------------------------------------------
//virtual
void lodModeLODTest::OnStateMovement()
{
		// debug
	//char text[64];
	//sprintf(text, "Mode: LOD State: Movement" );
	//lodDebugInfo::SetDebugInfo(12, text);

	inVirtualJoystick *pVJoy = inDeviceMgr::GetVirtualJoystick();
	inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();

	////	check for compass picks
	//maPoint3d pos;
	//int currCompassPartSelected;
	//currCompassPartSelected = check_compass_picks( l_nCompassType, pos );

	////	compass manip
	//if ( l_bCompassManip && ( !pVJoy->IsHeld(lodVJoystick::e_LeftClick) ) )
	//{
	//	l_bCompassManip = false;

	//	show_compassparts( m_nSavedCompassParts );
	//}
	//if ( currCompassPartSelected != -1 )
	//{
	//	if ( is_left_click(pVJoy) )
	//	{
	//		//	start the manip
	//		l_bCompassManip = true;
	//		l_nCompassPart = currCompassPartSelected;

	//		// save the current state of the compass so we can restore it later
	//		//SetSavedCompassParts( cmpsCompassMgr::GetParts(l_nCompassType) );
	//		SetSavedCompassParts( m_nCompassPart );
	//		show_compassparts( l_nCompassPart );

	//		//DBG_LOG2( " cmps(%d) full(%d)", l_nCompassPart, m_nCompassPart );

	//		SetMouseDownPoint( pos );

	//		m_OriginalPosition	= l_pSelectedObject->GetPosition();
	//		m_SelectionOffset	= m_OriginalPosition - m_MouseDownPoint;
	//	}
	//}

	//	//debug
	////sprintf(text, "original pos (%6.3f,%6.3f,%6.3f)", m_OriginalPosition.GetX(), m_OriginalPosition.GetY(), m_OriginalPosition.GetZ()  );
	////lodDebugInfo::SetDebugInfo(20, text);

	////	if the compass is being manipulated
	////
	//if ( l_bCompassManip )
	//{
	//	// Get the axis being moved
	//	//
	//	maVector3d axis;
	//	switch ( l_nCompassPart )
	//	{
	//		case cmpsCompass::e_X:
	//			axis.Set( 1, 0, 0 );
	//			break;
	//		case cmpsCompass::e_Y:
	//			axis.Set( 0, 1, 0 );
	//			break;
	//		case cmpsCompass::e_Z:
	//			axis.Set( 0, 0, 1 );
	//			break;
	//	}

	//	// Calculate the change in position
	//	maPoint3d intersect_point;
	//	tma3dCursorMgr::PlaneIntersection(	m_MouseDownPoint,
	//										-( cam3dMgr::GetCamera().GetDirection() ),
	//										intersect_point );
	//	maVector3d Diff = intersect_point - m_MouseDownPoint;
	//	float distance = axis * Diff;
	//	maVector3d new_position = m_OriginalPosition + axis * distance;

	//	// Set the new position
	//	l_pSelectedObject->UpdatePosition( new_position );
	//	cmpsCompassMgr::SetPosition( cmpsCompassMgr::e_Movement, new_position  );

	//		//debug
	//	//sprintf(text, "new    pos   (%6.3f,%6.3f,%6.3f)", new_position.GetX(), new_position.GetY(), new_position.GetZ()  );
	//	//lodDebugInfo::SetDebugInfo(21, text);
	//	//(text, "object pos   (%6.3f,%6.3f,%6.3f)", l_pSelectedObject->GetPosition().GetX(), l_pSelectedObject->GetPosition().GetY(), l_pSelectedObject->GetPosition().GetZ()  );
	//	//lodDebugInfo::SetDebugInfo(22, text);
	//}
	//else if ( ( pVJoy->IsPressed( lodVJoystick::e_RightClick ) && pVJoy->IsHeld( lodVJoystick::e_LeftClick ) )
	//		||
	//		  pKeyboard->IsPressed( inKeys::e_ESC ) )
	//{
	//	// User aborted moving
	//	l_pSelectedObject->UpdatePosition( m_OriginalPosition );
	//	cmpsCompassMgr::SetPosition( cmpsCompassMgr::e_Movement, m_OriginalPosition  );
	//}
}


//----------------------------------------------------------------------------
//	EndStateMovement - is executed ONCE on ending the state
//----------------------------------------------------------------------------
//virtual
void lodModeLODTest::EndStateMovement()
{
}


//----------------------------------------------------------------------------
//	BeginStateTestLOD - is executed ONCE at the start of the state
//----------------------------------------------------------------------------
//virtual
void lodModeLODTest::BeginStateTestLOD()
{
	//	set the selected object
	//l_pSelectedObject = cast_selected_to_lodobject();
	//DBG_ASSERT0( l_pSelectedObject != 0, "NULL object in modeLODTest state" );
}

//----------------------------------------------------------------------------
//	OnStateTestLOD - is executed every frame while in this state
//----------------------------------------------------------------------------
//virtual
void lodModeLODTest::OnStateTestLOD()
{
		// debug
	//char text[64];
	//sprintf(text, "Mode: LOD State: Test" );
	//lodDebugInfo::SetDebugInfo(12, text);

	//inVirtualJoystick *pVJoy = inDeviceMgr::GetVirtualJoystick();
	//inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();

	////	check for compass picks
	//maPoint3d pos;
	//int currCompassPartSelected;
	//currCompassPartSelected = check_compass_picks( l_nCompassType, pos );

	//// compass manip
	//if ( l_bCompassManip && ( !pVJoy->IsHeld(lodVJoystick::e_LeftClick) ) )
	//{
	//	l_bCompassManip = false;

	//	show_compassparts( m_nSavedCompassParts );
	//}
	//if ( currCompassPartSelected != -1 )
	//{
	//	if ( is_left_click(pVJoy) )
	//	{
	//		//	start the manip
	//		l_bCompassManip = true;
	//		l_nCompassPart = currCompassPartSelected;

	//		// save the current state of the compass so we can restore it later
	//		SetSavedCompassParts( cmpsCompassMgr::GetParts(l_nCompassType) );
	//		show_compassparts( l_nCompassPart );

	//		SetMouseDownPoint( pos );

	//		//
	//		m_OriginalOrientation = l_pSelectedObject->GetOrientation();

	//		int dummy;
	//		tma3dCursorMgr::GetCursorPos( m_StartRotationValue, dummy );

	//		cmpsCompassMgr::SetOrientation( cmpsCompassMgr::e_TestLOD, m_OriginalOrientation );
	//	}
	//}

	////	if the compass is being manipulated
	////
	//if ( l_bCompassManip )
	//{
	//	// Get the difference between the two points
	//	int dummy;
	//	int cur_pos;
	//	tma3dCursorMgr::GetCursorPos(cur_pos, dummy);

	//	maRotation rot = get_rotation_from_direction( l_pSelectedObject->GetOrientation(),
	//			float(cur_pos - m_StartRotationValue) * lc_fTestLODIncrement, l_nCompassPart );

	//	sprintf(text, "start rot(%6.3f), curpos( %6.3f)", m_StartRotationValue, cur_pos );
	//	lodDebugInfo::SetDebugInfo(20, text);

	//	m_StartRotationValue = cur_pos;

	//	rot *= l_pSelectedObject->GetOrientation();
	//	l_pSelectedObject->UpdateOrientation( rot );

	//	cmpsCompassMgr::SetOrientation( cmpsCompassMgr::e_TestLOD, rot  );

	//		//debug
	//	sprintf(text, "new rot(%6.3f,%6.3f,%6.3f)", rot.GetX(), rot.GetY(), rot.GetZ()  );
	//	lodDebugInfo::SetDebugInfo(21, text);
	//	sprintf(text, "obj rot(%6.3f,%6.3f,%6.3f)", l_pSelectedObject->GetOrientation().GetX(), l_pSelectedObject->GetOrientation().GetY(), l_pSelectedObject->GetOrientation().GetZ()  );
	//	lodDebugInfo::SetDebugInfo(22, text);
	//}
	//else if ( ( pVJoy->IsPressed( lodVJoystick::e_RightClick ) && pVJoy->IsHeld( lodVJoystick::e_LeftClick ) )
	//		||
	//		  pKeyboard->IsPressed( inKeys::e_ESC ) )
	//{
	//	// User aborted moving
	//	l_pSelectedObject->UpdateOrientation( m_OriginalOrientation );
	//	cmpsCompassMgr::SetOrientation( cmpsCompassMgr::e_Movement, m_OriginalOrientation  );
	//}
}

//----------------------------------------------------------------------------
//	EndStateTestLOD - is executed ONCE on ending the state
//----------------------------------------------------------------------------
//virtual
void lodModeLODTest::EndStateTestLOD()
{
}

