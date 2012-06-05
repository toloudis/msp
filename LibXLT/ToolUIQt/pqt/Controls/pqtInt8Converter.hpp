/****************************************************************************\
**	pqtInt8Converter.hpp
**
**		Wrapper for getting values out of properties scaled by units.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_INT8CONVERTER_HPP
#error pqtInt8Converter.hpp multiply included
#endif
#define PQT_INT8CONVERTER_HPP

#ifndef PRTY_INT8_HPP
#include "Core/prty/prtyInt8.hpp"
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
class pqtInt8Converter
{
#ifdef USE_QT
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
	static void SetCommonValue(pqtControl* io_pControl, envType::Int8 i_NewValue);
#endif // USE_QT
};

