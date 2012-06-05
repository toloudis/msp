/*****************************************************************************
**  rpnCommandFocusAllPanels.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderPanels/rpnCommandFocusAllPanels.hpp"

#include "Features/RenderPanels/rpnCommandChangeLayout.hpp"
#include "Features/RenderPanels/wxGUI/rpnPanelGrid.hpp"
#include "Features/RenderPanels/wxGUI/rpnRenderPanel.hpp"

#include "Support/fgmt/GUI/fgmtPropertyObject.hpp"
#include "Support/mtrl/GUI/mtrlOperations.hpp"
#include "Support/mtrl/GUI/mtrlPropertyObject.hpp"
#include "Support/mnm/mnmObject.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"

// Includes from old managed file
#define RPN_PANELLAYOUT_HPP
#ifndef RPN_RENDERPANE_HPP
#include "Features/RenderPanels/rpnRenderPane.hpp"
#endif

///////////////////////////////////////////////////
// Event Handler(s)
//
namespace rpnCommandFocusAllPanelsNS
{
	//--------------------------------------------------------------------
	//
	//--------------------------------------------------------------------
	void CommandExecuteHandler( cmaCommand* pCmd )
	{
		//DBG_WARNING0("Executing Focus Camera");
		rpnCommandFocusAllPanels* pCmdCL = dynamic_cast<rpnCommandFocusAllPanels*>(pCmd);

		DBG_ASSERT( pCmdCL != NULL, "Invalid command hooked up to FocusCamera command" );

		rpnCommandFocusAllPanels::FocusAllPanels();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const std::string rpnCommandFocusAllPanels::GetConstTagName()
{
	return std::string("FocusAllPanels");
}

//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------
rpnCommandFocusAllPanels::rpnCommandFocusAllPanels(const std::string& i_MenuDesc)
:	cmaCommand( GetConstTagName(),
				rpnCommandFocusAllPanelsNS::CommandExecuteHandler,
				NULL ),
	m_MenuDesc( i_MenuDesc )
{
	
	const int MAX_SIZE_OF_TAG = 48;
	const char * desc = "Focus Camera All Panels";
	std::string name;
	name = desc;
	//name.append( i_MenuDesc.c_str(), (MAX_SIZE_OF_TAG - sizeof(desc)) );

	this->SetTag(name);
	this->SetDescription(std::string("Focus the Camera in All Panels"));
	this->SetCategory(std::string("Focus Camera"));
}


//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
//virtual
rpnCommandFocusAllPanels::~rpnCommandFocusAllPanels()
{
}

//--------------------------------------------------------------------
// Static function so that other parts of code can execute this 
//	command also.
//--------------------------------------------------------------------
//static
void rpnCommandFocusAllPanels::FocusAllPanels()
{
	float m_fFocusRadius = 10.0f;
	// Get bounding box of selected objects
	maAxisBox bbox;
	const std::list<sel3dObject*> &selected_list = sel3dMgr::GetSelectedList();
	std::list<sel3dObject*>::const_iterator it, end = selected_list.end();
	for (it = selected_list.begin(); it != end; ++it)
	{
		if (mnmObject* pObject = dynamic_cast<mnmObject*>(*it))
		{
			bbox.Union( pObject->GetWorldBox() );
		}
		else if (fgmtPropertyObject* pSurface = dynamic_cast<fgmtPropertyObject*>(*it))
		{
			bbox.Union( pSurface->GetWorldBox() );
		}
		else if (mtrlPropertyObject* pMaterial = dynamic_cast<mtrlPropertyObject*>(*it))
		{
			bbox.Union( mtrlOperations::GetWorldBoxForMaterial(*pMaterial) );
		}
	}

	// Enforce a minimum zoom
	maPoint3d center(0.0f,0.0f,0.0f);
	float radius = m_fFocusRadius;
	if (!bbox.IsEmpty())
	{
		center = bbox.GetCenter();
		radius = bbox.GetRadius() * 1.5f;
		if (radius < m_fFocusRadius)
			radius = m_fFocusRadius;
	}
	
	// Focus each panel

#ifdef USE_WXWIDGETS
	if (rpnPanelGrid::Instance)
	{
		for (int i=0; i<4; i++)
		{
			rpnRenderPanel *pRenderPane = rpnPanelGrid::Instance->GetRenderPanel(i);
			if (pRenderPane != NULL)
			{
				pRenderPane->FocusCamera(center, radius);
			}
		}
	}
#endif

	// Need to also focus the cam3dMgr so that the camera manipulator gets updated
	cam3dMgr::FocusCamera( center, radius, bbox );
}
