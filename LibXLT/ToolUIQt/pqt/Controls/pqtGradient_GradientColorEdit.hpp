/****************************************************************************\
**	pqtGradient_GradientRGBAEdit.hpp
**
**		Intermediate class between the property (prtyGradient) and 
**	the control (GradientColorEdit).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_GRADIENT_GRADIENTCOLOREDIT_HPP
#error pqtGradient_GradientColorEdit.hpp multiply included
#endif
#define PQT_GRADIENT_GRADIENTCOLOREDIT_HPP

#ifndef PQT_CONTROL_HPP
#include "ToolUIQt/pqt/pqtControl.hpp"
#endif
#ifndef TQC_GRADIENTCOLOREDIT_HPP
#include "ToolUIQt/tqc/tqcGradientColorEdit.hpp"
#endif


//============================================================================
//============================================================================
class pqtGradient_GradientColorEdit : public pqtControl
{
#ifdef QT_FINISH_PORT
	public:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		pqtGradient_GradientColorEdit(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
							QWidget* i_pParent);

		//----------------------------------------------------------------------------
		//	Destructor - dispose of controls
		//----------------------------------------------------------------------------
		~pqtGradient_GradientColorEdit();

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
		void GradientColorEdit_ValueChanged(wxCommandEvent& i_Event);

		tqcGradientColorEdit* m_pActualControl;
#endif // USE_QT
};

