/*****************************************************************************
**	dcutCueDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/DirectorsCut/Cue/dcutCueDialogUtil.hpp"

#include "Systems/DirectorsCut/Cue/dcutCueDataUtil.hpp"
//#include "Systems/DirectorsCut/Cue/CameraCueForm.h"

#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"
#include "Support/tmln/tmlnTimeInterest.hpp"


//============================================================================
//============================================================================
namespace dcutCueDialogUtil
{
	namespace
	{
		//class dcutCueTimeInterest : public tmlnTimeInterest
		//{
		//	virtual void TimeChanged(float i_Time)
		//	{
		//		dcutCueDialogUtil::UpdateDialog(i_Time);
		//	}
		//	virtual void TimeRangeChanged(float i_MinTime, float i_MaxTime)
		//	{
		//	}
		//	virtual void TimeFormatChanged(int i_TimeFormat)
		//	{
		//	}
		//};

		//dcutCueTimeInterest* l_pInterest = 0;
		g2dSystem* l_pSystem = 0;

	}	// end of namespace


	//--------------------------------------------------------------------
	// Init - needs the system for creating rendering views
	//--------------------------------------------------------------------
	void Init(g2dSystem *i_pSystem)
	{
		l_pSystem = i_pSystem;
		//l_pInterest = new dcutCueTimeInterest();
		//tmlnTimeLine::AddTimeInterest(l_pInterest);
	}

	//--------------------------------------------------------------------
	// CleanUp
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//tmlnTimeLine::RemoveTimeInterest(l_pInterest);
		//delete l_pInterest;
	}

	//--------------------------------------------------------------------
	// Show
	//--------------------------------------------------------------------
	void  Show()
	{		
		guiMessageBox::Show("Camera Cue Dialog is disabled temporarily.", "No Camera Cue");
		return;


		//// Create tab page dialog
		//if (!CameraCueForm::FormInstance)
		//{
		//	maPoint2d size = tma3dScreenUtil::GetWindowSize();
		//	CameraCueForm::FormInstance = new CameraCueForm(size.m_X / size.m_Y);
		//	CameraCueForm::FormInstance->CreateViewers(*l_pSystem);
		//	tmaSystem::g_pMainForm->AddOwnedForm(CameraCueForm::FormInstance);
		//}
		//CameraCueForm::FormInstance->Show();
		//if (CameraCueForm::FormInstance->WindowState == FormWindowState::Minimized)
		//	CameraCueForm::FormInstance->WindowState = FormWindowState::Normal;

		//UpdateCameraNames();
	}

	//--------------------------------------------------------------------
	// Update dialog for current timeline time
	//--------------------------------------------------------------------
	void UpdateDialog(float i_Time)
	{

	}

	//--------------------------------------------------------------------
	// UpdateCameraNames - call this when cameras are added/deleted
	//	or if a name has been changed
	//--------------------------------------------------------------------
	void  UpdateCameraNames()
	{

	}

}	// end of namespace
