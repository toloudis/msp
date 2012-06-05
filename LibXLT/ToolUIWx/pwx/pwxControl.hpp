/****************************************************************************\
**	pwxControl.hpp
**
**		Class that ties the property and the actual control together.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_CONTROL_HPP
#error pwxControl.hpp multiply included
#endif
#define PWX_CONTROL_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

#include <vector>

#ifdef USE_WXWIDGETS 

//============================================================================
//============================================================================
class prtyPropertyUIInfo;
class prtyProperty;
class prtyPropertyCallback;
class prtyReferenceCreator;
class prtyPropertyReference;

//============================================================================
//============================================================================
class pwxControl : public wxEvtHandler
{
	public:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		pwxControl(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo);

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		virtual ~pwxControl();

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		const std::string& GetName() const;

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		//void SetName(const std::string& i_Name);

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		virtual wxWindow* GetControl() = 0;

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		int GetNumberOfProperties() const;

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		prtyProperty* GetProperty(int i_Index);

		//----------------------------------------------------------------------------
		// Create reference to property with given index in order to make an undo
		// operation. Result could be NULL if no reference could be created.
		//----------------------------------------------------------------------------
		shared_ptr<prtyPropertyReference> CreatePropertyReference(int i_Index);

	//	//----------------------------------------------------------------------------
	//	//----------------------------------------------------------------------------
	//	void SetProperty(prtyProperty* i_pProperty, int i_Index);
	//	void AddProperty(prtyProperty* i_pProperty);

		//----------------------------------------------------------------------------
		// Return true if this control is attached to this UI Info
		//----------------------------------------------------------------------------
		bool HasUIInfo(prtyPropertyUIInfo* i_pUII) const;

		//----------------------------------------------------------------------------
		// Update control to match the changes in the UI Info
		//----------------------------------------------------------------------------
		virtual void UpdateControl(prtyPropertyUIInfo* i_pUIInfo);

	//	//----------------------------------------------------------------------------
	//	//----------------------------------------------------------------------------
	//	void SetToolTip();

	//	//----------------------------------------------------------------------------
	//	//	Set the control
	//	//----------------------------------------------------------------------------
	//	void SetControl( System::Windows::Forms::Control^ i_pControl );
	
		//----------------------------------------------------------------------------
		// Force firing of property changed callback
		//----------------------------------------------------------------------------
		void TriggerPropertyChanged();

		//----------------------------------------------------------------------------
		// Display the confirmation string associated with the UIInfo,
		// if one exists. Returns true if the user confirms the change
		// or if no confirmation was needed.
		//----------------------------------------------------------------------------
		bool ConfirmChange();

	protected:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		virtual void PropertyChanged(prtyProperty* i_pProperty, bool i_bDirty);

		// Control maintains a weak_ptr to the UIInfo, so that it knows
		// when the UIInfo pointer is safe to use.
		weak_ptr<prtyPropertyUIInfo> m_pUIInfo;
		bool	m_bLocalChangeNoUpdate;

	private:
		std::string	m_Name;
		std::vector<prtyProperty*>	m_Properties;
		std::vector<prtyReferenceCreator*> m_ReferenceCreators;
		shared_ptr<prtyPropertyCallback> m_pCallback;
};

#endif // USE_WXWIDGETS
