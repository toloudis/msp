/****************************************************************************\
**	twcTextureFilePicker.hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twc/twcTextureFilePicker.hpp"

#include "Core/Gf/gfDirectoryCategories.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Core/prty/prtyInterestUtil.hpp"
#include "Core/prty/prtyTextureFileChooserUIInfo.hpp"

#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiMenuMgr.hpp"

#include <string>

#ifdef USE_WXWIDGETS


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
twcTextureFilePicker::twcTextureFilePicker(wxWindow* i_pParent)
:	wxPanel(i_pParent, wxID_ANY),
	m_Filter("All files (*.*)|*.*"),
	m_bShowFileNameOnly(false),
	m_pUIInfo(NULL)
{		
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );

	// Create text control with a good size width so that more of the
	// original text is shown when it is resized bigger. (It seemed like
	// a width of 100 was being used when not specifically specified).
	//m_pComboBox = new twcComboBox( this, wxID_ANY, L"", FromDIP(wxSize(180,-1)));
	
	//Text box
	m_pTextBox = new twcTextBox( this, wxID_ANY, L"", false, false, FromDIP(wxSize(150,-1)) );
	this->Connect( m_pTextBox->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(twcTextureFilePicker::OnTextChange) );
	bSizer1->Add( m_pTextBox, 1, wxALL, 0 );

	//Choice box
	m_pChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, FromDIP(wxSize(55,-1)), 0, NULL);
	m_pChoice->SetFont( wxFont( 7, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );
	this->Connect( m_pChoice->GetId(), wxEVT_COMMAND_CHOICE_SELECTED,
					wxCommandEventHandler(twcTextureFilePicker::OnChoiceSelected) );
	bSizer1->Add( m_pChoice, 0, wxALL, 0 );
	
	//function button
	itString icon_dir;
	fsFileUtil::LocatorToUnicodeString( guiMenuMgr::GetIconDirectory(), icon_dir );
	std::wstring icondir = icon_dir.GetString();

    wxLogNull nullLog;

	m_pInvokeButton = new wxBitmapButton(this, wxID_ANY, wxBitmap( icondir + L"\\texture-execute.png", wxBITMAP_TYPE_ANY), wxDefaultPosition, FromDIP(wxSize( 20,20 )), wxNO_BORDER);
	m_pInvokeButton->SetBitmapSelected(wxBitmap( icondir + L"\\texture-execute_pressed.png", wxBITMAP_TYPE_ANY));
	m_pInvokeButton->SetBitmapHover(wxBitmap( icondir + L"\\texture-execute_highlight.png", wxBITMAP_TYPE_ANY));
	
	this->Connect( m_pInvokeButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
					wxCommandEventHandler(twcTextureFilePicker::OnInvokeClick) );
	bSizer1->Add( m_pInvokeButton, 0, wxALL, 0 );

	// Set up font and colors
	this->InheritAttributes();
	
	this->SetSizer( bSizer1 );
	this->Layout();

	m_bButtonPress = false;
	m_CurrentMode = prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Texture);
	//update_slider();

	m_CurrentSelection = 0;
}

//--------------------------------------------------------------------
//	Fullpath
//--------------------------------------------------------------------
const fsLocator& twcTextureFilePicker::GetFullpath() const
{
	return m_Value;
}
void twcTextureFilePicker::SetFullpath(const fsLocator& i_Value)
{
	m_Value = i_Value;
	update_textbox();
}

//----------------------------------------------------------------------------
// uiinfo
//----------------------------------------------------------------------------
void twcTextureFilePicker::SetUIInfo(prtyPropertyUIInfo* i_pUIInfo)
{
	m_pUIInfo = i_pUIInfo;
}
prtyPropertyUIInfo* twcTextureFilePicker::GetUIInfo()
{
	return m_pUIInfo;
}

//--------------------------------------------------------------------
//	Filename
//--------------------------------------------------------------------
itString twcTextureFilePicker::GetFilename() const
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
const itString& twcTextureFilePicker::GetFilter() const
{
	return m_Filter;
}
void twcTextureFilePicker::SetFilter(const itString& i_Filter)
{
	m_Filter = i_Filter;
}

//--------------------------------------------------------------------
//	ShowFileNameOnly
//--------------------------------------------------------------------
const bool twcTextureFilePicker::GetShowFileNameOnly() const
{
	return m_bShowFileNameOnly;
}
void twcTextureFilePicker::SetShowFileNameOnly(const bool i_ShowFileNameOnly)
{
	m_bShowFileNameOnly = i_ShowFileNameOnly;
}

//--------------------------------------------------------------------
//	InitialDirectory
//--------------------------------------------------------------------
const fsLocator& twcTextureFilePicker::GetInitialDirectory() const
{
	return m_InitialDirectory;
}
void twcTextureFilePicker::SetInitialDirectory(const fsLocator& i_InitialDirectory)
{
	m_InitialDirectory = i_InitialDirectory;
}

//--------------------------------------------------------------------
//	DirectoryCategory - string name in order to group
//	current directory into different categories.
//--------------------------------------------------------------------
const std::string& twcTextureFilePicker::GetDirectoryCategory() const
{
	return m_DirectoryCategory;
}
void twcTextureFilePicker::SetDirectoryCategory(const std::string& i_DirectoryCategory)
{
	m_DirectoryCategory = i_DirectoryCategory;
}

//--------------------------------------------------------------------
// Clear the items in the choice box
//--------------------------------------------------------------------
void twcTextureFilePicker::ClearChoices() const
{
	m_pChoice->Clear();
}

//--------------------------------------------------------------------
// Append items in the choice box
//--------------------------------------------------------------------
void twcTextureFilePicker::AppendChoice(const std::string& i_Choice) const
{
	m_pChoice->Append(wxString(i_Choice.c_str(), wxConvUTF8));
}
//--------------------------------------------------------------------
// Select the desired item
//--------------------------------------------------------------------
void twcTextureFilePicker::SelectChoice(unsigned int i_Index)
{
	wxCommandEvent false_event;
	m_pChoice->SetSelection(i_Index);
	OnChoiceSelected(false_event);
}
//--------------------------------------------------------------------
// Select the desired item
//--------------------------------------------------------------------
void twcTextureFilePicker::SelectChoice(const std::string& i_Choice)
{
	wxCommandEvent false_event;
	if(m_pChoice->SetStringSelection(wxString(i_Choice.c_str(), wxConvUTF8)))
		OnChoiceSelected(false_event);
}

//--------------------------------------------------------------------
// Get the current mode
//--------------------------------------------------------------------
const std::string& twcTextureFilePicker::GetCurrentMode() const
{
	return m_CurrentMode;
}

//--------------------------------------------------------------------
// Get the index of the current choice selection
//--------------------------------------------------------------------
int twcTextureFilePicker::GetCurrentSelection()
{
	if(m_CurrentSelection < 0)
		m_CurrentSelection = 0;
	return m_CurrentSelection;
}

//--------------------------------------------------------------------
// Was the button just pressed
//--------------------------------------------------------------------
bool twcTextureFilePicker::IsButtonPress() const
{
	return m_bButtonPress;
}
void twcTextureFilePicker::ResetButtonPress()
{
	m_bButtonPress = false;
}

//--------------------------------------------------------------------
// Set the textbox enabled
//--------------------------------------------------------------------
void twcTextureFilePicker::SetTextEnabled(bool i_bEnabled)
{
	m_pTextBox->Enable(i_bEnabled);
}

//----------------------------------------------------------------------------
// figure out initial directory from our fullpath value
//----------------------------------------------------------------------------
fsLocator twcTextureFilePicker::get_initial_directory()
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
void twcTextureFilePicker::update_textbox()
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
void twcTextureFilePicker::notify_callback()
{
	// trigger the callback
	wxCommandEvent changed_event(wxEVT_VALUE_CHANGED, this->GetId());
	m_pTextBox->ProcessCommand(changed_event);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcTextureFilePicker::OnTextChange(wxCommandEvent& i_Event)
{	
	wxString lastvalue = m_pTextBox->last_value();
	if( m_CurrentMode == prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Texture) )
	{
		itString full_path(m_pTextBox->GetValue().wc_str());
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
				itString path(lastvalue.wc_str());
				fsFileUtil::UnicodeStringToLocator(path, m_Value);
			}
		}
		notify_callback();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcTextureFilePicker::OnChoiceSelected(wxCommandEvent& i_Event)
{
	m_CurrentSelection = m_pChoice->GetSelection();
	itString choice_text(m_pChoice->GetStringSelection().wc_str());
	std::string selection = itStringUtil::GetStdString(choice_text);
	if( m_CurrentMode == selection )
		return;

	m_CurrentMode = selection;
	update_textbox();
	std::vector<prtyInterest*> interests;
	prtyInterestUtil::GetInterestList(interests);
	for( int i = 0; i < interests.size(); ++i )
	{
		if( m_CurrentMode == interests[i]->GetName() )
		{
			//process interests when combo box is changed
			return;
		}
	}
	m_pTextBox->Enable(true);
	i_Event.Skip();
}

//----------------------------------------------------------------------------
// CheckInterestInvoke - runs through our interests and checks to see if they
// match our current mode
//----------------------------------------------------------------------------
prtyInterest* twcTextureFilePicker::CheckInterestInvoke()
{
	std::vector<prtyInterest*> interests;
	prtyInterestUtil::GetInterestList(interests);
	wxString new_text; 
	for( int i = 0; i < interests.size(); ++i )
	{
		if( m_CurrentMode == interests[i]->GetName() )
		{
			//determine if the new selection should remove the texture name field
			if( interests[i]->IsProcedural() )
			{
				new_text = m_pChoice->GetStringSelection();

				//remove the texture value if the new mode is a procedural texture
				m_Value.Clear();
			}
			else
			{
				new_text = m_pChoice->GetStringSelection() + wxString(L": ") + this->m_pTextBox->GetValue();
			}

			//set the new text field value and whether or not the text field is readonly
			this->m_pTextBox->SetValue( new_text );
			if( !interests[i]->IsEditable() )
			{
				m_pTextBox->Enable(false);
				//notify_callback();
			}
			return interests[i];
		}
	}
	return NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcTextureFilePicker::OnInvokeClick(wxCommandEvent& i_Event)
{	
	m_bButtonPress = false;
	if( m_CurrentMode == prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Texture) )
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

		if (dialog.ShowModal() == wxID_OK)
		{
			itString full_path(dialog.GetPath().wc_str());
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
	else if( m_CurrentMode == prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Reset) )
	{
		//m_pChoice->SetSelection(0);
		this->m_pTextBox->SetValue(L"");
		m_Value.Clear();
		update_textbox();
		m_bButtonPress = true;
		notify_callback();
	}
	else if( m_CurrentMode == prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Revert) )
	{
		m_bButtonPress = true;
		notify_callback();
	}
	else
	{
		prtyInterest* interest = CheckInterestInvoke();
		if(interest != NULL)
		{
			m_bButtonPress = true;
			prtyInterestUtil::InvokeInterest(interest);
			notify_callback();
		}
	}
}

#endif // USE_WXWIDGETS
