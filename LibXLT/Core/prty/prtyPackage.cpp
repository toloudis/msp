/*****************************************************************************
**  prtyPackage.cpp
**
**		see .hpp 
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/prty/prtyPackage.hpp"

#include "Core/prty/prtyInterestUtil.hpp"


//----------------------------------------------------------------------------
// initialize prty packages
//----------------------------------------------------------------------------
void prtyPackage::Init()
{
	prtyInterestUtil::Init();
}

//----------------------------------------------------------------------------
// clean up prty packages
//----------------------------------------------------------------------------
void prtyPackage::CleanUp()
{
	prtyInterestUtil::DeInit();
}