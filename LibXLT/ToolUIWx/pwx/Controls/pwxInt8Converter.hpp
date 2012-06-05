/****************************************************************************\
**	pwxInt8Converter.hpp
**
**		Wrapper for getting values out of properties scaled by units.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_INT8CONVERTER_HPP
#error pwxInt8Converter.hpp multiply included
#endif
#define PWX_INT8CONVERTER_HPP

#ifndef PRTY_INT8_HPP
#include "Core/prty/prtyInt8.hpp"
#endif
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS

class prtyPropertyUIInfo;
class pwxControl;

//============================================================================
//============================================================================
class pwxInt8Converter
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	static envType::Int8 GetPropertyValue(prtyProperty* i_pProperty);

	//----------------------------------------------------------------------------
	// Get value from multiple properties, returns true if all the same. 
	//----------------------------------------------------------------------------
	static bool GetCommonValue(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo, envType::Int8& o_NewValue);
	
	//----------------------------------------------------------------------------
	// Set value into properties for control
	//----------------------------------------------------------------------------
	static void SetCommonValue(pwxControl* io_pControl, envType::Int8 i_NewValue);
};

#endif // USE_WXWIDGETS
