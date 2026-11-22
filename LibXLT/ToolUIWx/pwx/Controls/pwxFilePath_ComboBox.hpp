/****************************************************************************\
**	pwxFilePath_ComboBox.hpp
**
**		Intermediate class between the property (prtyFilePath) and
**	the control (ComboBox). The choices come from a
**	prtyFilePathComboBoxUIInfo: a label for each path. A value that is
**	not one of the choices is added to the list, labelled with its path,
**	so the control never shows a blank.
\****************************************************************************/
#ifdef PWX_FILEPATH_COMBOBOX_HPP
#error pwxFilePath_ComboBox.hpp multiply included
#endif
#define PWX_FILEPATH_COMBOBOX_HPP

#ifndef PWX_CONTROL_HPP
#include "ToolUIWx/pwx/pwxControl.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#include <vector>

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class pwxFilePath_ComboBox : public pwxControl
{
	public:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		pwxFilePath_ComboBox(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
							 wxWindow* i_pParent);

		//----------------------------------------------------------------------------
		//	Destructor - dispose of controls
		//----------------------------------------------------------------------------
		virtual ~pwxFilePath_ComboBox();

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		virtual wxWindow* GetControl();

		//----------------------------------------------------------------------------
		// Update control to match the changes in the UI Info
		//----------------------------------------------------------------------------
		virtual void UpdateControl(prtyPropertyUIInfo* i_pUIInfo);

	protected:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		virtual void PropertyChanged(prtyProperty* i_pProperty, bool i_bDirty);

	private:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void ComboBox_ValueChanged(wxCommandEvent& i_Event);

		//----------------------------------------------------------------------------
		// Select the entry for the given path, adding one if there is none
		//----------------------------------------------------------------------------
		void SetValueIntoControl(const fsLocator& i_Value);

		//----------------------------------------------------------------------------
		// Path of the selected entry (empty if nothing is selected)
		//----------------------------------------------------------------------------
		fsLocator GetValueFromControl() const;

		wxChoice* m_pActualControl;
		std::vector<fsLocator> m_Values;	// path of each entry in m_pActualControl
		fsLocator m_OriginalValue;
		bool m_bHasValue;
};

#endif // USE_WXWIDGETS
