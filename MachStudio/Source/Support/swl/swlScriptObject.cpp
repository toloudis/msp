/*****************************************************************************
**	fgmtScriptObject.cpp
**
**	This class handles altering the fragment flags within a script object
**	after loading
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/swl/swlScriptObject.hpp"

#include "Support/swl/swlPropertyObject.hpp"


//============================================================================
//============================================================================
namespace
{
}	// end of namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
swlScriptObject::swlScriptObject()
{
	m_PropertyUI = new swlPropertyObject("", m_Data);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
swlScriptObject::~swlScriptObject()
{
	if (m_PropertyUI)
		delete m_PropertyUI;
	m_PropertyUI = NULL;
}

//--------------------------------------------------------------------
//	Return Fragment properties structure
//--------------------------------------------------------------------
const swlData& swlScriptObject::GetSwlData() const
{
	return m_Data;
}

//--------------------------------------------------------------------
// Get vector of fragments in order to store info to file
//--------------------------------------------------------------------
void swlScriptObject::SetSwlData(const swlData& i_Data)
{
	m_Data = i_Data;
}

//--------------------------------------------------------------------
//	Return Fragment ui properties
//--------------------------------------------------------------------
swlPropertyObject* swlScriptObject::GetPropertyUI() const
{
	return m_PropertyUI;
}

//--------------------------------------------------------------------
//	Register interest
//--------------------------------------------------------------------
void swlScriptObject::RegisterInterest(swlInterest* i_Interest)
{
	if (m_PropertyUI)
	{
		m_PropertyUI->RegisterInterest(i_Interest);
	}
}
