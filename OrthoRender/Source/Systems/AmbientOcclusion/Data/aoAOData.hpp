/****************************************************************************\
**  aoAOData.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef AO_AODATA_HPP
#error aoAOData.hpp multiply included
#endif
#define AO_AODATA_HPP

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
class aoAOData
{
public:

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	aoAOData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	aoAOData(const aoAOData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~aoAOData();

	//-------------------------------------------------
	bool operator == (const aoAOData& i_Item);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	aoAOData& operator=(const aoAOData& i_Data);

public:

	prtyFloat m_AORadius;
	prtyFloat m_AngleBias;
	prtyFloat m_Attenuation;
	prtyFloat m_Contrast;

	prtyFloat m_BlurWidth;
	prtyFloat m_BlurSharpness;

	prtyName m_Name;
};
