/*****************************************************************************
**	rpnDialogUtil.hpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef RPN_DIALOGUTIL_HPP
#error rpnDialogUtil.hpp multiply included
#endif
#define RPN_DIALOGUTIL_HPP


//============================================================================
//	forward references
//============================================================================
class camCamera;
class camsDirectorsCut;

//============================================================================
//============================================================================
class rpnDialogUtil
{
public:
	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	static void  Init();

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	static void  CleanUp();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static void SetupCameraDialog();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static const char* GetCamerasToolbarName();

	//--------------------------------------------------------------------
	// When render panel changes its active camera, it calls this
	// function to change the selected index in the camera list.
	//--------------------------------------------------------------------
	static void HighlightCamera(camCamera *i_pCamera);

	//--------------------------------------------------------------------
	// When render panel changes to a directors cut, it calls this
	// function to change the selected index in the camera list.
	//--------------------------------------------------------------------
	static void HighlightDirectorsCut(camsDirectorsCut *i_pDirectorsCut);

	//--------------------------------------------------------------------
	// Switch between editor camera and last selected scripted camera
	//--------------------------------------------------------------------
	static void DoSwapCam();

};	// end of static class

