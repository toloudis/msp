/****************************************************************************\
**	pwxFloatConverter.hpp
**
**		Wrapper for getting values out of properties scaled by units.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_FLOATCONVERTER_HPP
#error pwxFloatConverter.hpp multiply included
#endif
#define PWX_FLOATCONVERTER_HPP

#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS

class prtyPropertyUIInfo;
class pwxControl;

//============================================================================
//============================================================================
class pwxFloatConverter
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	static float GetPropertyValue(prtyProperty* i_pProperty);

	//----------------------------------------------------------------------------
	// Get value from multiple properties, returns true if all the same. 
	//----------------------------------------------------------------------------
	static bool GetCommonValue(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo, float& o_NewValue);
	
	//----------------------------------------------------------------------------
	// Set value into properties for control
	//----------------------------------------------------------------------------
	static void SetCommonValue(pwxControl* io_pControl, float i_NewValue);
};

#endif // USE_WXWIDGETS
