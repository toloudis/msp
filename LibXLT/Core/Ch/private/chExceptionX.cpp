/*****************************************************************************
**  chExceptionX.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/ch/chExceptionX.hpp"


//--------------------------------------------------------------------
// Returns error message string
//--------------------------------------------------------------------
std::string chInvalidChunkX::GetErrorMessage() const
{
	return "Invalid Chunk while reading file";
}
