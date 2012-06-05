/*****************************************************************************
**  emdlStaticTemplate.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/emdl/emdlStaticTemplate.hpp"


//--------------------------------------------------------------------
//	Constructor
//--------------------------------------------------------------------
emdlStaticTemplate::emdlStaticTemplate()
:	m_NumLowRes(0), m_NumHighRes(0)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
emdlStaticTemplate::~emdlStaticTemplate()
{
}

//--------------------------------------------------------------------
//	SetNumLowRes sets the number of fragments that are 
//	low resolution. These should be at the end of the fragment list.
//--------------------------------------------------------------------
void emdlStaticTemplate::SetNumLowRes( int i_NumLowRes )
{
	m_NumLowRes = i_NumLowRes;
}

//--------------------------------------------------------------------
//	SetNumHighRes sets the number of fragments that are 
//	high resolution. These will be at the end of the fragment list,
//	after the low resolution fragments.
//--------------------------------------------------------------------
void emdlStaticTemplate::SetNumHighRes( int i_NumHighRes )
{
	m_NumHighRes = i_NumHighRes;
}
