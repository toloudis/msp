/*****************************************************************************
**	cptrRenderProgressDialogUtil.hpp
**
**		API for opening the capture options dialog
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_RENDERPROGRESSDIALOGUTIL_HPP
#error cptrRenderProgressDialogUtil.hpp multiply included
#endif
#define CPTR_RENDERPROGRESSDIALOGUTIL_HPP

#ifndef IT_STRING_HPP
#include "Core/It/itString.hpp"
#endif 

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
	//  Raise - raise the dialog to the top of the window hierarchy
	//--------------------------------------------------------------------
	void  Raise();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetRenderBatchID( int i_ID );

	//--------------------------------------------------------------------
	// Set the current render layer label
	//--------------------------------------------------------------------
	void SetRenderLayerProgress( const itString& i_label );

	//--------------------------------------------------------------------
	//	set the percentage for scenes
	//--------------------------------------------------------------------
	void SetScenesRenderPercentage( float i_percentage, const itString& i_label );

	//--------------------------------------------------------------------
	//	set the percentage for cameras
	//--------------------------------------------------------------------
	void SetCamerasRenderPercentage( float i_percentage, const itString& i_label );

	//--------------------------------------------------------------------
	//	set the percentage for camera
	//--------------------------------------------------------------------
	void SetCameraRenderPercentage( float i_percentage, const itString& i_label );

	//--------------------------------------------------------------------
	//	set the time that has elapsed
	//--------------------------------------------------------------------
	void SetTimeElapsed( float i_fTimeElapsed );

	//--------------------------------------------------------------------
	//	set the percentage for scene
	//--------------------------------------------------------------------
	void SetSceneLabel( const itString& i_label );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void EnablePauseButton();
	void DisablePauseButton();


}	// end of namespace
