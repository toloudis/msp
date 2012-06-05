/*****************************************************************************
**	rpnOperations.hpp
**
**	Utility for complicated operations that would require too 
**	awkward include files in the header of rpnRenderPane
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef RPN_OPERATIONS_HPP
#error rpnOperations.hpp multiply included
#endif
#define RPN_OPERATIONS_HPP

#include <string>

//============================================================================
//	forward references
//============================================================================
class camCamera;
class camsDirectorsCut;
class gpxCamera;

//============================================================================
//============================================================================
namespace rpnOperations
{
	//--------------------------------------------------------------------
	//  Select camera object in active view
	//--------------------------------------------------------------------
	void  SelectActiveCamera();

	//--------------------------------------------------------------------
	//  Select camera object
	//--------------------------------------------------------------------
	void  SelectCamera(int i_CamIndex);

	//--------------------------------------------------------------------
	//  Set selected index in camera list GUI to this camera
	//--------------------------------------------------------------------
	void  HighlightCamera(camCamera *i_pCamera);

	//--------------------------------------------------------------------
	//  Change camera in the active render pane
	//--------------------------------------------------------------------
	void  ChangeCamera(gpxCamera *i_pCamera, const std::string& i_Label);

	//--------------------------------------------------------------------
	// Change camera in active render pane to the editor camera.
	// Used to localize the "Editor" string name.
	//--------------------------------------------------------------------
	void ChangeCameraToEditor();

	//--------------------------------------------------------------------
	// Set editor camera to current view's position
	//--------------------------------------------------------------------
	void SetEditCam();

	//--------------------------------------------------------------------
	// Move view to next or previous camera in the list
	//--------------------------------------------------------------------
	void NextCamera();
	void PreviousCamera();

	//--------------------------------------------------------------------
	//  Select directors cut object
	//--------------------------------------------------------------------
	void  SelectDirectorsCut(camsDirectorsCut *i_pDirectorsCut);

	//--------------------------------------------------------------------
	//  Set selected index in camera list GUI to this directors cut
	//--------------------------------------------------------------------
	void  HighlightDirectorsCut(camsDirectorsCut *i_pDirectorsCut);

	//--------------------------------------------------------------------
	//  Change directors cut in the active render pane
	//--------------------------------------------------------------------
	void  ChangeDirectorsCut(camsDirectorsCut *i_pDirectorsCut, const std::string& i_Label);

}	// end of namespace
