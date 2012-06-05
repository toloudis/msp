/****************************************************************************\
**	pqtVector3d_Vector3EditUpDown.hpp
**
**		Intermediate class between the property (prtyVector3d) and 
**	the control (Vector3EditUpDown).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_VECTOR3D_VECTOR3EDITUPDOWN_HPP
#error pqtVector3d_Vector3EditUpDown.hpp multiply included
#endif
#define PQT_VECTOR3D_VECTOR3EDITUPDOWN_HPP

#ifndef PQT_TEMPLATE_VECTOR3EDITUPDOWN_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_Vector3EditUpDown.hpp"
#endif
#ifndef PRTY_VECTOR3D_HPP
#include "Core/prty/prtyVector3d.hpp"
#endif
#ifndef PRTY_POINT3D_HPP
#include "Core/prty/prtyPoint3d.hpp"
#endif 


//============================================================================
//============================================================================
template <class PropertyType>
class pqtVector3d_Vector3EditUpDown_Converter
{
#ifdef QT_FINISH_PORT
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(tqcVector3EditUpDown* i_pActualControl,
									 maVector3d i_Value)
	{
		i_pActualControl->SetValue(i_Value);
	}

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static maVector3d GetValueFromControl(tqcVector3EditUpDown* i_pActualControl)
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
		return pqtControlUtil::GetScaledCommonValue<maVector3d, PropertyType>(i_pUIInfo, o_NewValue);
	}
	
	//----------------------------------------------------------------------------
	// Set value into properties for control
	//----------------------------------------------------------------------------
	static void SetCommonValue(pqtControl* io_pControl, maVector3d i_NewValue)
	{
		pqtControlUtil::SetScaledCommonValue<maVector3d, PropertyType>(io_pControl, i_NewValue);
	}
#endif // USE_QT
};

//============================================================================
//============================================================================
typedef pqtTemplate_Vector3EditUpDown<prtyVector3d, maVector3d, pqtVector3d_Vector3EditUpDown_Converter<prtyVector3d> > pqtVector3d_Vector3EditUpDown;
typedef pqtTemplate_Vector3EditUpDown<prtyPoint3d, maVector3d, pqtVector3d_Vector3EditUpDown_Converter<prtyPoint3d> > pqtPoint3d_Vector3EditUpDown;

