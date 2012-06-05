/*****************************************************************************
**	cmraDriverDataViewTypeInfo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverDataViewTypeInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverDataViewTypeInfo::cmraDriverDataViewTypeInfo()
:	m_ViewType( e_DataView_Medium ),
	m_fViewTolerance( 0.25f )
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverDataViewTypeInfo::~cmraDriverDataViewTypeInfo()
{
}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
cmraDriverDataViewTypeInfo* cmraDriverDataViewTypeInfo::Clone()
{
	return new cmraDriverDataViewTypeInfo(*this);
}
