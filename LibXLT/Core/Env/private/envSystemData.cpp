/**********************************************************
**  envSystemData.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\*********************************************************/

#include "Core/env/private/envSystemData.hpp"
#include "Core/env/private/envSystemDataPAC.hpp"


//============================================================================
//============================================================================
namespace envSystemData
{

//------------------------------------------------------------------------
//	Don't call Init() yourself; it is called by the package Init().
//------------------------------------------------------------------------
void Init()
{
	envSystemDataPAC::Init();
}

//------------------------------------------------------------------------
//	Don't call CleanUp() yourself; it is called by the package CleanUp().
//------------------------------------------------------------------------
void CleanUp() throw()
{
	envSystemDataPAC::CleanUp();
}

}

