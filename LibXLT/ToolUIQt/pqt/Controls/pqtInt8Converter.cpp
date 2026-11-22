/****************************************************************************\
**	pqtInt8Converter.hpp
**
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtInt8Converter.hpp"
#include "ToolUIQt/pqt/Controls/pqtControlUtil.hpp"

#ifdef USE_QT

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int8_t pqtInt8Converter::GetPropertyValue(prtyProperty* i_pProperty)
{
	return (static_cast<prtyInt8*>(i_pProperty))->GetValue();
}

//----------------------------------------------------------------------------
// Get value from multiple properties, returns true if all the same. 
//----------------------------------------------------------------------------
bool pqtInt8Converter::GetCommonValue(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo, int8_t& o_NewValue)
{
	return pqtControlUtil::GetCommonValue<int8_t, prtyInt8>(i_pUIInfo, o_NewValue);
}

//----------------------------------------------------------------------------
// Set value into properties for control
//----------------------------------------------------------------------------
void pqtInt8Converter::SetCommonValue(pqtControl* io_pControl, int8_t i_NewValue)
{
	pqtControlUtil::SetCommonValue<int8_t, prtyInt8>(io_pControl, i_NewValue);
}


#endif
