/****************************************************************************\
**	pwxRotation_Vector3EditUpDown.hpp
**
**		Intermediate class between the property (prtyRotation) and 
**	the control (Vector3EditUpDown).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_ROTATION_VECTOR3EDITUPDOWN_HPP
#error pwxRotation_Vector3EditUpDown.hpp multiply included
#endif
#define PWX_ROTATION_VECTOR3EDITUPDOWN_HPP

#ifndef PWX_TEMPLATE_VECTOR3EDITUPDOWN_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_Vector3EditUpDown.hpp"
#endif
#ifndef PRTY_ROTATION_HPP
#include "Core/prty/prtyRotation.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class pwxRotation_Vector3EditUpDown_Converter
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(twcVector3EditUpDown* i_pActualControl,
									 maRotation i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static maRotation GetValueFromControl(twcVector3EditUpDown* i_pActualControl);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	static maRotation GetPropertyValue(prtyProperty* i_pProperty);

	//----------------------------------------------------------------------------
	// Get value from multiple properties, returns true if all the same. 
	//----------------------------------------------------------------------------
	static bool GetCommonValue(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo, maRotation& o_NewValue);
	
	//----------------------------------------------------------------------------
	// Set value into properties for control
	//----------------------------------------------------------------------------
	static void SetCommonValue(pwxControl* io_pControl, maRotation i_NewValue);
};

//============================================================================
//============================================================================
typedef pwxTemplate_Vector3EditUpDown<prtyRotation, maRotation, pwxRotation_Vector3EditUpDown_Converter> pwxRotation_Vector3EditUpDown;

#endif // USE_WXWIDGETS
