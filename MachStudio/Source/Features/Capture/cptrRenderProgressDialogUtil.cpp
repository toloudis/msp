/*****************************************************************************
**	cptrRenderProgressDialogUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrRenderProgressDialogUtil.hpp"
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include "Features/Capture/cptrRenderBatchData.hpp"
#include "Features/Capture/cptrRenderBatchDataUtil.hpp"
#include "Features/Capture/cptrRenderStateDataParser.hpp"
#ifdef MSP_GUI_QT
#include "Features/Capture/qtGUI/cptrRenderProgressDialog.hpp"
#else
#include "Features/Capture/wxGUI/cptrRenderProgressDialog.hpp"
#endif
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"
#ifdef MSP_GUI_QT
#include "ToolUIQt/tqt/tqtSystem.hpp"
#else
#include "ToolUIWx/twx/twxSystem.hpp"
#endif


//============================================================================
//============================================================================
namespace cptrRenderProgressDialogUtil
{
	static int l_RenderBatchID = 0;
#ifdef USE_WXWIDGETS
    static wxWindowDisabler* l_WindowDisabler = nullptr;
#endif

	char* c_RENDERSTATE_EXTENSION = ".rst";	// Render STate save

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  Show()
	{

#ifdef USE_WXWIDGETS
		if (cptrRenderProgressDialog::DialogInstance == NULL)
		{
			DBG_ASSERT((twxSystem::g_pMainForm != NULL), "MainForm not yet initialized.");

			cptrRenderProgressDialog::DialogInstance = new cptrRenderProgressDialog( twxSystem::g_pMainForm );
			cptrRenderProgressDialog::DialogInstance->SetRenderBatchID( l_RenderBatchID );
		}

        l_WindowDisabler = new wxWindowDisabler(cptrRenderProgressDialog::DialogInstance);
        cptrRenderProgressDialog::DialogInstance->Show();

#elif defined(USE_QT)
		if (cptrRenderProgressDialog::DialogInstance == NULL)
		{
			DBG_ASSERT((tqtSystem::g_pMainForm != NULL), "MainForm not yet initialized.");

			cptrRenderProgressDialog::DialogInstance = new cptrRenderProgressDialog( tqtSystem::g_pMainForm );
			cptrRenderProgressDialog::DialogInstance->SetRenderBatchID( l_RenderBatchID );
		}

		// Application modal, so the rest of the interface is disabled while
		// rendering, like the wxWidgets window disabler.
		cptrRenderProgressDialog::DialogInstance->setWindowModality( Qt::ApplicationModal );
		cptrRenderProgressDialog::DialogInstance->show();
#endif
	}

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  Hide()
	{

#ifdef USE_WXWIDGETS
		if (cptrRenderProgressDialog::DialogInstance != NULL)
		{
            delete l_WindowDisabler;
            l_WindowDisabler = nullptr;
            cptrRenderProgressDialog::DialogInstance->Show(false);
		}
#elif defined(USE_QT)
		if (cptrRenderProgressDialog::DialogInstance != NULL)
		{
			cptrRenderProgressDialog::DialogInstance->hide();
		}
#endif
	}

	//--------------------------------------------------------------------
	//  Raise - raise the dialog to the top of the window hierarchy
	//--------------------------------------------------------------------
	void  Raise()
	{
#ifdef USE_WXWIDGETS
		if (cptrRenderProgressDialog::DialogInstance != NULL)
		{
			cptrRenderProgressDialog::DialogInstance->Raise();
		}
#elif defined(USE_QT)
		if (cptrRenderProgressDialog::DialogInstance != NULL)
		{
			cptrRenderProgressDialog::DialogInstance->raise();
		}
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetRenderBatchID( int i_ID )
	{
		l_RenderBatchID = i_ID;
	}

	//--------------------------------------------------------------------
	// Set the current render layer label
	//--------------------------------------------------------------------
	void SetRenderLayerProgress( const std::string& i_label )
	{
		if (i_label.size() > 0)
			DBG_LOG(i_label);

#if defined(USE_WXWIDGETS) || defined(USE_QT)
		if (cptrRenderProgressDialog::DialogInstance)
		{
			cptrRenderProgressDialog::DialogInstance->SetRenderLayerLabel( i_label );
		}
#endif
	}

	//--------------------------------------------------------------------
	//	set the percentage for scenes
	//--------------------------------------------------------------------
	void SetScenesRenderPercentage( float i_percentage, const std::string& i_label )
	{

#if defined(USE_WXWIDGETS) || defined(USE_QT)
		if (cptrRenderProgressDialog::DialogInstance)
		{
			cptrRenderProgressDialog::DialogInstance->SetScenesRenderPercentage(i_percentage, i_label);
		}
#endif
	}

	//--------------------------------------------------------------------
	//	set the percentage for cameras
	//--------------------------------------------------------------------
	void SetCamerasRenderPercentage( float i_percentage, const std::string& i_label )
	{

#if defined(USE_WXWIDGETS) || defined(USE_QT)
		if (cptrRenderProgressDialog::DialogInstance)
		{
			cptrRenderProgressDialog::DialogInstance->SetCamerasRenderPercentage(i_percentage, i_label);
		}
#endif
	}

	//--------------------------------------------------------------------
	//	set the percentage for camera
	//--------------------------------------------------------------------
	void SetCameraRenderPercentage( float i_percentage, const std::string& i_label )
	{

#if defined(USE_WXWIDGETS) || defined(USE_QT)
		if (cptrRenderProgressDialog::DialogInstance)
		{
			cptrRenderProgressDialog::DialogInstance->SetCameraRenderPercentage(i_percentage, i_label);
		}
#endif
	}

	//--------------------------------------------------------------------
	//	set the time that has elapsed
	//--------------------------------------------------------------------
	void SetTimeElapsed( float i_fTimeElapsed )
	{

#if defined(USE_WXWIDGETS) || defined(USE_QT)
		if (cptrRenderProgressDialog::DialogInstance)
		{
			cptrRenderProgressDialog::DialogInstance->SetTimeElapsed(i_fTimeElapsed);
		}
#endif
	}

	//--------------------------------------------------------------------
	//	set the percentage for scene
	//--------------------------------------------------------------------
	void SetSceneLabel( const std::string& i_label )
	{
		if (i_label.size() > 0)
			DBG_LOG(i_label);


#if defined(USE_WXWIDGETS) || defined(USE_QT)
		if (cptrRenderProgressDialog::DialogInstance)
		{
			cptrRenderProgressDialog::DialogInstance->SetSceneLabel(i_label);
		}
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void EnablePauseButton()
	{

#if defined(USE_WXWIDGETS) || defined(USE_QT)
		if (cptrRenderProgressDialog::DialogInstance)
		{
			cptrRenderProgressDialog::DialogInstance->EnablePauseButton();
		}
#endif
	}

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void DisablePauseButton()
	{

#if defined(USE_WXWIDGETS) || defined(USE_QT)
		if (cptrRenderProgressDialog::DialogInstance)
		{
			cptrRenderProgressDialog::DialogInstance->DisablePauseButton();
		}
#endif
	}


}	// end of namespace

