/****************************************************************************\
**	tqcFilePicker.hpp
**
**		Custom control with text box and browse button for 
**	choosing filenames.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_FILEPICKER_HPP
#error tqcFilePicker.hpp multiply included
#endif
#define TQC_FILEPICKER_HPP

#ifndef TQC_TEXTBOX_HPP
#include "ToolUIQt/tqc/tqcTextBox.hpp"
#endif 
#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif 

#ifdef USE_QT

//============================================================================
// Value changed callback (tqcEvent.hpp): this control calls it when the
//	value in the control has changed aftere ENTER is pressed, or the
//	focus leaves the text box. Users should set this callback, 
//	not watch for the individual enter pressed and leave events.
//============================================================================

//============================================================================
//============================================================================
class tqcFilePicker : public QWidget, public tqcValueChangedNotifier
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcFilePicker(QWidget* i_pParent);

		//--------------------------------------------------------------------
		//	Fullpath
		//--------------------------------------------------------------------
		const fsLocator& GetFullpath() const;
		void SetFullpath(const fsLocator& i_Fullpath);

		//--------------------------------------------------------------------
		//	Filename
		//--------------------------------------------------------------------
		itString GetFilename() const;

		//--------------------------------------------------------------------
		//	Filter
		//--------------------------------------------------------------------
		const itString& GetFilter() const;
		void SetFilter(const itString& i_Filter);

		//--------------------------------------------------------------------
		//	ShowFileNameOnly
		//--------------------------------------------------------------------
		const bool GetShowFileNameOnly() const;
		void SetShowFileNameOnly(const bool i_ShowFileNameOnly);

		//--------------------------------------------------------------------
		//	InitialDirectory
		//--------------------------------------------------------------------
		const fsLocator& GetInitialDirectory() const;
		void SetInitialDirectory(const fsLocator& i_InitialDirectory);

		//--------------------------------------------------------------------
		//	DirectoryCategory - string name in order to group
		//	current directory into different categories.
		//--------------------------------------------------------------------
		const std::string& GetDirectoryCategory() const;
		void SetDirectoryCategory(const std::string& i_DirectoryCategory);

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		fsLocator get_initial_directory();
		void update_textbox();
		void notify_callback();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnTextChange();
		void OnBrowseClick();

		tqcTextBox* m_pTextBox;
		QPushButton* m_pBrowseButton;
		
		fsLocator	m_Value;
		itString	m_Filter;
		bool		m_bShowFileNameOnly;
		fsLocator	m_InitialDirectory;
		std::string	m_DirectoryCategory;

};

#endif // USE_QT
