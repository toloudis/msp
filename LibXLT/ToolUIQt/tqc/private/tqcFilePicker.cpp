/****************************************************************************\
**	tqcFilePicker.hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/tqcFilePicker.hpp"

#include "Core/Gf/gfDirectoryCategories.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/It/itStringUtil.hpp"

#include "Tool/gui/guiMessageBox.hpp"

#ifdef QT_FINISH_PORT


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcFilePicker::tqcFilePicker(QWidget* i_pParent)
:	wxPanel(i_pParent, wxID_ANY),
	m_Filter("All files (*.*)|*.*"),
	m_bShowFileNameOnly(false)
{		
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );

	//m_pTextBox = new tqcTextBox( this );
	// Create text control with a good size width so that more of the
	// original text is shown when it is resized bigger. (It seemed like
	// a width of 100 was being used when not specifically specified).
	m_pTextBox = new tqcTextBox( this, wxID_ANY, L"", false, false, wxSize(180,-1) );
	this->Connect( m_pTextBox->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(tqcFilePicker::OnTextChange) );
	bSizer1->Add( m_pTextBox, 1, wxALL, 0 );
	
	m_pBrowseButton = new wxButton(this, wxID_ANY, L"...", wxDefaultPosition, wxSize( 30,20 ));
	this->Connect( m_pBrowseButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
					wxCommandEventHandler(tqcFilePicker::OnBrowseClick) );
	bSizer1->Add( m_pBrowseButton, 0, wxALL, 0 );

	// Set up font and colors
	this->InheritAttributes();
	
	this->SetSizer( bSizer1 );
	this->Layout();

	//update_slider();
}

//--------------------------------------------------------------------
//	Fullpath
//--------------------------------------------------------------------
const fsLocator& tqcFilePicker::GetFullpath() const
{
	return m_Value;
}
void tqcFilePicker::SetFullpath(const fsLocator& i_Value)
{
	m_Value = i_Value;
	update_textbox();
}

//--------------------------------------------------------------------
//	Filename
//--------------------------------------------------------------------
itString tqcFilePicker::GetFilename() const
{
    if (m_Value.GetNumNames() == 0)
        return itString("");
	else if (fsFileUtil::DirectoryExists(m_Value))
    {
        // If the fullpath is a directory, then the filename
        // should be empty. (The directory would be used to specify
        // the initial directory for the Browse dialog).
        return itString("");
    }
    else
    {
       return m_Value.GetLastName();
    }
}

//--------------------------------------------------------------------
//	Filter
//--------------------------------------------------------------------
const itString& tqcFilePicker::GetFilter() const
{
	return m_Filter;
}
void tqcFilePicker::SetFilter(const itString& i_Filter)
{
	m_Filter = i_Filter;
}

//--------------------------------------------------------------------
//	ShowFileNameOnly
//--------------------------------------------------------------------
const bool tqcFilePicker::GetShowFileNameOnly() const
{
	return m_bShowFileNameOnly;
}
void tqcFilePicker::SetShowFileNameOnly(const bool i_ShowFileNameOnly)
{
	m_bShowFileNameOnly = i_ShowFileNameOnly;
}

//--------------------------------------------------------------------
//	InitialDirectory
//--------------------------------------------------------------------
const fsLocator& tqcFilePicker::GetInitialDirectory() const
{
	return m_InitialDirectory;
}
void tqcFilePicker::SetInitialDirectory(const fsLocator& i_InitialDirectory)
{
	m_InitialDirectory = i_InitialDirectory;
}

//--------------------------------------------------------------------
//	DirectoryCategory - string name in order to group
//	current directory into different categories.
//--------------------------------------------------------------------
const std::string& tqcFilePicker::GetDirectoryCategory() const
{
	return m_DirectoryCategory;
}
void tqcFilePicker::SetDirectoryCategory(const std::string& i_DirectoryCategory)
{
	m_DirectoryCategory = i_DirectoryCategory;
}

//----------------------------------------------------------------------------
// figure out initial directory from our fullpath value
//----------------------------------------------------------------------------
fsLocator tqcFilePicker::get_initial_directory()
{
    if (m_Value.GetNumNames() > 1)
    {
        // If the file exists, we can set the initial
        // directory to be the filename, otherwise try
        // to get the intended directory from the 
        // rest of the fullpath.
		if (fsFileUtil::DirectoryExists(m_Value))
            return m_Value;
		else if (fsFileUtil::FileExists(m_Value))
		{
			fsLocator dir(m_Value);
			dir.Pop();
            return dir;
		}
    }
	else if (m_InitialDirectory.GetNumNames() > 0)
	{
		return m_InitialDirectory;
	}
	else if (!m_DirectoryCategory.empty())
	{
		fsLocator cur_dir;
		if (gfDirectoryCategories::GetCurDirectory(m_DirectoryCategory, cur_dir))
		{
			return cur_dir;
		}
	}

	// returning empty locator means "use current directory"
	return fsLocator();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcFilePicker::update_textbox()
{
	if (this->m_bShowFileNameOnly)
	{
		if (m_Value.GetNumNames() > 0)
			this->m_pTextBox->SetValue(m_Value.GetLastName().GetString());
		else
			this->m_pTextBox->SetValue(L"");
	}
	else
	{
		itString full_path;
		fsFileUtil::LocatorToUnicodeString( m_Value, full_path );
		this->m_pTextBox->SetValue(full_path.GetString());
		this->m_pTextBox->SetInsertionPointEnd(); // show end of fullpath, not beginning
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcFilePicker::notify_callback()
{
	// trigger the callback
	wxCommandEvent changed_event(wxEVT_VALUE_CHANGED, this->GetId());
	m_pTextBox->ProcessCommand(changed_event);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcFilePicker::OnTextChange(wxCommandEvent& i_Event)
{	
	wxString lastvalue = m_pTextBox->last_value();
	itString full_path(m_pTextBox->GetValue());
	fsFileUtil::UnicodeStringToLocator(full_path, m_Value);
	if(full_path.GetLength() != 0 )
	{
		if(!fsFileUtil::FileExists(m_Value))
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(m_Value, filename);
			std::string msg = "Cannot find file\n" + filename;
			guiMessageBox::Show(msg.c_str(), "Error", guiMessageBox::e_OKOnly);
			m_pTextBox->SetValue(lastvalue);
			itString path(lastvalue);
			fsFileUtil::UnicodeStringToLocator(path, m_Value);
		}
	}
	notify_callback();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcFilePicker::OnBrowseClick(wxCommandEvent& i_Event)
{	
	itString init_dir;
	fsFileUtil::LocatorToUnicodeString( get_initial_directory(), init_dir );

	wxFileDialog dialog( this,
					_T("Open File"),
					init_dir.GetString(),
					wxEmptyString,
					m_Filter.GetString()
				 );

	//dialog.CentreOnParent();

	if (dialog.ShowModal() == QMessageBox::Ok)
	{
		itString full_path(dialog.GetPath());
		fsFileUtil::UnicodeStringToLocator(full_path, m_Value);

		// maintain the current directory by category
		if (!m_DirectoryCategory.empty())
		{
			fsLocator cur_dir = m_Value;
			cur_dir.Pop();
			gfDirectoryCategories::SetCurDirectory(m_DirectoryCategory, cur_dir);
		}

		update_textbox();
		notify_callback();
	}
}

#endif // USE_QT
