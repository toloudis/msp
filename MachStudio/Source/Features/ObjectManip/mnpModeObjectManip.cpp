/*****************************************************************************
**	mnpModeObjectManip.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/ObjectManip/mnpModeObjectManip.hpp"
#include "Features/ObjectManip/mnpConstants.hpp"
#include "Features/ObjectManip/mnpFreeFormInteraction.hpp"
#include "Features/ObjectManip/mnpPlacementInteraction.hpp"
#include "Features/ObjectManip/mnpRotateInteraction.hpp"
#include "Features/ObjectManip/mnpScaleInteraction.hpp"
#include "Features/ObjectManip/mnpTranslatePlaneInteraction.hpp"
#include "Features/ObjectManip/mnpTranslateVectorInteraction.hpp"

#include "Features/FilmGates/fgtFrameMgr.hpp"
#include "Features/MaterialBrowse/mbrwPaintUtil.hpp"
#include "Features/Prefs/PrefsData.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Features/RenderPanels/rpnRenderingPrefs.hpp"
#include "MainApp/mnmApp.hpp"
#include "Support/brsh/brshPaintBrushMgr.hpp"
#include "Support/cams/camsFollowUtil.hpp"
#include "Support/cmps/cmpsCompassMgr.hpp"
#include "Support/cmps/cmpsSelectMgr.hpp"
#include "Support/fgmt/fgmtSelectInterest.hpp"
#include "Support/mnm/mnmAppUtil.hpp"
#include "Support/mnm/mnmAutoSaveMgr.hpp"
#include "Support/mnm/mnmCompassUtil.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmDebugInfo.hpp"
#include "Support/mnm/mnmObject.hpp"
#include "Support/mnm/mnmPickMask.hpp"
#include "Support/mnm/mnmVJoystick.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Support/mtrl/mtrlSelectInterest.hpp"
#include "Support/rprf/rprfPrefsUtil.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/vis/visMgr.hpp"

#include "Core/app/appSimTime.hpp"
#include "Core/app/appTime.hpp"
#include "Core/fs/private/fsFileNotifyMgr.hpp"
#include "Core/geo/geoPickRay.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Graphics/G3d/g3dPickInfo.hpp"
#include "Graphics/G3d/g3dScene.hpp"
#include "Input/in/inDeviceMgr.hpp"
#include "Input/in/inVirtualJoystick.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "Tool/gui/guiToolbarMgr.hpp"
#include "Tool/icn/icnIconLayer.hpp"
#include "Tool/icn/icnIconScale.hpp"
#include "Tool/pick3d/pick3dMgr.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"
#include "Tool/tma3d/tma3dCursorMgr.hpp"
#include "Tool/tma3d/tma3dRenderView.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"

#if(SGPU_APP == MS_FUSION) //#ifdef FUSION
#include "FCSupport/fcui/fcuiFormMgr.hpp"
#endif

#include <windows.h>
#include <time.h>


//============================================================================
//============================================================================
namespace
{
	const char* MENUITEM_OBJECTS			= "Objects";	// TODO - FIX - these must be exactly the same as in mnpCommands
	const char* MENUITEM_OBJECTS_TRANSLATE	= "Translate";
	const char* MENUITEM_OBJECTS_ROTATE		= "Rotate";
	const char* MENUITEM_OBJECTS_SCALE		= "Scale";
	const char* MENUITEM_OBJECTS_SELECT		= "Select";
	const char* MENUITEM_OBJECTS_PLACEMENT	= "Placement";

	const float lc_fFocusRadius	= 10.0f;

	const maVector3d lc_RotateX( 1, 0, 0 );
	const maVector3d lc_RotateY( 0, 1, 0 );
	const maVector3d lc_RotateZ( 0, 0, 1 );

	mnmObject* l_pSelectedObject = 0;
	mnmObject* l_pSelectedObjectLast = 0;

	bool l_bCompassManip = false;
	int	 l_nCompassType = 0;
	int  l_nCompassPart = 0;
	float l_fCompassScalePercent = 1.0f;
	float l_cfCOMPASSSCALEPERCENT_MAX = 3.0f;
	float l_cfCOMPASSSCALEPERCENT_INC = 0.05f;

	bool l_bDirty = false;

	bool l_bLocked = false;

	bool l_bPickChanged = false;

	bool l_bLocalTranslation = true;
	bool l_bFixedTranslationOrientation = false;
	maRotation l_TranslateOrientation;


	//pick3dPickList l_PickList;

	mnpModeObjectManip::ObjectManipStates l_CurrentManipState;

	//
	//	compass parts
	//

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void show_compassparts( int i_PartsToShow )
	{
		cmpsCompassMgr::SetParts( l_nCompassType, i_PartsToShow );
	}

	//
	//
	//

	//------------------------------------------------------------------------
	// get_rotation_axis returns world space rotation axis from
	//	base orientation
	//------------------------------------------------------------------------
	maVector3d get_rotation_axis(const maRotation& i_CurrentOrientation,
								 int i_nCompassPart)
	{
		maVector3d rot_axis(0,1,0);
		switch( i_nCompassPart )
		{
			case cmpsCompass::e_X:
			{
				rot_axis = lc_RotateX;
				i_CurrentOrientation.RotateVector( rot_axis );
				break;
			}
			case cmpsCompass::e_Y:
			{
				rot_axis = lc_RotateY;
				i_CurrentOrientation.RotateVector( rot_axis );
				break;
			}
			case cmpsCompass::e_Z:
			{
				rot_axis = lc_RotateZ;
				i_CurrentOrientation.RotateVector( rot_axis );
				break;
			}
			case cmpsCompass::e_Center:
			case cmpsCompass::e_All:
			{
				break;
			}
			default:
			{
				DBG_ASSERT(false, "invalid rotation direction!");
				break;
			}
		}

		//char text[64];
		//sprintf(text, "rotation axis (%6.3f,%6.3f,%6.3f)", rot_axis.GetX(), rot_axis.GetY(), rot_axis.GetZ()  );
		//mnmDebugInfo::SetDebugInfo(22, text);

		return rot_axis;
	}

	//----------------------------------------------------------------------------
	//	returns true if left button pressed over view window
	//----------------------------------------------------------------------------
	bool is_left_click(inVirtualJoystick *i_pVJoy)
	{
		return ( i_pVJoy->IsPressed(mnmVJoystick::e_LeftClick) &&
				 tma3dCursorMgr::IsCursorOverView() );
	}

	//----------------------------------------------------------------------------
	// Split out mouse down and up events so that picking only happens
	// when the event is confirmed.
	//----------------------------------------------------------------------------
	bool is_left_pressed(inVirtualJoystick *i_pVJoy)
	{
		return ( i_pVJoy->IsPressed(mnmVJoystick::e_LeftClick) &&
				 tma3dCursorMgr::IsCursorOverView() );
	}
	bool is_left_released(inVirtualJoystick *i_pVJoy)
	{
		return ( i_pVJoy->IsReleased(mnmVJoystick::e_LeftClick) &&
				 tma3dCursorMgr::IsCursorOverView() );
	}

	//----------------------------------------------------------------------------
	//	returns true if the tablet pen is down
	//----------------------------------------------------------------------------
	bool is_pen_pressed(inTablet *i_pTablet)
	{
		if(!i_pTablet->GetInkEnabled())
			return false;
		return i_pTablet->IsCursorDown();
	}

	void get_tablet_position(int &o_x, int &o_y)
	{
		inTablet* pTablet = inDeviceMgr::GetTablet();
		if(pTablet->GetInkEnabled() && pTablet->IsCursorDown())
			pTablet->GetPosition(o_x, o_y);
	}

	//----------------------------------------------------------------------------
	//	run a pick operation at the current cursor pos in current render view
	//----------------------------------------------------------------------------
	void DoPickAtCursor(g3dPickInfo& o_PickInfo, 
						bool i_bTablet = false, 
						envType::UInt32 i_PickMask = mnmPickMask::c_NoFilter)
	{
		// stop any render threads while doing GPU picking
		gpxRenderControl::ConfirmSingleThread();

		// GPU Pick technique
		tma3dRenderView *pRenderView = tma3dRenderView::GetActiveRenderView();
		int x=0, y=0;
		
		if(!i_bTablet)
			tma3dCursorMgr::GetCursorPos(x,y);
		else
			get_tablet_position(x,y);
		//DBG_TRACE("pick at (" << x << "," << y << ")  " << i_bTablet);

		pRenderView->DoPickRender(x,y, tmlnTimeLine::GetTimeInSeconds(), o_PickInfo, i_PickMask);

		// If the user clicks on the empty viewport, set the depth based on the camera target position.
		if(o_PickInfo.m_Depth == 0.0)
		{
			o_PickInfo.m_Depth = pRenderView->GetCameraDepth();
		}
	}

	//----------------------------------------------------------------------------
	//	check to see if there are any picks under the cursor,
	//	returns sel3dObject* for the selectable object picked or
	//  NULL if nothing picked
	//----------------------------------------------------------------------------
	sel3dObject* check_for_picks(mnpModeObjectManip::PickMode i_PickMode)
	{
/*	Pick ray technique	
		// fill in the ray
		geoPickRay ray;
		maVector3d dir;
		maPoint3d start;

		tma3dCursorMgr::GetCursorRay( start, dir );
		ray.SetFromStartEnd( start, start + dir );

		//	do the pick
		return pick3dMgr::Pick( ray, l_PickList );*/

		envType::UInt32 pick_mask = mnmPickMask::c_NoFilter;
		if (i_PickMode == mnpModeObjectManip::e_PickMaterials)
			pick_mask = mnmPickMask::c_Material;
		if (i_PickMode == mnpModeObjectManip::e_PickSurfaces)
			pick_mask = mnmPickMask::c_Surface;

		g3dPickInfo pickInfo;
		DoPickAtCursor(pickInfo, false, pick_mask);
		//cam3dMgr::SetManipDepth(pickInfo.m_Depth);
		envType::UInt32 pick_code = pickInfo.m_ObjectID;
		
		//if(brshPaintBrushMgr::IsEnabled())
		//	brshPaintBrushMgr::Paint(pickInfo);

		pick3dPickObject *pPickedObject = pick3dMgr::MatchPickCode(pick_code);
		sel3dObject *pPicked = sel3dCastUtil::ConvertPickToSelection(pPickedObject);
	
		//char text[64];
		//sprintf(text, "GPU pick = %d %s", pick_code, pPicked ? pPicked->GetDisplayName().c_str() : "NULL" );
		//mnmDebugInfo::SetDebugInfo(10, text);

		// Add in a special case here, if we are picking an object that
		// was already selected and it is a material object, then use the
		// picking to update the selected material.
		//if (pPicked && pPicked == sel3dMgr::GetSelected())
		//
		// Changed to do the pick on every mouse pick, not on the second pick
		bool bPickedObjectPart = false;
		if (pPicked)
		{
			// Look for selected part if the pick mode asks for it
			if (i_PickMode == mnpModeObjectManip::e_PickMaterials)
			{
				// Pick material
				if (sel3dObject *pMaterialObj = mtrlSelectInterest::GetPartFromPickCode(pPicked, pick_code))
					return pMaterialObj;
			}

			if (i_PickMode == mnpModeObjectManip::e_PickSurfaces)
			{
				// Pick surface 
				if (sel3dObject *pSurfaceObj = fgmtSelectInterest::GetPartFromPickCode(pPicked, pick_code))
					return pSurfaceObj;
			}
		}

		// Return object pick or NULL
		return pPicked;
	}

	//----------------------------------------------------------------------------
	//	check a compass for intersection and return the part
	//----------------------------------------------------------------------------
	int check_compass_picks( int compassType, maPoint3d& o_Pos )
	{
		// fill in the ray
		geoPickRay ray;
		maVector3d dir;
		maPoint3d start;

		tma3dCursorMgr::GetCursorRay( start, dir );
		//ray.SetFromStartEnd( start, start + dir );
		ray.SetFromStartEnd( start, start - dir );

		//DBG_LOG( "------------------------" );
		//DBG_LOG3( " ray start(%6.3f,%6.3f,%6.3f)", start.GetX(), start.GetY(), start.GetZ() );
		//DBG_LOG3( " ray end  (%6.3f,%6.3f,%6.3f)", dir.GetX(), dir.GetY(), dir.GetZ() );

		//	do the pick
		maPoint3d pos;
		int part = -1;

		int current_icon_layer_index = icnIconLayer::GetActiveIconLayer();
		if ( cmpsCompassMgr::RayPick( compassType, current_icon_layer_index, start, start+dir, pos, part ) )
		{
			if ( !l_bCompassManip )
			{
				cmpsCompassMgr::HighlightPart( compassType, part );

				o_Pos = pos;
			}
		}
		else
		{
			if ( !l_bCompassManip )
				cmpsCompassMgr::HighlightPart( compassType, -1 );
		}

		return part;
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	mnmObject* cast_selected_to_mnmobject()
	{
		sel3dObject* pObj = sel3dMgr::GetSelected();
		if (!pObj) return NULL;
		return dynamic_cast<mnmObject*>( pObj );
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void set_compass_to_object( int i_nCompassType, mnmObject* i_pObj )
	{
		DBG_ASSERT( i_pObj != 0, "select compass cannot be set to a NULL object" );

		mnmCompassUtil::SetUpCompass( i_nCompassType, i_pObj, l_fCompassScalePercent );

		// If we are doing world space translation, then don't allow the
		// translation compass to rotate.
		if (i_nCompassType == cmpsCompassMgr::e_Translate)
		{
			if (l_bFixedTranslationOrientation)
				// Keep orientation fixed while dragging the mouse
				cmpsCompassMgr::SetOrientation( i_nCompassType, l_TranslateOrientation);
			else if (!l_bLocalTranslation)
				// Set orientation to identity to keep compass in world space axes
				cmpsCompassMgr::SetOrientation( i_nCompassType, maRotation());
		}

		//cmpsCompass& compass = cmpsCompassMgr::GetCompass(l_nCompassType);
		//const maPoint3d& scale = compass.GetScale();
		//maPoint3d newscale = scale * l_fCompassScalePercent;
		//compass.SetScale(newscale);
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void set_compass_to_list( int i_nCompassType, const std::list<sel3dObject*>& i_Objects )
	{
		mnmCompassUtil::SetUpCompass( i_nCompassType, i_Objects, l_fCompassScalePercent );

		// If we are doing world space translation, then don't allow the
		// translation compass to rotate.
		if (!l_bLocalTranslation && i_nCompassType == cmpsCompassMgr::e_Translate)
		{
			if (l_bFixedTranslationOrientation)
				// Keep orientation fixed while dragging the mouse
				cmpsCompassMgr::SetOrientation( i_nCompassType, l_TranslateOrientation);
			else if (!l_bLocalTranslation)
				// Set orientation to identity to keep compass in world space axes
				cmpsCompassMgr::SetOrientation( i_nCompassType, maRotation());
		}

		//cmpsCompass& compass = cmpsCompassMgr::GetCompass(l_nCompassType);
		//const maPoint3d& scale = compass.GetScale();
		//maPoint3d newscale = scale * l_fCompassScalePercent;
		//compass.SetScale(newscale);
	}
	

	//--------------------------------------------------------------------
	// the callback function for menu item: Translate
	//--------------------------------------------------------------------
	//void menuitem_click_Translate( int i_ObjectID )
	//{
	//	modeMode* pMode = modeModeMgr::GetCurrentMode();

	//	//DBG_LOG2( "mode id %d (%s)", pMode->GetID(), pMode->GetMenuItemName() );

	//	mnpModeObjectManip* pModeOM = dynamic_cast<mnpModeObjectManip*>(pMode);
	//	if ( pModeOM != 0 )
	//	{
	//		pModeOM->SetStateTranslate();
	//	}
	//}

	//--------------------------------------------------------------------
	// the callback function for menu item: Rotate
	//--------------------------------------------------------------------
	void menuitem_click_Rotate( int i_ObjectID )
	{
		modeMode* pMode = modeModeMgr::GetCurrentMode();
		mnpModeObjectManip* pModeOM = dynamic_cast<mnpModeObjectManip*>(pMode);
		if ( pModeOM != 0 )
		{
			pModeOM->SetStateRotate();
		}
	}

	//--------------------------------------------------------------------
	// the callback function for menu item: Scale
	//--------------------------------------------------------------------
	void menuitem_click_Scale( int i_ObjectID )
	{
		modeMode* pMode = modeModeMgr::GetCurrentMode();
		mnpModeObjectManip* pModeOM = dynamic_cast<mnpModeObjectManip*>(pMode);
		if ( pModeOM != 0 )
		{
			pModeOM->SetStateScale();
		}
	}

	//--------------------------------------------------------------------
	// the callback function for menu item: Select
	//--------------------------------------------------------------------
	void menuitem_click_Select( int i_ObjectID )
	{
		modeMode* pMode = modeModeMgr::GetCurrentMode();
		mnpModeObjectManip* pModeOM = dynamic_cast<mnpModeObjectManip*>(pMode);
		if ( pModeOM != 0 )
		{
			pModeOM->SetStateSelect();
		}
	}

	//--------------------------------------------------------------------
	// the callback function for menu item: Placement
	//--------------------------------------------------------------------
	void menuitem_click_Placement( int i_ObjectID )
	{
		modeMode* pMode = modeModeMgr::GetCurrentMode();
		mnpModeObjectManip* pModeOM = dynamic_cast<mnpModeObjectManip*>(pMode);
		if ( pModeOM != 0 )
		{
			pModeOM->SetStatePlacement();
		}
	}

	//--------------------------------------------------------------------
	//	menu item functions
	//--------------------------------------------------------------------
	void menu_items_enable_for_selected_object()
	{
		//	set the object manipulation buttons based on the current
		//	selected object's manipulation flags.
		//
		if ( l_pSelectedObject->GetTranslateFlags() != mnmObject::e_TranslateNone )
		{
			guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_TRANSLATE, true );
			guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_PLACEMENT, true );
		}
		else
		{
			guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_TRANSLATE, false );
			guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_PLACEMENT, false );
		}

		if ( l_pSelectedObject->GetRotateFlags() != mnmObject::e_RotateNone )
			guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_ROTATE, true );
		else
			guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_ROTATE, false );

		if ( l_pSelectedObject->GetScaleFlags() != mnmObject::e_ScaleNone )
			guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_SCALE, true );
		else
			guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_SCALE, false );

		guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_SELECT, true );
	}
	void menu_items_enable()
	{
		guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_TRANSLATE, true );
		guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_ROTATE, true );
		guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_SCALE, true );
		guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_SELECT, true );
		guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_PLACEMENT, true );
	}
	void menu_items_disable()
	{
		guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_TRANSLATE, false );
		guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_ROTATE, false );
		guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_SCALE, false );
		guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_SELECT, false );
		guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_PLACEMENT, false );
	}
	//void menu_items_add()
	//{
	//	int objectID;
	//	objectID = guiMenuMgr::AddMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_TRANSLATE, true, "edit-translate.png" );
	//	guiMenuMgr::AttachEventToMenuObjects( objectID, menuitem_click_Translate );
	//	objectID = guiMenuMgr::AddMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_ROTATE, true, "edit-rotate.png" );
	//	guiMenuMgr::AttachEventToMenuObjects( objectID, menuitem_click_Rotate );
	//	objectID = guiMenuMgr::AddMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_SCALE, true, "edit-scale.png" );
	//	guiMenuMgr::AttachEventToMenuObjects( objectID, menuitem_click_Scale );
	//	objectID = guiMenuMgr::AddMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_SELECT, true, "edit-select.png" );
	//	guiMenuMgr::AttachEventToMenuObjects( objectID, menuitem_click_Select );
	//	objectID = guiMenuMgr::AddMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_PLACEMENT, true, "edit-placement.png" );
	//	guiMenuMgr::AttachEventToMenuObjects( objectID, menuitem_click_Placement );
	//}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void selected_object_clear()
	{
		//l_PickList.Clear();
		l_pSelectedObject = 0;

		cmpsCompassMgr::SetRenderable( l_nCompassType, false );

		menu_items_disable();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void selected_object_set()
	{
		// NOTE: assumes l_pSelectedObject has been set

		//char text[64];
		//sprintf(text, "object selected (mnmObject type)" );	//debug
		//mnmDebugInfo::SetDebugInfo(13, text);	//debug

		l_nCompassType = cmpsCompassMgr::e_Select;

		set_compass_to_object( l_nCompassType, l_pSelectedObject );

		menu_items_enable_for_selected_object();
	}

	//--------------------------------------------------------------------
	// Translate objects in the selected list that aren't the focus
	// of the translation compass.
	//--------------------------------------------------------------------
	void apply_delta_position(const maVector3d& i_Delta, bool i_bNewOperation)
	{
		const std::list<sel3dObject*> &selected_list = sel3dMgr::GetSelectedList();
		std::list<sel3dObject*>::const_iterator it, end = selected_list.end();
		for (it = selected_list.begin(); it != end; ++it)
		{
			// skip first one, it is receiving the translation from the compass
			if (it != selected_list.begin())
			{
				if (mnmObject* pObject = dynamic_cast<mnmObject*>(*it))
				{
					pObject->UpdatePosition(pObject->GetPosition() + i_Delta, i_bNewOperation);
				}
			}
		}
	}

	//--------------------------------------------------------------------
	// construct a 3x3 matrix for the given orientation and scale values
	//--------------------------------------------------------------------
	maMatrix3x3 construct_matrix(const maRotation& i_Orientation, 
								 const maVector3d &i_Scale)
	{
		maMatrix3x3 xform = i_Orientation.GetMatrix3x3();
		xform(0, 0) *= i_Scale.m_X;
		xform(0, 1) *= i_Scale.m_X;
		xform(0, 2) *= i_Scale.m_X;
		xform(1, 0) *= i_Scale.m_Y;
		xform(1, 1) *= i_Scale.m_Y;
		xform(1, 2) *= i_Scale.m_Y;
		xform(2, 0) *= i_Scale.m_Z;
		xform(2, 1) *= i_Scale.m_Z;
		xform(2, 2) *= i_Scale.m_Z;
		return xform;
	}

	
}



//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mnpModeObjectManip::mnpModeObjectManip()
:	m_bInitialized( false ),
	m_bAdded( false ),
	m_bClickStarted( false ),
	m_PickMode(e_PickObjects)
{
	SetMenuItemName( "Object Manip" );

	//	set-up the state call-backs
	m_StateTranslate.SetState( this, &mnpModeObjectManip::BeginStateTranslate, &mnpModeObjectManip::OnStateTranslate, &mnpModeObjectManip::EndStateTranslate );
	m_StateTranslateFreeForm.SetState( this, &mnpModeObjectManip::BeginStateTranslateFreeForm, &mnpModeObjectManip::OnStateTranslateFreeForm, &mnpModeObjectManip::EndStateTranslateFreeForm );
	m_StateRotate.SetState( this, &mnpModeObjectManip::BeginStateRotate, &mnpModeObjectManip::OnStateRotate, &mnpModeObjectManip::EndStateRotate );
	m_StateScale.SetState( this, &mnpModeObjectManip::BeginStateScale, &mnpModeObjectManip::OnStateScale, &mnpModeObjectManip::EndStateScale );
	m_StateSelect.SetState( this, &mnpModeObjectManip::BeginStateSelect, &mnpModeObjectManip::OnStateSelect, &mnpModeObjectManip::EndStateSelect );
	m_StatePlacement.SetState( this, &mnpModeObjectManip::BeginStatePlacement, &mnpModeObjectManip::OnStatePlacement, &mnpModeObjectManip::EndStatePlacement );

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// virtual
mnpModeObjectManip::~mnpModeObjectManip()
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
void mnpModeObjectManip::Initialize()
{
	if ( !m_bInitialized )
	{
		if ( m_bAdded )
		{
			menu_items_enable();
		}
		else
		{
			//menu_items_add();
			m_bAdded = true;
		}

		//SetStateTranslate();	// start state
		SetStateSelect();		// start state
	}

	m_bInitialized = true;

	mnmAppUtil::UpdateTitleBar(false);
	set_current_mode_text();

	//	set-up the auto-save mgr
	PrefsData& data = PrefsMgr::Data();
	mnmAutoSaveMgr::Initialize( data.m_bAutoSave.GetValue(), data.m_AutoSaveFrequencyInMinutes.GetValue(), data.m_AutoSaveBackups.GetValue() );

	visMgr::ShowIcons( visMgr::IsShowIcons()?true:false);
	fgtFrameMgr::Show(data.m_bActionSafeFrameVisible.GetValue(), data.m_bTitleSafeFrameVisible.GetValue());
}

//----------------------------------------------------------------------------
//	The object should clean up things that are not needed while the
//	mode is not running in the DeInitialize function.
//----------------------------------------------------------------------------
//virtual
void mnpModeObjectManip::DeInitialize()
{
	if ( m_bInitialized )
	{
		//	on leaving the mode don't remove the menu items for this mode,
		//	just disable them.
		//
		menu_items_disable();

		// clear selection when leaving this mode
		//selected_object_clear();
		cmpsCompassMgr::SetRenderable( l_nCompassType, false );

		mnmAutoSaveMgr::DeInitialize();

		rpnRenderingPrefs::RestoreDefaults();
		rpnRenderingPrefs::SetupRenderingHints();
	}

	m_bInitialized = false;
}


//----------------------------------------------------------------------------
//	The mode should do it's per frame "work" in the Think function.
//----------------------------------------------------------------------------
// virtual
void mnpModeObjectManip::Think()
{
	// update sim time to match our current value (no time flowing though)
	appSimTime::SetTime(tmlnTimeLine::GetTimeInSeconds(), 0.0f);

	// FIX: (?) we need to set this every frame since some other part of the program
	//	could delete the selected object. grrr.
	//
	if ( l_pSelectedObjectLast != l_pSelectedObject )
	{
		l_pSelectedObjectLast = l_pSelectedObject;
	}
	l_pSelectedObject = cast_selected_to_mnmobject();

	if ( ( l_pSelectedObject == 0 ) && ( l_pSelectedObjectLast != 0 ) )
	{
		selected_object_clear();
		SetStateSelect(false);
	}

	//rprfPrefsUtil::UpdateDialogVP();
	// Before we check for the mouse passing over compasses and do
	// GPU picking, we need to make sure that all of the icons that
	// scale based on cameras are correctly scaled based on the
	// camera we will picking with
	//int screen_width = (int)(tma3dScreenUtil::GetWindowSize().GetX());
	//icnIconScale::UpdateIconsScale( cam3dMgr::GetCamera(),  screen_width);

	// Update scale of compasses for this view also
	//int current_icon_layer_index = icnIconLayer::GetActiveIconLayer();
	//cmpsCompassMgr::ResizeCompasses( cam3dMgr::GetCamera(), current_icon_layer_index );

	// Update transforms in icon layer since we have been changing the
	// scales of the icons above. This could maybe just be built into the
	// GPU pick action, but I want to be safe for now.
	//api3dScene::GetScene()->UpdateLayerData(mnmApp::GetIconsLayerIndex());

	//	update the state
	//
	UpdateState();

	//
	inVirtualJoystick *pVJoy = inDeviceMgr::GetVirtualJoystick();
	inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();
	inTablet* pTablet = inDeviceMgr::GetTablet();

			// debug
	//char text[64];
	//sprintf(text, "compass: part %d  manip %s", l_nCompassPart, ( l_bCompassManip ? "true":"false" ) );
	//mnmDebugInfo::SetDebugInfo(11, text);
	//sprintf(text, "pick size = %d", l_PickList.GetSize() );
	//mnmDebugInfo::SetDebugInfo(10, text);
	//sprintf(text, "state = %s", ( (l_CurrentManipState==e_StateSelect)?"Select":"Other") );
	//mnmDebugInfo::SetDebugInfo(16, text);

	//sprintf(text, "Cursor Over View = %s", tma3dCursorMgr::IsCursorOverView()?"true":"false");
	//mnmDebugInfo::SetDebugInfo(8, text);
	//sprintf(text, "mnmVJoystick::e_LeftClick = %s", pVJoy->IsPressed(mnmVJoystick::e_LeftClick)?"true":"false");
	//mnmDebugInfo::SetDebugInfo(9, text);

	if(l_bPickChanged)
	{
		guiToolbarMgr::Show(mnpConstants::mc_Toolbar_PickMode_Name, true);
		l_bPickChanged = false;
	}

#if(SGPU_APP == MS_FUSION) //#ifdef FUSION
	if( !fcuiFormMgr::MainHasFocus() )
		return;
#endif
	//if ( tma3dCursorMgr::IsCursorOverView() )
	//{
	//	// If we are dragging a material, do picking of material continuously.
	//	if (mbrwPaintUtil::GetDoAutoPickMaterial())
	//	{
	//		check_for_picks(mnpModeObjectManip::e_PickMaterials);
	//	}
	//}

	//	if in a compass manip, don't select other things
	//	if not, get the pick list
	//
	if ( !l_bCompassManip )
	{
		// A mouse click triggers selection only if the mouse does not move
		// while the button is down.
		if ( m_bClickStarted )
		{
			// If mouse moves away from mouse down event, get out of the waiting state.
			if (!tma3dCursorMgr::IsCursorOverView())
			{
				m_bClickStarted = false;
			}
			else
			{
				const int c_PixelMoveThreshold = 3;
				int mupX = 0, mupY = 0;
				tma3dCursorMgr::GetCursorPos(mupX, mupY);
				if ((abs(mupX-m_MouseDownX) > c_PixelMoveThreshold) ||
					(abs(mupY-m_MouseDownY) > c_PixelMoveThreshold))
				{
					m_bClickStarted = false;
				}
			}
		}

		// ALT and CTRL are camera events
		if ( !(   pKeyboard->IsDown(inKeys::e_LALT) 
			   || pKeyboard->IsDown(inKeys::e_RALT)
			   || pKeyboard->IsDown(inKeys::e_RCTRL)
			   || pKeyboard->IsDown(inKeys::e_LCTRL)) )
		{
			// check to see if there is something we are selecting.
			//was ... if ( is_left_click(pVJoy) )

			// Selection picks only happen when mouse down and mouse up are
			// within a few pixels of each other.
			if ( is_left_pressed(pVJoy) )
			{
				m_bClickStarted = true;
				tma3dCursorMgr::GetCursorPos(m_MouseDownX, m_MouseDownY);
			}
			if ( m_bClickStarted )
			{
				if (is_left_released(pVJoy))
				{
					m_bClickStarted = false;
					
					// Now do selection change based on picking
					bool bAppendSelection = (  pKeyboard->IsDown(inKeys::e_LSHIFT) 
											|| pKeyboard->IsDown(inKeys::e_RSHIFT) );
					if ( sel3dObject* pPickedObj = check_for_picks(m_PickMode) )
					{
						// Check SHIFT key, which means "add to multiple selection"
						sel3dMgr::CreateUndoOperation();
						if (bAppendSelection)
							sel3dMgr::AddToSelection( pPickedObj );
						else
							sel3dMgr::Select( pPickedObj );

						set_current_state();
					}
					else
					{
						//SetStateSelect(false);

						// Empty pick, clear selection
						//bga - This can only be done if coordinated with the changing
						// focus between render panels? 
						//sel3dMgr::CreateUndoOperation();
						//sel3dMgr::ClearSelection();
					}
				}
				//else if (!pVJoy->IsHeld(mnmVJoystick::e_LeftClick))
				//{
				//	// If we missed the mouse up event somehow, clear out the
				//	// waiting state.
				//	m_bClickStarted = false;
				//}
			}
#ifdef ENABLE_PAINT
			//Check for tablet input
			if( is_pen_pressed(pTablet) )
			{
				g3dPickInfo pickInfo;
				DoPickAtCursor(pickInfo, true);
				int pressure = 0;
				pTablet->GetPressure(pressure);

				bool erase = false;
				pTablet->GetErase(erase);

				if(brshPaintBrushMgr::IsEnabled())
					brshPaintBrushMgr::Paint(pickInfo, pressure, erase);
			}
			if(brshPaintBrushMgr::IsEnabled())
			{
				brshPaintBrushMgr::DialogCheck();
			}
#endif
			//else
			//if ( pVJoy->IsPressed( mnmVJoystick::e_RightClick ) ) // && !pVJoy->IsPressed( mnmVJoystick::e_LeftClick ) )
			//{
			//	sel3dMgr::CreateUndoOperation();
			//	sel3dMgr::ClearSelection();
			//	selected_object_clear();
			//	SetStateSelect(false);
			//}
		}
		else
		{
			if ( is_left_click(pVJoy) )
			{
				g3dPickInfo pickInfo;
				DoPickAtCursor(pickInfo);
				//cam3dMgr::SetManipDepth(pickInfo.m_Depth);
			}
		}
		
	}

	if ( pKeyboard )
	{
		//
		// Check for input
		//

		//	make sure the Control key isn't held while hitting these keys.
		//
		//if (   !pKeyboard->IsDown(inKeys::e_RCTRL)
		//	&& !pKeyboard->IsDown(inKeys::e_LCTRL))
		//{
		//}
	}

	//	check if the app title bar needs to be updated.
	//
	if ( l_bDirty && !docSingleDocumentMgr::NeedsSave() )
	{
		l_bDirty = false;
		mnmAppUtil::UpdateTitleBar( l_bDirty );
	}
	else if ( !l_bDirty && docSingleDocumentMgr::NeedsSave() )
	{
		l_bDirty = true;
		mnmAppUtil::UpdateTitleBar( l_bDirty );
	}

	//	AutoSave
	//
	mnmAutoSaveMgr::Think();
	
	bool EnableRevision = FileUpdated::GetEnableRevisionFlag();

	if(EnableRevision)
	{
		time_t epochTime;
		epochTime = time(NULL);
		fsFileNotifyMgr::Check( epochTime ); // This checks for revision/update of existing objects.
//		fsFileNotifyMgr::CheckForNewObjects(); // This checks if new objects are added.
	}
		// Set up rendering preferences based on whether we are scrubbing the timeline
	bool bForceLowRes = (tmlnTimeLine::GetIsScrubbing() 
		&& PrefsMgr::Data().m_bScrubbingLowRes.GetValue());
	rpnRenderingPrefs::SetAllLowResolution( bForceLowRes );

	bool bForceFastRender = (tmlnTimeLine::GetIsScrubbing() 
		&& PrefsMgr::Data().m_bScrubbingFastRender.GetValue());
	rpnRenderingPrefs::SetAllFastRender( bForceFastRender );

	rpnRenderingPrefs::SetupRenderingHints();

	// By having the base class think go last, the drivers 
	// and other things that have registered Think() interests
	// get a chance to execute after the compass has changed things.
	modeModeTime::Think();


	// Position the compass to the bounds of the selection
	// after the mode think so that all objects are in their place now.
	//
	if (e_StateRotate == l_CurrentManipState)
	{
		// For rotation, orient the sphere so that it surrounds all of the objects
		//	that are selected.
		set_compass_to_list( l_nCompassType, sel3dMgr::GetSelectedList() );

		cmpsSelectMgr::SetNumBoxes(0); // Turn off other highlights
	}
	else
	{
		//Note: The selected object might have changed in the call to modeModeTime::Think() above.
		// One way that might have happened is if the selected object was reloaded
		// or had its filename changed. So, get a new pointer to the selected object 
		// for the next couple of operations with the compass.
		mnmObject *pCurSelectedObject = cast_selected_to_mnmobject();

		// Otherwise, put the compass only on the most recent selection and
		//	put highlight boxes on the others.
		if ((pCurSelectedObject != NULL) && (pCurSelectedObject == l_pSelectedObject))
		{
			set_compass_to_object( l_nCompassType, pCurSelectedObject );
		}

		// Put highlight boxes around selected objects
		mnmCompassUtil::HighlightSelected(sel3dMgr::GetSelectedList(), l_fCompassScalePercent);
	}
}

//--------------------------------------------------------------------
// PickMode - does a mouse click pick the object, material or surface
//--------------------------------------------------------------------
mnpModeObjectManip::PickMode mnpModeObjectManip::GetPickMode() const
{
	return m_PickMode;
}
void mnpModeObjectManip::SetPickMode(mnpModeObjectManip::PickMode i_PickMode)
{
	if(l_bLocked)
		return;
	m_PickMode = i_PickMode;
}

//--------------------------------------------------------------------
// Pick Mode Toolbar - Sets boolean for toolbar pick mode state change
//--------------------------------------------------------------------
void mnpModeObjectManip::SetPickModeChanged(bool i_bPickChanged)
{
	l_bPickChanged = i_bPickChanged;
}
bool mnpModeObjectManip::GetPickModeChanged()
{
	return l_bPickChanged;
}

//--------------------------------------------------------------------
//	SetCompassParts sets the parts of the compass that we are
//	currently using.
//--------------------------------------------------------------------
void mnpModeObjectManip::SetCompassPart(const int i_Parts)
{
	m_nCompassPart = i_Parts;

	show_compassparts( i_Parts );
}

//--------------------------------------------------------------------
//	change the scale of the compass
//--------------------------------------------------------------------
void mnpModeObjectManip::IncrementScaleOfCompass()
{
	l_fCompassScalePercent += l_cfCOMPASSSCALEPERCENT_INC;
	if (l_fCompassScalePercent > l_cfCOMPASSSCALEPERCENT_MAX) l_fCompassScalePercent = l_cfCOMPASSSCALEPERCENT_MAX;
}
void mnpModeObjectManip::DecrementScaleOfCompass()
{
	l_fCompassScalePercent -= l_cfCOMPASSSCALEPERCENT_INC;
	if (l_fCompassScalePercent < 0.0f) l_fCompassScalePercent = 0.0f;
}

//--------------------------------------------------------------------
//	Flip the compass
//--------------------------------------------------------------------
void mnpModeObjectManip::ToggleCompassFlipState()
{
	cmpsCompassMgr::ToggleCompassFlip();
}

//--------------------------------------------------------------------
//	Local space translation
//--------------------------------------------------------------------
void mnpModeObjectManip::SetLocalSpaceTranslation(bool i_bLocalSpace)
{
	l_bLocalTranslation = i_bLocalSpace;
}
bool mnpModeObjectManip::IsLocalSpaceTranslation()
{
	return l_bLocalTranslation;
}

//--------------------------------------------------------------------
//	move the timeline
//--------------------------------------------------------------------
//void mnpModeObjectManip::TimelineIncrementFrame()
//{
//	//	increment time by one frame
//	float newtime = tmlnTimeLine::GetValue() + tmlnTimeLine::GetFrameIncrement();
//	if ( newtime <= tmlnTimeLine::GetMaximum() )
//		tmlnTimeLine::SetValue( newtime );
//}
//void mnpModeObjectManip::TimelineDecrementFrame()
//{
//	//	decrement time by one frame
//	float newtime = tmlnTimeLine::GetValue() - tmlnTimeLine::GetFrameIncrement();
//	if ( newtime >= tmlnTimeLine::GetMinimum() )
//		tmlnTimeLine::SetValue( newtime );
//}

//----------------------------------------------------------------------------
//	Set States (externally)
//----------------------------------------------------------------------------
void mnpModeObjectManip::SetStateTranslate()
{
	if ( ( l_pSelectedObject != 0 ) && (l_pSelectedObject->GetTranslateFlags() != mnmObject::e_TranslateNone) )
	{
		l_CurrentManipState = e_StateTranslate;
		GotoState( m_StateTranslate );
	}
}
void mnpModeObjectManip::SetStateTranslateFreeForm()
{
	if ( ( l_pSelectedObject != 0 ) && (l_pSelectedObject->GetTranslateFlags() != mnmObject::e_TranslateNone) )
	{
		//l_CurrentManipState = e_StateTranslate;
		GotoState( m_StateTranslateFreeForm );
	}
}
void mnpModeObjectManip::SetStateRotate()
{
	if ( ( l_pSelectedObject != 0 ) && (l_pSelectedObject->GetRotateFlags() != mnmObject::e_RotateNone) )
	{
		l_CurrentManipState = e_StateRotate;
		GotoState( m_StateRotate );
	}
}
void mnpModeObjectManip::SetStateScale()
{
	if ( ( l_pSelectedObject != 0 ) && (l_pSelectedObject->GetScaleFlags() != mnmObject::e_ScaleNone) )
	{
		l_CurrentManipState = e_StateScale;
		GotoState( m_StateScale );
	}
}
void mnpModeObjectManip::SetStateSelect( bool i_bUpdateCurrentState /*= true*/ )
{
	if(l_bLocked)
		return;
	if ( i_bUpdateCurrentState )
	{
		l_CurrentManipState = e_StateSelect;
		//DBG_LOG("--------> select");
	}
	GotoState( m_StateSelect );
}
void mnpModeObjectManip::SetStatePlacement()
{
	if ( l_pSelectedObject != 0 )
	{
		l_CurrentManipState = e_StatePlacement;
		GotoState( m_StatePlacement );
	}
}
//--------------------------------------------------------------------
// Lock the selection
//--------------------------------------------------------------------
void mnpModeObjectManip::SetStateLockSelect()
{
	sel3dMgr::ToggleSelectionLock();
	l_bLocked = sel3dMgr::getSelectionLock();
	set_current_mode_text();
}

//--------------------------------------------------------------------
// select the next object in the selected list
//--------------------------------------------------------------------
//void mnpModeObjectManip::SelectNextObjectInPickList()
//{
//	if(l_bLocked)
//		return;
//	sel3dMgr::CreateUndoOperation();
//	sel3dObject* pItem = sel3dMgr::SetNextToSelected();
//	l_pSelectedObject = cast_selected_to_mnmobject();
//	if (l_pSelectedObject != 0)
//		selected_object_set();
//}


//--------------------------------------------------------------------
//	set the current state based on the last state of the object
//--------------------------------------------------------------------
void mnpModeObjectManip::set_current_state()
{
	switch( l_CurrentManipState )
	{
		case mnpModeObjectManip::e_StateTranslate:
			SetStateTranslate();
			//DBG_LOG( "setting...translate");
			break;
		case mnpModeObjectManip::e_StateScale:
			SetStateScale();
			//DBG_LOG( "setting...scale");
			break;
		case mnpModeObjectManip::e_StateRotate:
			SetStateRotate();
			//DBG_LOG( "setting...rotate");
			break;
		case mnpModeObjectManip::e_StateSelect:
			SetStateSelect();
			//DBG_LOG( "setting...select");
			break;
	}
}

//--------------------------------------------------------------------
//	set the current text based on the current mode
//--------------------------------------------------------------------
void mnpModeObjectManip::set_current_mode_text()
{
	if(sel3dMgr::getSelectionLock())
	{
		//guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Mode, "Locked" );
		guiStatusBarMgr::ShowModeBitmap();
	}
	else
	{
		guiStatusBarMgr::HideModeBitmap();
		switch( l_CurrentManipState )
		{
			case mnpModeObjectManip::e_StateTranslate:
				guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Mode, "Translate" );
				break;
			case mnpModeObjectManip::e_StateScale:
				guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Mode, "Scale" );
				break;
			case mnpModeObjectManip::e_StateRotate:
				guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Mode, "Rotate" );
				break;
			case mnpModeObjectManip::e_StateSelect:
				guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Mode, "Select" );
				break;
			case mnpModeObjectManip::e_StatePlacement:
				guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Mode, "Placement" );
				break;
			default:
				guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Mode, "-" );
				break;
		}
	}
}

//--------------------------------------------------------------------
//	Configure compass for a given state. This is called each time
//	the selected object changes (but the state doesn't)
//--------------------------------------------------------------------
void mnpModeObjectManip::SetCompassTranslateFreeForm()
{
	switch (l_pSelectedObject->GetTranslateFlags())
	{
		case mnmObject::e_TranslateAll:
			this->SetCompassPart( cmpsCompass::e_All );
			break;
		case mnmObject::e_TranslateX:
			this->SetCompassPart( cmpsCompass::e_X );
			break;
		case mnmObject::e_TranslateY:
			this->SetCompassPart( cmpsCompass::e_Y );
			break;
		case mnmObject::e_TranslateZ:
			this->SetCompassPart( cmpsCompass::e_Z );
			break;
		case mnmObject::e_TranslateXY:
			this->SetCompassPart( cmpsCompass::e_X | cmpsCompass::e_Y );
			break;
		case mnmObject::e_TranslateXZ:
			this->SetCompassPart( cmpsCompass::e_X | cmpsCompass::e_Z );
			break;
		case mnmObject::e_TranslateYZ:
			this->SetCompassPart( cmpsCompass::e_Y | cmpsCompass::e_Z );
			break;
		default:
			this->SetCompassPart( 0 );
			break;
	}
	SetSavedCompassParts( cmpsCompassMgr::GetParts(l_nCompassType) );
}
void mnpModeObjectManip::SetCompassTranslate()
{
	switch (l_pSelectedObject->GetTranslateFlags())
	{
		case mnmObject::e_TranslateAll:
			this->SetCompassPart( cmpsCompass::e_All );
			break;
		case mnmObject::e_TranslateX:
			this->SetCompassPart( cmpsCompass::e_X );
			break;
		case mnmObject::e_TranslateY:
			this->SetCompassPart( cmpsCompass::e_Y );
			break;
		case mnmObject::e_TranslateZ:
			this->SetCompassPart( cmpsCompass::e_Z );
			break;
		case mnmObject::e_TranslateXY:
			this->SetCompassPart( cmpsCompass::e_X | cmpsCompass::e_Y );
			break;
		case mnmObject::e_TranslateXZ:
			this->SetCompassPart( cmpsCompass::e_X | cmpsCompass::e_Z );
			break;
		case mnmObject::e_TranslateYZ:
			this->SetCompassPart( cmpsCompass::e_Y | cmpsCompass::e_Z );
			break;
		default:
			this->SetCompassPart( 0 );
			break;
	}
	SetSavedCompassParts( cmpsCompassMgr::GetParts(l_nCompassType) );
}
void mnpModeObjectManip::SetCompassRotate()
{
	switch (l_pSelectedObject->GetRotateFlags())
	{
		case mnmObject::e_RotateAll:
			this->SetCompassPart( cmpsCompass::e_X | cmpsCompass::e_Y | cmpsCompass::e_Z );
			break;
		case mnmObject::e_RotateX:
			this->SetCompassPart( cmpsCompass::e_X );
			break;
		case mnmObject::e_RotateY:
			this->SetCompassPart( cmpsCompass::e_Y );
			break;
		case mnmObject::e_RotateZ:
			this->SetCompassPart( cmpsCompass::e_Z );
			break;
		case mnmObject::e_RotateXY:
			this->SetCompassPart( cmpsCompass::e_X | cmpsCompass::e_Y );
			break;
		case mnmObject::e_RotateXZ:
			this->SetCompassPart( cmpsCompass::e_X | cmpsCompass::e_Z );
			break;
		case mnmObject::e_RotateYZ:
			this->SetCompassPart( cmpsCompass::e_Y | cmpsCompass::e_Z );
			break;
		default:
			this->SetCompassPart( 0 );
			break;
	}
	SetSavedCompassParts( cmpsCompassMgr::GetParts(l_nCompassType) );

	//DBG_LOG1( " resetting compass with parts %x", cmpsCompassMgr::GetParts(l_nCompassType) );
}
void mnpModeObjectManip::SetCompassScale()
{
	switch (l_pSelectedObject->GetScaleFlags())
	{
		case mnmObject::e_ScaleCylindrical:
		case mnmObject::e_ScaleUniform:
			this->SetCompassPart( cmpsCompass::e_X | cmpsCompass::e_Y | cmpsCompass::e_Z );
			break;
		default:
			this->SetCompassPart( 0 );
			break;
	}
	SetSavedCompassParts( cmpsCompassMgr::GetParts(l_nCompassType) );
}
void mnpModeObjectManip::SetCompassSelect()
{
}
void mnpModeObjectManip::SetCompassPlacement()
{
}


//
//	state functions
//

//----------------------------------------------------------------------------
//	BeginStateTranslate - is executed ONCE at the start of the state
//----------------------------------------------------------------------------
//virtual
void mnpModeObjectManip::BeginStateTranslate()
{
	//DBG_LOG1("Translate for %x", &l_pSelectedObject);

	//	set the object
	//l_pSelectedObject = cast_selected_to_mnmobject();
	//DBG_ASSERT( l_pSelectedObject != 0, "NULL object in modeObjectManip state" );

	//	set up the compass
	l_nCompassType = cmpsCompassMgr::e_Translate;

	SetCompassTranslate();

	set_compass_to_object( l_nCompassType, l_pSelectedObject );
	set_current_mode_text();
}

//----------------------------------------------------------------------------
//	OnStateTranslate - is executed every frame while in this state
//----------------------------------------------------------------------------
//virtual
void mnpModeObjectManip::OnStateTranslate()
{
	// debug
	//char text[64];
	//sprintf(text, "Mode: Obj Manip State: Translate" );
	//mnmDebugInfo::SetDebugInfo(12, text);

	inVirtualJoystick *pVJoy = inDeviceMgr::GetVirtualJoystick();
	inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();

	// multiple conditions can end the current interaction
	bool end_interaction = false;

	//	if the selected object changed (but the state didn't) then
	//	update the compass to show the correct parts
	//
	if ( l_pSelectedObjectLast != l_pSelectedObject )
	{
		menu_items_enable_for_selected_object();
		SetCompassTranslate();
		end_interaction = true;
	}

	// check if the compass should be active for the object at this time
	bool compass_enabled = (l_pSelectedObject) ? 
		l_pSelectedObject->IsOperationEnabled(mnmObject::e_Translate, tmlnTimeLine::GetValue()) : false;

	if(compass_enabled)
		if(tmlnTimeLine::GetLocked()) 
			compass_enabled = false;
	
	cmpsCompassMgr::SetActive( cmpsCompassMgr::e_Translate, compass_enabled);

	//	if the alt or ctrl keys are held, don't allow object manipulation
	if (  (    pKeyboard->IsDown(inKeys::e_LALT) 
			|| pKeyboard->IsDown(inKeys::e_RALT)
			|| pKeyboard->IsDown(inKeys::e_RCTRL)
			|| pKeyboard->IsDown(inKeys::e_LCTRL)) )
	{	
		compass_enabled = false;
	}

	//	check for ending conditions if currently interacting
	if ( l_bCompassManip )
	{
		
		// release of left mouse
		if ( !pVJoy->IsHeld(mnmVJoystick::e_LeftClick) )
		{
			// Release of mouse button, finish translation
			if (m_CurrentInteraction)
				m_CurrentInteraction->FinishInteraction();
			end_interaction = true;
		}
		else if (( pVJoy->IsPressed( mnmVJoystick::e_RightClick ) && pVJoy->IsHeld( mnmVJoystick::e_LeftClick ) )
			|| pKeyboard->IsPressed( inKeys::e_ESC ) )
		{
			// User aborted moving, restore original position
			if (m_CurrentInteraction)
				m_CurrentInteraction->AbortInteraction();
			end_interaction = true;
		}

		// if interacting, but not enabled at the given time, should the interaction be ended or just ignored?
		//if (!compass_enabled) end_interaction = true;

		if (end_interaction)
		{
			l_bCompassManip = false;
			l_bFixedTranslationOrientation = false;
			show_compassparts( m_nSavedCompassParts );
			m_CurrentInteraction.reset();
		}
	}

	// Only handle interaction if the manipulator is active at the given time for this object.
	// The compasses will be disabled when a driver is controlled the object's transformation
	// and it cannot be changed by the manipulator.
	if (compass_enabled)
	{
		bool bNewOperation = false;
		//	check for compass picks
		maPoint3d pos;
		int currCompassPartSelected = -1;
		currCompassPartSelected = check_compass_picks( l_nCompassType, pos );
		if ( currCompassPartSelected != -1 )
		{
			if ( is_left_click(pVJoy) )
			{
				//	start the manip
				l_bCompassManip = true;
				l_nCompassPart = currCompassPartSelected;
				bNewOperation = true;

				//DBG_LOG2( " cmps(%d) full(%d)", l_nCompassPart, m_nCompassPart );

				if (l_bLocalTranslation)
				{
					// Orient the axes to the rotation of the object
					l_TranslateOrientation = l_pSelectedObject->GetWorldOrientation();
				}
				else
					// Orient to world space axes
					l_TranslateOrientation = maRotation();
				l_bFixedTranslationOrientation = true;

				// Get the axis being moved
				//
				maVector3d proj_axis;
				bool bPlaneMovement = false;
				switch ( l_nCompassPart )
				{
					case cmpsCompass::e_X:
						proj_axis.Set( 1, 0, 0 );
						break;
					case cmpsCompass::e_Y:
						proj_axis.Set( 0, 1, 0 );
						break;
					case cmpsCompass::e_Z:
						proj_axis.Set( 0, 0, 1 );
						break;
					case cmpsCompass::e_PlaneYZ:
						bPlaneMovement = true;
						proj_axis.Set( 1, 0, 0 );
						l_nCompassPart |= (cmpsCompass::e_Y | cmpsCompass::e_Z);	// show axes along with rectangle
						break;
					case cmpsCompass::e_PlaneXZ:
						bPlaneMovement = true;
						proj_axis.Set( 0, 1, 0 );
						l_nCompassPart |= (cmpsCompass::e_X | cmpsCompass::e_Z);	// show axes along with rectangle
						break;
					case cmpsCompass::e_PlaneXY:
						bPlaneMovement = true;
						proj_axis.Set( 0, 0, 1 );
						l_nCompassPart |= (cmpsCompass::e_X | cmpsCompass::e_Y);	// show axes along with rectangle
						break;
				}

				// save the current state of the compass so we can restore it later
				//SetSavedCompassParts( cmpsCompassMgr::GetParts(l_nCompassType) );
				SetSavedCompassParts( m_nCompassPart );
				show_compassparts( l_nCompassPart );


				// Orient the axes to the rotation from the mouse down event,
				// in order to fix the angle
				l_TranslateOrientation.RotateVector(proj_axis);

				if (bPlaneMovement)
				{
					// Begin interaction that projects mouse motion on to a plane
					// defined by the mouse down point and a plane normal
					m_CurrentInteraction.reset(
						new mnpTranslatePlaneInteraction(l_pSelectedObject, pos, proj_axis));
				}
				else
				{
					// Begin interaction that projects mouse motion along the
					// vector defined by the mouse down point along this projection axis
					m_CurrentInteraction.reset(
						new mnpTranslateVectorInteraction(l_pSelectedObject, pos, proj_axis));
				}
			}
		}

		//	if the compass is being manipulated
		//
		if ( l_bCompassManip )
		{
			// Get mouse cursor position
			int mouseX, mouseY;
			tma3dCursorMgr::GetCursorPos(mouseX, mouseY);

			if (m_CurrentInteraction)
				m_CurrentInteraction->MouseMotion(mouseX, mouseY);
		}
	}

}


//----------------------------------------------------------------------------
//	EndStateTranslate - is executed ONCE on ending the state
//----------------------------------------------------------------------------
//virtual
void mnpModeObjectManip::EndStateTranslate()
{
	cmpsCompassMgr::SetRenderable( cmpsCompassMgr::e_Translate, false );
}


//----------------------------------------------------------------------------
//	BeginStateTranslateFreeForm - is executed ONCE at the start of the state
//----------------------------------------------------------------------------
//virtual
void mnpModeObjectManip::BeginStateTranslateFreeForm()
{
	//	set the selected object.
	//l_pSelectedObject = cast_selected_to_mnmobject();
	//DBG_ASSERT( l_pSelectedObject != 0, "NULL object in modeObjectManip state" );

	//	set up the compass
	l_nCompassType = cmpsCompassMgr::e_Translate;

	SetCompassTranslate();

	set_compass_to_object( l_nCompassType, l_pSelectedObject );
	set_current_mode_text();
}

//----------------------------------------------------------------------------
//	OnStateTranslateFreeForm - is executed every frame while in this state
//----------------------------------------------------------------------------
//virtual
void mnpModeObjectManip::OnStateTranslateFreeForm()
{
		// debug
	//char text[64];
	//sprintf(text, "Mode: Obj Manip State: TranslateFreeForm" );
	//mnmDebugInfo::SetDebugInfo(12, text);

	inVirtualJoystick *pVJoy = inDeviceMgr::GetVirtualJoystick();
	inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();
	inTablet* pTablet = inDeviceMgr::GetTablet();

	// multiple conditions can end the current interaction
	bool end_interaction = false;

	//	if the selected object changed (but the state didn't) then
	//	update the compass to show the correct parts
	//
	if ( l_pSelectedObjectLast != l_pSelectedObject )
	{
		menu_items_enable_for_selected_object();
		SetCompassTranslate();
	}

	// check if the compass should be active for the object at this time
	bool compass_enabled = (l_pSelectedObject) ? 
		l_pSelectedObject->IsOperationEnabled(mnmObject::e_Translate, tmlnTimeLine::GetValue()) : false;
	cmpsCompassMgr::SetActive( cmpsCompassMgr::e_Translate, compass_enabled);

	//	if the alt or ctrl keys are held, don't allow object manipulation
	if (  (    pKeyboard->IsDown(inKeys::e_LALT) 
			|| pKeyboard->IsDown(inKeys::e_RALT)
			|| pKeyboard->IsDown(inKeys::e_RCTRL)
			|| pKeyboard->IsDown(inKeys::e_LCTRL)) )
	{
		compass_enabled = false;
	}

	//	check for ending conditions if currently interacting
	if ( l_bCompassManip )
	{
		// release of left mouse
		if ( !pVJoy->IsHeld(mnmVJoystick::e_LeftClick) )
		{
			if (m_CurrentInteraction)
				m_CurrentInteraction->FinishInteraction();
			end_interaction = true;
		}
		else if (( pVJoy->IsPressed( mnmVJoystick::e_RightClick ) && pVJoy->IsHeld( mnmVJoystick::e_LeftClick ) )
			|| pKeyboard->IsPressed( inKeys::e_ESC ) )
		{
			// User aborted moving, restore original position
			if (m_CurrentInteraction)
				m_CurrentInteraction->AbortInteraction();
		}

		// if interacting, but not enabled at the given time, should the interaction be ended or just ignored?
		//if (!compass_enabled) end_interaction = true;

		if (end_interaction)
		{
			l_bCompassManip = false;
			show_compassparts( m_nSavedCompassParts );
			m_CurrentInteraction.reset();
		}
	}

	// Only handle interaction if the manipulator is active at the given time for this object.
	// The compasses will be disabled when a driver is controlled the object's transformation
	// and it cannot be changed by the manipulator.
	if (compass_enabled)
	{
		bool bNewOperation = false;
		//	check for compass picks
		maPoint3d pos;
		int currCompassPartSelected;
		currCompassPartSelected = check_compass_picks( l_nCompassType, pos );

		// compass manip
		if ( currCompassPartSelected != -1 )
		{
			if ( is_left_click(pVJoy) )
			{
				bNewOperation = true;
				l_bCompassManip = true;
				l_nCompassPart = currCompassPartSelected;

				// save the current state of the compass so we can restore it later
				SetSavedCompassParts( cmpsCompassMgr::GetParts(l_nCompassType) );
				show_compassparts( l_nCompassPart );

				//DBG_LOG2( "-cmps(%d) full(%d)", l_nCompassPart, m_nCompassPart );

				m_CurrentInteraction.reset(
					new mnpFreeFormInteraction(l_pSelectedObject, pos) );
			}
		}

		//	if the compass is being manipulated
		//
		if ( l_bCompassManip )
		{
			// Get mouse cursor position
			int mouseX, mouseY;
			tma3dCursorMgr::GetCursorPos(mouseX, mouseY);

			if (m_CurrentInteraction)
				m_CurrentInteraction->MouseMotion(mouseX, mouseY);
		}
	}
}

//----------------------------------------------------------------------------
//	EndStateTranslateFreeForm - is executed ONCE on ending the state
//----------------------------------------------------------------------------
//virtual
void mnpModeObjectManip::EndStateTranslateFreeForm()
{
	cmpsCompassMgr::SetRenderable( cmpsCompassMgr::e_Translate, false );
}

//----------------------------------------------------------------------------
//	BeginStateRotate - is executed ONCE at the start of the state
//----------------------------------------------------------------------------
//virtual
void mnpModeObjectManip::BeginStateRotate()
{
	//	set the selected object
	//l_pSelectedObject = cast_selected_to_mnmobject();
	//DBG_ASSERT( l_pSelectedObject != 0, "NULL object in modeObjectManip state" );

	//	set up the compass
	l_nCompassType = cmpsCompassMgr::e_Rotate;

	SetCompassRotate();

	set_compass_to_object( l_nCompassType, l_pSelectedObject );
	set_current_mode_text();
}

//----------------------------------------------------------------------------
//	OnStateRotate - is executed every frame while in this state
//----------------------------------------------------------------------------
//virtual
void mnpModeObjectManip::OnStateRotate()
{
		// debug
	//char text[64];
	//sprintf(text, "Mode: Obj Manip State: Rotate" );
	//mnmDebugInfo::SetDebugInfo(12, text);

	inVirtualJoystick *pVJoy = inDeviceMgr::GetVirtualJoystick();
	inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();

	// multiple conditions can end the current interaction
	bool end_interaction = false;

	//	if the selected object changed (but the state didn't) then
	//	update the compass to show the correct parts
	//
	if ( l_pSelectedObjectLast != l_pSelectedObject )
	{
		menu_items_enable_for_selected_object();
		SetCompassRotate();
		end_interaction = true;
	}


	// check if the compass should be active for the object at this time
	bool compass_enabled = (l_pSelectedObject) ? 
		l_pSelectedObject->IsOperationEnabled(mnmObject::e_Rotate, tmlnTimeLine::GetValue()) : false;
	cmpsCompassMgr::SetActive( cmpsCompassMgr::e_Rotate, compass_enabled);

	if(compass_enabled)
		if(tmlnTimeLine::GetLocked()) 
			compass_enabled = false;
	//	if the alt or ctrl keys are held, don't allow object manipulation
	if (  (    pKeyboard->IsDown(inKeys::e_LALT) 
			|| pKeyboard->IsDown(inKeys::e_RALT)
			|| pKeyboard->IsDown(inKeys::e_RCTRL)
			|| pKeyboard->IsDown(inKeys::e_LCTRL)) )
	{
		compass_enabled = false;
	}

	//	check for ending conditions if currently interacting
	if ( l_bCompassManip )
	{
		// release of left mouse
		if ( !pVJoy->IsHeld(mnmVJoystick::e_LeftClick) )
		{
			if (m_CurrentInteraction)
				m_CurrentInteraction->FinishInteraction();
			end_interaction = true;
		}
		else if (( pVJoy->IsPressed( mnmVJoystick::e_RightClick ) && pVJoy->IsHeld( mnmVJoystick::e_LeftClick ) )
			|| pKeyboard->IsPressed( inKeys::e_ESC ) )
		{
			if (m_CurrentInteraction)
				m_CurrentInteraction->AbortInteraction();
			end_interaction = true;
		}

		// if interacting, but not enabled at the given time, should the interaction be ended or just ignored?
		//if (!compass_enabled) end_interaction = true;

		if (end_interaction)
		{
			l_bCompassManip = false;
			show_compassparts( m_nSavedCompassParts );
			m_CurrentInteraction.reset();
		}
	}

	// Only handle interaction if the manipulator is active at the given time for this object.
	// The compasses will be disabled when a driver is controlled the object's transformation
	// and it cannot be changed by the manipulator.
	if (compass_enabled)
	{
		bool bNewOperation = false;
		//	check for compass picks
		maPoint3d pos;
		int currCompassPartSelected;
		currCompassPartSelected = check_compass_picks( l_nCompassType, pos );
		if ( currCompassPartSelected != -1 )
		{
			if ( is_left_click(pVJoy) )
			{
				//	start the manip
				bNewOperation = true;
				l_bCompassManip = true;
				l_nCompassPart = currCompassPartSelected;

				// save the current state of the compass so we can restore it later
				SetSavedCompassParts( cmpsCompassMgr::GetParts(l_nCompassType) );
				show_compassparts( l_nCompassPart );

				// save axis of rotation
				maVector3d rot_axis = get_rotation_axis( l_pSelectedObject->GetOrientation(), l_nCompassPart);

				int mouseX, mouseY;
				tma3dCursorMgr::GetCursorPos(mouseX, mouseY);
				
				m_CurrentInteraction.reset(
					new mnpRotateInteraction(l_pSelectedObject, mouseX, mouseY, rot_axis) );
			}
		}

		//	if the compass is being manipulated
		//
		if ( l_bCompassManip )
		{
			// Get mouse cursor position
			int mouseX, mouseY;
			tma3dCursorMgr::GetCursorPos(mouseX, mouseY);

			if (m_CurrentInteraction)
				m_CurrentInteraction->MouseMotion(mouseX, mouseY);
		}
	}
}

//----------------------------------------------------------------------------
//	EndStateRotate - is executed ONCE on ending the state
//----------------------------------------------------------------------------
//virtual
void mnpModeObjectManip::EndStateRotate()
{
	cmpsCompassMgr::SetRenderable( cmpsCompassMgr::e_Rotate, false );
}

//----------------------------------------------------------------------------
//	BeginStateScale - is executed ONCE at the start of the state
//----------------------------------------------------------------------------
//virtual
void mnpModeObjectManip::BeginStateScale()
{
	//	set the selected object
	//l_pSelectedObject = cast_selected_to_mnmobject();
	//DBG_ASSERT( l_pSelectedObject != 0, "NULL object in modeObjectManip state" );

	l_pSelectedObject->UpdatePosition( l_pSelectedObject->GetPosition() );

	l_nCompassType = cmpsCompassMgr::e_Scale;

	SetCompassScale();

	set_compass_to_object( l_nCompassType, l_pSelectedObject );
	set_current_mode_text();
}

//----------------------------------------------------------------------------
//	OnStateScale - is executed every frame while in this state
//----------------------------------------------------------------------------
//virtual
void mnpModeObjectManip::OnStateScale()
{
		// debug
	//char text[64];
	//sprintf(text, "Mode: Obj Manip State: Scale" );
	//mnmDebugInfo::SetDebugInfo(12, text);

	inVirtualJoystick *pVJoy = inDeviceMgr::GetVirtualJoystick();
	inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();

	// multiple conditions can end the current interaction
	bool end_interaction = false;

	//	if the selected object changed (but the state didn't) then
	//	update the compass to show the correct parts
	//
	if ( l_pSelectedObjectLast != l_pSelectedObject )
	{
		menu_items_enable_for_selected_object();
		SetCompassScale();
		end_interaction = true;
	}

	// check if the compass should be active for the object at this time
	bool compass_enabled = (l_pSelectedObject) ? 
		l_pSelectedObject->IsOperationEnabled(mnmObject::e_Scale, tmlnTimeLine::GetValue()) : false;
	cmpsCompassMgr::SetActive( cmpsCompassMgr::e_Scale, compass_enabled);

	if(compass_enabled)
		if(tmlnTimeLine::GetLocked()) 
			compass_enabled = false;
	//	if the alt or ctrl keys are held, don't allow object manipulation
	if (  (    pKeyboard->IsDown(inKeys::e_LALT) 
			|| pKeyboard->IsDown(inKeys::e_RALT)
			|| pKeyboard->IsDown(inKeys::e_RCTRL)
			|| pKeyboard->IsDown(inKeys::e_LCTRL)) )
	{
		compass_enabled = false;
	}

	//	check for ending conditions if currently interacting
	if ( l_bCompassManip )
	{
		// release of left mouse
		if ( !pVJoy->IsHeld(mnmVJoystick::e_LeftClick) )
		{
			// Release of mouse button, finish translation
			if (m_CurrentInteraction)
				m_CurrentInteraction->FinishInteraction();
			end_interaction = true;
		}
		else if (( pVJoy->IsPressed( mnmVJoystick::e_RightClick ) && pVJoy->IsHeld( mnmVJoystick::e_LeftClick ) )
			|| pKeyboard->IsPressed( inKeys::e_ESC ) )
		{
			// User aborted moving, restore original scale
			if (m_CurrentInteraction)
				m_CurrentInteraction->AbortInteraction();
			end_interaction = true;
		}

		// if interacting, but not enabled at the given time, should the interaction be ended or just ignored?
		//if (!compass_enabled) end_interaction = true;

		if (end_interaction)
		{
			l_bCompassManip = false;
			show_compassparts( m_nSavedCompassParts );
			m_CurrentInteraction.reset();
		}
	}

	// Only handle interaction if the manipulator is active at the given time for this object.
	// The compasses will be disabled when a driver is controlled the object's transformation
	// and it cannot be changed by the manipulator.
	if (compass_enabled)
	{
		//	check for compass picks
		maPoint3d pos;
		int currCompassPartSelected;
		currCompassPartSelected = check_compass_picks( l_nCompassType, pos );
		bool bNewOperation = false;

		if ( currCompassPartSelected != -1 )
		{
			if (  is_left_click(pVJoy)  )
			{
				l_bCompassManip = true;
				l_nCompassPart = currCompassPartSelected;
				bNewOperation = true;

				// save the current state of the compass so we can restore it later
				//SetSavedCompassParts( cmpsCompassMgr::GetParts(l_nCompassType) );
				SetSavedCompassParts( m_nCompassPart );
				show_compassparts( l_nCompassPart );

				//maAxisBox box = l_pSelectedObject->GetWorldBox();
				//switch ( l_nCompassPart )
				//{
				//	case cmpsCompass::e_X:
				//		m_fOriginalSize = box.GetDiffX();
				//		break;
				//	case cmpsCompass::e_Y:
				//		m_fOriginalSize = box.GetDiffY();
				//		break;
				//	case cmpsCompass::e_Z:
				//		m_fOriginalSize = box.GetDiffZ();
				//		break;
				//	default:
				//		DBG_LOG( "Invalid compass part " << l_nCompassPart << " for scale" );
				//		m_fOriginalSize = box.GetMaxY() - box.GetMinY();
				//		break;
				//}
				//DBG_LOG4( "OS(%6.3f) = maxy %6.3f - miny %6.3f for part %d", m_fOriginalSize, box.GetMaxY(), box.GetMinY(), l_nCompassPart );
				
				// Get the axis being moved
				maVector3d proj_axis;
				switch ( l_nCompassPart )
				{
					case cmpsCompass::e_X:
						proj_axis.Set( 1, 0, 0 );
						break;
					case cmpsCompass::e_Y:
						proj_axis.Set( 0, 1, 0 );
						break;
					case cmpsCompass::e_Z:
						proj_axis.Set( 0, 0, 1 );
						break;
				}

				// Move local axis into world space
				l_pSelectedObject->GetOrientation().RotateVector(proj_axis);

				// Begin scale interaction that projects mouse motion along the
				// vector defined by the mouse down point along this projection axis
				m_CurrentInteraction.reset(
					new mnpScaleInteraction(l_pSelectedObject, pos, proj_axis, 
											cmpsCompassMgr::GetCompassFlip()));
			}
		}

		//	if the compass is being manipulated
		//
		if ( l_bCompassManip )
		{
			// Get mouse cursor position
			int mouseX, mouseY;
			tma3dCursorMgr::GetCursorPos(mouseX, mouseY);

			if (m_CurrentInteraction)
				m_CurrentInteraction->MouseMotion(mouseX, mouseY);
		}
	}
}

//----------------------------------------------------------------------------
//	EndStateScale - is executed ONCE on ending the state
//----------------------------------------------------------------------------
//virtual
void mnpModeObjectManip::EndStateScale()
{
	cmpsCompassMgr::SetRenderable( cmpsCompassMgr::e_Scale, false );
}

//----------------------------------------------------------------------------
//	BeginStateSelect - is executed ONCE at the start of the state
//----------------------------------------------------------------------------
//virtual
void mnpModeObjectManip::BeginStateSelect()
{
	if ( l_pSelectedObject )
		selected_object_set();
	SetCompassSelect();
	set_current_mode_text();
}

//----------------------------------------------------------------------------
//	OnStateSelect - is executed every frame while in this state
//----------------------------------------------------------------------------
//virtual
void mnpModeObjectManip::OnStateSelect()
{
		// debug
	//char text[64];
	//sprintf(text, "Mode: Obj Manip State: Select" );
	//mnmDebugInfo::SetDebugInfo(12, text);

	//	// debug
	//maPoint3d pos,tar;
	//pos = cam3dMgr::GetCamera().GetPosition();
	//tar = cam3dMgr::GetCamera().GetTarget();
	////char text[64];
	//sprintf(text, "cam (%06.3f, %06.3f %06.3f)  target (%06.3f, %06.3f %06.3f)", pos.GetX(), pos.GetY(), pos.GetZ(), tar.GetX(), tar.GetY(), tar.GetZ() );
	//mnmDebugInfo::SetDebugInfo(15, text);

	// TODO: optimize this so it doesn't happen all the time.
	//
	//sprintf(text, "" );	//debug
	//sel3dObject* pPObj = sel3dMgr::GetSelected();
	//if ( pPObj )
	//{
	//	mnmObject* pObj = dynamic_cast<mnmObject*>( pPObj );
	//	//DBG_ASSERT( pObj != 0, "Selected object couldn't cast to an mnmObject" );
	//	if (pObj)
	//	{
	//		sprintf(text, "object selected %x (%06.3f, %06.3f, %06.3f)", pPObj, pObj->GetPosition().GetX(), pObj->GetPosition().GetY(), pObj->GetPosition().GetZ() );	//debug
	//	}
	//}
	//mnmDebugInfo::SetDebugInfo(13, text);	//debug

//	l_pSelectedObjectLast	= l_pSelectedObject;
//	l_pSelectedObject		= cast_selected_to_mnmobject();


	inVirtualJoystick *pVJoy = inDeviceMgr::GetVirtualJoystick();
	inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();
	if (tma3dCursorMgr::IsCursorOverView())
	{
		// On mouse down, store location of mouse
		if ( pVJoy->IsPressed(mnmVJoystick::e_LeftClick)  )
			tma3dCursorMgr::GetCursorPos(m_MouseDownX, m_MouseDownY);
		else if ( pVJoy->IsReleased(mnmVJoystick::e_LeftClick)  )
		{
			// CTRL usually means to send events to the camera manipulator,
			// but when CTRL is used in a single click with no mouse motion, then
			// it should be interpreted as a "toggle selection" mouse pick.
			if (pKeyboard->IsDown(inKeys::e_RCTRL)
			   || pKeyboard->IsDown(inKeys::e_LCTRL))
			{
				int mouseUpX, mouseUpY;
				tma3dCursorMgr::GetCursorPos(mouseUpX, mouseUpY);
				const int c_MouseMotionThreshold = 4; // number of pixel motion that can still be counted as a single click
				if (abs(mouseUpX-m_MouseDownX) + abs(mouseUpY-m_MouseDownY) < c_MouseMotionThreshold)
				{
					// Check for picked object with GPU picker
					if ( sel3dObject* pPickedObj = check_for_picks(m_PickMode) )
					{
						// Do toggle selection on what was picked
						sel3dMgr::CreateUndoOperation();
						if (sel3dMgr::IsSelected(pPickedObj))
							sel3dMgr::RemoveFromSelection( pPickedObj );
						else
							sel3dMgr::AddToSelection( pPickedObj );
					}
				}
			}
		}
	}

	//	if there is an object selected and it's not been set already then set it.
	//
	if (   ( l_pSelectedObject )
		&& ( l_pSelectedObjectLast != l_pSelectedObject ) )
	{
		selected_object_set();
		SetCompassSelect();

		//	if the last manip state wasn't select then change to it automatically
		if (l_CurrentManipState != e_StateSelect)
		{
			set_current_state();
		}
	}
}

//----------------------------------------------------------------------------
//	EndStateSelect - is executed ONCE on ending the state
//----------------------------------------------------------------------------
//virtual
void mnpModeObjectManip::EndStateSelect()
{
	cmpsCompassMgr::SetRenderable( cmpsCompassMgr::e_Select, false );
}

//----------------------------------------------------------------------------
//	BeginStatePlacement - is executed ONCE at the start of the state
//----------------------------------------------------------------------------
//virtual
void mnpModeObjectManip::BeginStatePlacement()
{
	l_nCompassType = cmpsCompassMgr::e_Translate;
	SetCompassPlacement();
	set_current_mode_text();

	// Since there is no mouse click that begins state placement, we need to
	// start the interaction right away.
	//bga - the placement mode was using m_MouseDownPoint to define the plane,
	// but was never initializing that variable. I changed it here to use the
	// object's bbox center to define the XZ plane of motion.

	//maPoint3d mouse_down = l_pSelectedObject->GetPosition();
	maPoint3d mouse_down = l_pSelectedObject->GetWorldBox().GetCenter();
	m_CurrentInteraction.reset(
					new mnpPlacementInteraction(l_pSelectedObject, mouse_down));
}

//----------------------------------------------------------------------------
//	OnStatePlacement - is executed every frame while in this state
//----------------------------------------------------------------------------
//virtual
void mnpModeObjectManip::OnStatePlacement()
{
		// debug
	//char text[64];
	//sprintf(text, "Mode: Obj Manip State: Placement" );
	//mnmDebugInfo::SetDebugInfo(12, text);

	//	if the selected object changed (but the state didn't) then
	//	update the compass to show the correct parts
	//
	if ( l_pSelectedObjectLast != l_pSelectedObject )
	{
		menu_items_enable_for_selected_object();
		SetCompassPlacement();
	}

	// TODO: optimize this so it doesn't happen all the time.
	//
	//sprintf(text, "" );	//debug
	//sel3dObject* pPObj = sel3dMgr::GetSelected();
	//if ( pPObj )
	//{
	//	sprintf(text, "object Placement" );	//debug
	//}
	//mnmDebugInfo::SetDebugInfo(13, text);	//debug

	//	if there is an object selected.
	//
	if ( l_pSelectedObject )
	{
		// Get mouse cursor position
		int mouseX, mouseY;
		tma3dCursorMgr::GetCursorPos(mouseX, mouseY);

		if (m_CurrentInteraction)
			m_CurrentInteraction->MouseMotion(mouseX, mouseY);


		// debug
		//sprintf(text, "new pos (%6.3f, %6.3f, %6.3f)", new_position.GetX(), new_position.GetY(), new_position.GetZ() );
		//mnmDebugInfo::SetDebugInfo(21, text);

		//	check for the clicks
		//
		inVirtualJoystick *pVJoy = inDeviceMgr::GetVirtualJoystick();
		inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();

		if ( !(   pKeyboard->IsDown(inKeys::e_LALT) 
			   || pKeyboard->IsDown(inKeys::e_RALT)
			   || pKeyboard->IsDown(inKeys::e_RCTRL)
			   || pKeyboard->IsDown(inKeys::e_LCTRL)) )
		{
			// check to see if there is something we are selecting
			//
			if ( is_left_click(pVJoy) )
			{
				//if ( pKeyboard->IsHeld(inKeys::e_LCTRL) || pKeyboard->IsHeld(inKeys::e_RCTRL))
				//{
				//	// if a control key is pressed then CLONE the object
				//	//
				//	// FIX: - the system prop exposure in the modes
				//	//
				//	propPropObject* pObj = dynamic_cast<propPropObject*>(l_pSelectedObject);
				//	if ( pObj != 0 )
				//	{
				//		itString selobj_filename;
				//		pObj->GetFilename( selobj_filename );
				//		if ( selobj_filename.GetLength() > 0 )
				//		{
				//			//	Add the prop
				//			propOperations::AddProp( propScriptData( selobj_filename ) );
				//		}
				//	}
				//	else
				//	{
				//		guiMessageBox::Show( "Cannot clone this object", "Object Clone Error", guiMessageBox::e_OKOnly );
				//	}

				//	//	clear it out.
				//	//sel3dMgr::ClearSelection();
				//	//selected_object_clear();
				//	//SetStateSelect(false);
				//}
				//else
				//{
				//	//sel3dMgr::ClearSelection();
				//	//selected_object_clear();
					
					// End placement state when left mouse clicked
					if (m_CurrentInteraction)
					{
						m_CurrentInteraction->FinishInteraction();
						m_CurrentInteraction.reset();
					}
					SetStateSelect(true);
				//}
			}
			//else
			//if ( pVJoy->IsPressed( mnmVJoystick::e_RightClick ) ) // && !pVJoy->IsPressed( mnmVJoystick::e_LeftClick ) )
			//{
			//	sel3dMgr::CreateUndoOperation();
			//	sel3dMgr::ClearSelection();
			//	selected_object_clear();
			//	SetStateSelect(false);
			//}
		}
	}
}

//----------------------------------------------------------------------------
//	EndStatePlacement - is executed ONCE on ending the state
//----------------------------------------------------------------------------
//virtual
void mnpModeObjectManip::EndStatePlacement()
{
	cmpsCompassMgr::SetRenderable( cmpsCompassMgr::e_Translate, false );
}

