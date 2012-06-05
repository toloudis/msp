//****************************************************************************
//	actnTitleCardMgr.hpp
//
//	A manager for TitleCard
//
//	StudioGPU
//	Copyright(C) 2010 - All Rights Reserved
//****************************************************************************
#ifdef ACTN_TITLECARD_HPP
#error actnTitleCard.hpp multiply included
#endif
#define ACTN_TITLECARD_HPP

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class fsLocator;
class nameString;


struct TitleCardData
{
	itString ObjName;
	itString TitleText;
	itString BodyText;
	float TitleSize;
	float BodySize ;
	float Init_Time;
	//maPoint3d TitleColor;
	//maPoint3d BodyColor;
};




//============================================================================
//============================================================================
class actnTitleCard
{
public:
	
	//----------------------------------------------------------------
	// constructor
	//----------------------------------------------------------------
	actnTitleCard();

	actnTitleCard(const itString& i_ObjName, float i_inittime);

	//----------------------------------------------------------------
	// destructor
	//----------------------------------------------------------------
	~actnTitleCard();

	//----------------------------------------------------------------
	//----------------------------------------------------------------
	TitleCardData* GetData();

private:
	TitleCardData* m_Data;
};
