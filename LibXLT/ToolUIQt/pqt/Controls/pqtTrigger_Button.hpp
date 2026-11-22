/****************************************************************************\
**	pqtTrigger_Button.hpp
**
**		Intermediate class between the property (prtyTrigger) and 
**	the control (Button).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_TRIGGER_BUTTON_HPP
#error pqtTrigger_Button.hpp multiply included
#endif
#define PQT_TRIGGER_BUTTON_HPP

#ifndef PQT_CONTROL_HPP
#include "ToolUIQt/pqt/pqtControl.hpp"
#endif

#ifdef USE_QT

//============================================================================
//============================================================================
class pqtTrigger_Button : public pqtControl
{
	public:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		pqtTrigger_Button(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
							QWidget* i_pParent);

		//----------------------------------------------------------------------------
		//	Destructor - dispose of controls
		//----------------------------------------------------------------------------
		~pqtTrigger_Button();

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
		void button_Clicked();

		QPushButton* m_pActualControl;
};

#endif // USE_QT
