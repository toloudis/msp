/****************************************************************************\
**	pqtRotation_Vector3Edit.hpp
**
**		Intermediate class between the property (prtyRotation) and 
**	the control (Vector3Edit).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_ROTATION_VECTOR3EDIT_HPP
#error pqtRotation_Vector3Edit.hpp multiply included
#endif
#define PQT_ROTATION_VECTOR3EDIT_HPP

#ifndef PQT_TEMPLATE_VECTOR3EDIT_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_Vector3Edit.hpp"
#endif
#ifndef PRTY_ROTATION_HPP
#include "Core/prty/prtyRotation.hpp"
#endif


//============================================================================
//============================================================================
class pqtRotation_Vector3Edit_Converter
{
#ifdef QT_FINISH_PORT
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(tqcVector3Edit* i_pActualControl,
									 maRotation i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static maRotation GetValueFromControl(tqcVector3Edit* i_pActualControl);

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
	static void SetCommonValue(pqtControl* io_pControl, maRotation i_NewValue);
#endif // USE_QT
};

//============================================================================
//============================================================================
typedef pqtTemplate_Vector3Edit<prtyRotation, maRotation, pqtRotation_Vector3Edit_Converter> pqtRotation_Vector3Edit;

