/****************************************************************************\
**	tqcTextureFilePicker.hpp
**
**		Custom control with text box, edit button, and browse button for 
**	choosing and manipulating texture files.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_TEXTUREFILEPICKER_HPP
#error tqcTextureFilePicker.hpp multiply included
#endif
#define TQC_TEXTUREFILEPICKER_HPP

#ifndef TQC_TEXTBOX_HPP
#include "ToolUIQt/tqc/tqcTextBox.hpp"
#endif 
#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif 

#ifdef QT_FINISH_PORT

//============================================================================
// wxEVT_VALUE_CHANGED event : this control throws this event type when the
//	value in the control has changed after ENTER is pressed, or the
//	focus leaves the text box. Users should register for this event, 
//	not for the individual enter pressed and leave events.
//============================================================================

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class prtyInterest;
class prtyPropertyUIInfo;

//============================================================================
//============================================================================
class tqcTextureFilePicker : public wxPanel
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcTextureFilePicker(QWidget* i_pParent);

		//--------------------------------------------------------------------
		//	Fullpath
		//--------------------------------------------------------------------
		const fsLocator& GetFullpath() const;
		void SetFullpath(const fsLocator& i_Fullpath);

		//--------------------------------------------------------------------
		// uiinfo
		//--------------------------------------------------------------------
		void SetUIInfo(prtyPropertyUIInfo* i_pUIInfo);
		prtyPropertyUIInfo* GetUIInfo();

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

		//--------------------------------------------------------------------
		// Clear the items in the choice box
		//--------------------------------------------------------------------
		void ClearChoices() const;
		void AppendChoice(const std::string& i_Choice) const;
		void SelectChoice(unsigned int i_Index);
		void SelectChoice(const std::string& i_Choice);

		//--------------------------------------------------------------------
		// get the current mode of operation
		//--------------------------------------------------------------------
		const std::string& GetCurrentMode() const;

		//--------------------------------------------------------------------
		// Get the index of the current choice selection
		//--------------------------------------------------------------------
		int GetCurrentSelection();

		//--------------------------------------------------------------------
		// Was the button just pressed
		//--------------------------------------------------------------------
		bool IsButtonPress() const;
		void ResetButtonPress();

		//--------------------------------------------------------------------
		// Set the textbox enabled
		//--------------------------------------------------------------------
		void SetTextEnabled(bool i_bEnabled);

		//----------------------------------------------------------------------------
		// CheckInterestInvoke - runs through our interests and checks to see if they
		// match our current mode
		//----------------------------------------------------------------------------
		prtyInterest* CheckInterestInvoke();

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		fsLocator get_initial_directory();
		void update_textbox();
		void notify_callback();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnTextChange(wxCommandEvent& i_Event);
		void OnChoiceSelected(wxCommandEvent& i_Event);
		void OnInvokeClick(wxCommandEvent& i_Event);

		tqcTextBox* m_pTextBox;
		wxChoice* m_pChoice;
		wxBitmapButton* m_pInvokeButton;
		
		fsLocator	m_Value;
		itString	m_Filter;
		bool		m_bShowFileNameOnly;
		bool		m_bButtonPress;
		fsLocator	m_InitialDirectory;
		std::string	m_DirectoryCategory;
		std::string m_CurrentMode;
		int			m_CurrentSelection;

		prtyPropertyUIInfo* m_pUIInfo;
};

#endif // USE_QT
