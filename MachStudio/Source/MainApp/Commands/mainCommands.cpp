/*****************************************************************************
**  mainCommands.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/Commands/mainCommands.hpp"

#include "MainApp/Commands/mainCommandSetResolution.hpp"
#include "MainApp/Commands/mainOperations.hpp"
#include "MainApp/mainCheckAppVersion.hpp"
#include "MainApp/mainConstants.hpp"
#include "MainApp/MainForm.h"
#include "MainApp/wxGUI/wxHelpUtil.hpp"
#include "MainApp/wxGUI/wxMainForm.hpp"

#include "Features/Capture/cptrRenderUtil.hpp"
#include "Features/Capture/wxGUI/ExportConstants.hpp"
#include "Features/Channels/chnlDialogUtil.hpp"
#include "Features/ObjectManip/mnpConstants.hpp"
#include "Features/Prefs/prefsMgr.hpp"
#include "Features/Prefs/wxGUI/prefsLayoutMgr.hpp"
#include "Features/ProjectSetup/Data/ProjectSetupData.hpp"
#include "Features/ProjectSetup/ProjectSetupMgr.hpp"
#include "Features/RenderPrefs/rndrPrefsMgr.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/cmps/cmpsCompassMgr.hpp"
#include "Support/fgmt/GUI/fgmtOperations.hpp"
#include "Support/mnm/mnmAppUtil.hpp"
#include "Support/mnm/mnmAutoSaveMgr.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmSecurityMgr.hpp"
#include "Support/mode/modeConstants.hpp"
#include "Support/pyth/pythCommands.hpp"
#include "Systems/Character/Undo/chtrOperations.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "AudioDS/sn/snSoundManager.hpp"
#include "Core/fs/fsCategoryConfigFileUtil.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiCursor.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "Tool/gui/guiToolbarMgr.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"


#ifdef USE_WXWIDGETS
#include <assert.h>
#include <shellapi.h>
#endif


//============================================================================
//
//============================================================================
namespace
{
	const char* lc_Resolutions_FileName = "Resolutions.cfg";

	//
	//	Command Functions
	//

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_MuteAudio(bool i_bChecked)
	{
		PrefsData& prefsdata = PrefsMgr::Data();
		prefsdata.m_bMuteAudio.SetValue(i_bChecked);

		snSoundManager::Mute( i_bChecked );

		guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Mute, i_bChecked?"Mute":"" );
		guiStatusBarMgr::SetToolTip( mnmConstants::e_SBPanel_Mute, 
			i_bChecked ? "Audio: Off" : "Audio: On" );
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_Help()
	{
#ifdef USE_WXWIDGETS
		wxHelpUtil::ShowHelpInfo();
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_Online_CheckLatestVersion()
	{
#ifdef USE_WXWIDGETS
		// TODO - Please read the docs for ShellExecute closely. To really bulletproof your code, they recommend initializing COM. See the docs here, and look for the part that says "COM should be initialized as shown here". The short answer is to do this (if you haven't already init'd COM):
		//CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)

		envAppVersion user_version( mainConstants::mc_ExecutableVersion );
		envAppVersion latest_version;
		itString download_file;
		std::string version_URL;

		//	pick the appropriate XML file based on the application and version
#if (SGPU_APP == MS_PRO)
		if (user_version < envAppVersion(2,0,0,0))
			version_URL = std::string("http://www.studiogpu.com/download/version.xml");
		else
			version_URL = std::string("http://www.studiogpu.com/download/version_msp2.xml");
#elif (SGPU_APP == MS_CORE)
#ifdef MODELINGPACKAGE_RHINO
		version_URL = std::string("http://www.studiogpu.com/download/version_mscrhino.xml");
#else
		version_URL = std::string("http://www.studiogpu.com/download/version_msc.xml");
#endif
#endif	// msapp == ms_core

		bool bLatest = mainCheckAppVersion::IsLatestVersion(user_version, version_URL, latest_version, download_file);
		if (bLatest)
		{
			std::string msg = "Your version of the application is up to date.\n";
			guiMessageBox::Show(msg.c_str(), "Check for Updates", guiMessageBox::e_OKOnly);
		}
		else
		{
			std::string msg = "Your version of application (" + user_version.GetString() + ") is not up to date.\nThe latest version is (" + latest_version.GetString() + ")\nThe latest version is at http://studiogpu.com/downloads\nDo you want to go there now?";
			int result = guiMessageBox::Show(msg.c_str(), "Check for Updates", guiMessageBox::e_YesNo);

			DBG_WARNING("Update Status: User version of application (" << user_version.GetString().c_str() << ") is not up to date.\nThe latest version is (" << latest_version.GetString().c_str() << ")\nGo to http://studiogpu.com/downloads");

			switch (result)
			{
				case guiMessageBox::e_Yes:
					ShellExecute(NULL, L"open", L"http://studiogpu.com/downloads", NULL, NULL, SW_SHOWNORMAL);
					break;
				default:
				case guiMessageBox::e_No:
					break;
			}
		}
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_Online_Support()
	{
#ifdef USE_WXWIDGETS
		// TODO - Please read the docs for ShellExecute closely. To really bulletproof your code, they recommend initializing COM. See the docs here, and look for the part that says "COM should be initialized as shown here". The short answer is to do this (if you haven't already init'd COM):
		//CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)

		ShellExecute(NULL, L"open", L"http://studiogpu.com/support",
				NULL, NULL, SW_SHOWNORMAL);
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_Online_Manual()
	{
#ifdef USE_WXWIDGETS
		ShellExecute(NULL, L"open", L"http://studiogpu.com/userguide",
				NULL, NULL, SW_SHOWNORMAL);
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_Online_Tutorials()
	{
#ifdef USE_WXWIDGETS
		ShellExecute(NULL, L"open", L"http://www.studiogpu.com/support/tutorials",
				NULL, NULL, SW_SHOWNORMAL);
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_Online_StudioGPU()
	{
#ifdef USE_WXWIDGETS
		ShellExecute(NULL, L"open", L"http://studiogpu.com",
				NULL, NULL, SW_SHOWNORMAL);
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_About()
	{
#ifdef USE_WXWIDGETS
		wxHelpUtil::ShowHelpAbout();
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_File_New()
	{
		//SceneSetupDialogUtil::Show();
		if (cptrRenderUtil::GetCaptureProgress())
			return;

		//	only perform if the security is present
		if (mnmSecurityMgr::CheckSecurity1())
		{
			// stop any render threads
			gpxRenderControl::ConfirmSingleThread();

			guiSingleDocHandler::New();
		}
		captRenderOutputDataUtil::SetToDefault(fsLocator());
		//fsFileNotifyMgr::DeInitialize();
		cmmSystemDialogUtil::UpdateDialog();
		mnmAppUtil::UpdateTitleBar( false );
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_File_Open()
	{
		if (cptrRenderUtil::GetCaptureProgress())
			return;
		// FIX: - shouldn't need these two lines once all of the scene files have the project chunk
		//
		guiCursor::SetWaitCursor();
		ProjectSetupData data = ProjectSetupMgr::Data();
		fsFileUtil::LocatorToANSIFilename( gfPaths::GetPath( gfPaths::e_AppPath ), data.m_ProjectDirectory );

		//	only perform if the security is present
		if (mnmSecurityMgr::CheckSecurity3())
		{
			// stop any render threads
			gpxRenderControl::ConfirmSingleThread();
			
			guiSingleDocHandler::Open();
		
			mnmAppUtil::UpdateTitleBar( false );
		}
		guiCursor::EndWaitCursor();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_File_Close()
	{
		//	only perform if the security is present
		if (mnmSecurityMgr::CheckSecurity4())
		{
			// stop any render threads
			gpxRenderControl::ConfirmSingleThread();

			guiSingleDocHandler::New();
		}

	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_File_Revert()
	{
		if (cptrRenderUtil::GetCaptureProgress())
			return;
		fsLocator curfile = docSingleDocumentMgr::GetFilename();
		if (curfile.GetNumNames() > 0)
		{
			//	only perform if the security is present
			if (mnmSecurityMgr::CheckSecurity5())
			{
				// stop any render threads
				gpxRenderControl::ConfirmSingleThread();
								
				// Confirm if the user wants to revert the scene
				int res = guiMessageBox::Show(" Are you sure you want to revert ?", 
					"Confirm Revert", 
					guiMessageBox::e_YesNo);
				
				if (res == guiMessageBox::e_Yes)
				{
					guiSingleDocHandler::Open(curfile, true);

					DBG_LOG( docSingleDocumentMgr::GetFilename() << " reverted." );
				}
			}
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_File_Save()
	{
		//	only perform if the security is present
		if (!mnmSecurityMgr::CheckSecurity3())
			return;

		// stop any render threads (needed for saving?)
		gpxRenderControl::ConfirmSingleThread();
		mnmAutoSaveMgr::BackupSavedFile();
		guiCursor::SetWaitCursor();
		guiSingleDocHandler::Save();
		guiCursor::EndWaitCursor();

		DBG_LOG( docSingleDocumentMgr::GetFilename() << " saved." );

#ifdef USE_WXWIDGETS
		// We can use this as an opportunity to save preferences also
		prefsLayoutMgr::SaveLastLayout();
#endif

		mnmAppUtil::UpdateTitleBar( false );
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_File_SaveAs()
	{
		//	only perform if the security is present
		if (!mnmSecurityMgr::CheckSecurity4())
			return;

		// stop any render threads (needed for saving?)
		gpxRenderControl::ConfirmSingleThread();
		guiCursor::SetWaitCursor();
		guiSingleDocHandler::SaveAs();
		guiCursor::EndWaitCursor();

		DBG_LOG( docSingleDocumentMgr::GetFilename() << " saved." );

#ifdef USE_WXWIDGETS
		// We can use this as an opportunity to save preferences also	
		prefsLayoutMgr::SaveLastLayout();
#endif

		mnmAppUtil::UpdateTitleBar( false );
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_File_Exit()
	{
		// stop any render threads
		gpxRenderControl::ConfirmSingleThread();

#ifdef USE_WXWIDGETS
		wxMainForm::Exit();
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_FileToolbar(bool i_bChecked)
	{
		guiToolbarMgr::Show(mnpConstants::mc_Toolbar_File_Name, i_bChecked);
	}
	bool Get_FileToolbar()
	{
		return guiToolbarMgr::IsVisible(mnpConstants::mc_Toolbar_File_Name);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_ActionToolbar(bool i_bChecked)
	{
		guiToolbarMgr::Show(mnpConstants::mc_Toolbar_Actions_Name, i_bChecked);
	}
	bool Get_ActionToolbar()
	{
		return guiToolbarMgr::IsVisible(mnpConstants::mc_Toolbar_Actions_Name);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_PickModeToolbar(bool i_bChecked)
	{
		guiToolbarMgr::Show(mnpConstants::mc_Toolbar_PickMode_Name, i_bChecked);
	}
	bool Get_PickModeToolbar()
	{
		return guiToolbarMgr::IsVisible(mnpConstants::mc_Toolbar_PickMode_Name);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_ScriptToolbar(bool i_bChecked)
	{
		guiToolbarMgr::Show(pythCommands::mc_Toolbar_Scripts_Name, i_bChecked);
	}
	bool Get_ScriptToolbar()
	{
		return guiToolbarMgr::IsVisible(pythCommands::mc_Toolbar_Scripts_Name);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_ModeToolbar(bool i_bChecked)
	{
		guiToolbarMgr::Show(modeConstants::mc_Toolbar_Modes_Name, i_bChecked);
	}
	bool Get_ModeToolbar()
	{
		return guiToolbarMgr::IsVisible(modeConstants::mc_Toolbar_Modes_Name);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_ExportToolbar(bool i_bChecked)
	{
		guiToolbarMgr::Show(exportConstants::mc_Toolbar_Export_Name, i_bChecked);
	}
	bool Get_ExportToolbar()
	{
		return guiToolbarMgr::IsVisible(exportConstants::mc_Toolbar_Export_Name);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Show_TimeLine()
	{
#ifdef USE_WXWIDGETS
		twxPaneMgr::Show("Time Slider");
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Show_Tags()
	{
#ifdef USE_WXWIDGETS
		//twxPaneMgr::Show("Tags");
#endif
	}

//	//--------------------------------------------------------------------
//	//--------------------------------------------------------------------
//	void Set_TimeLine(bool i_bChecked)
//	{

//#ifdef USE_WXWIDGETS
//		twxPaneMgr::Show("Time Slider", i_bChecked);
//#endif
//	}
//	bool Get_TimeLine()
//	{

//	#ifdef USE_WXWIDGETS
//		return twxPaneMgr::IsVisible("Time Slider");
//	#else
//		return false;
//	#endif
//	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_StatusBar(bool i_bChecked)
	{

#ifdef USE_WXWIDGETS
		if (twxSystem::g_pMainForm && twxSystem::g_pStatusBar)
		{
			if (i_bChecked)
				twxSystem::g_pMainForm->SetStatusBar(twxSystem::g_pStatusBar);
			else
				twxSystem::g_pMainForm->SetStatusBar(NULL);

			twxSystem::g_pStatusBar->Show(i_bChecked);
			twxPaneMgr::Update();
		}
#endif
	}
	bool Get_StatusBar()
	{

	#ifdef USE_WXWIDGETS
		return (twxSystem::g_pStatusBar) ? twxSystem::g_pStatusBar->IsShown() : false;
	#else
		return false;
	#endif

	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_AxisCompass(bool i_bChecked)
	{
		bool bShowWorldAxis = PrefsMgr::Data().m_bAxisCompassVisible.GetValue();
		PrefsMgr::Data().m_bAxisCompassVisible.SetValue( !bShowWorldAxis );
		//cmpsCompassMgr::SetRenderable( cmpsCompassMgr::e_World, i_bChecked );
	}
	bool Get_AxisCompass()
	{
		//return 	cmpsCompassMgr::GetRenderable( cmpsCompassMgr::e_World );
		return PrefsMgr::Data().m_bAxisCompassVisible.GetValue();;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_Shadows(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableShadows.SetValue(i_bChecked);
	}
	bool Get_Shadows()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableShadows.GetValue();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//void Set_Headlight(bool i_bChecked)
	//{
	//	rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bHeadlightOn.SetValue(i_bChecked);

	//	 
	//}
	//bool Get_Headlight()
	//{
	//	return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bHeadlightOn.GetValue();
	//}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_SSAO(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableSSAO.SetValue(i_bChecked);
	}
	bool Get_SSAO()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableSSAO.GetValue();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_Preview(bool i_bChecked)
	{
		// remember the state here, so it can be restored.
		static rprfPrefsData::eRenderPassChoice sCurrentPass = rprfPrefsData::e_PassBeauty;
		if (i_bChecked)
		{
			sCurrentPass = (rprfPrefsData::eRenderPassChoice)rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_RenderPassVP.GetValue();
			rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_RenderPassVP.SetValue(rprfPrefsData::e_PassPreview);
		}
		else
		{
			rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_RenderPassVP.SetValue(sCurrentPass);
		}
	}
	bool Get_Preview()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_RenderPassVP.GetValue() == rprfPrefsData::e_PassPreview;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_Next_RenderPass()
	{
		rndrPrefsMgr::IncrementPass(rndrPrefsMgr::e_ViewportPrefs);
	}
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_Prev_RenderPass()
	{
		rndrPrefsMgr::DecrementPass(rndrPrefsMgr::e_ViewportPrefs);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_HDR_Renderer()
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_RenderPassVP.SetValue(rprfPrefsData::e_PassBeauty);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_AO_Renderer()
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_RenderPassVP.SetValue(rprfPrefsData::e_PassAO);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_GI_Renderer()
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_RenderPassVP.SetValue(rprfPrefsData::e_PassGI);
	}

	void Set_Depth_Renderer()
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_RenderPassVP.SetValue(rprfPrefsData::e_PassDepth);
	}

	void Set_Shadows_Renderer()
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_RenderPassVP.SetValue(rprfPrefsData::e_PassShadowMask);
	}

	void Set_Normals_Renderer()
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_RenderPassVP.SetValue(rprfPrefsData::e_PassNormals);
	}

	void Set_Glow_Renderer()
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_RenderPassVP.SetValue(rprfPrefsData::e_PassGlow);
	}
	
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_RT_Renderer()
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_RenderPassVP.SetValue(rprfPrefsData::e_PassMaterials);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_SSAO_Low()
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_SSAOQuality.SetValue(0);
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_SSAOQualityVP.SetValue(0);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_SSAO_Med()
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_SSAOQuality.SetValue(1);
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_SSAOQualityVP.SetValue(1);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_SSAO_High()
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_SSAOQuality.SetValue(2);
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_SSAOQualityVP.SetValue(2);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_SSAODepthPeel(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableSSAODepthPeeling.SetValue(i_bChecked);
	}
	bool Get_SSAODepthPeel()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableSSAODepthPeeling.GetValue();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_Reflections(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableReflection.SetValue(i_bChecked);
	}
	bool Get_Reflections()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableReflection.GetValue();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
//	void Set_DeferParticles(bool i_bChecked)
//	{
//		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableDeferredTransparency.SetValue(i_bChecked);
//	}
//	bool Get_DeferParticles()
//	{
//		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableDeferredTransparency.GetValue();
//	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_Environments(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableEnvironment.SetValue(i_bChecked);

		 
	}
	bool Get_Environments()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableEnvironment.GetValue();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_Glow(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableGlow.SetValue(i_bChecked);

		
	}
	bool Get_Glow()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableGlow.GetValue();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_Outline(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableOutline.SetValue(i_bChecked);
	}
	bool Get_Outline()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableOutline.GetValue();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_DOF(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableDOF.SetValue(i_bChecked);
	}
	bool Get_DOF()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableDOF.GetValue();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_Lit(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableLitPass.SetValue(i_bChecked);
	}
	bool Get_Lit()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableLitPass.GetValue();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_Diffuse(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableDiffuseLighting.SetValue(i_bChecked);
	}
	bool Get_Diffuse()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableDiffuseLighting.GetValue();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_Specular(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableSpecularLighting.SetValue(i_bChecked);
	}
	bool Get_Specular()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableSpecularLighting.GetValue();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_Transparent(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableTransparent.SetValue(i_bChecked);
	}
	bool Get_Transparent()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableTransparent.GetValue();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_MotionBlur(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bMotionBlurEnable.SetValue(i_bChecked);
	}
	bool Get_MotionBlur()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bMotionBlurEnable.GetValue();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
//	void Set_AO(bool i_bChecked)
//	{
//		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableAO.SetValue(i_bChecked);
//	}
//	bool Get_AO()
//	{
//		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableAO.GetValue();
//	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_LowRes(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bLowResolution.SetValue(i_bChecked);
	}
	bool Get_LowRes()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bLowResolution.GetValue();
	}

	//--------------------------------------------------------------------
	//tessellation control
	//--------------------------------------------------------------------
	void Set_Tessellation(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bUseHardwareTessellation.SetValue(i_bChecked);
	}
	bool Get_Tessellation()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bUseHardwareTessellation.GetValue();
	}

	//--------------------------------------------------------------------
	//wireframe control
	//--------------------------------------------------------------------
	void Set_Wireframe(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_RenderPassVP.SetValue(rprfPrefsData::e_PassWireframe);
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bRenderWireframe.SetValue(i_bChecked);
	}
	bool Get_Wireframe()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bRenderWireframe.GetValue();
	}

	//--------------------------------------------------------------------
	//Hair
	//--------------------------------------------------------------------
	void Set_Hair(bool i_bChecked)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableHair.SetValue(i_bChecked);
	}
	bool Get_Hair()
	{
		return rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_bEnableHair.GetValue();
	}

	//--------------------------------------------------------------------
	// Test functions for handling error cases that shut down
	//	the program.
	//--------------------------------------------------------------------
	void KillApp()
	{
		// NULL pointer dereferencing
		//int *pBadPtr = 0;
		//*pBadPtr = 99;

		// Out of memory error
		throw std::bad_alloc();
	}
}

//--------------------------------------------------------------------
// SetupMenu
//--------------------------------------------------------------------
void mainCommands::SetupMenu()
{
	PrefsData prefsdata = PrefsMgr::Data();

	//
	//	commands
	//
	int menu_id;
	cmaCommand* pCmd;

	//
	//	Set Resolutions
	//
	guiMenuMgr::AddMenu( "View", "Resolutions" );
	guiMenuMgr::AddMenu( "View", "Toolbars" );

	//	read in the configuration file for resolutions
	//
	fsLocator cfgdir;
	cfgdir = gfPaths::GetPath(mnmPaths::e_DefaultConfigs);
	cfgdir.Push( lc_Resolutions_FileName );
	category_list_type resolutions(2, std::vector<std::string>(0));
	fsCategoryConfigFileUtil::SetCategoryTag(std::string("Category"));
	fsCategoryConfigFileUtil::SetElementTag(std::string("Resolution"));
	fsCategoryConfigFileUtil::ReadConfigFile( cfgdir, resolutions );
	
	//	Set-up the various parts of the application with each resolution
	//	from the list
	//
	for (int i=0; i < resolutions[0].size(); ++i)
	{
		//DBG_LOG3("%02d. %s - %s", i, resolutions[0][i].c_str(), resolutions[1][i].c_str());

		//	add the category (if it isn't already added)
		guiMenuMgr::AddMenu( "Resolutions", resolutions[0][i].c_str() );
		
		//	add the resolution to the menu, command, and system
		//char tempstr[64];
		//sprintf(tempstr,"panel res %s", resolutions[1][i].c_str());
		std::ostringstream oss;
		oss.setf(0, std::ios::floatfield);
		oss << "Panel res: "<< resolutions[1][i];
		std::string tempstr(oss.str());
		menu_id = guiMenuMgr::AddMenuItem( resolutions[0][i].c_str(), tempstr.c_str() );
		int width,height;
		sscanf( resolutions[1][i].c_str(), "%dx%d ", &width, &height );
		pCmd = new mainCommandSetResolution( width, height );
		const std::string tag(pCmd->GetTag());
		guiCommandMgr::Add( pCmd, tag, menu_id );
		//sprintf(tempstr,"%s - %s", resolutions[0][i].c_str(), resolutions[1][i].c_str());
		std::ostringstream os;
		os.setf(0, std::ios::floatfield);
		os << resolutions[0][i] << "-" <<resolutions[1][i];
		std::string temps(os.str());
		
		cmmSystemDialogUtil::AddSystemCommand( "Resolutions", temps.c_str(), pCmd );
	}

	//	COMMAND: File -> New
	pCmd = new cmaCommandSimple("New", 
								"File", 
								"Start a new scene",
								&Execute_File_New );
	menu_id = guiMenuMgr::AddMenuItem( "File", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "File", "New", pCmd );

	//	COMMAND: File -> Open
	pCmd = new cmaCommandSimple("Open", 
								"File", 
								"Open a scene",
								&Execute_File_Open );
	menu_id = guiMenuMgr::AddMenuItem( "File", pCmd->GetTag().c_str(), mnpConstants::mc_Toolbar_File_Name, "file-open.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "File", "Open", pCmd );

	//	COMMAND: File -> Close
	//pCmd = new cmaCommandSimple("Close", 
	//							"File", 
	//							"Close the current scene",
	//							&Execute_File_Close );
	//menu_id = guiMenuMgr::AddMenuItem( "File", pCmd->GetTag().c_str() );
	//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	//cmmSystemDialogUtil::AddSystemCommand( "File", "Close", pCmd );

	//	COMMAND: File -> Save
	pCmd = new cmaCommandSimple("Save", 
								"File", 
								"Save the current scene",
								&Execute_File_Save );
	menu_id = guiMenuMgr::AddMenuItem( "File", pCmd->GetTag().c_str(), mnpConstants::mc_Toolbar_File_Name, "file-save.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "File", "Save", pCmd );

	//	COMMAND: File -> SaveAs
	pCmd = new cmaCommandSimple("Save As", 
								"File", 
								"Save the current scene under a new name",
								&Execute_File_SaveAs );
	menu_id = guiMenuMgr::AddMenuItem( "File", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "File", "Save As", pCmd );

	//	COMMAND: File -> Revert
	pCmd = new cmaCommandSimple("Revert", 
								"File", 
								"Revert the current scene to the version saved",
								&Execute_File_Revert );
	menu_id = guiMenuMgr::AddMenuItem( "File", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "File", "Revert", pCmd );

	//	COMMAND: Bake
	/*pCmd = new cmaCommandSimple("Bake", 
								"File", 
								"Bake materials and lighting",
								&fgmtOperations::Bake );
	menu_id = guiMenuMgr::AddMenuItem( "File", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "File", "Bake", pCmd );*/

	//	COMMAND: File -> Exit
	pCmd = new cmaCommandSimple("Exit", 
								"File", 
								"Exit the application",
								&Execute_File_Exit );
	menu_id = guiMenuMgr::AddMenuItem( "File", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "File", "Exit", pCmd );

	//bga - Note, this command is registered here instead of in system characters
	// in order to control the order of the hotkeys in relation to the LoadAnimation
	// command.
	//
	//	COMMAND: Create -> Load Object
	pCmd = new cmaCommandSimple("Load Objects", 
								"Create", 
								"Loads new geometry objects into scene",
								&chtrOperations::BrowseLoadObjects );
	menu_id = guiMenuMgr::AddMenuItem( "Create", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );


	//	COMMAND: Load Animation
	pCmd = new cmaCommandSimple("Load Animation", 
								"Create", 
								"Load animation files unto the selected object",
								&mainOperations::BrowseLoadAnimation );
	menu_id = guiMenuMgr::AddMenuItem( "Create", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		

	//	COMMAND: File toolbar
	pCmd = new cmaCommandToggle("File Toolbar", 
								"Toolbars", 
								"View the file toolbar",
								&Set_FileToolbar, 
								&Get_FileToolbar );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Toolbars", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "View File Toolbar", pCmd );

	//	COMMAND: Action toolbar
	pCmd = new cmaCommandToggle("Action Toolbar", 
								"Toolbars", 
								"View the action toolbar",
								&Set_ActionToolbar, 
								&Get_ActionToolbar );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Toolbars", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "View Action Toolbar", pCmd );

	//	COMMAND: Mode toolbar
	pCmd = new cmaCommandToggle("Render Toolbar", 
								"Toolbars", 
								"View the Render toolbar",
								&Set_ModeToolbar, 
								&Get_ModeToolbar );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Toolbars", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "View Render Toolbar", pCmd );

	//COMMAND: Export toolbar
	pCmd = new cmaCommandToggle("Export Toolbar", 
								"Toolbars", 
								"View the Export toolbar",
								&Set_ExportToolbar, 
								&Get_ExportToolbar );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Toolbars", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "View Export Toolbar", pCmd );

	//	COMMAND: Pick Mode toolbar
	pCmd = new cmaCommandToggle("Pick Mode Toolbar", 
								"Toolbars", 
								"View the PickMode toolbar",
								&Set_PickModeToolbar, 
								&Get_PickModeToolbar );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Toolbars", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "View PickMode Toolbar", pCmd );

	//	COMMAND: Pick Mode toolbar
#if(SGPU_APP != MS_CORE)
	pCmd = new cmaCommandToggle("Python Script Toolbar", 
								"Toolbars", 
								"View the Script toolbar",
								&Set_ScriptToolbar, 
								&Get_ScriptToolbar );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Toolbars", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "View Script Toolbar", pCmd );
#endif

	//	COMMAND:  TimeLine
	pCmd = new cmaCommandSimple("Time Slider", 
								"Windows", 
								"View the Time Slider",
								&Show_TimeLine );
	menu_id = guiMenuMgr::AddMenuItem( "Windows", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "View Time Slider", pCmd );

	//	COMMAND:  Tags
	pCmd = new cmaCommandSimple("Tags", 
								"Windows", 
								"View the valid tags for custom sections",
								&Show_Tags );
	menu_id = guiMenuMgr::AddMenuItem( "Windows", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "View Tags Dialog", pCmd );

	//	COMMAND:  StatusBar
	pCmd = new cmaCommandToggle("Status Bar", 
								"View", 
								"View the StatusBar",
								&Set_StatusBar, 
								&Get_StatusBar );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "View StatusBar", pCmd );

	//	COMMAND:  Channels
	pCmd = new cmaCommandSimple("Timeline Editor", 
								"Windows", 
								"View the Channels dialog",
								&chnlDialogUtil::ShowChannelEditor );
	menu_id = guiMenuMgr::AddMenuItem( "Windows", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "View Channels", pCmd );

	//	COMMAND: View AxisCompass
	pCmd = new cmaCommandToggle("Axis Compass", 
								"View", 
								"Toggle Axis Compass Visibility",
								&Set_AxisCompass,
								&Get_AxisCompass );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle AxisCompass Visibility", pCmd );


	guiMenuMgr::AddSeparator("View");
	guiMenuMgr::AddMenu( "View", "Visualize Passes" );

	//	COMMAND: Use next pass
	pCmd = new cmaCommandSimple("Next Pass", 
								"Visualize Passes", 
								"Show next render pass",
								&Set_Next_RenderPass );
	menu_id = guiMenuMgr::AddMenuItem( "Visualize Passes", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Show next render pass", pCmd );

	//	COMMAND: Use previous pass
	pCmd = new cmaCommandSimple("Previous Pass", 
								"Visualize Passes", 
								"Show previous render pass",
								&Set_Prev_RenderPass );
	menu_id = guiMenuMgr::AddMenuItem( "Visualize Passes", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Show previous render pass", pCmd );


	//	COMMAND: Use HDR renderer
	pCmd = new cmaCommandSimple("Beauty", 
								"Visualize Passes", 
								"Use the beauty pass",
								&Set_HDR_Renderer );
	menu_id = guiMenuMgr::AddMenuItem( "Visualize Passes", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Use the beauty pass", pCmd );


	//	COMMAND: Use AO-Only renderer
	pCmd = new cmaCommandSimple("AO Only", 
								"Visualize Passes", 
								"Use the AO Only pass",
								&Set_AO_Renderer );
	menu_id = guiMenuMgr::AddMenuItem( "Visualize Passes", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Use AO Only renderer", pCmd );

	//	COMMAND: Use GI-Only renderer
	pCmd = new cmaCommandSimple("GI Only", 
								"Visualize Passes", 
								"Use the GI Only renderer",
								&Set_GI_Renderer );
	menu_id = guiMenuMgr::AddMenuItem( "Visualize Passes", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Use GI Only renderer", pCmd );

	//	COMMAND: Use Depth Buffer renderer
	pCmd = new cmaCommandSimple("Depth Buffer", 
		"Visualize Passes", 
		"Use the Depth Buffer renderer",
		&Set_Depth_Renderer );
	menu_id = guiMenuMgr::AddMenuItem( "Visualize Passes", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Use Depth Buffer renderer", pCmd );

	//	COMMAND: Use Shadows-Only renderer
	pCmd = new cmaCommandSimple("Shadows Only", 
		"Visualize Passes", 
		"Use the Shadows Only renderer",
		&Set_Shadows_Renderer );
	menu_id = guiMenuMgr::AddMenuItem( "Visualize Passes", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Use Shadows Only renderer", pCmd );

	//	COMMAND: Use Normals renderer
	pCmd = new cmaCommandSimple("Normals", 
		"Visualize Passes", 
		"Use the Normals renderer",
		&Set_Normals_Renderer );
	menu_id = guiMenuMgr::AddMenuItem( "Visualize Passes", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Use Normals renderer", pCmd );

	//	COMMAND: Use GI-Only renderer
	pCmd = new cmaCommandSimple("Glow", 
								"Visualize Passes", 
								"Use the Glow renderer",
								&Set_Glow_Renderer );
	menu_id = guiMenuMgr::AddMenuItem( "Visualize Passes", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Use Glow renderer", pCmd );

	//** NOTE: Any new render pass menu items should also be added to the context menu
	// in rpnRenderPanel.cpp **


	//	COMMAND: View Shadows
	pCmd = new cmaCommandToggle("Preview Toggle", 
								"View", 
								"Toggle Preview/Beauty pass",
								&Set_Preview,
								&Get_Preview );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Preview/Beauty Pass", pCmd );

	//	COMMAND: View SSAO
	pCmd = new cmaCommandToggle("Ambient Occlusion", 
								"View", 
								"Toggle Ambient Occlusion Visibility",
								&Set_SSAO,
								&Get_SSAO );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Ambient Occlusion Visibility", pCmd );

	guiMenuMgr::AddMenu( "View", "AO Sampling" );
	//	COMMAND: SSAO low sampling 
	pCmd = new cmaCommandSimple("Low", 
								"Ambient Occlusion Sampling", 
								"Ambient Occlusion low sampling",
								&Set_SSAO_Low );
	menu_id = guiMenuMgr::AddMenuItem( "AO Sampling", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "AO low sampling", pCmd );
	//	COMMAND: SSAO med sampling 
	pCmd = new cmaCommandSimple("Medium", 
								"Ambient Occlusion Sampling", 
								"Ambient Occlusion medium sampling",
								&Set_SSAO_Med );
	menu_id = guiMenuMgr::AddMenuItem( "AO Sampling", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "AO medium sampling", pCmd );
	//	COMMAND: SSAO high sampling 
	//pCmd = new cmaCommandSimple("High", 
	//							"Ambient Occlusion Sampling", 
	//							"Ambient Occlusion high sampling",
	//							&Set_SSAO_High );
	//menu_id = guiMenuMgr::AddMenuItem( "AO Sampling", pCmd->GetTag().c_str() );
	//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	//cmmSystemDialogUtil::AddSystemCommand( "Main", "AO high sampling", pCmd );
	pCmd = new cmaCommandToggle("AO Multiple Depths", 
		"View", 
		"Toggle Ambient Occlusion Multiple Depth Layers",
		&Set_SSAODepthPeel,
		&Get_SSAODepthPeel );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Ambient Occlusion Multiple Depth Layers", pCmd );

	//	COMMAND: View Headlight
	//pCmd = new cmaCommandToggle("Headlight", 
	//							"View", 
	//							"Toggle Headlight Enabled",
	//							&Set_Headlight,
	//							&Get_Headlight );
	//menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	//cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Headlight Enabled", pCmd );

	guiMenuMgr::AddMenu( "View", "Advanced Render Flags" );
	//	COMMAND: View Passes DOF
//	pCmd = new cmaCommandToggle("DOF", 
//		"Advanced Render Flags", 
//		"Toggle Depth Of Field",
//		&Set_DOF,
//		&Get_DOF );
//	menu_id = guiMenuMgr::AddCheckableMenuItem( "Advanced Render Flags", pCmd->GetTag().c_str() );
//	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
//	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Depth Of Field", pCmd );

	//	COMMAND: View Passes Glow
	pCmd = new cmaCommandToggle("Glow", 
		"Advanced Render Flags", 
		"Toggle Glow Visibility",
		&Set_Glow,
		&Get_Glow );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Advanced Render Flags", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Glow Visibility", pCmd );

	// COMMAND: View Passes Outline
	pCmd = new cmaCommandToggle("Outline", 
		"Advanced Render Flags", 
		"Toggle Outline Visibility",
		&Set_Outline,
		&Get_Outline );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Advanced Render Flags", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Outline Visibility", pCmd );

	//	COMMAND: View Passes Environments
	pCmd = new cmaCommandToggle("Environment Lights", 
		"Advanced Render Flags", 
		"Toggle Environments Visibility",
		&Set_Environments,
		&Get_Environments );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Advanced Render Flags", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Environments Visibility", pCmd );

	//	COMMAND: View Passes Lit
//	pCmd = new cmaCommandToggle("Lit", 
//		"Advanced Render Flags", 
//		"Toggle Lit Pass",
//		&Set_Lit,
//		&Get_Lit );
//	menu_id = guiMenuMgr::AddCheckableMenuItem( "Advanced Render Flags", pCmd->GetTag().c_str() );
//	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
//	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Lit Pass", pCmd );

	//	COMMAND: View Passes Diffuse
//	pCmd = new cmaCommandToggle("Diffuse", 
//		"Advanced Render Flags", 
//		"Toggle Diffuse Pass",
//		&Set_Diffuse,
//		&Get_Diffuse );
//	menu_id = guiMenuMgr::AddCheckableMenuItem( "Advanced Render Flags", pCmd->GetTag().c_str() );
//	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
//	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Diffuse Pass", pCmd );

	//	COMMAND: View Passes Specular
//	pCmd = new cmaCommandToggle("Specular", 
//		"Advanced Render Flags", 
//		"Toggle Specular Pass",
//		&Set_Specular,
//		&Get_Specular );
//	menu_id = guiMenuMgr::AddCheckableMenuItem( "Advanced Render Flags", pCmd->GetTag().c_str() );
//	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
//	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Specular Pass", pCmd );

	//	COMMAND: View Passes Shadows
	pCmd = new cmaCommandToggle("Shadows", 
		"Advanced Render Flags", 
		"Toggle Shadow Pass",
		&Set_Shadows,
		&Get_Shadows );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Advanced Render Flags", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Shadow Pass", pCmd );

	//	COMMAND: View Passes Transparent
//	pCmd = new cmaCommandToggle("Transparent", 
//		"Advanced Render Flags", 
//		"Toggle Transparent Pass",
//		&Set_Transparent,
//		&Get_Transparent );
//	menu_id = guiMenuMgr::AddCheckableMenuItem( "Advanced Render Flags", pCmd->GetTag().c_str() );
//	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
//	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Transparent Pass", pCmd );

	//	COMMAND: View Reflections
	pCmd = new cmaCommandToggle("Reflections", 
								"Advanced Render Flags", 
								"Toggle Reflections Visibility",
								&Set_Reflections,
								&Get_Reflections );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Advanced Render Flags", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Reflections Visibility", pCmd );

	//	COMMAND: Defer Particles
	//pCmd = new cmaCommandToggle("DeferParticles", 
	//							"View", 
	//							"Defer Particle rendering",
	//							&Set_DeferParticles,
	//							&Get_DeferParticles );
	//menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	//cmmSystemDialogUtil::AddSystemCommand( "Main", "Defer Particle rendering", pCmd );



	//	COMMAND: View AO
/*
	pCmd = new cmaCommandToggle("AO", 
								"View", 
								"Toggle Ambient Occlusion Visibility",
								&Set_AO,
								&Get_AO );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Ambient Occlusion Visibility", pCmd );
*/


	//	COMMAND: View LowRes
//	pCmd = new cmaCommandToggle("LowRes", 
//								"View", 
//								"Toggle Low Resolution",
//								&Set_LowRes,
//								&Get_LowRes );
//	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
//	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
//	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Low Resolution", pCmd );

#ifdef HAIR_SUPPORTED
	//	COMMAND: View Hair
	pCmd = new cmaCommandToggle("Hair", 
		"View", 
		"Enables Hair Rendering",
		&Set_Hair,
		&Get_Hair );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Enables Hair Rendering", pCmd );
#endif//HAIR_SUPPORTED

	//	COMMAND: View Tessellation
	pCmd = new cmaCommandToggle("Enable Tessellation", 
		"Hardware Tessellation", 
		"Toggle Hardware Tessellation",
		&Set_Tessellation,
		&Get_Tessellation );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Hardware Tessellation", pCmd );

	//	COMMAND: View Wireframe
	pCmd = new cmaCommandToggle("Wireframe", 
		"View", 
		"Toggle Wireframe rendering",
		&Set_Wireframe,
		&Get_Wireframe );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "Toggle Wireframe", pCmd );

	//	COMMAND: Mute Audio
	pCmd = new cmaCommandToggle("Mute Audio", 
								"Actions", 
								"Mute the audio during scrubbing or playback",
								&Set_MuteAudio, 
								&snSoundManager::IsMuted );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Actions", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Actions", "Mute Audio", pCmd );

	//	COMMAND: Help dialog
	pCmd = new cmaCommandSimple("Help Info", 
								"Help", 
								"Help dialog",
								&Execute_Help );
	menu_id = guiMenuMgr::AddMenuItem( "Help", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Help", "Help Info", pCmd );

	guiMenuMgr::AddSeparator( "Help" );

	//	COMMAND: Launch web site: Support
	pCmd = new cmaCommandSimple("Support", 
								"Help", 
								"On-line support page",
								&Execute_Online_Support );
	menu_id = guiMenuMgr::AddMenuItem( "Help", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Help", "Support", pCmd );

	//	COMMAND: Launch web site: Manual
	pCmd = new cmaCommandSimple("Manual", 
								"Help", 
								"On-line Manual",
								&Execute_Online_Manual );
	menu_id = guiMenuMgr::AddMenuItem( "Help", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Help", "Manual", pCmd );

	//	COMMAND: Launch web site: Tutorials
	pCmd = new cmaCommandSimple("Tutorials", 
								"Help", 
								"On-line Tutorials",
								&Execute_Online_Tutorials );
	menu_id = guiMenuMgr::AddMenuItem( "Help", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Help", "Tutorials", pCmd );

	//	COMMAND: Launch web site: studio|gpu
	pCmd = new cmaCommandSimple("studio|gpu", 
								"Help", 
								"studio|gpu Home Page",
								&Execute_Online_StudioGPU );
	menu_id = guiMenuMgr::AddMenuItem( "Help", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Help", "studio|gpu", pCmd );

	guiMenuMgr::AddSeparator( "Help" );

	//	COMMAND: check for latest version of application
	pCmd = new cmaCommandSimple("Check for Updates", 
								"Help", 
								"Check if there is a newer version of application",
								&Execute_Online_CheckLatestVersion );
	menu_id = guiMenuMgr::AddMenuItem( "Help", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Help", "Updates", pCmd );

	guiMenuMgr::AddSeparator( "Help" );

	//	COMMAND: About dialog
	pCmd = new cmaCommandSimple("About", 
								"Help", 
								"About dialog",
								&Execute_About );
	menu_id = guiMenuMgr::AddMenuItem( "Help", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Help", "About", pCmd );

//------------------------------------------------------------------------------
	
	//	COMMAND: Kill app
	//guiMenuMgr::AddMenu("Testing", "");
	//pCmd = new cmaCommandSimple("KillApp", 
	//							"Testing", 
	//							"Shut down application with error, test handling.",
	//							&KillApp );
	//menu_id = guiMenuMgr::AddMenuItem( "Testing", pCmd->GetTag().c_str() );
	//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
}