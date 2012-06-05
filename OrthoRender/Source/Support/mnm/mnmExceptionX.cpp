/****************************************************************************\
**  mnmExceptionX.cpp
**
**      mnmExceptionX.cpp defines the game-specific exception classes used
**	by Ultimate Ride.
**
**	Extra Large Technology
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/

#include "Support/mnm/mnmExceptionX.hpp"

//========================================================================
//	mnmInvalidTileFileX is thrown when a prop file (.prd) is either
//	corrupt or is an unknown version.
//========================================================================
mnmInvalidTileFileX::mnmInvalidTileFileX(const fsLocator& i_Locator)
:	m_Locator(i_Locator)
{
}

//========================================================================
//========================================================================
mnmInvalidTileFileX::~mnmInvalidTileFileX()
{
}

//========================================================================
//	Index() returns the error code/string index.
//========================================================================
envError::Code mnmInvalidTileFileX::Index() const
{
	return envError::GetCode(100, 1);
}

//========================================================================
//	GetLocator() returns the offending locator
//========================================================================
const fsLocator& mnmInvalidTileFileX::GetLocator() const
{
	return m_Locator;
}
