/****************************************************************************\
**	pqtFloatConverter.hpp
**
**		Wrapper for getting values out of properties scaled by units.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_FLOATCONVERTER_HPP
#error pqtFloatConverter.hpp multiply included
#endif
#define PQT_FLOATCONVERTER_HPP

#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
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
class pqtFloatConverter
{
#ifdef USE_QT
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
	static void SetCommonValue(pqtControl* io_pControl, float i_NewValue);
#endif // USE_QT
};

