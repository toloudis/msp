/****************************************************************************\
**	pqtInt32Converter.hpp
**
**		Wrapper for getting values out of properties scaled by units.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_INT32CONVERTER_HPP
#error pqtInt32Converter.hpp multiply included
#endif
#define PQT_INT32CONVERTER_HPP

#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif
#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif


//============================================================================
//============================================================================
class prtyPropertyUIInfo;
class pqtControl;


//============================================================================
//============================================================================
class pqtInt32Converter
{
public:
#ifdef USE_QT
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
	static void SetCommonValue(pqtControl* io_pControl, int i_NewValue);
#endif // USE_QT
};

