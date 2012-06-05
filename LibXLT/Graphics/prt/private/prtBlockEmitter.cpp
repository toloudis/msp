/*****************************************************************************
**	prtBlockEmitter.cpp
**
**		prtBlockEmitter is a prtEmitter which emits particles from a block
**	(rectangle in 3D).
**	
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/prt/prtBlockEmitter.hpp"

#include "Core/ma/maFunctions.hpp"


//--------------------------------------------------------------------
//	This constructor makes a block with all sides equal (a cube).
//--------------------------------------------------------------------
prtBlockEmitter::prtBlockEmitter(float i_Side)
:	m_X(i_Side * 0.5f),
	m_Y(i_Side * 0.5f),
	m_Z(i_Side * 0.5f)
{
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtBlockEmitter::prtBlockEmitter(float i_XSide, float i_YSide, float i_ZSide)
:	m_X(i_XSide * 0.5f),
	m_Y(i_YSide * 0.5f),
	m_Z(i_ZSide * 0.5f)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtBlockEmitter::~prtBlockEmitter()
{
}

//--------------------------------------------------------------------
//	GetPosition returns a random location in the emitter area
//--------------------------------------------------------------------
maPoint3d prtBlockEmitter::GetPosition() const
{
	return maPoint3d(	maFunctions::FloatRand(-m_X, m_X),
						maFunctions::FloatRand(-m_Y, m_Y),
						maFunctions::FloatRand(-m_Z, m_Z));
}

//--------------------------------------------------------------------
//	GetSides returns the lengths of the sides
//--------------------------------------------------------------------
void prtBlockEmitter::GetSides(float& o_X, float& o_Y, float& o_Z) const 
{	
	o_X = m_X * 2.0f;
	o_Y = m_Y * 2.0f;
	o_Z = m_Z * 2.0f; 
}
