/*****************************************************************************
**  rpnCommandChangeLayout.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderPanels/rpnCommandChangeLayout.hpp"

#include "Features/RenderPanels/rpnPanelLayout.hpp"
#include "Features/RenderPanels/wxGUI/rpnPanelGrid.hpp"


#include "Tool/cma/cmaCommandMgr.hpp"

#include "Core/dbg/dbgLog.hpp"

#ifdef _MANAGED
using namespace StudioFramework;
#endif

///////////////////////////////////////////////////
// Event Handler(s)
//
namespace rpnCommandChangeLayoutNS
{
	//--------------------------------------------------------------------
	//
	//--------------------------------------------------------------------
	void CommandExecuteHandler( cmaCommand* pCmd )
	{
		rpnCommandChangeLayout* pCmdCL = dynamic_cast<rpnCommandChangeLayout*>(pCmd);

		DBG_ASSERT0( pCmdCL != NULL, "Invalid command hooked up to ChangeLayout command" );

		rpnCommandChangeLayout::ChangeLayout(pCmdCL->GetLayoutStyle());
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const std::string rpnCommandChangeLayout::GetConstTagName()
{
	return std::string("ChangeLayout");
}

//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------
rpnCommandChangeLayout::rpnCommandChangeLayout( LayoutStyle  i_Style,
												const std::string& i_MenuDesc)
:	cmaCommand( GetConstTagName(),
				rpnCommandChangeLayoutNS::CommandExecuteHandler,
				NULL ),
	m_Style(i_Style),
	m_MenuDesc( i_MenuDesc )
{
	const int MAX_SIZE_OF_TAG = 48;
	const char * desc = "Layout: ";
	std::string name;
	name = desc;
	name.append( i_MenuDesc.c_str(), (MAX_SIZE_OF_TAG - sizeof(desc)) );

	this->SetTag(name);
	this->SetDescription(std::string("Set the render panel layout"));
	this->SetCategory(std::string("Render Panels"));
}


//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
//virtual
rpnCommandChangeLayout::~rpnCommandChangeLayout()
{
}

//--------------------------------------------------------------------
// Static function so that other parts of code can execute this 
//	command also.
//--------------------------------------------------------------------
//static
void rpnCommandChangeLayout::ChangeLayout(LayoutStyle  i_Style)
{
#ifdef _MANAGED
	switch (i_Style)
	{
	default:
	case rpnCommandChangeLayout::e_SinglePane:
		rpnPanelLayout::g_pPanelLayout->LayoutStyle = TerawattManagedControls::PanelLayout::Layouts::e_SinglePane;
		//rpnPanelLayout::g_RenderPanes[0]->SetTextVisible(false);
		break;
	case rpnCommandChangeLayout::e_TwoStacked:
		rpnPanelLayout::g_pPanelLayout->LayoutStyle = TerawattManagedControls::PanelLayout::Layouts::e_TwoStacked;
		//rpnPanelLayout::g_RenderPanes[0]->SetTextVisible(true);
		break;
	case rpnCommandChangeLayout::e_TwoSideBySide:
		rpnPanelLayout::g_pPanelLayout->LayoutStyle = TerawattManagedControls::PanelLayout::Layouts::e_TwoSideBySide;
		//rpnPanelLayout::g_RenderPanes[0]->SetTextVisible(true);
		break;
	case rpnCommandChangeLayout::e_FourPanels:
		rpnPanelLayout::g_pPanelLayout->LayoutStyle = TerawattManagedControls::PanelLayout::Layouts::e_FourPanes;
		//rpnPanelLayout::g_RenderPanes[0]->SetTextVisible(true);
		rpnPanelLayout::g_RenderPanes[1]->SetCamera( &cam3dMgr::GetTopCamera(), "Top");
		rpnPanelLayout::g_RenderPanes[2]->SetCamera( &cam3dMgr::GetFrontCamera(), "Front");
		rpnPanelLayout::g_RenderPanes[3]->SetCamera( &cam3dMgr::GetSideCamera(), "Side");
		break;
	}
#endif
#ifdef USE_WXWIDGETS
	switch (i_Style)
	{
	default:
	case rpnCommandChangeLayout::e_SinglePane:
		rpnPanelGrid::Instance->SetLayoutStyle( rpnPanelGrid::e_SinglePane );
		break;
	case rpnCommandChangeLayout::e_TwoStacked:
		rpnPanelGrid::Instance->SetLayoutStyle( rpnPanelGrid::e_TwoStacked );
		break;
	case rpnCommandChangeLayout::e_TwoSideBySide:
		rpnPanelGrid::Instance->SetLayoutStyle( rpnPanelGrid::e_TwoSideBySide );
		break;
	case rpnCommandChangeLayout::e_FourPanels:
		rpnPanelGrid::Instance->SetLayoutStyle( rpnPanelGrid::e_FourPanels );
		rpnPanelGrid::Instance->SetCamera( 1, &cam3dMgr::GetTopCamera(), "Top");
		rpnPanelGrid::Instance->SetCamera( 2, &cam3dMgr::GetFrontCamera(), "Front");
		rpnPanelGrid::Instance->SetCamera( 3, &cam3dMgr::GetSideCamera(), "Side");
		break;
	}
#endif
}

