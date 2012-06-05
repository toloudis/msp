/*****************************************************************************
**	actnTitleCard.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/actn/actnTitleCard.hpp"

//----------------------------------------------------------------
// constructor
//----------------------------------------------------------------
actnTitleCard::actnTitleCard()
{
}

actnTitleCard::actnTitleCard(const itString& i_ObjName, float i_inittime)
{
	m_Data = new TitleCardData();
	m_Data->BodySize = 24.0;
	m_Data->BodyText = itString("BODY");
	m_Data->ObjName = i_ObjName;
	m_Data->TitleSize = 36.0;
	m_Data->TitleText = itString("TITLE");
	m_Data->Init_Time = i_inittime;
}

//----------------------------------------------------------------
// destructor
//----------------------------------------------------------------
actnTitleCard::~actnTitleCard()
{
	delete m_Data;
}

TitleCardData* actnTitleCard::GetData()
{
	return m_Data;
}