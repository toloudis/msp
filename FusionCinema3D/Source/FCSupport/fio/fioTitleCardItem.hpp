/*****************************************************************************
**	fioTitleCardData.hpp
**
**		handles helper functions to separate app functionality from the UI.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef FIO_TITLECARDITEM_HPP
#error fioTitleCardItem.hpp multiply defined
#endif
#define FIO_TITLECARDITEM_HPP

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif


//============================================================================
// Forward References
//============================================================================
class fsLocator;
class itString;


//============================================================================
//============================================================================
class fioTitleCardItemData
{
public:
	int TitleX, TitleY;
	int BodyX, BodyY;
	int TitleWidth, TitleHeight;
	int BodyWidth, BodyHeight;
	int PointSize;
	bool isBold;
	itString FontFamily;
	float DocumentColor[3];
	int TitleMaxCols, TitleMaxRows;
	int BodyMaxCols, BodyMaxRows;
	int TitleSize, BodySize;
};

