/*****************************************************************************
**  rpnPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderPanels/rpnPackage.hpp"

#include "Features/RenderPanels/rpnCommands.hpp"
#include "Features/RenderPanels/GUI/rpnDialogUtil.hpp"
#include "Features/RenderPanels/rpnDocumentInterest.hpp"
#include "Features/RenderPanels/wxGUI/rpnPanelGrid.hpp"
#include "Features/RenderPanels/wxGUI/rpnRenderPanel.hpp"
//#include "Features/RenderPanels/rpnThinkInterest.hpp"

//	tools
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Support/mnm/mnmThinkMgr.hpp"

//	lib

// Includes from old managed file
#define RPN_PANELLAYOUT_HPP
#ifndef RPN_RENDERPANE_HPP
#include "Features/RenderPanels/rpnRenderPane.hpp"
#endif

//============================================================================
//============================================================================
namespace
{
	// Callbacks for when camera list need to be updated.
	class CameraListCallback : public camsCameraMgr::CameraListChangedCallback,
								public camsDirectorsCutMgr::DirectorsCutListChangedCallback
	{
	public:
		virtual void CameraListChanged()
		{
			for (int i=0; i<4; ++i)
			{
				// Make sure all render panels have a valid camera

#ifdef USE_WXWIDGETS
				if (rpnPanelGrid::Instance != NULL)
					rpnPanelGrid::Instance->GetRenderPanel(i)->ConfirmCamera();
#endif
			}
		}
		virtual void DirectorsCutListChanged()
		{
			for (int i=0; i<4; ++i)
			{
				// Make sure all render panels aren't pointing 
				// at bad directors cut object

#ifdef USE_WXWIDGETS
				if (rpnPanelGrid::Instance != NULL)
					rpnPanelGrid::Instance->GetRenderPanel(i)->ConfirmCamera();
#endif
			}
		}
	};
	CameraListCallback l_CameraCallback;

	//rpnThinkInterest*		l_pPanelsTI = 0;
}

//============================================================================
//============================================================================
namespace rpnPackage
{

	//--------------------------------------------------------------------
	// Init -- initialize the package
	//--------------------------------------------------------------------
	void Init()
	{
		// Add document chunk to handle saving layout styles
		docSingleTypeMgr::AddDocumentInterest(new rpnDocumentInterest());

		// Track changes in the camera list
		camsCameraMgr::AddCallback(&l_CameraCallback);
		camsDirectorsCutMgr::AddCallback(&l_CameraCallback);


#ifdef USE_WXWIDGETS
		if (rpnPanelGrid::Instance != NULL)
			rpnPanelGrid::Instance->SetLayoutStyle( rpnPanelGrid::e_SinglePane );
#endif

		// Setup the camera list dialog
		rpnDialogUtil::Init();

		// Create commands to enable hot keys
		rpnCommands::SetupMenu();

		// register the think interest
		//if ( l_pPanelsTI == 0 )
		//{
		//	l_pPanelsTI = new rpnThinkInterest();
		//	mnmThinkMgr::RegisterThinkInterest( l_pPanelsTI );
		//}
	}

	//--------------------------------------------------------------------
	// CleanUp -
	//--------------------------------------------------------------------
	void CleanUp()
	{
		camsCameraMgr::RemoveCallback(&l_CameraCallback);

		rpnDialogUtil::CleanUp();

		//	Unregister the Think Interest
		//if ( l_pPanelsTI != 0 )
		//{
		//	mnmThinkMgr::UnRegisterThinkInterest( l_pPanelsTI );
		//	delete l_pPanelsTI;
		//	l_pPanelsTI = 0;
		//}
	}
}
