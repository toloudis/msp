/*****************************************************************************
**	camsFollowUtil.hpp
**
**	Keeps track of the currently used camera by index.
**
**	Notice that this index now includes both cameras and director's cuts.
**	The indices are numbered so that the cameras come before the directors cuts.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef CAMS_FOLLOWUTIL_HPP
#error camsFollowUtil.hpp multiply included
#endif
#define CAMS_FOLLOWUTIL_HPP

#include <string>

//============================================================================
//============================================================================
class nameString;
class camCamera;

//============================================================================
//============================================================================
namespace camsFollowUtil
{
	//--------------------------------------------------------------------
	// Return total number of cameras and directors cuts
	//--------------------------------------------------------------------
	int GetTotalCount();

	//--------------------------------------------------------------------
	// Index of scripted camera to follow,
	// DirectorsCuts are indexed after cameras.
	// use -1 for editor camera
	//--------------------------------------------------------------------
	int GetFollowIndex();
	void SetFollowIndex(int i_Index);

	//--------------------------------------------------------------------
	//	move to the next index in the follow list (and cycle around)
	//--------------------------------------------------------------------
	void SetFollowIndexNext();
	void SetFollowIndexPrev();

	//--------------------------------------------------------------------
	// Set FollowIndex to appropriate negative values without
	//	"magic numbers"
	//--------------------------------------------------------------------
	void SetEditorCamera();
	//void SetDirectorsCut();

	//--------------------------------------------------------------------
	// Check Follow Index without use of magic numbers
	//--------------------------------------------------------------------
	bool IsEditorCamera();
	//bool IsDirectorsCut();

	//--------------------------------------------------------------------
	//  Get name of camera that is curently being used
	//--------------------------------------------------------------------
	void GetCurrentCameraName( nameString &o_Name );

	//--------------------------------------------------------------------
	// Get description of camera that is curently being used
	//--------------------------------------------------------------------
	void GetCurrentCameraDescription( std::string& o_Description );

	//--------------------------------------------------------------------
	// Update camera if following one of the scripted cameras
	//--------------------------------------------------------------------
	camCamera* GetFollowCamera();

	//--------------------------------------------------------------------
	// Set up current camera in position of camera with given
	//	index.  This is used in multiple camera views immediately
	//	before a render.
	//--------------------------------------------------------------------
	camCamera* ConfigureCamera(int i_CameraIndex);

}	// end of namespace
