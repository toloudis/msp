/****************************************************************************\
**	ControlsTestData.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "ControlsTestData.hpp"


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ControlsTestData::ControlsTestData()
:	m_bBooleanTest("Boolean Test", false),
	m_bTriggerTest("Trigger Test"),
	m_Int8Test("Int8 Test", 0),
	m_Int32Test("Int32 Test", 0),
	m_FloatTest("Float Test", 0.0f)
{
}
