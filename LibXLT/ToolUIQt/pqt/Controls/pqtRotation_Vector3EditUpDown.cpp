/****************************************************************************\
**	pqtRotation_Vector3EditUpDown.hpp
**
**		Intermediate class between the property (prtyRotation) and 
**	the control (Vector3EditUpDown).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtRotation_Vector3EditUpDown.hpp"

#include "Core/ma/maConstants.hpp"


#ifdef QT_FINISH_PORT

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtRotation_Vector3EditUpDown_Converter::SetValueIntoControl(tqcVector3EditUpDown* i_pActualControl,
											maRotation i_Value)
{
	float x = 0, y = 0, z = 0;
	i_Value.GetEuler(x,y,z);

	// HACK [rjk] to handle when GetEuler returns a NaN
	if (x != x) {x = 0; DBG_ERROR("x = NaN!!!");}
	if (y != y) {y = 0; DBG_ERROR("y = NaN!!!");}
	if (z != z) {z = 0; DBG_ERROR("z = NaN!!!");}

	// Convert to degrees for display
	x *= maConstants::c_fRadToAngle;
	y *= maConstants::c_fRadToAngle;
	z *= maConstants::c_fRadToAngle;

	i_pActualControl->SetValue(maVector3d(x,y,z));
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
maRotation pqtRotation_Vector3EditUpDown_Converter::GetValueFromControl(tqcVector3EditUpDown* i_pActualControl)
{
	maVector3d angles = i_pActualControl->GetValue();

	maRotation rot;
	rot.SetEuler( angles.m_X * maConstants::c_fAngleToRad, 
				  angles.m_Y * maConstants::c_fAngleToRad, 
				  angles.m_Z * maConstants::c_fAngleToRad);
	return rot;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
maRotation pqtRotation_Vector3EditUpDown_Converter::GetPropertyValue(prtyProperty* i_pProperty)
{
	return (static_cast<prtyRotation*>(i_pProperty))->GetValue();
}

//----------------------------------------------------------------------------
// Get value from multiple properties, returns true if all the same. 
//----------------------------------------------------------------------------
bool pqtRotation_Vector3EditUpDown_Converter::GetCommonValue(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo, maRotation& o_NewValue)
{
	return pqtControlUtil::GetCommonValue<maRotation, prtyRotation>(i_pUIInfo, o_NewValue);
}

//----------------------------------------------------------------------------
// Set value into properties for control
//----------------------------------------------------------------------------
void pqtRotation_Vector3EditUpDown_Converter::SetCommonValue(pqtControl* io_pControl, maRotation i_NewValue)
{
	pqtControlUtil::SetCommonValue<maRotation, prtyRotation>(io_pControl, i_NewValue);
}

#endif
