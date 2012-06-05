/****************************************************************************\
**	pwxRotation_Vector3Edit.hpp
**
**		Intermediate class between the property (prtyRotation) and 
**	the control (Vector3Edit).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_ROTATION_VECTOR3EDIT_HPP
#error pwxRotation_Vector3Edit.hpp multiply included
#endif
#define PWX_ROTATION_VECTOR3EDIT_HPP

#ifndef PWX_TEMPLATE_VECTOR3EDIT_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_Vector3Edit.hpp"
#endif
#ifndef PRTY_ROTATION_HPP
#include "Core/prty/prtyRotation.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class pwxRotation_Vector3Edit_Converter
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(twcVector3Edit* i_pActualControl,
									 maRotation i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static maRotation GetValueFromControl(twcVector3Edit* i_pActualControl);

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
typedef pwxTemplate_Vector3Edit<prtyRotation, maRotation, pwxRotation_Vector3Edit_Converter> pwxRotation_Vector3Edit;

#endif // USE_WXWIDGETS
