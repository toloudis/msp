/****************************************************************************\
**  mnmCompassUtil.hpp
**
**      mnmCompassUtil provides an interface to the compass system.  It
**	does all the work of grabbing camera info, etc. and passes the data
**	to the compass system.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MNM_COMPASSUTIL_HPP
#error mnmCompassUtil.hpp multiply included
#endif
#define MNM_COMPASSUTIL_HPP

#include <list>

//============================================================================
//============================================================================
class camCamera;
class mnmObject;
class sel3dObject;


//============================================================================
//============================================================================
namespace mnmCompassUtil
{
	//------------------------------------------------------------------------
	//	UpdateCompass - update the specified compass appropriately
	//------------------------------------------------------------------------
	//void UpdateCompass( int i_CompassType, const camCamera &i_Camera );

	//------------------------------------------------------------------------
	//	SetUpCompass - set up the compass to render for an object
	//------------------------------------------------------------------------
	void SetUpCompass( int i_CompassType, 
					   mnmObject* i_pObject,
					   float i_ScalingFactor );

	//------------------------------------------------------------------------
	//	SetUpCompass - set up the compass to surround the bounding box
	//		defined by all of the selected objects
	//------------------------------------------------------------------------
	void SetUpCompass( int i_CompassType, 
					   const std::list<sel3dObject*> &i_SelectedList,
					   float i_ScalingFactor );

	//------------------------------------------------------------------------
	//	HighlightSelected - put boxes around selected objects
	//------------------------------------------------------------------------
	void HighlightSelected(const std::list<sel3dObject*> &i_SelectedList,
						   float i_ScalingFactor);

};

