/****************************************************************************\
**  envInitX.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Core/env/envInitX.hpp"


//------------------------------------------------------------------------
//	The constructor argument is the name of the package that is reporting
//	a failure to initialize.
//------------------------------------------------------------------------
envInitX::envInitX(const char* i_Package)
:	m_Package(i_Package)
{
}

//--------------------------------------------------------------------
// Returns error message string
//--------------------------------------------------------------------
std::string envInitX::GetErrorMessage() const
{
	return "Error initializing package: " + m_Package;
}	
