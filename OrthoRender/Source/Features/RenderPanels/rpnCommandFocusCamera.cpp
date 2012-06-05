/*****************************************************************************
**  rpnCommandFocusCamera.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderPanels/rpnCommandFocusCamera.hpp"

#include "Features/RenderPanels/rpnPanelLayout.hpp"
#include "Features/RenderPanels/rpnCommandChangeLayout.hpp"
#include "Features/RenderPanels/wxGUI/rpnPanelGrid.hpp"
#include "Features/RenderPanels/wxGUI/rpnRenderPanel.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"

#include "Support/mnm/mnmObject.hpp"

#include "Core/dbg/dbgLog.hpp"

#ifdef _MANAGED
using namespace StudioFramework;
#endif

///////////////////////////////////////////////////
// Event Handler(s)
//
namespace rpnCommandFocusCameraNS
{
	//--------------------------------------------------------------------
	//
	//--------------------------------------------------------------------
	void CommandExecuteHandler( cmaCommand* pCmd )
	{
		DBG_WARNING0("Executing Focus Camera");
		rpnCommandFocusCamera* pCmdCL = dynamic_cast<rpnCommandFocusCamera*>(pCmd);

		DBG_ASSERT0( pCmdCL != NULL, "Invalid command hooked up to FocusCamera command" );

		rpnCommandFocusCamera::FocusCamera();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const std::string rpnCommandFocusCamera::GetConstTagName()
{
	return std::string("FocusCamera");
}

//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------
rpnCommandFocusCamera::rpnCommandFocusCamera(const std::string& i_MenuDesc)
:	cmaCommand( GetConstTagName(),
				rpnCommandFocusCameraNS::CommandExecuteHandler,
				NULL ),
	m_MenuDesc( i_MenuDesc )
{
	
	const int MAX_SIZE_OF_TAG = 48;
	const char * desc = "Focus Camera";
	std::string name;
	name = desc;
	//name.append( i_MenuDesc.c_str(), (MAX_SIZE_OF_TAG - sizeof(desc)) );

	this->SetTag(name);
	this->SetDescription(std::string("Focus the Camera"));
	this->SetCategory(std::string("Render Panels"));
}


//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
//virtual
rpnCommandFocusCamera::~rpnCommandFocusCamera()
{
}

//--------------------------------------------------------------------
// Static function so that other parts of code can execute this 
//	command also.
//--------------------------------------------------------------------
//static
void rpnCommandFocusCamera::FocusCamera()
{
	inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();
	float m_fFocusRadius = 10.0f;
	// Get bounding box of selected objects
	maAxisBox bbox;
	const std::list<pick3dPickObject*> &selected_list = sel3dMgr::GetSelectedList();
	std::list<pick3dPickObject*>::const_iterator it, end = selected_list.end();
	for (it = selected_list.begin(); it != end; ++it)
	{
		mnmObject* pObject = dynamic_cast<mnmObject*>(*it);
		if (pObject)
		{
			bbox.Union( pObject->GetWorldBox() );
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
	
	// Focus just the active camera
	cam3dMgr::FocusCamera( center, radius );
	
}

