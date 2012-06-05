/*****************************************************************************
**  pntPoint.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/pnt/pntPoint.hpp"

#include "Core/dbg/dbgLog.hpp"


//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
pntPoint::pntPoint()
:	m_Point(0.0f,0.0f,0.0f),
	m_Name(""),
	m_pCallback(NULL)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
pntPoint::~pntPoint()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const maPoint3d& pntPoint::GetPosition() const
{
	return m_Point;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void pntPoint::SetPosition(const maPoint3d & i_Pos)
{
	m_Point = i_Pos;
	if (m_pCallback)
		m_pCallback->PointChanged(this);
}


//--------------------------------------------------------------------
// Name for curve
//--------------------------------------------------------------------
const std::string& pntPoint::GetName() const
{
	return m_Name;
}
void pntPoint::SetName(const char* i_Name)
{
	m_Name = i_Name;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
maPoint3d pntPoint::Evaluate() const
{
	return this->m_Point;
}

//--------------------------------------------------------------------
// Set callback for when point changes value.
//--------------------------------------------------------------------
void pntPoint::SetCallback(PointChangedCallback* i_pCallback)
{
	DBG_ASSERT0(m_pCallback == NULL || i_pCallback == NULL, "pntPoint, Callback has already been assigned, need to expand API");
	m_pCallback = i_pCallback;
}
