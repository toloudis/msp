/*****************************************************************************
**  orthoModeObjectManip.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/ObjectManip/orthoModeObjectManip.hpp"

#include "Features/Capture/orthoAvatarDataUtil.hpp"
#include "Features/Capture/orthoPackage.hpp"
#include "Features/Requests/orthoRemoteCommandMgr.hpp"
#include "Features/Capture/cptrRenderOutputDataUtil.hpp"
#include "Features/Requests/Tasks/orthoRequestTaskUtil.hpp"

#include "Graphics/Cam/camCameraManipOrbit.hpp"
#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsFollowUtil.hpp"
#include "Support/fgmt/fgmtSelectInterest.hpp"
#include "Features/FilmGates/fgtFrameMgr.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Support/mnm/mnmAppUtil.hpp"
#include "Support/mnm/mnmAutoSaveMgr.hpp"
#include "Support/mnm/mnmCompassUtil.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmDebugInfo.hpp"
#include "Support/mnm/mnmObject.hpp"
#include "Support/mnm/mnmVJoystick.hpp"
#include "Features/ObjectManip/mnpPackage.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Support/mtrl/mtrlSelectInterest.hpp"
#include "Features/Prefs/PrefsData.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Features/RenderPanels/rpnOperations.hpp"
#include "Features/RenderPanels/rpnRenderingPrefs.hpp"


//	compass
//#include "Support/cmps/cmpsCompass.hpp"
#include "Support/cmps/cmpsCompassMgr.hpp"
#include "Support/cmps/cmpsSelectMgr.hpp"

//	tools
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/pick3d/pick3dMgr.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Tool/tma3d/tma3dCursorMgr.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"
#include "Tool/tma3d/tma3dRenderView.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/vis/visMgr.hpp"

//	library
#include "Tool/api3d/api3dSubdiv.hpp"
#include "Core/App/appSimTime.hpp"
#include "Core/App/appTime.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Core/Dbg/dbgLog.hpp"
#include "Core/Geo/geoPickRay.hpp"
#include "InputDI/In/inDeviceMgr.hpp"
#include "InputDI/In/inVirtualJoystick.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Core/Ma/maConstants.hpp"
//#include "Core/Ma/maFloatRGBA.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "Core/undo/undoUndoMgr.hpp"



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

	const float lc_fRotateIncrement = maConstants::c_fPI / 200.0f;
	const float lc_fFocusRadius	= 10.0f;

	const maVector3d lc_RotateX( 1, 0, 0 );
	const maVector3d lc_RotateY( 0, 1, 0 );
	const maVector3d lc_RotateZ( 0, 0, 1 );

	std::string	l_CameraToRender("Close-Up");

	int l_MaterialPartIndex = 0;

	mnmObject* l_pSelectedObject = 0;
	mnmObject* l_pSelectedObjectLast = 0;

	bool l_bCompassManip = false;
	int	 l_nCompassType = 0;
	int  l_nCompassPart = 0;
	float l_fCompassScalePercent = 1.0f;
	float l_cfCOMPASSSCALEPERCENT_MAX = 3.0f;
	float l_cfCOMPASSSCALEPERCENT_INC = 0.05f;

	bool l_bDirty = false;

	pick3dPickList l_PickList;

	orthoModeObjectManip::ObjectManipStates l_CurrentManipState;

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
				DBG_ASSERT0(false, "invalid rotation direction!");
				break;
			}
		}

		char text[64];
		sprintf(text, "rotation axis (%6.3f,%6.3f,%6.3f)", rot_axis.GetX(), rot_axis.GetY(), rot_axis.GetZ()  );
		mnmDebugInfo::SetDebugInfo(22, text);

		return rot_axis;
	}

	//------------------------------------------------------------------------
	//	get_rotation_from_direction returns the rotation from the direction
	//------------------------------------------------------------------------
	maRotation get_rotation_from_direction( const maVector3d& i_RotationAxis,
											float i_fDirection )
	{
		// keep the direction between 0 an 2*PI
		while ( i_fDirection > maConstants::c_fPI_Times_2 )
		{
			i_fDirection -= maConstants::c_fPI_Times_2;
		}

		while ( i_fDirection < 0 )
		{
			i_fDirection += maConstants::c_fPI_Times_2;
		}

		return maRotation( i_RotationAxis, i_fDirection );
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
	//	check to see if there are any picks under the cursor
	//----------------------------------------------------------------------------
	bool check_for_picks(orthoModeObjectManip::PickMode i_PickMode)
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

		// GPU Pick technique
		tma3dRenderView *pRenderView = tma3dRenderView::GetActiveRenderView();
		int x=0, y=0;
		tma3dCursorMgr::GetCursorPos(x,y);
		envType::UInt32 pick_code = pRenderView->DoPickRender(x,y, tmlnTimeLine::GetValue());

		pick3dPickObject *pPicked = pick3dMgr::MatchPickCode(pick_code);
	
		char text[64];
		sprintf(text, "GPU pick = %d %s", pick_code, pPicked ? pPicked->GetPick3dName().c_str() : "NULL" );
		mnmDebugInfo::SetDebugInfo(10, text);

		// Add in a special case here, if we are picking an object that
		// was already selected and it is a material object, then use the
		// picking to update the selected material.
		//if (pPicked && pPicked == sel3dMgr::GetSelected())
		//
		// Changed to do the pick on every mouse pick, not on the second pick
		bool bPickedObjectPart = false;
		if (pPicked)
		{
			if (i_PickMode == orthoModeObjectManip::e_PickAll ||
				i_PickMode == orthoModeObjectManip::e_PickMaterials)
			{
				// Pick material
				bPickedObjectPart = mtrlSelectInterest::SelectFromPickCode(pPicked, pick_code);
			}

			if (i_PickMode == orthoModeObjectManip::e_PickAll ||
				i_PickMode == orthoModeObjectManip::e_PickSurfaces)
			{
				// Pick surface 
				bPickedObjectPart = fgmtSelectInterest::SelectFromPickCode(pPicked, pick_code);
			}
		}

		// If we picked the object part, then we don't want to also pick the object
		if ((!bPickedObjectPart) ||
			(i_PickMode == orthoModeObjectManip::e_PickAll))
		{
			// Put result into pick list, because the rest of the
			// code expects that
			l_PickList.Clear();
			if (pPicked)
			{
				l_PickList.AddItem(pPicked, 0.0f);
				return true;
			}
		}

		return false;
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
		ray.SetFromStartEnd( start, start + dir );

		//DBG_LOG0( "------------------------" );
		//DBG_LOG3( " ray start(%6.3f,%6.3f,%6.3f)", start.GetX(), start.GetY(), start.GetZ() );
		//DBG_LOG3( " ray end  (%6.3f,%6.3f,%6.3f)", dir.GetX(), dir.GetY(), dir.GetZ() );

		//	do the pick
		maPoint3d pos;
		int part = -1;
		if ( cmpsCompassMgr::RayPick( compassType, start, start+dir, pos, part ) )
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
		pick3dPickObject* pObj = sel3dMgr::GetSelected();
		if (!pObj) return NULL;
		return dynamic_cast<mnmObject*>( pObj );
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void set_compass_to_object( int i_nCompassType, mnmObject* i_pObj )
	{
		DBG_ASSERT0( i_pObj != 0, "select compass cannot be set to a NULL object" );

		mnmCompassUtil::SetUpCompass( i_nCompassType, i_pObj );

		cmpsCompass& compass = cmpsCompassMgr::GetCompass(l_nCompassType);
		const maPoint3d& scale = compass.GetScale();
		maPoint3d newscale = scale * l_fCompassScalePercent;
		compass.SetScale(newscale);
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void set_compass_to_list( int i_nCompassType, const std::list<pick3dPickObject*>& i_Objects )
	{
		mnmCompassUtil::SetUpCompass( i_nCompassType, i_Objects );

		cmpsCompass& compass = cmpsCompassMgr::GetCompass(l_nCompassType);
		const maPoint3d& scale = compass.GetScale();
		maPoint3d newscale = scale * l_fCompassScalePercent;
		compass.SetScale(newscale);
	}
	
	//----------------------------------------------------------------------------
	//	Determine the point to rotate around considering the selected list
	//----------------------------------------------------------------------------
	maPoint3d compute_pivot_point()
	{	
		const std::list<pick3dPickObject*> &selected_list = sel3dMgr::GetSelectedList();

		int count = 0;
		maAxisBox combined_box;
		mnmObject *pFirstObject = NULL;
		std::list<pick3dPickObject*>::const_iterator it, end = selected_list.end();
		for (it = selected_list.begin(); it != end; ++it)
		{
			if (mnmObject* pObject = dynamic_cast<mnmObject*>(*it))
			{
				if (!pFirstObject) pFirstObject = pObject; // record the top selection object

				combined_box.Union( pObject->GetWorldBox() );
				count++;
			}
		}

		if (count > 1)
		{
			// Pivot point is center of combined box
			return combined_box.GetCenter();
		}
		else if ((count == 1) && (pFirstObject != NULL))
		{
			// If only one object, use its pivot point
			return pFirstObject->GetWorldPivot();
		}
		return maPoint3d(0,0,0);
	}

	//--------------------------------------------------------------------
	// the callback function for menu item: Translate
	//--------------------------------------------------------------------
	//void menuitem_click_Translate( int i_ObjectID )
	//{
	//	modeMode* pMode = modeModeMgr::GetCurrentMode();

	//	//DBG_LOG2( "mode id %d (%s)", pMode->GetID(), pMode->GetMenuItemName() );

	//	orthoModeObjectManip* pModeOM = dynamic_cast<orthoModeObjectManip*>(pMode);
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
		orthoModeObjectManip* pModeOM = dynamic_cast<orthoModeObjectManip*>(pMode);
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
		orthoModeObjectManip* pModeOM = dynamic_cast<orthoModeObjectManip*>(pMode);
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
		orthoModeObjectManip* pModeOM = dynamic_cast<orthoModeObjectManip*>(pMode);
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
		orthoModeObjectManip* pModeOM = dynamic_cast<orthoModeObjectManip*>(pMode);
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
	/*
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
	*/}
	void menu_items_enable()
	{
	/*
		guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_TRANSLATE, true );
		guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_ROTATE, true );
		guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_SCALE, true );
		guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_SELECT, true );
		guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_PLACEMENT, true );
	*/}
	void menu_items_disable()
	{
	/*
		guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_TRANSLATE, false );
		guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_ROTATE, false );
		guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_SCALE, false );
		guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_SELECT, false );
		guiMenuMgr::EnableMenuItem( MENUITEM_OBJECTS, MENUITEM_OBJECTS_PLACEMENT, false );
	*/}
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
		l_PickList.Clear();
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
		const std::list<pick3dPickObject*> &selected_list = sel3dMgr::GetSelectedList();
		std::list<pick3dPickObject*>::const_iterator it, end = selected_list.end();
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

	//--------------------------------------------------------------------
	// apply a delta rotation around the given pivot point to the 
	//	object passed in
	//--------------------------------------------------------------------
	void apply_delta_rotation(mnmObject* i_pObject,
							  const maRotation& i_Delta, 
							  const maPoint3d &i_Pivot, 
							  bool i_bNewOperation)
	{
		// Apply the delta rotation
		i_pObject->UpdateOrientation( i_Delta * i_pObject->GetOrientation(), i_bNewOperation );
		
		maVector3d pivot_delta = i_pObject->GetWorldPivot()- i_Pivot;
		if (pivot_delta != maVector3d(0,0,0))
		{
			maVector3d xformed_delta(pivot_delta); 
			i_Delta.RotateVector(xformed_delta);

			maVector3d compensation = xformed_delta - pivot_delta;
			i_pObject->UpdatePosition( compensation + i_pObject->GetPosition(), i_bNewOperation );
		}
	}

	//--------------------------------------------------------------------
	// Rotate objects in the selected list that aren't the focus
	// of the orientation compass.
	//--------------------------------------------------------------------
	void apply_delta_rotation(const maRotation& i_Delta, 
							  const maPoint3d &i_Pivot, 
							  bool i_bNewOperation)
	{
		const std::list<pick3dPickObject*> &selected_list = sel3dMgr::GetSelectedList();
		std::list<pick3dPickObject*>::const_iterator it, end = selected_list.end();
		for (it = selected_list.begin(); it != end; ++it)
		{
			// skip first one, it is receiving the rotation from the compass
			//if (it != selected_list.begin())
			{
				if (mnmObject* pObject = dynamic_cast<mnmObject*>(*it))
				{
					// this line rotates without pivot
					//pObject->UpdateOrientation( i_Delta * pObject->GetOrientation(), i_bNewOperation );

					// this line applies rotation and translation based on pivot position
					apply_delta_rotation(pObject, i_Delta, i_Pivot, i_bNewOperation);
				}
			}
		}
	}
}




//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
orthoModeObjectManip::orthoModeObjectManip()
:	m_bInitialized( false ),
	m_bAdded( false )
{
	SetMenuItemName( "Object Manip" );

	//	set-up the state call-backs
	m_StateTranslate.SetState( this, &orthoModeObjectManip::BeginStateTranslate, &orthoModeObjectManip::OnStateTranslate, &orthoModeObjectManip::EndStateTranslate );
	m_StateTranslateFreeForm.SetState( this, &orthoModeObjectManip::BeginStateTranslateFreeForm, &orthoModeObjectManip::OnStateTranslateFreeForm, &orthoModeObjectManip::EndStateTranslateFreeForm );
	m_StateRotate.SetState( this, &orthoModeObjectManip::BeginStateRotate, &orthoModeObjectManip::OnStateRotate, &orthoModeObjectManip::EndStateRotate );
	m_StateScale.SetState( this, &orthoModeObjectManip::BeginStateScale, &orthoModeObjectManip::OnStateScale, &orthoModeObjectManip::EndStateScale );
	m_StateSelect.SetState( this, &orthoModeObjectManip::BeginStateSelect, &orthoModeObjectManip::OnStateSelect, &orthoModeObjectManip::EndStateSelect );
	m_StatePlacement.SetState( this, &orthoModeObjectManip::BeginStatePlacement, &orthoModeObjectManip::OnStatePlacement, &orthoModeObjectManip::EndStatePlacement );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// virtual
orthoModeObjectManip::~orthoModeObjectManip()
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
void orthoModeObjectManip::Initialize()
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

//	mnmAppUtil::UpdateTitleBar(false);

	//	set-up the auto-save mgr
	PrefsData& data = PrefsMgr::Data();
	mnmAutoSaveMgr::Initialize( data.m_bAutoSave.GetValue(), data.m_AutoSaveFrequency.GetValue()*60, data.m_AutoSaveBackups.GetValue() );

	visMgr::ShowIcons(true);
	fgtFrameMgr::Show(data.m_bFilmGatesVisible.GetValue());

	//	set the lod level
	//m_OldSubdivLevel = api3dSubdiv::GetSubdivLevel();
	cptrRenderOutputData& rodata = cptrRenderOutputDataUtil::Data();
	api3dSubdiv::SetSubdivLevel( rodata.m_nSubdivLevel.GetValue() );

	visMgr::ShowIcons(false);

	tmlnTimeLine::SetValue( tmlnTimeLine::GetMinimum() + 0.25f );

//	orthoRemoteCommandMgr::Initialize();
}


//----------------------------------------------------------------------------
//	The object should clean up things that are not needed while the
//	mode is not running in the DeInitialize function.
//----------------------------------------------------------------------------
//virtual
void orthoModeObjectManip::DeInitialize()
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

//	orthoRemoteCommandMgr::DeInitialize();

	m_bInitialized = false;
}




//----------------------------------------------------------------------------
//	The mode should do it's per frame "work" in the Think function.
//----------------------------------------------------------------------------
// virtual
void orthoModeObjectManip::Think()
{
	// update sim time to match our current value (no time flowing though)
	appSimTime::SetTime(tmlnTimeLine::GetValue(), 0.0f);

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
	//	update the state
	//
	UpdateState();

	//
	//inVirtualJoystick *pVJoy = inDeviceMgr::GetVirtualJoystick();
	//inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();
	//
	//		// debug
	//char text[64];
	//sprintf(text, "compass: part %d  manip %s", l_nCompassPart, ( l_bCompassManip ? "true":"false" ) );
	//mnmDebugInfo::SetDebugInfo(11, text);
	////sprintf(text, "pick size = %d", l_PickList.GetSize() );
	////mnmDebugInfo::SetDebugInfo(10, text);
	//sprintf(text, "state = %s", ( (l_CurrentManipState==e_StateSelect)?"Select":"Other") );
	//mnmDebugInfo::SetDebugInfo(16, text);

	//sprintf(text, "Cursor Over View = %s", tma3dCursorMgr::IsCursorOverView()?"true":"false");
	//mnmDebugInfo::SetDebugInfo(8, text);
	//sprintf(text, "mnmVJoystick::e_LeftClick = %s", pVJoy->IsPressed(mnmVJoystick::e_LeftClick)?"true":"false");
	//mnmDebugInfo::SetDebugInfo(9, text);

	////	if in a compass manip, don't select other things
	////	if not, get the pick list
	////
	//if ( !l_bCompassManip )
	//{
	//	if ( !(   pKeyboard->IsDown(inKeys::e_LALT) 
	//		   || pKeyboard->IsDown(inKeys::e_RALT)
	//		   || pKeyboard->IsDown(inKeys::e_RCTRL)
	//		   || pKeyboard->IsDown(inKeys::e_LCTRL)) )
	//	{
	//		// check to see if there is something we are selecting
	//		if ( is_left_click(pVJoy) )
	//		{
	//			// if the shift key is held, accumulate picks.
	//			//
	//			//if ( !( pKeyboard->IsHeld(inKeys::e_LSHIFT) || pKeyboard->IsHeld(inKeys::e_RSHIFT)) )
	//			{
	//				//	remove all the picks so they don't accumulate
	//				//
	//				l_PickList.Clear();
	//			}
	//			//else
	//			//{
	//			//	// don't clear the list
	//			//}

	//			if ( check_for_picks(m_PickMode) )
	//			{
	//				DBG_LOG1( "pick list size = %d", l_PickList.GetSize() );

	//				// CTRL is already used for a type of camera movement, so can't use it here.

	//				// Check CTRL key, which means "remove from selection"
	//				//bool bRemoveSelection = (  pKeyboard->IsDown(inKeys::e_LCTRL) 
	//				//						|| pKeyboard->IsDown(inKeys::e_RCTRL) );
	//				//if (bRemoveSelection)
	//				//{
	//				//	pick3dPickObject* pick_object = l_PickList.GetFirstPickObject();
	//				//	if (pick_object)
	//				//	{
	//				//		sel3dMgr::CreateUndoOperation();
	//				//		sel3dMgr::RemoveFromSelection(pick_object);
	//				//	}
	//				//}
	//				//else
	//				{
	//					// Check SHIFT key, which means "add to multiple selection"
	//					sel3dMgr::CreateUndoOperation();
	//					bool bAppendSelection = (  pKeyboard->IsDown(inKeys::e_LSHIFT) 
	//											|| pKeyboard->IsDown(inKeys::e_RSHIFT) );
	//					sel3dMgr::SetPickList( l_PickList, bAppendSelection );
	//				}

	//				set_current_state();
	//			}
	//			else
	//			{
	//				//SetStateSelect(false);
	//			}
	//		}
	//		//else
	//		//if ( pVJoy->IsPressed( mnmVJoystick::e_RightClick ) ) // && !pVJoy->IsPressed( mnmVJoystick::e_LeftClick ) )
	//		//{
	//		//	sel3dMgr::CreateUndoOperation();
	//		//	sel3dMgr::ClearSelection();
	//		//	selected_object_clear();
	//		//	SetStateSelect(false);
	//		//}
	//	}
	//}

	//
	//
//	orthoRemoteCommandMgr::CheckInput();

	bool changedScene = orthoRequestTaskUtil::ExecuteJobs();

	//
	if (!changedScene) 
	{
		if (e_StateRotate == l_CurrentManipState)
		{
			// For rotation, orient the sphere so that it surrounds all of the objects
			//	that are selected.
			set_compass_to_list( l_nCompassType, sel3dMgr::GetSelectedList() );

			cmpsSelectMgr::SetNumBoxes(0); // Turn off other highlights
		}
		else
		{
			// Otherwise, put the compass only on the most recent selection and
			//	put highlight boxes on the others.
			if (l_pSelectedObject != NULL)
			{
				set_compass_to_object( l_nCompassType, l_pSelectedObject );
			}

			// Put highlight boxes around selected objects
			mnmCompassUtil::HighlightSelected(sel3dMgr::GetSelectedList());
		}
	}

	//cmpsCompassMgr::Think( cam3dMgr::GetCamera().GetPosition() );

	//	check if the app title bar needs to be updated.
	//
	if ( l_bDirty && !docSingleDocumentMgr::NeedsSave() )
	{
		l_bDirty = false;
//		mnmAppUtil::UpdateTitleBar( l_bDirty );
	}
	else if ( !l_bDirty && docSingleDocumentMgr::NeedsSave() )
	{
		l_bDirty = true;
//		mnmAppUtil::UpdateTitleBar( l_bDirty );
	}

	//	AutoSave
	//
	mnmAutoSaveMgr::Think();

	// Set up rendering preferences based on whether we are scrubbing the timeline
	bool bForceLowRes = (tmlnTimeLine::GetIsScrubbing() 
		&& PrefsMgr::Data().m_bScrubbingLowRes.GetValue());
	rpnRenderingPrefs::SetAllLowResolution( bForceLowRes );
	rpnRenderingPrefs::SetupRenderingHints();

	// By having the base class think go last, the drivers 
	// and other things that have registered Think() interests
	// get a chance to execute after the compass has changed things.
	modeModeTime::Think();
}

//--------------------------------------------------------------------
//	SetCompassParts sets the parts of the compass that we are
//	currently using.
//--------------------------------------------------------------------
void orthoModeObjectManip::SetCompassPart(const int i_Parts)
{
	m_nCompassPart = i_Parts;

	show_compassparts( i_Parts );
}

//--------------------------------------------------------------------
//	change the scale of the compass
//--------------------------------------------------------------------
void orthoModeObjectManip::IncrementScaleOfCompass()
{
	l_fCompassScalePercent += l_cfCOMPASSSCALEPERCENT_INC;
	if (l_fCompassScalePercent > l_cfCOMPASSSCALEPERCENT_MAX) l_fCompassScalePercent = l_cfCOMPASSSCALEPERCENT_MAX;
}
void orthoModeObjectManip::DecrementScaleOfCompass()
{
	l_fCompassScalePercent -= l_cfCOMPASSSCALEPERCENT_INC;
	if (l_fCompassScalePercent < 0.0f) l_fCompassScalePercent = 0.0f;
}

//--------------------------------------------------------------------
//	move the timeline
//--------------------------------------------------------------------
void orthoModeObjectManip::TimelineIncrementFrame()
{
	//	increment time by one frame
	float newtime = tmlnTimeLine::GetValue() + tmlnTimeLine::GetFrameIncrement();
	if ( newtime <= tmlnTimeLine::GetMaximum() )
		tmlnTimeLine::SetValue( newtime );
}
void orthoModeObjectManip::TimelineDecrementFrame()
{
	//	decrement time by one frame
	float newtime = tmlnTimeLine::GetValue() - tmlnTimeLine::GetFrameIncrement();
	if ( newtime >= tmlnTimeLine::GetMinimum() )
		tmlnTimeLine::SetValue( newtime );
}

//----------------------------------------------------------------------------
//	Set States (externally)
//----------------------------------------------------------------------------
void orthoModeObjectManip::SetStateTranslate()
{
	if ( ( l_pSelectedObject != 0 ) && (l_pSelectedObject->GetTranslateFlags() != mnmObject::e_TranslateNone) )
	{
		l_CurrentManipState = e_StateTranslate;
		GotoState( m_StateTranslate );
	}
}
void orthoModeObjectManip::SetStateTranslateFreeForm()
{
	if ( ( l_pSelectedObject != 0 ) && (l_pSelectedObject->GetTranslateFlags() != mnmObject::e_TranslateNone) )
	{
		//l_CurrentManipState = e_StateTranslate;
		GotoState( m_StateTranslateFreeForm );
	}
}
void orthoModeObjectManip::SetStateRotate()
{
	if ( ( l_pSelectedObject != 0 ) && (l_pSelectedObject->GetRotateFlags() != mnmObject::e_RotateNone) )
	{
		l_CurrentManipState = e_StateRotate;
		GotoState( m_StateRotate );
	}
}
void orthoModeObjectManip::SetStateScale()
{
	if ( ( l_pSelectedObject != 0 ) && (l_pSelectedObject->GetScaleFlags() != mnmObject::e_ScaleNone) )
	{
		l_CurrentManipState = e_StateScale;
		GotoState( m_StateScale );
	}
}
void orthoModeObjectManip::SetStateSelect( bool i_bUpdateCurrentState /*= true*/ )
{
	if ( i_bUpdateCurrentState )
	{
		l_CurrentManipState = e_StateSelect;
		//DBG_LOG0("--------> select");
	}
	GotoState( m_StateSelect );
}
void orthoModeObjectManip::SetStatePlacement()
{
	if ( l_pSelectedObject != 0 )
	{
		l_CurrentManipState = e_StatePlacement;
		GotoState( m_StatePlacement );
	}
}


//--------------------------------------------------------------------
// select the next object in the selected list
//--------------------------------------------------------------------
void orthoModeObjectManip::SelectNextObjectInPickList()
{
	sel3dMgr::CreateUndoOperation();
	pick3dPickObject* pItem = sel3dMgr::SetNextToSelected();
	l_pSelectedObject = cast_selected_to_mnmobject();
	if (l_pSelectedObject != 0)
		selected_object_set();
}


//--------------------------------------------------------------------
//	set the current state based on the last state of the object
//--------------------------------------------------------------------
void orthoModeObjectManip::set_current_state()
{
	switch( l_CurrentManipState )
	{
		case orthoModeObjectManip::e_StateTranslate:
			SetStateTranslate();
			//DBG_LOG0( "setting...translate");
			break;
		case orthoModeObjectManip::e_StateScale:
			SetStateScale();
			//DBG_LOG0( "setting...scale");
			break;
		case orthoModeObjectManip::e_StateRotate:
			SetStateRotate();
			//DBG_LOG0( "setting...rotate");
			break;
		case orthoModeObjectManip::e_StateSelect:
			SetStateSelect();
			//DBG_LOG0( "setting...select");
			break;
	}
}

//--------------------------------------------------------------------
//	Configure compass for a given state. This is called each time
//	the selected object changes (but the state doesn't)
//--------------------------------------------------------------------
void orthoModeObjectManip::SetCompassTranslateFreeForm()
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
void orthoModeObjectManip::SetCompassTranslate()
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
void orthoModeObjectManip::SetCompassRotate()
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
void orthoModeObjectManip::SetCompassScale()
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
void orthoModeObjectManip::SetCompassSelect()
{
}
void orthoModeObjectManip::SetCompassPlacement()
{
}


//
//	state functions
//

//----------------------------------------------------------------------------
//	BeginStateTranslate - is executed ONCE at the start of the state
//----------------------------------------------------------------------------
//virtual
void orthoModeObjectManip::BeginStateTranslate()
{
	DBG_LOG1("Translate for %x", &l_pSelectedObject);

	//	set the object
	//l_pSelectedObject = cast_selected_to_mnmobject();
	//DBG_ASSERT0( l_pSelectedObject != 0, "NULL object in modeObjectManip state" );

	//	set up the compass
	l_nCompassType = cmpsCompassMgr::e_Translate;

	SetCompassTranslate();

	set_compass_to_object( l_nCompassType, l_pSelectedObject );
	//guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Mode, "Translate" );
}

//----------------------------------------------------------------------------
//	OnStateTranslate - is executed every frame while in this state
//----------------------------------------------------------------------------
//virtual
void orthoModeObjectManip::OnStateTranslate()
{
	// debug
	char text[64];
	sprintf(text, "Mode: Obj Manip State: Translate" );
	mnmDebugInfo::SetDebugInfo(12, text);

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
			end_interaction = true;
		else if (( pVJoy->IsPressed( mnmVJoystick::e_RightClick ) && pVJoy->IsHeld( mnmVJoystick::e_LeftClick ) )
			|| pKeyboard->IsPressed( inKeys::e_ESC ) )
		{
			// User aborted moving, restore original position

			if (sel3dMgr::GetNumSelected() > 1)
				undoUndoMgr::BeginMultipleOperationBlock();

			// Extract out the frame delta movement for this object
			// in order to apply it to the other selected objects
			maVector3d delta_pos = m_OriginalPosition - l_pSelectedObject->GetPosition();
			const bool new_operation = false;
			apply_delta_position(delta_pos, new_operation);

			l_pSelectedObject->UpdatePosition( m_OriginalPosition, new_operation );
			cmpsCompassMgr::SetPosition( cmpsCompassMgr::e_Translate, m_OriginalPosition  );
			end_interaction = true;

			if (sel3dMgr::GetNumSelected() > 1)
				undoUndoMgr::EndMultipleOperationBlock();
		}

		// if interacting, but not enabled at the given time, should the interaction be ended or just ignored?
		//if (!compass_enabled) end_interaction = true;

		if (end_interaction)
		{
			l_bCompassManip = false;
			show_compassparts( m_nSavedCompassParts );
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

				// save the current state of the compass so we can restore it later
				//SetSavedCompassParts( cmpsCompassMgr::GetParts(l_nCompassType) );
				SetSavedCompassParts( m_nCompassPart );
				show_compassparts( l_nCompassPart );

				//DBG_LOG2( " cmps(%d) full(%d)", l_nCompassPart, m_nCompassPart );

				SetMouseDownPoint( pos );

				m_OriginalPosition	= l_pSelectedObject->GetPosition();
				m_SelectionOffset	= m_OriginalPosition - m_MouseDownPoint;
			}
		}

			//debug
		//sprintf(text, "original pos (%6.3f,%6.3f,%6.3f)", m_OriginalPosition.GetX(), m_OriginalPosition.GetY(), m_OriginalPosition.GetZ()  );
		//mnmDebugInfo::SetDebugInfo(20, text);

		//	if the compass is being manipulated
		//
		if ( l_bCompassManip )
		{
			// Get the axis being moved
			//
			maVector3d axis;
			switch ( l_nCompassPart )
			{
				case cmpsCompass::e_X:
					axis.Set( 1, 0, 0 );
					break;
				case cmpsCompass::e_Y:
					axis.Set( 0, 1, 0 );
					break;
				case cmpsCompass::e_Z:
					axis.Set( 0, 0, 1 );
					break;
			}

			// Calculate the change in position
			maPoint3d intersect_point;
			tma3dCursorMgr::PlaneIntersection(	m_MouseDownPoint,
												-( cam3dMgr::GetCamera().GetDirection() ),
												intersect_point );
			maVector3d Diff = intersect_point - m_MouseDownPoint;
			float distance = axis * Diff;
			maVector3d new_position = m_OriginalPosition + axis * distance;
			
			if (sel3dMgr::GetNumSelected() > 1)
				undoUndoMgr::BeginMultipleOperationBlock();

			// Extract out the frame delta movement for this object
			// in order to apply it to the other selected objects
			maVector3d delta_pos = new_position - l_pSelectedObject->GetPosition();
			apply_delta_position(delta_pos, bNewOperation);

			// Set the new position
			l_pSelectedObject->UpdatePosition( new_position, bNewOperation );
			cmpsCompassMgr::SetPosition( cmpsCompassMgr::e_Translate, new_position  );

			if (sel3dMgr::GetNumSelected() > 1)
				undoUndoMgr::EndMultipleOperationBlock();

				//debug
			//sprintf(text, "new    pos   (%6.3f,%6.3f,%6.3f)", new_position.GetX(), new_position.GetY(), new_position.GetZ()  );
			//mnmDebugInfo::SetDebugInfo(21, text);
			//(text, "object pos   (%6.3f,%6.3f,%6.3f)", l_pSelectedObject->GetPosition().GetX(), l_pSelectedObject->GetPosition().GetY(), l_pSelectedObject->GetPosition().GetZ()  );
			//mnmDebugInfo::SetDebugInfo(22, text);
		}
	}

}


//----------------------------------------------------------------------------
//	EndStateTranslate - is executed ONCE on ending the state
//----------------------------------------------------------------------------
//virtual
void orthoModeObjectManip::EndStateTranslate()
{
	cmpsCompassMgr::SetRenderable( cmpsCompassMgr::e_Translate, false );
}


//----------------------------------------------------------------------------
//	BeginStateTranslateFreeForm - is executed ONCE at the start of the state
//----------------------------------------------------------------------------
//virtual
void orthoModeObjectManip::BeginStateTranslateFreeForm()
{
	//	set the selected object.
	//l_pSelectedObject = cast_selected_to_mnmobject();
	//DBG_ASSERT0( l_pSelectedObject != 0, "NULL object in modeObjectManip state" );

	//	set up the compass
	l_nCompassType = cmpsCompassMgr::e_Translate;

	SetCompassTranslate();

	set_compass_to_object( l_nCompassType, l_pSelectedObject );
	//guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Mode, "Free-Form" );
}

//----------------------------------------------------------------------------
//	OnStateTranslateFreeForm - is executed every frame while in this state
//----------------------------------------------------------------------------
//virtual
void orthoModeObjectManip::OnStateTranslateFreeForm()
{
		// debug
	char text[64];
	sprintf(text, "Mode: Obj Manip State: TranslateFreeForm" );
	mnmDebugInfo::SetDebugInfo(12, text);

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
			end_interaction = true;
		else if (( pVJoy->IsPressed( mnmVJoystick::e_RightClick ) && pVJoy->IsHeld( mnmVJoystick::e_LeftClick ) )
			|| pKeyboard->IsPressed( inKeys::e_ESC ) )
		{
			// User aborted moving, restore original position

			if (sel3dMgr::GetNumSelected() > 1)
				undoUndoMgr::BeginMultipleOperationBlock();

			// Extract out the frame delta movement for this object
			// in order to apply it to the other selected objects
			maVector3d delta_pos = m_OriginalPosition - l_pSelectedObject->GetPosition();
			const bool new_operation = false;
			apply_delta_position(delta_pos, new_operation);

			l_pSelectedObject->UpdatePosition( m_OriginalPosition, new_operation );
			cmpsCompassMgr::SetPosition( cmpsCompassMgr::e_Translate, m_OriginalPosition  );
			end_interaction = true;

			if (sel3dMgr::GetNumSelected() > 1)
				undoUndoMgr::EndMultipleOperationBlock();
		}

		// if interacting, but not enabled at the given time, should the interaction be ended or just ignored?
		//if (!compass_enabled) end_interaction = true;

		if (end_interaction)
		{
			l_bCompassManip = false;
			show_compassparts( m_nSavedCompassParts );
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

				DBG_LOG2( "-cmps(%d) full(%d)", l_nCompassPart, m_nCompassPart );

				SetMouseDownPoint( pos );

				m_OriginalPosition = l_pSelectedObject->GetPosition();
				m_SelectionOffset = m_OriginalPosition - m_MouseDownPoint;
			}
		}

		//	if the compass is being manipulated
		//
		if ( l_bCompassManip )
		{
			maPoint3d intersect_point;
			tma3dCursorMgr::PlaneIntersection( m_MouseDownPoint,
											maVector3d( 0, 1, 0 ),
											intersect_point );

			maPoint3d new_position = intersect_point + m_SelectionOffset;

			if (sel3dMgr::GetNumSelected() > 1)
				undoUndoMgr::BeginMultipleOperationBlock();

			// Extract out the frame delta movement for this object
			// in order to apply it to the other selected objects
			maVector3d delta_pos = new_position - l_pSelectedObject->GetPosition();
			apply_delta_position(delta_pos, bNewOperation);

			l_pSelectedObject->UpdatePosition( new_position, bNewOperation );
			cmpsCompassMgr::SetPosition( cmpsCompassMgr::e_Translate, new_position  );

			if (sel3dMgr::GetNumSelected() > 1)
				undoUndoMgr::EndMultipleOperationBlock();
		}
	}
}

//----------------------------------------------------------------------------
//	EndStateTranslateFreeForm - is executed ONCE on ending the state
//----------------------------------------------------------------------------
//virtual
void orthoModeObjectManip::EndStateTranslateFreeForm()
{
	cmpsCompassMgr::SetRenderable( cmpsCompassMgr::e_Translate, false );
}

//----------------------------------------------------------------------------
//	BeginStateRotate - is executed ONCE at the start of the state
//----------------------------------------------------------------------------
//virtual
void orthoModeObjectManip::BeginStateRotate()
{
	//	set the selected object
	//l_pSelectedObject = cast_selected_to_mnmobject();
	//DBG_ASSERT0( l_pSelectedObject != 0, "NULL object in modeObjectManip state" );

	//	set up the compass
	l_nCompassType = cmpsCompassMgr::e_Rotate;

	SetCompassRotate();

	set_compass_to_object( l_nCompassType, l_pSelectedObject );
	//guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Mode, "Rotate" );
}

//----------------------------------------------------------------------------
//	OnStateRotate - is executed every frame while in this state
//----------------------------------------------------------------------------
//virtual
void orthoModeObjectManip::OnStateRotate()
{
		// debug
	char text[64];
	sprintf(text, "Mode: Obj Manip State: Rotate" );
	mnmDebugInfo::SetDebugInfo(12, text);

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
			end_interaction = true;
		else if (( pVJoy->IsPressed( mnmVJoystick::e_RightClick ) && pVJoy->IsHeld( mnmVJoystick::e_LeftClick ) )
			|| pKeyboard->IsPressed( inKeys::e_ESC ) )
		{
			//FIX - If multiple selection, then need to reset orientation of
			// other objects in selection.

			// User aborted moving
			l_pSelectedObject->UpdateOrientation( m_OriginalOrientation );
			cmpsCompassMgr::SetOrientation( cmpsCompassMgr::e_Rotate, m_OriginalOrientation  );
			end_interaction = true;
		}

		// if interacting, but not enabled at the given time, should the interaction be ended or just ignored?
		//if (!compass_enabled) end_interaction = true;

		if (end_interaction)
		{
			l_bCompassManip = false;
			show_compassparts( m_nSavedCompassParts );
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

				SetMouseDownPoint( pos );

				//
				m_OriginalOrientation = l_pSelectedObject->GetOrientation();

				// set the pivot point based on the selection list
				m_PivotPoint = compute_pivot_point();

				// save axis of rotation
				m_RotationAxis = get_rotation_axis( l_pSelectedObject->GetOrientation(), l_nCompassPart);

				int dummy;
				tma3dCursorMgr::GetCursorPos( m_StartRotationValue, dummy );

				cmpsCompassMgr::SetOrientation( cmpsCompassMgr::e_Rotate, m_OriginalOrientation );
			}
		}

		//	if the compass is being manipulated
		//
		if ( l_bCompassManip )
		{
			// Get the difference between the two points
			int dummy;
			int cur_pos;
			tma3dCursorMgr::GetCursorPos(cur_pos, dummy);

			maRotation rot = get_rotation_from_direction( m_RotationAxis,
					float(cur_pos - m_StartRotationValue) * lc_fRotateIncrement );

			sprintf(text, "start rot(%6.3f), curpos( %6.3f)", m_StartRotationValue, cur_pos );
			mnmDebugInfo::SetDebugInfo(20, text);

			m_StartRotationValue = cur_pos;

			if (sel3dMgr::GetNumSelected() > 1)
				undoUndoMgr::BeginMultipleOperationBlock();

			// Apply delta rotation to all selected objects
			apply_delta_rotation(rot, m_PivotPoint, bNewOperation);

			maRotation new_rot = rot * l_pSelectedObject->GetOrientation();
			cmpsCompassMgr::SetOrientation( cmpsCompassMgr::e_Rotate, new_rot  );

			if (sel3dMgr::GetNumSelected() > 1)
				undoUndoMgr::EndMultipleOperationBlock();

				//debug
//			sprintf(text, "new rot(%6.3f,%6.3f,%6.3f)", rot.GetX(), rot.GetY(), rot.GetZ()  );
//			mnmDebugInfo::SetDebugInfo(21, text);
//			sprintf(text, "obj rot(%6.3f,%6.3f,%6.3f)", l_pSelectedObject->GetOrientation().GetX(), l_pSelectedObject->GetOrientation().GetY(), l_pSelectedObject->GetOrientation().GetZ()  );
//			mnmDebugInfo::SetDebugInfo(22, text);
		}
	}
}

//----------------------------------------------------------------------------
//	EndStateRotate - is executed ONCE on ending the state
//----------------------------------------------------------------------------
//virtual
void orthoModeObjectManip::EndStateRotate()
{
	cmpsCompassMgr::SetRenderable( cmpsCompassMgr::e_Rotate, false );
}

//----------------------------------------------------------------------------
//	BeginStateScale - is executed ONCE at the start of the state
//----------------------------------------------------------------------------
//virtual
void orthoModeObjectManip::BeginStateScale()
{
	//	set the selected object
	//l_pSelectedObject = cast_selected_to_mnmobject();
	//DBG_ASSERT0( l_pSelectedObject != 0, "NULL object in modeObjectManip state" );

	l_pSelectedObject->UpdatePosition( l_pSelectedObject->GetPosition() );

	l_nCompassType = cmpsCompassMgr::e_Scale;

	SetCompassScale();

	set_compass_to_object( l_nCompassType, l_pSelectedObject );
	//guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Mode, "Scale" );
}

//----------------------------------------------------------------------------
//	OnStateScale - is executed every frame while in this state
//----------------------------------------------------------------------------
//virtual
void orthoModeObjectManip::OnStateScale()
{
		// debug
	char text[64];
	sprintf(text, "Mode: Obj Manip State: Scale" );
	mnmDebugInfo::SetDebugInfo(12, text);

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
			end_interaction = true;
		else if (( pVJoy->IsPressed( mnmVJoystick::e_RightClick ) && pVJoy->IsHeld( mnmVJoystick::e_LeftClick ) )
			|| pKeyboard->IsPressed( inKeys::e_ESC ) )
		{
			// User aborted moving
			l_pSelectedObject->UpdateScale( m_OriginalScale );
			//cmpsCompassMgr::SetScale( cmpsCompassMgr::e_Scale, m_OriginalScale  );
			end_interaction = true;
		}

		// if interacting, but not enabled at the given time, should the interaction be ended or just ignored?
		//if (!compass_enabled) end_interaction = true;

		if (end_interaction)
		{
			l_bCompassManip = false;
			show_compassparts( m_nSavedCompassParts );
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

		if ( currCompassPartSelected != -1 )
		{
			if (  is_left_click(pVJoy)  )
			{
				l_bCompassManip = true;
				l_nCompassPart = currCompassPartSelected;

				// save the current state of the compass so we can restore it later
				//SetSavedCompassParts( cmpsCompassMgr::GetParts(l_nCompassType) );
				SetSavedCompassParts( m_nCompassPart );
				show_compassparts( l_nCompassPart );

				SetMouseDownPoint( pos );

				//	save the scale
				m_OriginalScale = l_pSelectedObject->GetScale();

				maAxisBox box = l_pSelectedObject->GetWorldBox();
				switch ( l_nCompassPart )
				{
					case cmpsCompass::e_X:
						m_fOriginalSize = box.GetDiffX();
						break;
					case cmpsCompass::e_Y:
						m_fOriginalSize = box.GetDiffY();
						break;
					case cmpsCompass::e_Z:
						m_fOriginalSize = box.GetDiffZ();
						break;
					default:
						DBG_LOG1( "Invalid compass part %d for scale", l_nCompassPart );
						m_fOriginalSize = box.GetMaxY() - box.GetMinY();
						break;
				}
				//DBG_LOG4( "OS(%6.3f) = maxy %6.3f - miny %6.3f for part %d", m_fOriginalSize, box.GetMaxY(), box.GetMinY(), l_nCompassPart );
			}
		}

		//	if the compass is being manipulated
		//
		if ( l_bCompassManip )
		{
			// Get the axis being moved
			maVector3d axis;
			switch ( l_nCompassPart )
			{
				case cmpsCompass::e_X:
					axis.Set( 1, 0, 0 );
					break;
				case cmpsCompass::e_Y:
					axis.Set( 0, 1, 0 );
					break;
				case cmpsCompass::e_Z:
					axis.Set( 0, 0, 1 );
					break;
			}

			// Move local axis into world space
			l_pSelectedObject->GetOrientation().RotateVector(axis);

			// Calculate the change in position
			maPoint3d intersect_point;
			tma3dCursorMgr::PlaneIntersection(	GetMouseDownPoint(),
												-( cam3dMgr::GetCamera().GetDirection() ),
												intersect_point );

			// Scale is measured by distance to the origin of the object
			// (Can we use the pivot point here in the future?)
			float orig_dist = axis * (GetMouseDownPoint() - l_pSelectedObject->GetWorldPivot());
			float cur_dist = axis * (intersect_point - l_pSelectedObject->GetWorldPivot());
			float scale_delta = (orig_dist > 0) ? cur_dist / orig_dist : 1.0f;

			const float c_fMinScale = 0.05f;
			if (scale_delta < c_fMinScale)
				scale_delta = c_fMinScale;

			// This is uniform scale, should handle non-uniform also?
			maPoint3d scaleaxis = m_OriginalScale * scale_delta;
			l_pSelectedObject->UpdateScale( scaleaxis );

				//debug
			sprintf(text, "orig_dist(%6.3f) cur_dist(%6.3f) scale_delta(%6.3f)", orig_dist, cur_dist, scale_delta );
			mnmDebugInfo::SetDebugInfo(20, text);
			sprintf(text, "new scale(%6.3f,%6.3f,%6.3f)", scaleaxis.GetX(), scaleaxis.GetY(), scaleaxis.GetZ()  );
			mnmDebugInfo::SetDebugInfo(21, text);
			(text, "obj scale(%6.3f,%6.3f,%6.3f)", l_pSelectedObject->GetScale().GetX(), l_pSelectedObject->GetScale().GetY(), l_pSelectedObject->GetScale().GetZ()  );
			mnmDebugInfo::SetDebugInfo(22, text);
		}
	}
}

//----------------------------------------------------------------------------
//	EndStateScale - is executed ONCE on ending the state
//----------------------------------------------------------------------------
//virtual
void orthoModeObjectManip::EndStateScale()
{
	cmpsCompassMgr::SetRenderable( cmpsCompassMgr::e_Scale, false );
}

//----------------------------------------------------------------------------
//	BeginStateSelect - is executed ONCE at the start of the state
//----------------------------------------------------------------------------
//virtual
void orthoModeObjectManip::BeginStateSelect()
{
	if ( l_pSelectedObject )
		selected_object_set();
	SetCompassSelect();
//	guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Mode, "Select" );
}

//----------------------------------------------------------------------------
//	OnStateSelect - is executed every frame while in this state
//----------------------------------------------------------------------------
//virtual
void orthoModeObjectManip::OnStateSelect()
{
		// debug
	char text[64];
	sprintf(text, "Mode: Obj Manip State: Select" );
	mnmDebugInfo::SetDebugInfo(12, text);

	//	// debug
	//maPoint3d pos,tar;
	//pos = cam3dMgr::GetCamera().GetPosition();
	//tar = cam3dMgr::GetCamera().GetTarget();
	////char text[64];
	//sprintf(text, "cam (%06.3f, %06.3f %06.3f)  target (%06.3f, %06.3f %06.3f)", pos.GetX(), pos.GetY(), pos.GetZ(), tar.GetX(), tar.GetY(), tar.GetZ() );
	//mnmDebugInfo::SetDebugInfo(15, text);

	// TODO: optimize this so it doesn't happen all the time.
	//
	sprintf(text, "" );	//debug
	pick3dPickObject* pPObj = sel3dMgr::GetSelected();
	if ( pPObj )
	{
		mnmObject* pObj = dynamic_cast<mnmObject*>( pPObj );
		//DBG_ASSERT0( pObj != 0, "Selected object couldn't cast to an mnmObject" );
		if (pObj)
		{
			sprintf(text, "object selected %x (%06.3f, %06.3f, %06.3f)", pPObj, pObj->GetPosition().GetX(), pObj->GetPosition().GetY(), pObj->GetPosition().GetZ() );	//debug
		}
	}
	mnmDebugInfo::SetDebugInfo(13, text);	//debug

//	l_pSelectedObjectLast	= l_pSelectedObject;
//	l_pSelectedObject		= cast_selected_to_mnmobject();


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
void orthoModeObjectManip::EndStateSelect()
{
	cmpsCompassMgr::SetRenderable( cmpsCompassMgr::e_Select, false );
}

//----------------------------------------------------------------------------
//	BeginStatePlacement - is executed ONCE at the start of the state
//----------------------------------------------------------------------------
//virtual
void orthoModeObjectManip::BeginStatePlacement()
{
	SetCompassPlacement();
	//guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Mode, "Placement" );
}

//----------------------------------------------------------------------------
//	OnStatePlacement - is executed every frame while in this state
//----------------------------------------------------------------------------
//virtual
void orthoModeObjectManip::OnStatePlacement()
{
		// debug
	char text[64];
	sprintf(text, "Mode: Obj Manip State: Placement" );
	mnmDebugInfo::SetDebugInfo(12, text);

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
	sprintf(text, "" );	//debug
	pick3dPickObject* pPObj = sel3dMgr::GetSelected();
	if ( pPObj )
	{
		sprintf(text, "object Placement" );	//debug
	}
	mnmDebugInfo::SetDebugInfo(13, text);	//debug

	//	if there is an object selected.
	//
	if ( l_pSelectedObject )
	{
		//	move the object based on the mouse cursor
		//
		//maPoint3d new_pos;
		//if ( get_intersection( new_pos ) )
		//{
		//	l_pSelectedObject->UpdatePosition( new_pos );
		//}

		maPoint3d intersect_point;
		tma3dCursorMgr::PlaneIntersection( m_MouseDownPoint,
										   maVector3d( 0, 1, 0 ),
										   intersect_point );

		maPoint3d new_position = intersect_point; // + m_SelectionOffset;

		// temp
		if ( new_position.Length() > 5000.0f )
		{
			new_position.Normalize();
			new_position = new_position * 20.0f;
		}

		if (sel3dMgr::GetNumSelected() > 1)
			undoUndoMgr::BeginMultipleOperationBlock();

		// Extract out the frame delta movement for this object
		// in order to apply it to the other selected objects
		maVector3d delta_pos = new_position - l_pSelectedObject->GetPosition();
		const bool new_operation = false;
		apply_delta_position(delta_pos, new_operation);

		// now update the position
		l_pSelectedObject->UpdatePosition( new_position, new_operation );

		if (sel3dMgr::GetNumSelected() > 1)
			undoUndoMgr::EndMultipleOperationBlock();

		// debug
		sprintf(text, "new pos (%6.3f, %6.3f, %6.3f)", new_position.GetX(), new_position.GetY(), new_position.GetZ() );
		mnmDebugInfo::SetDebugInfo(21, text);

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
void orthoModeObjectManip::EndStatePlacement()
{
}

