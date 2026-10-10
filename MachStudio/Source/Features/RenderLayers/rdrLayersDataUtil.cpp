/*****************************************************************************
**	rdrLayersDataUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderLayers/rdrLayersDataUtil.hpp"

#include "Features/RenderLayers/rdrLayersDialogUtil.hpp"
#ifdef MSP_GUI_QT
#include "Features/RenderLayers/qtGUI/rdrLayersDialog.hpp"
#else
#include "Features/RenderLayers/wxGUI/rdrLayersDialog.hpp"
#endif
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#ifdef MSP_GUI_QT
#include "ToolUIQt/tqt/tqtSystem.hpp"
#else
#include "ToolUIWx/twx/twxSystem.hpp"
#endif


//============================================================================
//============================================================================
namespace rdrLayersDataUtil
{

	void  AddToMenu()
	{
		//
		//	commands
		//
		//int menu_id;
		//cmaCommand* pCmd = NULL;
		//
		////	COMMAND: Render Log window
		//pCmd = new cmaCommandSimple("Render Layers", 
		//							"Windows", 
		//							"View the Render Layers Dialog",
		//								
		//							&rdrLayersDialogUtil::Show );
		//menu_id = guiMenuMgr::AddMenuItem( "Windows", pCmd->GetTag().c_str() );
		//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		//cmmSystemDialogUtil::AddSystemCommand( "Windows", "Render Layers", pCmd );
		//rdrLayersDialogUtil::Init();		
	}

}	// end of namespace rdrLayersDataUtil