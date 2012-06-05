/****************************************************************************\
**	pwxFloatConverter.hpp
**
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/Controls/pwxFloatConverter.hpp"
#include "ToolUIWx/pwx/Controls/pwxControlUtil.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float pwxFloatConverter::GetPropertyValue(prtyProperty* i_pProperty)
{
	return (static_cast<prtyFloat*>(i_pProperty))->GetScaledValue();
}

//----------------------------------------------------------------------------
// Get value from multiple properties, returns true if all the same. 
//----------------------------------------------------------------------------
bool pwxFloatConverter::GetCommonValue(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo, float& o_NewValue)
{
	return pwxControlUtil::GetScaledCommonValue<float, prtyFloat>(i_pUIInfo, o_NewValue);
}

//----------------------------------------------------------------------------
// Set value into properties for control
//----------------------------------------------------------------------------
void pwxFloatConverter::SetCommonValue(pwxControl* io_pControl, float i_NewValue)
{
	pwxControlUtil::SetScaledCommonValue<float, prtyFloat>(io_pControl, i_NewValue);
}


#endif
