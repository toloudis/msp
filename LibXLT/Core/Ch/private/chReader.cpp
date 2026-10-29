/*****************************************************************************
**  chReader.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Core/ch/chReader.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
chReader::chReader()
: m_bNewerVersion( false )
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chReader::~chReader()
{
}

//--------------------------------------------------------------------
//	Temporary: ReadItString into an itString (see the .hpp)
//--------------------------------------------------------------------
void chReader::ReadItString(itString& o_Val)
{
	std::string utf8;
	ReadItString(utf8);
	o_Val = utf8;
}

//------------------------------------------------------------------------
//	A flag that code can set if the file is newer
//------------------------------------------------------------------------
//virtual 
const bool chReader::IsNewerVersion() const
{
	return m_bNewerVersion;
}

//virtual 
void chReader::SetNewerVersion(const bool i_bIsNewerFlag)
{
	m_bNewerVersion = i_bIsNewerFlag;
}

