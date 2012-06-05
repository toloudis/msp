/****************************************************************************\
**	pqtBoolean_CheckBox.hpp
**
**		Intermediate class between the property (prtyBoolean) and 
**	the control (CheckBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_BOOLEAN_CHECKBOX_HPP
#error pqtBoolean_CheckBox.hpp multiply included
#endif
#define PQT_BOOLEAN_CHECKBOX_HPP

#ifndef PQT_CONTROL_HPP
#include "ToolUIQt/pqt/pqtControl.hpp"
#endif

#ifdef USE_QT
#include <QtGui/QCheckBox>


//============================================================================
//============================================================================
class pqtBoolean_CheckBox : public pqtControl
{
	public:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		pqtBoolean_CheckBox(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
							QWidget* i_pParent);

		//----------------------------------------------------------------------------
		//	Destructor - dispose of controls
		//----------------------------------------------------------------------------
		~pqtBoolean_CheckBox();

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		virtual QWidget* GetControl();

		//----------------------------------------------------------------------------
		//	Note: right now, PropertyChanged will not create a circular update because
		//	of the m_bLocalChangeNoUpdate flag.  This IS NOT true the other way
		//	around.  If a control calls its "ValueChanged" then the property will
		//	call all of its callback controls and one of them could have been the one
		//	that originally updated the property value(s).  By checking the diff of 
		//	the values we can avoid a repetitive setting of the control's values.
		//----------------------------------------------------------------------------
		virtual void PropertyChanged(prtyProperty* i_pProperty, bool i_bDirty);

		//----------------------------------------------------------------------------
		// Update control to match the changes in the UI Info
		//----------------------------------------------------------------------------
		virtual void UpdateControl(prtyPropertyUIInfo* i_pUIInfo);

	private:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
#ifdef QT_FINISH_PORT
		void CheckBox_ValueChanged(wxCommandEvent& i_Event);
#endif

		QCheckBox* m_pActualControl;
};

#endif // USE_QT
