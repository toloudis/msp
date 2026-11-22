/****************************************************************************\
**	pwxInt8Converter.hpp
**
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/Controls/pwxInt8Converter.hpp"
#include "ToolUIWx/pwx/Controls/pwxControlUtil.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int8_t pwxInt8Converter::GetPropertyValue(prtyProperty* i_pProperty)
{
	return (static_cast<prtyInt8*>(i_pProperty))->GetValue();
}

//----------------------------------------------------------------------------
// Get value from multiple properties, returns true if all the same. 
//----------------------------------------------------------------------------
bool pwxInt8Converter::GetCommonValue(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo, int8_t& o_NewValue)
{
	return pwxControlUtil::GetCommonValue<int8_t, prtyInt8>(i_pUIInfo, o_NewValue);
}

//----------------------------------------------------------------------------
// Set value into properties for control
//----------------------------------------------------------------------------
void pwxInt8Converter::SetCommonValue(pwxControl* io_pControl, int8_t i_NewValue)
{
	pwxControlUtil::SetCommonValue<int8_t, prtyInt8>(io_pControl, i_NewValue);
}


#endif
