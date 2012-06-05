/****************************************************************************\
**	pqtInt32Converter.hpp
**
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtInt32Converter.hpp"
#include "ToolUIQt/pqt/Controls/pqtControlUtil.hpp"


#ifdef USE_QT

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int pqtInt32Converter::GetPropertyValue(prtyProperty* i_pProperty)
{
	return (static_cast<prtyInt32*>(i_pProperty))->GetValue();
}

//----------------------------------------------------------------------------
// Get value from multiple properties, returns true if all the same. 
//----------------------------------------------------------------------------
bool pqtInt32Converter::GetCommonValue(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo, int& o_NewValue)
{
	return pqtControlUtil::GetCommonValue<int, prtyInt32>(i_pUIInfo, o_NewValue);
}

//----------------------------------------------------------------------------
// Set value into properties for control
//----------------------------------------------------------------------------
void pqtInt32Converter::SetCommonValue(pqtControl* io_pControl, int i_NewValue)
{
	pqtControlUtil::SetCommonValue<int, prtyInt32>(io_pControl, i_NewValue);
}


#endif
