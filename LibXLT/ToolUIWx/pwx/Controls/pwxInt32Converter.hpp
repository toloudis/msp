/****************************************************************************\
**	pwxInt32Converter.hpp
**
**		Wrapper for getting values out of properties scaled by units.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_INT32CONVERTER_HPP
#error pwxInt32Converter.hpp multiply included
#endif
#define PWX_INT32CONVERTER_HPP

#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS

class prtyPropertyUIInfo;
class pwxControl;

//============================================================================
//============================================================================
class pwxInt32Converter
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	static int GetPropertyValue(prtyProperty* i_pProperty);

	//----------------------------------------------------------------------------
	// Get value from multiple properties, returns true if all the same. 
	//----------------------------------------------------------------------------
	static bool GetCommonValue(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo, int& o_NewValue);
	
	//----------------------------------------------------------------------------
	// Set value into properties for control
	//----------------------------------------------------------------------------
	static void SetCommonValue(pwxControl* io_pControl, int i_NewValue);
};

#endif // USE_WXWIDGETS
