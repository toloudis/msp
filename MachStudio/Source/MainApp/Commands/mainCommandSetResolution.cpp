/*****************************************************************************
**  mainCommandSetResolution.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/Commands/mainCommandSetResolution.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"
#include "Core/dbg/dbgMsg.hpp"
#include "Core/undo/undoUndoMgr.hpp"

#include "MainApp/MainForm.h"
#include "MainApp/wxGUI/wxMainForm.hpp"

#include <sstream>


///////////////////////////////////////////////////
// Event Handler(s)
//
namespace mainCommandSetResolutionNS
{
	//--------------------------------------------------------------------
	//
	//--------------------------------------------------------------------
	void CommandExecuteHandler( cmaCommand* pCmd )
	{
		mainCommandSetResolution* pCom = dynamic_cast<mainCommandSetResolution*>(pCmd);
		DBG_ASSERT( pCom != NULL, "Invalid command hooked up to SetResolution command" );

#ifdef USE_WXWIDGETS
		wxMainForm::ResizeRenderWindow(pCom->GetWidth(),pCom->GetHeight());
#endif
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const std::string mainCommandSetResolution::GetConstTagName()
{
	return std::string("mainSetResolution");
}

//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------
mainCommandSetResolution::mainCommandSetResolution(int i_Width, int i_Height)
:	cmaCommand( GetConstTagName(),
				mainCommandSetResolutionNS::CommandExecuteHandler,
				NULL ),
	m_Width(i_Width),
	m_Height(i_Height)
{
	//char name[32];
	//sprintf(name, "Set Res %dx%d", i_Width, i_Height);
	std::ostringstream oss;
	oss.setf(0, std::ios::floatfield);
	oss.setf(std::ios::fixed, std::ios::floatfield);
	oss << "Set Res "<<i_Width<<"x"<<i_Height;
	std::string name(oss.str());
	this->SetTag(name.c_str());
	this->SetDescription(std::string("Set the render panel resolution"));
	this->SetCategory(std::string("Resolutions"));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int mainCommandSetResolution::GetWidth()
{
	return m_Width;
}
int mainCommandSetResolution::GetHeight()
{
	return m_Height;
}

