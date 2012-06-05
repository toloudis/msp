/****************************************************************************\
**  mnmCompassUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/mnm/mnmCompassUtil.hpp"

//#include "Support/mnm/mnmDebugInfo.hpp"
#include "Support/mnm/mnmObject.hpp"

#include "Support/cmps/cmpsCompassMgr.hpp"
#include "Support/cmps/cmpsSelectMgr.hpp"

//	tool library
#include "Tool/cam3d/cam3dMgr.hpp"

//	library
#include "Core/dbg/dbgLog.hpp"


//------------------------------------------------------------------------
//	UpdateCompass - update the specified compass appropriately
//------------------------------------------------------------------------
void mnmCompassUtil::UpdateCompass( int i_CompassType, const camCamera &i_Camera )
{
	if ( cmpsCompassMgr::GetNumberOfCompasses() == 0 )
		return;

	switch (i_CompassType)
	{
		case cmpsCompassMgr::e_World:
			{
				// set the orientation of the screen compass
				maMatrix4x4 matrix;
				maRotation rot;
				i_Camera.GetCameraMatrix(matrix);
				rot.SetValue( matrix );
				cmpsCompassMgr::SetOrientation( i_CompassType, rot );

				// This axis is in camera space. In order to keep it in the 
				// lower left corner, we need to adjust when the camera's
				// aspect ratio changes.
				float aspect = i_Camera.GetAspect();
				float yoff = (aspect > 0) ? (-2.0f / aspect) : -1.5f; 
				cmpsCompassMgr::SetPosition( i_CompassType, maPoint3d(2.1f,yoff,2.5f) );
			}
			break;
		default:
		case cmpsCompassMgr::e_Rotate:
		case cmpsCompassMgr::e_Scale:
		case cmpsCompassMgr::e_Translate:
		case cmpsCompassMgr::e_Select:
			break;
	}
}


//------------------------------------------------------------------------
//	SetUpCompass - set up the compass to render for an object
//------------------------------------------------------------------------
void mnmCompassUtil::SetUpCompass( int i_CompassType, mnmObject* i_pObject )
{
	DBG_ASSERT0( i_pObject, "NULL object sent to setupcompass()" );

	if ( cmpsCompassMgr::GetNumberOfCompasses() == 0 )
		return;

	//maPoint3d pos = i_pObject->GetPosition();
	maAxisBox box = i_pObject->GetWorldBox();
	maPoint3d pos = box.GetCenter();

	maPoint3d camera_pos = cam3dMgr::GetCamera().GetPosition();

	//DBG_LOG3( " pos(%6.3f,%6.3f,%6.3f)", pos.GetX(), pos.GetY(), pos.GetZ() );
	//DBG_LOG3( " box(%6.3f,%6.3f,%6.3f)", pt.GetX(), pt.GetY(), pt.GetZ() );

	cmpsCompassMgr::SetPosition( i_CompassType, i_pObject->GetPosition() );
	cmpsCompassMgr::SetOrientation( i_CompassType, i_pObject->GetOrientation() );
	cmpsCompassMgr::SetBounds( i_CompassType, i_pObject->GetWorldBox(), 
		i_pObject->GetWorldPivot(), camera_pos );

	// If object is on back side of camera, do not render the compass.
	// (How do we handle this for multiple views and multiple cameras?)
	maVector3d dir = cam3dMgr::GetCamera().GetDirection();
	bool bInFrontOfCamera = ((pos - camera_pos) * dir > 0);
	cmpsCompassMgr::SetRenderable( i_CompassType, bInFrontOfCamera );

	// debug
	//char text[64];
	//sprintf(text, "Compass: visible (%s)", (bInFrontOfCamera ? "true":"false") );
	//mnmDebugInfo::SetDebugInfo(7, text);
	//sprintf(text, "Compass Camera: (%6.2f, %6.2f, %6.2f)", camera_pos.GetX(), camera_pos.GetY(), camera_pos.GetZ() );
	//mnmDebugInfo::SetDebugInfo(8, text);
}

//------------------------------------------------------------------------
//	SetUpCompass - set up the compass to surround the bounding box
//		defined by all of the selected objects
//------------------------------------------------------------------------
void mnmCompassUtil::SetUpCompass( int i_CompassType, 
								   const std::list<pick3dPickObject*> &i_SelectedList )
{
	int count = 0;
	maAxisBox combined_box;
	mnmObject *pFirstObject = NULL;
	std::list<pick3dPickObject*>::const_iterator it, end = i_SelectedList.end();
	for (it = i_SelectedList.begin(); it != end; ++it)
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
		// Orient compass to box and its center point
		maPoint3d center = combined_box.GetCenter();
		maPoint3d camera_pos = cam3dMgr::GetCamera().GetPosition();

		cmpsCompassMgr::SetPosition( i_CompassType, center );
		cmpsCompassMgr::SetOrientation( i_CompassType, pFirstObject->GetOrientation() );

		// Use center of bounding box as pivot
		cmpsCompassMgr::SetBounds( i_CompassType, combined_box, 
			center, camera_pos );

		// If object is on back side of camera, do not render the compass.
		// (How do we handle this for multiple views and multiple cameras?)
		maVector3d dir = cam3dMgr::GetCamera().GetDirection();
		bool bInFrontOfCamera = ((center - camera_pos) * dir > 0);
		cmpsCompassMgr::SetRenderable( i_CompassType, bInFrontOfCamera );
	}
	else if ((count == 1) && (pFirstObject != NULL))
	{
		// If only one object, use the other SetUpCompass function
		mnmCompassUtil::SetUpCompass(i_CompassType, pFirstObject);
	}
}

//------------------------------------------------------------------------
//	HighlightSelected - put boxes around selected objects
//------------------------------------------------------------------------
void mnmCompassUtil::HighlightSelected(const std::list<pick3dPickObject*> &i_SelectedList)
{
	// First, get a count of the number of highlights needed;
	int count = 0;
	std::list<pick3dPickObject*>::const_iterator it, end = i_SelectedList.end();
	for (it = i_SelectedList.begin(); it != end; ++it)
	{
		// Skip first object in list, this will have a different compass
		// on it based on the mode
		if (it == i_SelectedList.begin())
			continue;

		if (dynamic_cast<mnmObject*>(*it))
			count++;
	}

	cmpsSelectMgr::SetNumBoxes(count);

	// Set size of bounding boxes for each object
	maPoint3d camera_pos = cam3dMgr::GetCamera().GetPosition();
	int ind = 0;
	for (it = i_SelectedList.begin(); it != end; ++it)
	{
		// Skip first object in list, this will have a different compass
		// on it based on the mode
		if (it == i_SelectedList.begin())
			continue;

		if (mnmObject* pObject = dynamic_cast<mnmObject*>(*it))
		{
			cmpsSelectMgr::SetBounds( ind++, pObject->GetWorldBox(), 
				pObject->GetWorldPivot(), camera_pos );	
		}
	}
}
