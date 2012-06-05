/****************************************************************************\
**	pqtFloatConverter.hpp
**
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtFloatConverter.hpp"
#include "ToolUIQt/pqt/Controls/pqtControlUtil.hpp"


#ifdef USE_QT

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float pqtFloatConverter::GetPropertyValue(prtyProperty* i_pProperty)
{
	return (static_cast<prtyFloat*>(i_pProperty))->GetScaledValue();
}

//----------------------------------------------------------------------------
// Get value from multiple properties, returns true if all the same. 
//----------------------------------------------------------------------------
bool pqtFloatConverter::GetCommonValue(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo, float& o_NewValue)
{
	return pqtControlUtil::GetScaledCommonValue<float, prtyFloat>(i_pUIInfo, o_NewValue);
}

//----------------------------------------------------------------------------
// Set value into properties for control
//----------------------------------------------------------------------------
void pqtFloatConverter::SetCommonValue(pqtControl* io_pControl, float i_NewValue)
{
	pqtControlUtil::SetScaledCommonValue<float, prtyFloat>(io_pControl, i_NewValue);
}
#endif
