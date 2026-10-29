/****************************************************************************\
**	tqcTextureFilePicker.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/tqcTextureFilePicker.hpp"

#include "Core/Gf/gfDirectoryCategories.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Core/prty/prtyInterestUtil.hpp"
#include "Core/prty/prtyTextureFileChooserUIInfo.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiMenuMgr.hpp"

#include <string>

#ifdef USE_QT

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcTextureFilePicker::tqcTextureFilePicker(QWidget* i_pParent)
:	QWidget(i_pParent),
	m_Filter("All files (*.*)|*.*"),
	m_bShowFileNameOnly(false),
	m_pUIInfo(NULL)
{
	QHBoxLayout* bSizer1 = new QHBoxLayout(this);
	bSizer1->setContentsMargins(0, 0, 0, 0);
	bSizer1->setSpacing(0);

	//Text box
	m_pTextBox = new tqcTextBox( this );
	m_pTextBox->setMinimumWidth(150);
	m_pTextBox->SetValueChangedCallback([this]() { OnTextChange(); });
	bSizer1->addWidget( m_pTextBox, 1 );

	//Choice box
	m_pChoice = new QComboBox( this );
	m_pChoice->setFixedWidth(55);
	QFont small_font = m_pChoice->font();
	small_font.setPointSize(7);
	m_pChoice->setFont(small_font);
	QObject::connect(m_pChoice, QOverload<int>::of(&QComboBox::activated), [this](int) { OnChoiceSelected(); });
	bSizer1->addWidget( m_pChoice, 0 );

	//function button
	itString icon_dir;
	fsFileUtil::LocatorToUnicodeString( guiMenuMgr::GetIconDirectory(), icon_dir );
	QString icondir = QString::fromWCharArray(icon_dir.GetString());
	QIcon icon(icondir + "\\texture-execute.png");
	icon.addFile(icondir + "\\texture-execute_highlight.png", QSize(), QIcon::Active);
	icon.addFile(icondir + "\\texture-execute_pressed.png", QSize(), QIcon::Selected);
	m_pInvokeButton = new QToolButton(this);
	m_pInvokeButton->setIcon(icon);
	m_pInvokeButton->setIconSize(QSize(16, 16));
	m_pInvokeButton->setFixedSize(20, 20);
	m_pInvokeButton->setAutoRaise(true);
	QObject::connect(m_pInvokeButton, &QToolButton::clicked, [this]() { OnInvokeClick(); });
	bSizer1->addWidget( m_pInvokeButton, 0 );

	m_bButtonPress = false;
	m_CurrentMode = prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Texture);
	m_CurrentSelection = 0;
}

//--------------------------------------------------------------------
//	Fullpath
//--------------------------------------------------------------------
const fsLocator& tqcTextureFilePicker::GetFullpath() const
{
	return m_Value;
}
void tqcTextureFilePicker::SetFullpath(const fsLocator& i_Value)
{
	m_Value = i_Value;
	update_textbox();
}

//--------------------------------------------------------------------
// uiinfo
//--------------------------------------------------------------------
void tqcTextureFilePicker::SetUIInfo(prtyPropertyUIInfo* i_pUIInfo)
{
	m_pUIInfo = i_pUIInfo;
}
prtyPropertyUIInfo* tqcTextureFilePicker::GetUIInfo()
{
	return m_pUIInfo;
}

//--------------------------------------------------------------------
//	Filename
//--------------------------------------------------------------------
itString tqcTextureFilePicker::GetFilename() const
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
const itString& tqcTextureFilePicker::GetFilter() const
{
	return m_Filter;
}
void tqcTextureFilePicker::SetFilter(const itString& i_Filter)
{
	m_Filter = i_Filter;
}

//--------------------------------------------------------------------
//	ShowFileNameOnly
//--------------------------------------------------------------------
const bool tqcTextureFilePicker::GetShowFileNameOnly() const
{
	return m_bShowFileNameOnly;
}
void tqcTextureFilePicker::SetShowFileNameOnly(const bool i_ShowFileNameOnly)
{
	m_bShowFileNameOnly = i_ShowFileNameOnly;
}

//--------------------------------------------------------------------
//	InitialDirectory
//--------------------------------------------------------------------
const fsLocator& tqcTextureFilePicker::GetInitialDirectory() const
{
	return m_InitialDirectory;
}
void tqcTextureFilePicker::SetInitialDirectory(const fsLocator& i_InitialDirectory)
{
	m_InitialDirectory = i_InitialDirectory;
}

//--------------------------------------------------------------------
//	DirectoryCategory - string name in order to group
//	current directory into different categories.
//--------------------------------------------------------------------
const std::string& tqcTextureFilePicker::GetDirectoryCategory() const
{
	return m_DirectoryCategory;
}
void tqcTextureFilePicker::SetDirectoryCategory(const std::string& i_DirectoryCategory)
{
	m_DirectoryCategory = i_DirectoryCategory;
}

//--------------------------------------------------------------------
// Clear the items in the choice box
//--------------------------------------------------------------------
void tqcTextureFilePicker::ClearChoices() const
{
	m_pChoice->clear();
}

//--------------------------------------------------------------------
// Append items in the choice box
//--------------------------------------------------------------------
void tqcTextureFilePicker::AppendChoice(const std::string& i_Choice) const
{
	m_pChoice->addItem(tqtQString(i_Choice));
}

//--------------------------------------------------------------------
// Select the desired item
//--------------------------------------------------------------------
void tqcTextureFilePicker::SelectChoice(unsigned int i_Index)
{
	m_pChoice->setCurrentIndex((int)i_Index);
	OnChoiceSelected();
}

//--------------------------------------------------------------------
// Select the desired item
//--------------------------------------------------------------------
void tqcTextureFilePicker::SelectChoice(const std::string& i_Choice)
{
	int index = m_pChoice->findText(tqtQString(i_Choice));
	if (index >= 0)
	{
		m_pChoice->setCurrentIndex(index);
		OnChoiceSelected();
	}
}

//--------------------------------------------------------------------
// Get the current mode
//--------------------------------------------------------------------
const std::string& tqcTextureFilePicker::GetCurrentMode() const
{
	return m_CurrentMode;
}

//--------------------------------------------------------------------
// Get the index of the current choice selection
//--------------------------------------------------------------------
int tqcTextureFilePicker::GetCurrentSelection()
{
	if(m_CurrentSelection < 0)
		m_CurrentSelection = 0;
	return m_CurrentSelection;
}

//--------------------------------------------------------------------
// Was the button just pressed
//--------------------------------------------------------------------
bool tqcTextureFilePicker::IsButtonPress() const
{
	return m_bButtonPress;
}
void tqcTextureFilePicker::ResetButtonPress()
{
	m_bButtonPress = false;
}

//--------------------------------------------------------------------
// Set the textbox enabled
//--------------------------------------------------------------------
void tqcTextureFilePicker::SetTextEnabled(bool i_bEnabled)
{
	m_pTextBox->setEnabled(i_bEnabled);
}

//----------------------------------------------------------------------------
// figure out initial directory from our fullpath value
//----------------------------------------------------------------------------
fsLocator tqcTextureFilePicker::get_initial_directory()
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
void tqcTextureFilePicker::update_textbox()
{
	if (this->m_bShowFileNameOnly)
	{
		if (m_Value.GetNumNames() > 0)
			this->m_pTextBox->SetValue(QString::fromUtf8(m_Value.GetLastName().c_str()));
		else
			this->m_pTextBox->SetValue(QString());
	}
	else
	{
		itString full_path;
		fsFileUtil::LocatorToUnicodeString( m_Value, full_path );
		this->m_pTextBox->SetValue(QString::fromWCharArray(full_path.GetString()));
		// show end of fullpath, not beginning
		if (QLineEdit* pLineEdit = this->m_pTextBox->GetLineEdit())
			pLineEdit->end(false);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcTextureFilePicker::notify_callback()
{
	NotifyValueChanged();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcTextureFilePicker::OnTextChange()
{
	QString lastvalue = m_pTextBox->last_value();
	if( m_CurrentMode == prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Texture) )
	{
		itString full_path(m_pTextBox->GetValue().toStdWString().c_str());
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
				itString path(lastvalue.toStdWString().c_str());
				fsFileUtil::UnicodeStringToLocator(path, m_Value);
			}
		}
		notify_callback();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcTextureFilePicker::OnChoiceSelected()
{
	m_CurrentSelection = m_pChoice->currentIndex();
	std::string selection = tqtStdString(m_pChoice->currentText());
	if( m_CurrentMode == selection )
		return;

	m_CurrentMode = selection;
	update_textbox();

	std::vector<prtyInterest*> interests;
	prtyInterestUtil::GetInterestList(interests);
	for( size_t i = 0; i < interests.size(); ++i )
	{
		if( m_CurrentMode == interests[i]->GetName() )
		{
			//process interests when combo box is changed
			return;
		}
	}
	m_pTextBox->setEnabled(true);
}

//----------------------------------------------------------------------------
// CheckInterestInvoke - runs through our interests and checks to see if they
// match our current mode
//----------------------------------------------------------------------------
prtyInterest* tqcTextureFilePicker::CheckInterestInvoke()
{
	std::vector<prtyInterest*> interests;
	prtyInterestUtil::GetInterestList(interests);
	QString new_text;
	for( size_t i = 0; i < interests.size(); ++i )
	{
		if( m_CurrentMode == interests[i]->GetName() )
		{
			//determine if the new selection should remove the texture name field
			if( interests[i]->IsProcedural() )
			{
				new_text = m_pChoice->currentText();
				//remove the texture value if the new mode is a procedural texture
				m_Value.Clear();
			}
			else
			{
				new_text = m_pChoice->currentText() + ": " + this->m_pTextBox->GetValue();
			}

			//set the new text field value and whether or not the text field is readonly
			this->m_pTextBox->SetValue( new_text );
			if( !interests[i]->IsEditable() )
			{
				m_pTextBox->setEnabled(false);
			}
			return interests[i];
		}
	}
	return NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcTextureFilePicker::OnInvokeClick()
{
	m_bButtonPress = false;
	if( m_CurrentMode == prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Texture) )
	{
		itString init_dir;
		fsFileUtil::LocatorToUnicodeString( get_initial_directory(), init_dir );
		QString path = QFileDialog::getOpenFileName( this,
													 "Open File",
													 QString::fromWCharArray(init_dir.GetString()),
													 tqtFileDialogFilter(QString::fromWCharArray(m_Filter.GetString())) );
		if (!path.isEmpty())
		{
			itString full_path(QDir::toNativeSeparators(path).toStdWString().c_str());
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
		this->m_pTextBox->SetValue(QString());
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

#endif // USE_QT
