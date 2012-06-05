/****************************************************************************\
**	pwxVector3d_Vector3Edit.hpp
**
**		Intermediate class between the property (prtyVector3d) and 
**	the control (Vector3Edit).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_VECTOR3D_VECTOR3EDIT_HPP
#error pwxVector3d_Vector3Edit.hpp multiply included
#endif
#define PWX_VECTOR3D_VECTOR3EDIT_HPP

#ifndef PWX_TEMPLATE_VECTOR3EDIT_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_Vector3Edit.hpp"
#endif
#ifndef PRTY_VECTOR3D_HPP
#include "Core/prty/prtyVector3d.hpp"
#endif
#ifndef PRTY_POINT3D_HPP
#include "Core/prty/prtyPoint3d.hpp"
#endif 

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
template <class PropertyType>
class pwxVector3d_Vector3Edit_Converter
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(twcVector3Edit* i_pActualControl,
									 maVector3d i_Value)
	{
		i_pActualControl->SetValue(i_Value);
	}

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static maVector3d GetValueFromControl(twcVector3Edit* i_pActualControl)
	{
		return i_pActualControl->GetValue();
	}
	
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	static maVector3d GetPropertyValue(prtyProperty* i_pProperty)
	{
		return (static_cast<PropertyType*>(i_pProperty))->GetScaledValue();
	}

	//----------------------------------------------------------------------------
	// Get value from multiple properties, returns true if all the same. 
	//----------------------------------------------------------------------------
	static bool GetCommonValue(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo, maVector3d& o_NewValue)
	{
		return pwxControlUtil::GetScaledCommonValue<maVector3d, PropertyType>(i_pUIInfo, o_NewValue);
	}
	
	//----------------------------------------------------------------------------
	// Set value into properties for control
	//----------------------------------------------------------------------------
	static void SetCommonValue(pwxControl* io_pControl, maVector3d i_NewValue)
	{
		pwxControlUtil::SetScaledCommonValue<maVector3d, PropertyType>(io_pControl, i_NewValue);
	}
};

//============================================================================
//============================================================================
typedef pwxTemplate_Vector3Edit<prtyVector3d, maVector3d, pwxVector3d_Vector3Edit_Converter<prtyVector3d> > pwxVector3d_Vector3Edit;
typedef pwxTemplate_Vector3Edit<prtyPoint3d, maVector3d, pwxVector3d_Vector3Edit_Converter<prtyPoint3d> > pwxPoint3d_Vector3Edit;

#endif // USE_WXWIDGETS
