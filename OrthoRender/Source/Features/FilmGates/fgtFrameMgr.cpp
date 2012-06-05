/*****************************************************************************
**	fgtFrameMgr.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/FilmGates/fgtFrameMgr.hpp"

#include "Features/FilmGates/fgtCommandShowFrame.hpp"

#include "Features/Capture/cptrCategoryConfigFileUtil.hpp"
#include "Support/mnm/mnmPaths.hpp"

#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"

#include <vector>
//#include <assert.h>


//============================================================================
//============================================================================
namespace fgtFrameMgr
{
	//========================================================================
	//========================================================================
	namespace
	{
		std::vector<fgtFrame*>	l_Frames;
		std::vector<bool>		l_FramesVisibleState;
		bool l_bVisibleState = true;
		int l_SelIndex = -1;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void clear_frames()
		{
			envSTLHelpers::DeleteContainer(l_Frames);
			l_FramesVisibleState.clear();
		}

		const char* lc_FilmGates_FileName	= "FilmGates.cfg";
	}	// end of namespace


	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init()
	{
	}

	//--------------------------------------------------------------------
	//  CleanUp
	//--------------------------------------------------------------------
	void  CleanUp()
	{
		clear_frames();
		l_SelIndex = -1;
	}

	//--------------------------------------------------------------------
	//  Clear
	//--------------------------------------------------------------------
	//void  Clear()
	//{
	//	clear_frames();
	//	l_SelIndex = -1;
	//}

	//--------------------------------------------------------------------
	//  Get number of frames
	//--------------------------------------------------------------------
	int GetNumFrames()
	{
		return l_Frames.size();
	}

	//--------------------------------------------------------------------
	//  Add frames
	//--------------------------------------------------------------------
	void AddFrames()
	{
		//	read in the configuration file for filmgate
		//
		fsLocator cfgdir;
		cfgdir = gfPaths::GetPath(mnmPaths::e_DefaultConfigs);
		cfgdir.Push( lc_FilmGates_FileName );
		category_list_type filmgatelist(2, std::vector<std::string>(0));
		cptrCategoryConfigFileUtil::SetCategoryTag(std::string("Category"));
		cptrCategoryConfigFileUtil::SetElementTag(std::string("FilmGate"));
		cptrCategoryConfigFileUtil::ReadConfigFile( cfgdir, filmgatelist );
		
		//	Set-up the various parts of the application with each filmgate
		//	from the list
		//
		for (int i=0; i < filmgatelist[0].size(); ++i)
		{
			//DBG_LOG3("%02d. %s - %s", i, filmgate[0][i].c_str(), filmgate[1][i].c_str());

			//	add the safe frame
			char tempstr[64];
			sprintf( tempstr,"frame - %s", filmgatelist[1][i].c_str() );
			//sprintf( tempstr,"frame - %s", filmgate[1][i].c_str() );
			int width,height;
			sscanf( filmgatelist[1][i].c_str(), "%dx%d ", &width, &height );
			fgtFrameMgr::AddFrame( new fgtFrame( width, height, tempstr ), filmgatelist[0][i] );
		}
	}

	//--------------------------------------------------------------------
	//  Add new frame
	//--------------------------------------------------------------------
	int  AddFrame( fgtFrame* i_pFrame, std::string i_Category )
	{
		int index = l_Frames.size();
		l_Frames.resize( index + 1 );
		l_FramesVisibleState.resize( index + 1 );

		l_Frames[index] = i_pFrame;
		l_FramesVisibleState[index] = false;

		//	add to main menu
		//
//WXGUI
/*
		guiMenuMgr::AddMenu( "Film Gates", i_Category.c_str() );
		int menuID = guiMenuMgr::AddCheckableMenuItem( i_Category.c_str(), i_pFrame->GetMenuDesc().c_str() );
		cmaCommand* pCmd = new fgtCommandShowFrame( i_pFrame->GetMenuDesc().c_str() );
		guiCommandMgr::Add( pCmd, 
							fgtCommandShowFrame::GetConstTagName(), 
							menuID );
		cmaCommandMgr::CommandSetChecked( pCmd, false );

		//	add to system dialog, system tab
		//
		std::string display_text("Toggle Visibility of ");
		display_text += i_pFrame->GetMenuDesc();
		cmmSystemDialogUtil::AddSystemCommand( "Film Gates", display_text.c_str(), pCmd );
		//DBG_LOG2( "Menu %d %s", menuID, i_pFrame->GetMenuDesc().c_str() );
*/
		return index;
	}

	//--------------------------------------------------------------------
	//  Select frame with given index
	//--------------------------------------------------------------------
	//void  SelectFrame(int i_Index)
	//{
	//	l_SelIndex = i_Index;
	//}


	//--------------------------------------------------------------------
	//  Get the frame
	//--------------------------------------------------------------------
	fgtFrame* GetFrame(int i_Index)
	{
		DBG_ASSERT0( (i_Index >= 0 && i_Index < l_Frames.size()), "index out of range" );

		return l_Frames[i_Index];
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Show( const std::string& i_Tag, bool i_bShow )
	{
		int index = l_Frames.size();
		int i;
		for (i=0; i < index; i++ )
		{
			//	if the description matches then decide whether to show it or not
			//	based on the parameter.
			//
			//DBG_LOG5("   %02d-%02d) (%s) vs (%s) show=%s", i, index, l_Frames[i]->GetMenuDesc().c_str(), i_Tag.c_str(), (i_bShow?"true":"false"));
			if ( strcmp(l_Frames[i]->GetMenuDesc().c_str(), i_Tag.c_str()) == 0 )
			{
				l_Frames[i]->Show( i_bShow );
				l_FramesVisibleState[i] = i_bShow;
				return;
			}
		}
	}

	//--------------------------------------------------------------------
	//	show or hide all frame gates.  This will also keep track of
	//	the current state of each frame gate before hiding them.
	//--------------------------------------------------------------------
	void Show( bool i_bShow )
	{
		if ( i_bShow )
		{
			//	now set the state of each frame
			//
			int index = l_Frames.size();
			int i;
			for (i=0; i < index; i++ )
			{
				l_Frames[i]->Show( l_FramesVisibleState[i] );
			}
		}
		else
		{
			//	HIDE ALL 

			//	first store the state of each frame gate
			//
			int index = l_Frames.size();
			int i;
			for (i=0; i < index; i++ )
			{
				l_FramesVisibleState[i] = l_Frames[i]->IsVisible();
				l_Frames[i]->Show(i_bShow);
			}
		}

		l_bVisibleState = i_bShow;
	}

	//--------------------------------------------------------------------
	//	is the manager's state visible or not
	//--------------------------------------------------------------------
	bool IsVisible()
	{
		return l_bVisibleState;
	}

	//--------------------------------------------------------------------
	//	recreate/resize the frames.  This is needed when the render
	//	window filmgate changes.
	//--------------------------------------------------------------------
	void ResizeFrames()
	{
		int index = l_Frames.size();
		int i;
		for (i=0; i < index; i++ )
		{
			l_Frames[i]->Resize();
		}
	}
}
