/****************************************************************************\
**  fogFogData.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef FOG_FOGDATA_HPP
#error fogFogData.hpp multiply included
#endif
#define FOG_FOGDATA_HPP

#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
#endif
#ifndef PRTY_ENUM_HPP
#include "Core/prty/prtyEnum.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_NAME_HPP
#include "Core/prty/prtyName.hpp"
#endif


//============================================================================
//============================================================================
class fogFogData
{
public:
	enum FogMode
	{
		e_None = 0,
		e_Linear,
		e_Exponential,
		e_ExponentialSq
	};

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	fogFogData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	fogFogData(const fogFogData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~fogFogData();

	//-------------------------------------------------
	bool operator == (const fogFogData& i_Item);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	fogFogData& operator=(const fogFogData& i_Data);

public:
	prtyEnum	m_Mode;
	prtyColor	m_Color;
	prtyFloat	m_Density;
	prtyFloat	m_Start;
	prtyFloat	m_End;

	prtyName m_Name;
};
