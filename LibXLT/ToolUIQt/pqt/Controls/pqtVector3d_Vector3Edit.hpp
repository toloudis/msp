/****************************************************************************\
**	pqtVector3d_Vector3Edit.hpp
**
**		Intermediate class between the property (prtyVector3d) and 
**	the control (Vector3Edit).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_VECTOR3D_VECTOR3EDIT_HPP
#error pqtVector3d_Vector3Edit.hpp multiply included
#endif
#define PQT_VECTOR3D_VECTOR3EDIT_HPP

#ifndef PQT_TEMPLATE_VECTOR3EDIT_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_Vector3Edit.hpp"
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
class pqtVector3d_Vector3Edit_Converter
{
#ifdef QT_FINISH_PORT
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(tqcVector3Edit* i_pActualControl,
									 maVector3d i_Value)
	{
		i_pActualControl->SetValue(i_Value);
	}

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static maVector3d GetValueFromControl(tqcVector3Edit* i_pActualControl)
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
typedef pqtTemplate_Vector3Edit<prtyVector3d, maVector3d, pqtVector3d_Vector3Edit_Converter<prtyVector3d> > pqtVector3d_Vector3Edit;
typedef pqtTemplate_Vector3Edit<prtyPoint3d, maVector3d, pqtVector3d_Vector3Edit_Converter<prtyPoint3d> > pqtPoint3d_Vector3Edit;

