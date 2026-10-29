/****************************************************************************\
**	tqcFilePicker.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/tqcFilePicker.hpp"

#include "Core/Gf/gfDirectoryCategories.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Tool/gui/guiMessageBox.hpp"

#ifdef USE_QT

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcFilePicker::tqcFilePicker(QWidget* i_pParent)
:	QWidget(i_pParent),
	m_Filter("All files (*.*)|*.*"),
	m_bShowFileNameOnly(false)
{
	QHBoxLayout* bSizer1 = new QHBoxLayout(this);
	bSizer1->setContentsMargins(0, 0, 0, 0);
	bSizer1->setSpacing(0);

	// Create text control with a good size width so that more of the
	// original text is shown when it is resized bigger.
	m_pTextBox = new tqcTextBox( this );
	m_pTextBox->setMinimumWidth(180);
	m_pTextBox->SetValueChangedCallback([this]() { OnTextChange(); });
	bSizer1->addWidget( m_pTextBox, 1 );

	m_pBrowseButton = new QPushButton("...", this);
	m_pBrowseButton->setFixedWidth(30);
	QObject::connect(m_pBrowseButton, &QPushButton::clicked, [this]() { OnBrowseClick(); });
	bSizer1->addWidget( m_pBrowseButton, 0 );
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
std::string tqcFilePicker::GetFilename() const
{
    if (m_Value.GetNumNames() == 0)
        return std::string();
	else if (fsFileUtil::DirectoryExists(m_Value))
    {
        // If the fullpath is a directory, then the filename
        // should be empty. (The directory would be used to specify
        // the initial directory for the Browse dialog).
        return std::string();
    }
    else
    {
       return m_Value.GetLastName();
    }
}

//--------------------------------------------------------------------
//	Filter
//--------------------------------------------------------------------
const std::string& tqcFilePicker::GetFilter() const
{
	return m_Filter;
}
void tqcFilePicker::SetFilter(const std::string& i_Filter)
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
			this->m_pTextBox->SetValue(QString::fromUtf8(m_Value.GetLastName().c_str()));
		else
			this->m_pTextBox->SetValue(QString());
	}
	else
	{
		std::string full_path;
		fsFileUtil::LocatorToUnicodeString( m_Value, full_path );
		this->m_pTextBox->SetValue(QString::fromStdString(full_path));
		// show end of fullpath, not beginning
		if (QLineEdit* pLineEdit = this->m_pTextBox->GetLineEdit())
			pLineEdit->end(false);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcFilePicker::notify_callback()
{
	NotifyValueChanged();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcFilePicker::OnTextChange()
{
	QString lastvalue = m_pTextBox->last_value();
	std::string full_path(m_pTextBox->GetValue().toStdString());
	fsFileUtil::UnicodeStringToLocator(full_path, m_Value);
	if(full_path.size() != 0 )
	{
		if(!fsFileUtil::FileExists(m_Value))
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(m_Value, filename);
			std::string msg = "Cannot find file\n" + filename;
			guiMessageBox::Show(msg.c_str(), "Error", guiMessageBox::e_OKOnly);
			m_pTextBox->SetValue(lastvalue);
			std::string path(lastvalue.toStdString());
			fsFileUtil::UnicodeStringToLocator(path, m_Value);
		}
	}
	notify_callback();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcFilePicker::OnBrowseClick()
{
	std::string init_dir;
	fsFileUtil::LocatorToUnicodeString( get_initial_directory(), init_dir );
	QString path = QFileDialog::getOpenFileName( this,
												 "Open File",
												 QString::fromStdString(init_dir),
												 tqtFileDialogFilter(QString::fromStdString(m_Filter)) );
	if (!path.isEmpty())
	{
		std::string full_path(QDir::toNativeSeparators(path).toStdString());
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
