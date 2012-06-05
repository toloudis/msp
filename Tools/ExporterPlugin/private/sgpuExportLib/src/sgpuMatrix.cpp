/****************************************************************************\
**  sgpuMatrix.cpp
**
**      sgpuMatrix.hpp defines the sgpuMatrix class
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "sgpuMatrix.hpp"
#include "sgpuException.hpp"
#include "sgpuUtilsImpl.hpp"

#include <sstream>

//========================================================================
//	default constructor
//========================================================================
sgpuMatrix::sgpuMatrix()
{
	Identity();
}

//========================================================================
//	constructor
//========================================================================
sgpuMatrix::sgpuMatrix(float i_M0,  float i_M1,  float i_M2,  float i_M3,
						 float i_M4,  float i_M5,  float i_M6,  float i_M7,
						 float i_M8,  float i_M9,  float i_M10, float i_M11,
						 float i_M12, float i_M13, float i_M14, float i_M15 )
{
	m_Vec[0].m_Data[0] =i_M0; m_Vec[1].m_Data[0]=i_M4; m_Vec[2].m_Data[0]=i_M8;   m_Vec[3].m_Data[0]=i_M12;
	m_Vec[0].m_Data[1] =i_M1; m_Vec[1].m_Data[1]=i_M5; m_Vec[2].m_Data[1]=i_M9;   m_Vec[3].m_Data[1]=i_M13;
	m_Vec[0].m_Data[2] =i_M2; m_Vec[1].m_Data[2]=i_M6; m_Vec[2].m_Data[2]=i_M10;  m_Vec[3].m_Data[2]=i_M14;
	m_Vec[0].m_Data[3] =i_M3; m_Vec[1].m_Data[3]=i_M7; m_Vec[2].m_Data[3]=i_M11;  m_Vec[3].m_Data[3]=i_M15;
}

//========================================================================
//	constructor
//========================================================================
sgpuMatrix::sgpuMatrix( const sgpuVector3 &i_V0, const sgpuVector3 & i_V1, const sgpuVector3 & i_V2, const sgpuVector3 & i_V3 )
{
	m_Vec[0] = i_V0;
	m_Vec[1] = i_V1;
	m_Vec[2] = i_V2;
	m_Vec[3] = i_V3;
	//since the last element of vector
	//is a dont care element,
	//we have to correct it here
	m_Vec[0].m_Data[3] = 0.0f;
	m_Vec[1].m_Data[3] = 0.0f;
	m_Vec[2].m_Data[3] = 0.0f;
	m_Vec[3].m_Data[3] = 1.0f;
}

//========================================================================
//	operator: equality (==)
//========================================================================
bool sgpuMatrix::operator == (const sgpuMatrix& i_A) const
{
	return  i_A.m_Vec[0] == m_Vec[0] && 
		    i_A.m_Vec[1] == m_Vec[1] &&
			i_A.m_Vec[2] == m_Vec[2] &&
			i_A.m_Vec[3] == m_Vec[3] &&
			//since the equality of sgpuVector3
			//dont cover the last element
			//it is checked here
			i_A.m_Vec[0].m_Data[3] == m_Vec[0].m_Data[3] && 
			i_A.m_Vec[1].m_Data[3] == m_Vec[1].m_Data[3] && 
			i_A.m_Vec[2].m_Data[3] == m_Vec[2].m_Data[3] &&
			i_A.m_Vec[3].m_Data[3] == m_Vec[3].m_Data[3] ;
}
//========================================================================
//	operator: Approximate equal,
//  precision is specified by SGPU_EPSILON_EQUAL_PRECISION (sgpuExportLib.hpp)
//========================================================================
bool sgpuMatrix::EpsilonEqual( const sgpuMatrix& i_A) const
{
	return m_Vec[0].EpsilonEqual( i_A.m_Vec[0] ) &&
		   m_Vec[1].EpsilonEqual( i_A.m_Vec[1] ) &&
		   m_Vec[2].EpsilonEqual( i_A.m_Vec[2] ) &&
		   m_Vec[3].EpsilonEqual( i_A.m_Vec[3] ) &&		   
		   //since the equality of sgpuVector3
		   //dont cover the last element
		   //it is checked here
		   fEpsilonEqual( i_A.m_Vec[0].m_Data[3], m_Vec[0].m_Data[3] ) && 
		   fEpsilonEqual( i_A.m_Vec[1].m_Data[3], m_Vec[1].m_Data[3] ) && 
		   fEpsilonEqual( i_A.m_Vec[2].m_Data[3], m_Vec[2].m_Data[3] ) &&
		   fEpsilonEqual( i_A.m_Vec[3].m_Data[3], m_Vec[3].m_Data[3] );
}
//========================================================================
//	MakeTranslate()
//========================================================================
void 
sgpuMatrix::MakeTranslate( float i_Tx, float i_Ty, float i_Tz )
{
	m_Vec[0].m_Data[0]=1;		m_Vec[0].m_Data[1]=0;		m_Vec[0].m_Data[2]=0;		m_Vec[0].m_Data[3]=0;
	m_Vec[1].m_Data[0]=0;		m_Vec[1].m_Data[1]=1;		m_Vec[1].m_Data[2]=0;		m_Vec[1].m_Data[3]=0;
	m_Vec[2].m_Data[0]=0;		m_Vec[2].m_Data[1]=0;		m_Vec[2].m_Data[2]=1;		m_Vec[2].m_Data[3]=0;
	m_Vec[3].m_Data[0]=i_Tx;	m_Vec[3].m_Data[1]=i_Ty;	m_Vec[3].m_Data[2]=i_Tz;	m_Vec[3].m_Data[3]=1;
}

//========================================================================
//	Rotate()
//========================================================================
void 
sgpuMatrix::MakeRotate( float i_AngleRad, float i_AxisX, float i_AxisY, float i_AxisZ  )
{
	maVector3d axis_vec(i_AxisX, i_AxisY, i_AxisZ);
	axis_vec.Normalize();

	float c = float(cos(i_AngleRad));
	float s = float(sin(i_AngleRad));
	float t = 1 - c;

	float x = axis_vec.m_X;
	float y = axis_vec.m_Y;
	float z = axis_vec.m_Z;

	float sx = s * x;
	float sy = s * y;
	float sz = s * z;

	float txx = t*x*x;
	float txy = t*x*y;
	float txz = t*x*z;
	float tyy = t*y*y;
	float tyz = t*y*z;
	float tzz = t*z*z;

	m_Vec[0].m_Data[0]	= txx + c;
	m_Vec[0].m_Data[1]	= txy + sz;
	m_Vec[0].m_Data[2]	= txz - sy;
	m_Vec[0].m_Data[3]	= 0;
	m_Vec[1].m_Data[0]	= txy - sz;
	m_Vec[1].m_Data[1]	= tyy + c;
	m_Vec[1].m_Data[2]	= tyz + sx;
	m_Vec[1].m_Data[3]	= 0;
	m_Vec[2].m_Data[0]	= txz + sy;
	m_Vec[2].m_Data[1]	= tyz - sx;
	m_Vec[2].m_Data[2]	= tzz + c;
	m_Vec[2].m_Data[3]	= 0;
	m_Vec[3].m_Data[0]	= 0;
	m_Vec[3].m_Data[1]	= 0;
	m_Vec[3].m_Data[2]	= 0;
	m_Vec[3].m_Data[3]	= 1;
	
}


//========================================================================
//	MakeScale()
//========================================================================
void 
sgpuMatrix::MakeScale( float i_Sx, float i_Sy, float i_Sz )
{
	m_Vec[0].m_Data[0]=i_Sx;		m_Vec[0].m_Data[1]=0;			m_Vec[0].m_Data[2]=0;			m_Vec[0].m_Data[3]=0;
	m_Vec[1].m_Data[0]=0;			m_Vec[1].m_Data[1]=i_Sy;		m_Vec[1].m_Data[2]=0;			m_Vec[1].m_Data[3]=0;
	m_Vec[2].m_Data[0]=0;			m_Vec[2].m_Data[1]=0;			m_Vec[2].m_Data[2]=i_Sz;		m_Vec[2].m_Data[3]=0;
	m_Vec[3].m_Data[0]=0;			m_Vec[3].m_Data[1]=0;			m_Vec[3].m_Data[2]=0;			m_Vec[3].m_Data[3]=1;
}


//========================================================================
//	Identity()
//========================================================================
void 
sgpuMatrix::Identity()
{
	m_Vec[0].m_Data[0]=1;		m_Vec[0].m_Data[1]=0;		m_Vec[0].m_Data[2]=0;		m_Vec[0].m_Data[3]=0;
	m_Vec[1].m_Data[0]=0;		m_Vec[1].m_Data[1]=1;		m_Vec[1].m_Data[2]=0;		m_Vec[1].m_Data[3]=0;
	m_Vec[2].m_Data[0]=0;		m_Vec[2].m_Data[1]=0;		m_Vec[2].m_Data[2]=1;		m_Vec[2].m_Data[3]=0;
	m_Vec[3].m_Data[0]=0;		m_Vec[3].m_Data[1]=0;		m_Vec[3].m_Data[2]=0;		m_Vec[3].m_Data[3]=1;
}

//========================================================================
//	operator: Multiply (*)
//========================================================================
sgpuMatrix 
sgpuMatrix::operator * (const sgpuMatrix& i_A) const
{
      return sgpuMatrix(	m_Vec[0].m_Data[0]*i_A.m_Vec[0].m_Data[0]  + m_Vec[0].m_Data[1]*i_A.m_Vec[1].m_Data[0]  + m_Vec[0].m_Data[2]*i_A.m_Vec[2].m_Data[0]  + m_Vec[0].m_Data[3]*i_A.m_Vec[3].m_Data[0],    // row 1
							m_Vec[0].m_Data[0]*i_A.m_Vec[0].m_Data[1]  + m_Vec[0].m_Data[1]*i_A.m_Vec[1].m_Data[1]  + m_Vec[0].m_Data[2]*i_A.m_Vec[2].m_Data[1]  + m_Vec[0].m_Data[3]*i_A.m_Vec[3].m_Data[1],    
							m_Vec[0].m_Data[0]*i_A.m_Vec[0].m_Data[2]  + m_Vec[0].m_Data[1]*i_A.m_Vec[1].m_Data[2]  + m_Vec[0].m_Data[2]*i_A.m_Vec[2].m_Data[2]  + m_Vec[0].m_Data[3]*i_A.m_Vec[3].m_Data[2],    
							m_Vec[0].m_Data[0]*i_A.m_Vec[0].m_Data[3]  + m_Vec[0].m_Data[1]*i_A.m_Vec[1].m_Data[3]  + m_Vec[0].m_Data[2]*i_A.m_Vec[2].m_Data[3]  + m_Vec[0].m_Data[3]*i_A.m_Vec[3].m_Data[3],    

							m_Vec[1].m_Data[0]*i_A.m_Vec[0].m_Data[0]  + m_Vec[1].m_Data[1]*i_A.m_Vec[1].m_Data[0]  + m_Vec[1].m_Data[2]*i_A.m_Vec[2].m_Data[0]  + m_Vec[1].m_Data[3]*i_A.m_Vec[3].m_Data[0],    // row 1
							m_Vec[1].m_Data[0]*i_A.m_Vec[0].m_Data[1]  + m_Vec[1].m_Data[1]*i_A.m_Vec[1].m_Data[1]  + m_Vec[1].m_Data[2]*i_A.m_Vec[2].m_Data[1]  + m_Vec[1].m_Data[3]*i_A.m_Vec[3].m_Data[1],    
							m_Vec[1].m_Data[0]*i_A.m_Vec[0].m_Data[2]  + m_Vec[1].m_Data[1]*i_A.m_Vec[1].m_Data[2]  + m_Vec[1].m_Data[2]*i_A.m_Vec[2].m_Data[2]  + m_Vec[1].m_Data[3]*i_A.m_Vec[3].m_Data[2],    
							m_Vec[1].m_Data[0]*i_A.m_Vec[0].m_Data[3]  + m_Vec[1].m_Data[1]*i_A.m_Vec[1].m_Data[3]  + m_Vec[1].m_Data[2]*i_A.m_Vec[2].m_Data[3]  + m_Vec[1].m_Data[3]*i_A.m_Vec[3].m_Data[3],    

							m_Vec[2].m_Data[0]*i_A.m_Vec[0].m_Data[0]  + m_Vec[2].m_Data[1]*i_A.m_Vec[1].m_Data[0]  + m_Vec[2].m_Data[2]*i_A.m_Vec[2].m_Data[0]  + m_Vec[2].m_Data[3]*i_A.m_Vec[3].m_Data[0],    // row 1
							m_Vec[2].m_Data[0]*i_A.m_Vec[0].m_Data[1]  + m_Vec[2].m_Data[1]*i_A.m_Vec[1].m_Data[1]  + m_Vec[2].m_Data[2]*i_A.m_Vec[2].m_Data[1]  + m_Vec[2].m_Data[3]*i_A.m_Vec[3].m_Data[1],    
							m_Vec[2].m_Data[0]*i_A.m_Vec[0].m_Data[2]  + m_Vec[2].m_Data[1]*i_A.m_Vec[1].m_Data[2]  + m_Vec[2].m_Data[2]*i_A.m_Vec[2].m_Data[2]  + m_Vec[2].m_Data[3]*i_A.m_Vec[3].m_Data[2],    
							m_Vec[2].m_Data[0]*i_A.m_Vec[0].m_Data[3]  + m_Vec[2].m_Data[1]*i_A.m_Vec[1].m_Data[3]  + m_Vec[2].m_Data[2]*i_A.m_Vec[2].m_Data[3]  + m_Vec[2].m_Data[3]*i_A.m_Vec[3].m_Data[3],    

							m_Vec[3].m_Data[0]*i_A.m_Vec[0].m_Data[0]  + m_Vec[3].m_Data[1]*i_A.m_Vec[1].m_Data[0]  + m_Vec[3].m_Data[2]*i_A.m_Vec[2].m_Data[0]  + m_Vec[3].m_Data[3]*i_A.m_Vec[3].m_Data[0],    // row 1
							m_Vec[3].m_Data[0]*i_A.m_Vec[0].m_Data[1]  + m_Vec[3].m_Data[1]*i_A.m_Vec[1].m_Data[1]  + m_Vec[3].m_Data[2]*i_A.m_Vec[2].m_Data[1]  + m_Vec[3].m_Data[3]*i_A.m_Vec[3].m_Data[1],    
							m_Vec[3].m_Data[0]*i_A.m_Vec[0].m_Data[2]  + m_Vec[3].m_Data[1]*i_A.m_Vec[1].m_Data[2]  + m_Vec[3].m_Data[2]*i_A.m_Vec[2].m_Data[2]  + m_Vec[3].m_Data[3]*i_A.m_Vec[3].m_Data[2],    
							m_Vec[3].m_Data[0]*i_A.m_Vec[0].m_Data[3]  + m_Vec[3].m_Data[1]*i_A.m_Vec[1].m_Data[3]  + m_Vec[3].m_Data[2]*i_A.m_Vec[2].m_Data[3]  + m_Vec[3].m_Data[3]*i_A.m_Vec[3].m_Data[3]
							);
}


//========================================================================
//	operator: Multiply (*=)
//========================================================================
void 
sgpuMatrix::operator *= (const sgpuMatrix& i_A)
{
	//	The old version of this function tried to do the work in place;
	//	however, it did not account for values changing in the middle
	//	of the calculation.  (For instance, computing m_Mat[0] as a new
	//	value, and then using m_Mat[0], expecting it to still be the old
	//	value).  This way is safer, simpler, easier, and also more
	//	oriented towards exception safety.
	*this = (*this) * i_A;
}

