/*****************************************************************************
**	cptrRenderBatchDialog.hpp
**
**		ementation for the dialog
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_RENDERBATCHDIALOG_HPP
#error cptrRenderBatchDialog.hpp multiply included
#endif
#define CPTR_RENDERBATCHDIALOG_HPP

//	needed before App so USE_WXWIDGETS is set
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

//	App
#ifndef CPTR_RENDERBATCHDATA_HPP
#include "Features/Capture/cptrRenderBatchData.hpp"
#endif
#ifdef USE_WXWIDGETS
#include "Features/Capture/wxGUI/cptrRenderBatchDialogBase.h"
#endif


#ifdef USE_WXWIDGETS

//============================================================================
// Class RenderBatchDialog
//============================================================================
class cptrRenderBatchDialog : public cptrRenderBatchDialogBase
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cptrRenderBatchDialog(cptrRenderBatchData& i_Data, wxWindow* parent);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//~cptrRenderBatchDialog();

	private:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void set_continue_button();

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void ReadFile(const fsLocator &i_Loc);

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void SaveFile(const fsLocator &i_Loc);

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void UpdateFileDirectory( fsLocator& i_dir );

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void SetDataValues();

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void SetComponentValues();

	//
	//	Dialog Events
	//
	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void OnClose( wxCloseEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void checkList_Scenes_OnCheckListBoxDClick( wxCommandEvent& event );
		virtual void checkList_Scenes_OnCheckListBoxToggled( wxCommandEvent& event );
		virtual void button_AddScene_OnButtonClick( wxCommandEvent& event );
		virtual void button_MoveDown_OnButtonClick( wxCommandEvent& event );
		virtual void button_MoveUp_OnButtonClick( wxCommandEvent& event );
		virtual void button_AddCurrent_OnButtonClick( wxCommandEvent& event );
		virtual void button_DeleteScene_OnButtonClick( wxCommandEvent& event );
		virtual void textCtrl_BatchFile_OnTextEnter( wxCommandEvent& event );
		virtual void button_LoadBatch_OnButtonClick( wxCommandEvent& event );
		virtual void button_SaveBatch_OnButtonClick( wxCommandEvent& event );
		virtual void button_Continue_OnButtonClick( wxCommandEvent& event );

	private:
		cptrRenderBatchData& m_Data;

};

#endif