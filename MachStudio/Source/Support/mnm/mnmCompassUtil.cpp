/****************************************************************************\
**  mnmCompassUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/mnm/mnmCompassUtil.hpp"

//#include "Support/mnm/mnmDebugInfo.hpp"
#include "Support/mnm/mnmObject.hpp"

#include "Support/cmps/cmpsCompassMgr.hpp"
#include "Support/cmps/cmpsSelectMgr.hpp"

//	tool library
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/icn/icnIconLayer.hpp"
#include "Tool/sel3d/sel3dObject.hpp"

//	library

namespace
{
	//------------------------------------------------------------------------
	// Find out how many objects inthe selected list can be 
	// manipulated with compasses (are derived from mnmObject).
	// Returns first object found within io_pFirstObject
	//------------------------------------------------------------------------
	int count_mnm_objects(const std::list<sel3dObject*> &i_SelectedList,
						  mnmObject* &io_pFirstObject)
	{
		int count = 0;
		io_pFirstObject = NULL;
		std::list<sel3dObject*>::const_iterator it, end = i_SelectedList.end();
		for (it = i_SelectedList.begin(); it != end; ++it)
		{
			if (mnmObject* pObject = dynamic_cast<mnmObject*>(*it))
			{
				if (!io_pFirstObject) io_pFirstObject = pObject; // record the top selection object
				count++;
			}
		}
		return count;
	}
}

//------------------------------------------------------------------------
//	UpdateCompass - update the specified compass appropriately
//------------------------------------------------------------------------
//void mnmCompassUtil::UpdateCompass( int i_CompassType, const camCamera &i_Camera )
//{
//	if ( cmpsCompassMgr::GetNumberOfCompasses() == 0 )
//		return;
//
//	switch (i_CompassType)
//	{
//		case cmpsCompassMgr::e_World:
//			{
//				// set the orientation of the screen compass
//				maMatrix4x4 matrix;
//				maRotation rot;
//				i_Camera.GetCameraMatrix(matrix);
//				rot.SetValue( matrix );
//				cmpsCompassMgr::SetOrientation( i_CompassType, rot );
//
//				// This axis is in camera space. In order to keep it in the 
//				// lower left corner, we need to adjust when the camera's
//				// aspect ratio changes.
//				float aspect = i_Camera.GetAspect();
//				float yoff = (aspect > 0) ? (-2.0f / aspect) : -1.5f; 
//				cmpsCompassMgr::SetPosition( i_CompassType, maPoint3d(2.1f,yoff,2.5f) );
//			}
//			break;
//		default:
//		case cmpsCompassMgr::e_Rotate:
//		case cmpsCompassMgr::e_Scale:
//		case cmpsCompassMgr::e_Translate:
//		case cmpsCompassMgr::e_Select:
//			break;
//	}
//}


//------------------------------------------------------------------------
//	SetUpCompass - set up the compass to render for an object
//------------------------------------------------------------------------
void mnmCompassUtil::SetUpCompass( int i_CompassType, 
								   mnmObject* i_pObject,
								   float i_ScalingFactor )
{
	DBG_ASSERT( i_pObject, "NULL object sent to setupcompass()" );

	if ( cmpsCompassMgr::GetNumberOfCompasses() == 0 )
		return;

	// Orientation is the same for all camera views
	cmpsCompassMgr::SetOrientation( i_CompassType, i_pObject->GetWorldOrientation());

	// Need to turn on the visibility of the compass in general,
	// each view might turn off their copy.
	cmpsCompassMgr::SetRenderable( i_CompassType, true );

	// The selected object (i.e. point light icons) may change size depending 
	// on the camera position, so you can only really ask for the bounding box
	// of the object by first giving it a camera. With multiple render panels,
	// we have to do this for each camera for each panel. The different panels
	// are represented by icon layers.
	const int num_icon_layers = icnIconLayer::GetNumIconLayers();
	for (int li=0; li<num_icon_layers; ++li)
	{
		const camCamera *pCamera = icnIconLayer::GetCameraForIconLayer(li);
		if (!pCamera) pCamera = &(cam3dMgr::GetCamera());

		maAxisBox box = i_pObject->GetWorldBox(li);
		maPoint3d camera_pos = pCamera->GetPosition();

		cmpsCompassMgr::AdjustCompassToBounds( i_CompassType, li,
			i_pObject->GetPosition(), box, 
			i_pObject->GetWorldPivot(), *pCamera, i_ScalingFactor );

		// If object is on back side of camera, do not render the compass.
		// (How do we handle this for multiple views and multiple cameras?)
		maVector3d dir = pCamera->GetDirection();
		maPoint3d pos = box.GetCenter();
		bool bInFrontOfCamera = ((pos - camera_pos) * dir > 0);
		cmpsCompassMgr::SetLayerRenderable( i_CompassType, li, bInFrontOfCamera );
	}

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
								   const std::list<sel3dObject*> &i_SelectedList,
								   float i_ScalingFactor )
{
	mnmObject *pFirstObject = NULL;
	int count = count_mnm_objects(i_SelectedList, pFirstObject);
	if (count > 1)
	{
		// Orientation is the same for all camera views
		cmpsCompassMgr::SetOrientation( i_CompassType, pFirstObject->GetWorldOrientation());
		
		// Need to turn on the visibility of the compass in general,
		// each view might turn off their copy.
		cmpsCompassMgr::SetRenderable( i_CompassType, true );

		// The selected object (i.e. point light icons) may change size depending 
		// on the camera position, so you can only really ask for the bounding box
		// of the object by first giving it a camera. With multiple render panels,
		// we have to do this for each camera for each panel. The different panels
		// are represented by icon layers.
		const int num_icon_layers = icnIconLayer::GetNumIconLayers();
		for (int li=0; li<num_icon_layers; ++li)
		{
			const camCamera *pCamera = icnIconLayer::GetCameraForIconLayer(li);
			if (!pCamera) pCamera = &(cam3dMgr::GetCamera());

			maAxisBox combined_box;
			std::list<sel3dObject*>::const_iterator it, end = i_SelectedList.end();
			for (it = i_SelectedList.begin(); it != end; ++it)
			{
				if (mnmObject* pObject = dynamic_cast<mnmObject*>(*it))
				{
					combined_box.Union( pObject->GetWorldBox(li) );
				}
			}

			// Orient compass to box and its center point
			maPoint3d center = combined_box.GetCenter();

			// Use center of bounding box as pivot
			cmpsCompassMgr::AdjustCompassToBounds( i_CompassType, li, center, combined_box, 
				center, *pCamera, i_ScalingFactor );

			// If object is on back side of camera, do not render the compass.
			maVector3d dir = pCamera->GetDirection();
			maPoint3d camera_pos = pCamera->GetPosition();
			bool bInFrontOfCamera = ((center - camera_pos) * dir > 0);
			cmpsCompassMgr::SetLayerRenderable( i_CompassType, li, bInFrontOfCamera );
		}
	}
	else if ((count == 1) && (pFirstObject != NULL))
	{
		// If only one object, use the other SetUpCompass function
		mnmCompassUtil::SetUpCompass(i_CompassType, pFirstObject, i_ScalingFactor);
	}
}

//------------------------------------------------------------------------
//	HighlightSelected - put boxes around selected objects
//------------------------------------------------------------------------
void mnmCompassUtil::HighlightSelected(const std::list<sel3dObject*> &i_SelectedList,
									   float i_ScalingFactor)
{
	// First, get a count of the number of highlights needed;
	int count = 0;
	std::list<sel3dObject*>::const_iterator it, end = i_SelectedList.end();
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

	// The selected object (i.e. point light icons) may change size depending 
	// on the camera position, so you can only really ask for the bounding box
	// of the object by first giving it a camera. With multiple render panels,
	// we have to do this for each camera for each panel. The different panels
	// are represented by icon layers.
	const int num_icon_layers = icnIconLayer::GetNumIconLayers();
	for (int li=0; li<num_icon_layers; ++li)
	{
		const camCamera *pCamera = icnIconLayer::GetCameraForIconLayer(li);
		if (!pCamera) pCamera = &(cam3dMgr::GetCamera());

		// Set size of bounding boxes for each object
		int ind = 0;
		for (it = i_SelectedList.begin(); it != end; ++it)
		{
			// Skip first object in list, this will have a different compass
			// on it based on the mode
			if (it == i_SelectedList.begin())
				continue;

			if (mnmObject* pObject = dynamic_cast<mnmObject*>(*it))
			{
				cmpsSelectMgr::SetBounds( ind++, li, pObject->GetWorldBox(li), 
					pObject->GetWorldPivot(), *pCamera, i_ScalingFactor );	
			}
		}
	}

	cmpsSelectMgr::SetRenderable(true);
}
