//****************************************************************************
//	fgtFrameMgr.hpp
//
///		Manage the group of frames
//
//	StudioGPU
//	Copyright(C) 2005 - All Rights Reserved
//****************************************************************************
#ifdef FGT_FRAMEMGR_HPP
#error fgtFrameMgr.hpp multiply included
#endif
#define FGT_FRAMEMGR_HPP

#ifndef FGT_FRAME_HPP
#include "Features/FilmGates/fgtFrame.hpp"
#endif


//============================================================================
//============================================================================
namespace fgtFrameMgr
{
	//--------------------------------------------------------------------
	/// Init
	//--------------------------------------------------------------------
	void  Init();

	//--------------------------------------------------------------------
	/// CleanUp
	//--------------------------------------------------------------------
	void  CleanUp();

	//--------------------------------------------------------------------
	/// Read in the valid safe frames from an XML file
	//--------------------------------------------------------------------
	void Read();

	//--------------------------------------------------------------------
	///	show or hide all safe frames.  This will also keep track of
	///	the current state of each frame before hiding them.
	///
	/// @param i_bShow show or hide the frame
	//--------------------------------------------------------------------
	void Show( bool i_bActionSafeShow, bool i_bTitleSafeShow );

	//--------------------------------------------------------------------
	///	show or hide all safe frames.  This will also keep track of
	///	the current state of each frame before hiding them.
	//--------------------------------------------------------------------
	void Show(bool i_bShow);

	//--------------------------------------------------------------------
	///	recreate/resize the frames.  This is needed when the render
	///	window resolution changes.
	//--------------------------------------------------------------------
	void ResizeFrames();

	//--------------------------------------------------------------------
	/// Display action safe frame
	//--------------------------------------------------------------------
	void SetViewActionFrame(bool i_bVal);
	bool GetViewActionFrame();

	//--------------------------------------------------------------------
	/// Display Title safe frame
	//--------------------------------------------------------------------
	void SetViewTitleFrame(bool i_bVal);
	bool GetViewTitleFrame();
}

