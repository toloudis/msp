/****************************************************************************\
**  sgpuVector.cpp
**
**      sgpuVector.hpp defines the sgpuVector class
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "sgpuVector.hpp"
#include "sgpuMatrix.hpp"
#include "sgpuUtilsImpl.hpp"
#include "sgpuException.hpp"

#include "Core/ma/maMatrix4x4.hpp"

#include <cmath>
//========================================================================
//	default constructor
//========================================================================
sgpuVector3::sgpuVector3()
{
	m_Data[0] = m_Data[1] = m_Data[2]  = 0.0f;
	//dont care what is in m_Data[3]
}

//========================================================================
//	constructor
//========================================================================
sgpuVector3::sgpuVector3(float i_F0,  float i_F1,  float i_F2  )
{
	m_Data[0]=i_F0;  
	m_Data[1]=i_F1;
	m_Data[2]=i_F2;
	//dont care what is in m_Data[3]
}


//========================================================================
//	operator: Multiply (*)
//========================================================================
float sgpuVector3::operator * (const sgpuVector3& i_V) const
{
      return m_Data[0] * i_V( 0 ) + m_Data[1] * i_V( 1 ) + m_Data[2] * i_V( 2 );
}

//========================================================================
//	operator: Multiply (*)
//========================================================================
sgpuVector3 sgpuVector3::operator * (const sgpuMatrix& i_TM ) const
{
	maVector3d maVec( m_Data[0], m_Data[1], m_Data[2]);
	const sgpuVector3 &vec0 = i_TM.m_Vec[1];
	maMatrix4x4 maTM( i_TM(0,0), i_TM(0, 1), i_TM(0,2), i_TM(0,3),
		i_TM(1,0), i_TM(1,1), i_TM(1,2), i_TM(1,3),
		i_TM(2,0), i_TM(2,1), i_TM(2,2), i_TM(2,3),
		i_TM(3,0), i_TM(3,1), i_TM(3, 2), i_TM(3,3) );
	maVec = maTM * maVec;
	return sgpuVector3( maVec[0], maVec[1], maVec[2] );

}
//========================================================================
//	operator: Addition (+)
//========================================================================
sgpuVector3 sgpuVector3::operator + (const sgpuVector3& i_V) const
{
      return sgpuVector3( m_Data[0] + i_V( 0 ),  m_Data[1] + i_V( 1 ),  m_Data[2] + i_V( 2 ) );
}


//========================================================================
//	operator: Addition (+=)
//========================================================================
void 
sgpuVector3::operator+= (const sgpuVector3& i_V)
{
	//	The old version of this function tried to do the work in place;
	//	however, it did not account for values changing in the middle
	//	of the calculation.  (For instance, computing m_Mat[0] as a new
	//	value, and then using m_Mat[0], expecting it to still be the old
	//	value).  This way is safer, simpler, easier, and also more
	//	oriented towards exception safety.
	*this = (*this) + i_V;
}


//========================================================================
//	operator: Substraction (-)
//========================================================================
sgpuVector3 sgpuVector3::operator - (const sgpuVector3& i_V) const
{
      return sgpuVector3( m_Data[0] - i_V ( 0 ),  m_Data[1] - i_V( 1 ),  m_Data[2] - i_V( 2 ) );
}


//========================================================================
//	operator: Substraction (-=)
//========================================================================
void 
sgpuVector3::operator-= (const sgpuVector3& i_V)
{
	//	The old version of this function tried to do the work in place;
	//	however, it did not account for values changing in the middle
	//	of the calculation.  (For instance, computing m_Mat[0] as a new
	//	value, and then using m_Mat[0], expecting it to still be the old
	//	value).  This way is safer, simpler, easier, and also more
	//	oriented towards exception safety.
	*this = (*this) - i_V;
}


//========================================================================
//	operator: Scaling (*)
//========================================================================
sgpuVector3 sgpuVector3::operator * (const float i_F) const
{
      return sgpuVector3( m_Data[0] * i_F,  m_Data[1] * i_F,  m_Data[2] * i_F  );
}


//========================================================================
//	operator: Scale (*=)
//========================================================================
void 
sgpuVector3::operator*= (const float i_F)
{
	//	The old version of this function tried to do the work in place;
	//	however, it did not account for values changing in the middle
	//	of the calculation.  (For instance, computing m_Mat[0] as a new
	//	value, and then using m_Mat[0], expecting it to still be the old
	//	value).  This way is safer, simpler, easier, and also more
	//	oriented towards exception safety.
	*this = (*this) * i_F;
}

bool sgpuVector3::operator == (const sgpuVector3& i_A) const
{
	return  i_A.m_Data[0] == m_Data[0] && 
		    i_A.m_Data[1] == m_Data[1] &&
			i_A.m_Data[2] == m_Data[2];

}

bool sgpuVector3::EpsilonEqual( const sgpuVector3& i_A) const
{
	return	fEpsilonEqual( m_Data[0],  i_A.m_Data[0] ) &&
			fEpsilonEqual( m_Data[1],  i_A.m_Data[1] ) &&
			fEpsilonEqual( m_Data[2],  i_A.m_Data[2] );
}