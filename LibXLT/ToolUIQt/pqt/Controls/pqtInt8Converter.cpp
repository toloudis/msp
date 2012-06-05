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
envType::Int8 pqtInt8Converter::GetPropertyValue(prtyProperty* i_pProperty)
{
	return (static_cast<prtyInt8*>(i_pProperty))->GetValue();
}

//----------------------------------------------------------------------------
// Get value from multiple properties, returns true if all the same. 
//----------------------------------------------------------------------------
bool pqtInt8Converter::GetCommonValue(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo, envType::Int8& o_NewValue)
{
	return pqtControlUtil::GetCommonValue<envType::Int8, prtyInt8>(i_pUIInfo, o_NewValue);
}

//----------------------------------------------------------------------------
// Set value into properties for control
//----------------------------------------------------------------------------
void pqtInt8Converter::SetCommonValue(pqtControl* io_pControl, envType::Int8 i_NewValue)
{
	pqtControlUtil::SetCommonValue<envType::Int8, prtyInt8>(io_pControl, i_NewValue);
}


#endif
