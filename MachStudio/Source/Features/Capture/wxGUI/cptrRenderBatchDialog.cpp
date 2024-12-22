/*****************************************************************************
**	cptrRenderBatchDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/wxGUI/cptrRenderBatchDialog.hpp"

#include "Features/Capture/cptrModeRender.hpp"
#include "Features/Capture/cptrRenderBatchDialogUtil.hpp"
#include "Features/Capture/cptrRenderBatchDataParser.hpp"
#include "Features/Capture/cptrRenderBatchDataUtil.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"

#undef CreateFile
#undef DeleteFile

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiFileDialogUtils.hpp"
#include "Tool/gui/guiMessageBox.hpp"

#include <wx/dynarray.h>
#include <wx/ctrlsub.h>

//#include <windows.h>

#ifdef USE_WXWIDGETS
#define ID_DEFAULT wxID_ANY // Default


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cptrRenderBatchDialog::cptrRenderBatchDialog(cptrRenderBatchData& i_Data, wxWindow* parent)
:	cptrRenderBatchDialogBase(parent),
	m_Data(i_Data)
{
	//	the continue button sets this to false.
	m_Data.m_BatchAborted = true;

	//	if there is a current scene enable the current scene button
	std::string sceneFilename;
	if ( docSingleDocumentMgr::GetFilename().GetNumNames() > 0 )
	{
		m_button_AddCurrent->Enable(true);
	}
	else
	{
		m_button_AddCurrent->Enable(true);
	}
	m_checkBox_SkipScene->SetValue(true);

	set_continue_button();
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cptrRenderBatchDialog::set_continue_button()
{
	if (   (m_Data.m_BatchFilename.GetValue().GetNumNames() > 0)
		|| (m_checkList_Scenes->GetCount() > 0)
		)
	{
		m_button_Continue->Enable(true);
	}
	else
	{
		m_button_Continue->Enable(false);
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cptrRenderBatchDialog::ReadFile(const fsLocator &i_Loc)
{
	try
	{
		cptrRenderBatchData& data = cptrRenderBatchDataUtil::Data();
		cptrRenderBatchDataUtil::ReadData( i_Loc, data );
		m_Data = data;
		//DBG_LOG( "Read " << m_Data.m_Scenes.size() << " items" );
	}
	catch ( const envExceptionX& i_Ex )
	{
		std::string msg = "Error reading batch data, " + i_Ex.GetErrorMessage();
		DBG_ERROR(msg);
		guiMessageBox::Show(msg.c_str(), "Batch Error", guiMessageBox::e_OKOnly);
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cptrRenderBatchDialog::SaveFile(const fsLocator &i_Loc)
{
	SetDataValues();

	// Create document file
	//
	if (fsFileUtil::FileExists(i_Loc))
	{
		fsFileUtil::DeleteFile(i_Loc);
	}

	fsFileUtil::CreateFile(i_Loc);

	try
	{
		cptrRenderBatchDataUtil::WriteData(i_Loc,m_Data);
	}
	catch ( const envExceptionX& i_Ex )
	{
		std::string msg = "Error writing batch data, " + i_Ex.GetErrorMessage();
		DBG_ERROR(msg);
		guiMessageBox::Show(msg.c_str(), "Batch Error", guiMessageBox::e_OKOnly);
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cptrRenderBatchDialog::UpdateFileDirectory( fsLocator& i_dir )
{
	itString sdir;
	fsFileUtil::LocatorToUnicodeString(i_dir, sdir);

	m_textCtrl_BatchFile->SetValue( sdir.GetString() );

	cptrRenderBatchData& data = cptrRenderBatchDataUtil::Data();
	data.m_BatchFilename.SetValue(i_dir);
	m_Data.m_BatchFilename.SetValue(i_dir);

	//DBG_LOG("cap batch Batch (%s)" << m_Data.m_BatchFilename.GetValue());
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cptrRenderBatchDialog::SetDataValues()
{
	int numitems = m_checkList_Scenes->GetCount();

	m_Data.m_Scenes.clear();
	m_Data.m_Scenes.resize( numitems );

	//DBG_LOG( "items in list box = " << numitems );
	int i;
	for ( i = 0 ; i < numitems ; ++i )
	{
		fsLocator dir;
		fsFileUtil::UnicodeStringToLocator( itString((const char*)m_checkList_Scenes->GetString(i).c_str()), dir);
		m_Data.m_Scenes[i].m_Filename.SetValue(dir);
		m_Data.m_Scenes[i].m_bChecked.SetValue(m_checkList_Scenes->IsChecked(i));

		//std::string sceneFilename;
		//sceneFilename = itStringUtil::GetStdString( m_Data.m_Scenes[i].m_Filename.GetValue().GetLastName() );
 		//DBG_LOG( i << " - " << sceneFilename.c_str(), << " (" << (m_Data.m_Scenes[i].m_bChecked ? "true":"false") << ")" );
	}

	m_Data.m_SkipDialog = m_checkBox_SkipScene->GetValue();

	cptrRenderBatchDataUtil::UpdateData(m_Data);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cptrRenderBatchDialog::SetComponentValues()
{
	m_textCtrl_BatchFile->Clear();
	m_checkList_Scenes->Clear();

	//DBG_LOG( "setting items = " << m_Data.m_Scenes.size() );

	int i;
	for ( i = 0 ; i < m_Data.m_Scenes.size() ; ++i )
	{
		itString filename;
		fsFileUtil::LocatorToUnicodeString(m_Data.m_Scenes[i].m_Filename.GetValue(), filename);
		m_checkList_Scenes->Append(filename.GetString());
		m_checkList_Scenes->Check(i, m_Data.m_Scenes[i].m_bChecked.GetValue() );
	}

	m_checkBox_SkipScene->SetValue(m_Data.m_SkipDialog.GetValue());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void cptrRenderBatchDialog::OnClose( wxCloseEvent& event )
{
	SetDataValues();

	cptrRenderBatchDialogUtil::Hide();
	cptrRenderBatchDialogUtil::SetDialogClosing( true );
	if (m_Data.m_BatchAborted.GetValue())
		cptrRenderBatchDialogUtil::SetDialogExitting(true);

	//this->Close();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
void cptrRenderBatchDialog::checkList_Scenes_OnCheckListBoxDClick( wxCommandEvent& event )
{
	set_continue_button();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
void cptrRenderBatchDialog::checkList_Scenes_OnCheckListBoxToggled( wxCommandEvent& event )
{ 
	set_continue_button();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
void cptrRenderBatchDialog::button_AddScene_OnButtonClick( wxCommandEvent& event )
{ 
	std::string filter = "scene files (*.mab)|*.mab|All files (*.*)|*.*";
	fsLocator file_loc;
	fsLocator initial_dir;

	//	set initial dir to the scene dir or just "Shots"
	//
	//initial_dir = gfPaths::GetAppPath();
	//initial_dir.Push("Shots");

	if ( docSingleDocumentMgr::GetFilename().GetNumNames() > 0 )
	{
		initial_dir = docSingleDocumentMgr::GetFilename();
		initial_dir.Pop();
	}
	itString init_dir;
	itString filter_str(filter.c_str());
	fsFileUtil::LocatorToUnicodeString( initial_dir, init_dir );
	wxFileDialog dialog(NULL,_T("Open File"), init_dir.GetString(),wxEmptyString,
						filter_str.GetString(),wxFD_MULTIPLE);

	if(dialog.ShowModal() == wxID_OK)
	{
		wxArrayString paths;
		dialog.GetPaths(paths);
		for ( size_t i = 0 ; i < paths.size() ; i ++ )
		{
            fsFileUtil::UnicodeStringToLocator(itString(paths[i].wc_str()), file_loc);
			itString filename;
			fsFileUtil::LocatorToUnicodeString(file_loc, filename);
			m_checkList_Scenes->Append(filename.GetString());
			m_checkList_Scenes->Check(m_checkList_Scenes->GetCount()-1, true );
		}
	}


	//	let the user choose
	/*if (guiFileDialogUtils::GetOpenFileName(filter, initial_dir, file_loc))
	{
		itString filename;
		fsFileUtil::LocatorToUnicodeString(file_loc, filename);
		m_checkList_Scenes->Append(filename.GetString());
		m_checkList_Scenes->Check(m_checkList_Scenes->GetCount()-1, true );
	}*/

	set_continue_button();
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
void cptrRenderBatchDialog::button_MoveUp_OnButtonClick( wxCommandEvent& event )
{
	int selection = m_checkList_Scenes->GetSelection();
	bool sel = m_checkList_Scenes->IsChecked(selection);
	wxString prev, curr;
	prev = m_checkList_Scenes->GetString(selection -1 );
	curr = m_checkList_Scenes->GetString(selection);
	if(selection == wxNOT_FOUND) return;
	//m_checkList_Scenes->Insert(prev,selection );
	m_checkList_Scenes->Insert(curr,selection -1 );
	m_checkList_Scenes->Delete(selection + 1);
	if(sel) 
	{
		m_checkList_Scenes->Check(selection -1, true);
		
	}
	m_checkList_Scenes->SetSelection(selection -1 );
	set_continue_button();
	
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
void cptrRenderBatchDialog::button_MoveDown_OnButtonClick( wxCommandEvent& event )
{
	int selection = m_checkList_Scenes->GetSelection();
	bool sel = m_checkList_Scenes->IsChecked(selection);
	wxString prev, curr;
	prev = m_checkList_Scenes->GetString(selection -1 );
	curr = m_checkList_Scenes->GetString(selection);
	if(selection == wxNOT_FOUND) return;
	m_checkList_Scenes->Insert(curr,selection+2);
	m_checkList_Scenes->Delete(selection);
	if(sel) 
	{
		m_checkList_Scenes->Check(selection + 1, true );
		
	}
	m_checkList_Scenes->SetSelection(selection +1 );
	set_continue_button();
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
void cptrRenderBatchDialog::button_AddCurrent_OnButtonClick( wxCommandEvent& event )
{ 
	fsLocator file_loc;
	if ( docSingleDocumentMgr::GetFilename().GetNumNames() > 0 )
	{
		file_loc = docSingleDocumentMgr::GetFilename();

		itString filename;
		fsFileUtil::LocatorToUnicodeString(file_loc, filename);
		m_checkList_Scenes->Append(filename.GetString());
		m_checkList_Scenes->Check(m_checkList_Scenes->GetCount()-1, true );
	}

	set_continue_button();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
void cptrRenderBatchDialog::button_DeleteScene_OnButtonClick( wxCommandEvent& event )
{
	wxArrayInt selections;

	m_checkList_Scenes->GetSelections(selections);
	for (int i = 0; i < selections.size(); ++i)
	{
		m_checkList_Scenes->Delete(selections[i]);
	}

	set_continue_button();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
void cptrRenderBatchDialog::textCtrl_BatchFile_OnTextEnter( wxCommandEvent& event )
{ 
	//	TO DO verify a typed in filename + path
	event.Skip(); 
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
void cptrRenderBatchDialog::button_LoadBatch_OnButtonClick( wxCommandEvent& event )
{ 
	std::string filter = "scene batch files (*.scb)|*.scb|All files (*.*)|*.*";
	fsLocator file_loc;
	fsLocator initial_dir;
	//initial_dir = gfPaths::GetAppPath();
	//initial_dir.Push("Footage");

	if (guiFileDialogUtils::GetOpenFileName(filter, initial_dir, file_loc))
	{
		std::string name;
		name = itStringUtil::GetStdString(file_loc.GetLastName());
		//DBG_LOG( "Loading batch file (" << name.c_str() << ")" );

		ReadFile(file_loc);

		SetComponentValues();

		UpdateFileDirectory( file_loc );

		set_continue_button();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
void cptrRenderBatchDialog::button_SaveBatch_OnButtonClick( wxCommandEvent& event )
{ 
	std::string filter = "scene batch files (*.scb)|*.scb|All files (*.*)|*.*";
	fsLocator file_loc;
	//file_loc = gfPaths::GetAppPath();
	//file_loc.Push("Footage");
	if (guiFileDialogUtils::GetSaveFileName(filter, file_loc))
	{
		DBG_LOG( "Saving batch file " << file_loc.GetLastName() );

		SaveFile(file_loc);

		UpdateFileDirectory( file_loc );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
void cptrRenderBatchDialog::button_Continue_OnButtonClick( wxCommandEvent& event )
{ 
	cptrRenderBatchDialogUtil::Hide();

	m_Data.m_BatchAborted = false;

	this->Close();
}

#endif
