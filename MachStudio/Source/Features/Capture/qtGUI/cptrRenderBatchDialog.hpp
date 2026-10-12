/*****************************************************************************
**	cptrRenderBatchDialog.hpp
**
**		Dialog for setting up a batch of scenes to render, in Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_RENDERBATCHDIALOG_HPP
#error cptrRenderBatchDialog.hpp multiply included
#endif
#define CPTR_RENDERBATCHDIALOG_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifndef CPTR_RENDERBATCHDATA_HPP
#include "Features/Capture/cptrRenderBatchData.hpp"
#endif

#ifdef USE_QT

class QCheckBox;
class QLineEdit;
class QListWidget;
class QPushButton;

//============================================================================
// Class cptrRenderBatchDialog
//============================================================================
class cptrRenderBatchDialog : public QDialog
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cptrRenderBatchDialog(cptrRenderBatchData& i_Data, QWidget* parent);

	protected:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void closeEvent( QCloseEvent* i_pEvent ) override;

	private:
		void set_continue_button();
		void ReadFile(const fsLocator &i_Loc);
		void SaveFile(const fsLocator &i_Loc);
		void UpdateFileDirectory( fsLocator& i_dir );
		void SetDataValues();
		void SetComponentValues();
		void add_scene(const itString& i_Filename, bool i_bChecked);

		//--------------------------------------------------------------------
		// event handling
		//--------------------------------------------------------------------
		void button_AddScene_OnButtonClick();
		void button_MoveDown_OnButtonClick();
		void button_MoveUp_OnButtonClick();
		void button_AddCurrent_OnButtonClick();
		void button_DeleteScene_OnButtonClick();
		void button_LoadBatch_OnButtonClick();
		void button_SaveBatch_OnButtonClick();
		void button_Continue_OnButtonClick();

		QListWidget* m_checkList_Scenes;
		QPushButton* m_button_AddCurrent;
		QCheckBox* m_checkBox_SkipScene;
		QLineEdit* m_textCtrl_BatchFile;
		QPushButton* m_button_Continue;

		cptrRenderBatchData& m_Data;
};

#endif
