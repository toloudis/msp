/*****************************************************************************
**	cptrRenderProgressDialogUtil.hpp
**
**		API for opening the capture options dialog
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_RENDERPROGRESSDIALOGUTIL_HPP
#error cptrRenderProgressDialogUtil.hpp multiply included
#endif
#define CPTR_RENDERPROGRESSDIALOGUTIL_HPP

#include <string>


//============================================================================
//============================================================================
namespace cptrRenderProgressDialogUtil
{
	//--------------------------------------------------------------------
	//  Show
	//--------------------------------------------------------------------
	void  Show();

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  Hide();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetRenderBatchID( int i_ID );

	//--------------------------------------------------------------------
	//	set the percentage for scenes
	//--------------------------------------------------------------------
	void SetScenesRenderPercentage( float i_percentage, std::string& i_label );

	//--------------------------------------------------------------------
	//	set the percentage for cameras
	//--------------------------------------------------------------------
	void SetCamerasRenderPercentage( float i_percentage, std::string& i_label );

	//--------------------------------------------------------------------
	//	set the percentage for camera
	//--------------------------------------------------------------------
	void SetCameraRenderPercentage( float i_percentage, std::string& i_label );

	//--------------------------------------------------------------------
	//	set the time that has elapsed
	//--------------------------------------------------------------------
	void SetTimeElapsed( float i_fTimeElapsed );

	//--------------------------------------------------------------------
	//	set the percentage for scene
	//--------------------------------------------------------------------
	void SetSceneLabel( std::string& i_label );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void EnablePauseButton();
	void DisablePauseButton();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	float LoadRenderPosition();
	void SaveRenderPosition();
	void DeleteRenderPosition();

}	// end of namespace
