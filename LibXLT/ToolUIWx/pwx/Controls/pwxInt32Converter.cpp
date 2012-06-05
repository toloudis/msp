/****************************************************************************\
**	pwxInt32Converter.hpp
**
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/Controls/pwxInt32Converter.hpp"
#include "ToolUIWx/pwx/Controls/pwxControlUtil.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int pwxInt32Converter::GetPropertyValue(prtyProperty* i_pProperty)
{
	return (static_cast<prtyInt32*>(i_pProperty))->GetValue();
}

//----------------------------------------------------------------------------
// Get value from multiple properties, returns true if all the same. 
//----------------------------------------------------------------------------
bool pwxInt32Converter::GetCommonValue(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo, int& o_NewValue)
{
	return pwxControlUtil::GetCommonValue<int, prtyInt32>(i_pUIInfo, o_NewValue);
}

//----------------------------------------------------------------------------
// Set value into properties for control
//----------------------------------------------------------------------------
void pwxInt32Converter::SetCommonValue(pwxControl* io_pControl, int i_NewValue)
{
	pwxControlUtil::SetCommonValue<int, prtyInt32>(io_pControl, i_NewValue);
}


#endif
