/*****************************************************************************
**	cmraDriverDataSubjectInfo.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverDataSubjectInfo.hpp"

#include "Core/dbg/dbgLog.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverDataSubjectInfo::cmraDriverDataSubjectInfo()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverDataSubjectInfo::~cmraDriverDataSubjectInfo()
{
}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
cmraDriverDataSubjectInfo* cmraDriverDataSubjectInfo::Clone()
{
	return new cmraDriverDataSubjectInfo(*this);
}

//--------------------------------------------------------------------
//	copy the information
//--------------------------------------------------------------------
cmraDriverDataSubjectInfo& cmraDriverDataSubjectInfo::operator = (const cmraDriverDataSubjectInfo& i_Info)
{
	//	copy the object names
	//
	this->m_ObjectNames.clear();

	int size = i_Info.m_ObjectNames.size();
	this->m_ObjectNames.resize( size );

	int i;
	for ( i = 0; i < size ; i++ )
	{
		this->m_ObjectNames[i] = i_Info.m_ObjectNames[i];
		//DBG_LOG3( "cDDSI %02d (%s) (%s)", i, this->m_ObjectNames[i].GetString().c_str(), i_Info.m_ObjectNames[i].GetString().c_str() );
	}

	return *this;
}
