/*****************************************************************************
**  mainCommandSetResolution.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/Commands/mainCommandSetResolution.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"
#include "Core/dbg/dbgAssert.hpp"
#include "Core/undo/undoUndoMgr.hpp"

#include "MainApp/MainForm.h"
#include "MainApp/wxGUI/wxMainForm.hpp"

#ifdef _MANAGED
using namespace StudioFramework;
#endif


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
		DBG_ASSERT0( pCom != NULL, "Invalid command hooked up to SetResolution command" );

#ifdef _MANAGED
		MainForm::FormInstance->ResizeRenderWindow(pCom->GetWidth(),pCom->GetHeight());
#endif
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
	char name[32];
	sprintf(name, "Set Res %dx%d", i_Width, i_Height);
	this->SetTag(name);
	this->SetDescription(std::string("Set the render panel resolution"));
	this->SetCategory(std::string("Resolution"));
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

