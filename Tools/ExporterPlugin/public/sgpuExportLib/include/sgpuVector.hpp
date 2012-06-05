/****************************************************************************\
**  sgpuVector.hpp
**
**      sgpuVector.hpp defines class for 4x4 transformation matrices.
**
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_VECTOR_HPP
#define SGPU_VECTOR_HPP

#include "sgpuExportLib.hpp"

class sgpuMatrix;

//============================================================================
//============================================================================
class SGPUEXPORTLIB_API sgpuVector3
{
public:

	//========================================================================
	//	default constructor zero vector
	//========================================================================
	sgpuVector3();

	//========================================================================
	//	constructor
	//========================================================================
	sgpuVector3( float i_V0,  float i_V1,  float i_V2 );

	//========================================================================
	//	operator: ()()												
	//																
	//		Returns the specified vector element ( iElem).		
	//      0 <= i_Elem <= 3
	//========================================================================
	inline float&	operator ()( const int i_Elem  );
	inline float&	operator []( const int i_Elem  );
	//========================================================================
	//	operator: ()()												
	//																
	//		Returns the specified vector element (Row, Column).
	//      0 <= i_Elem <= 3
	//========================================================================
	inline float	operator ()( const int i_Elem ) const;
	inline float	operator []( const int i_Elem ) const;
	//========================================================================
	//	operator: multiplication (*)
	//  dot prduct of vectors
	//========================================================================
	float operator * (const sgpuVector3 & i_A) const;
	//========================================================================
	//	operator: thisVec * i_TM
	//========================================================================
	sgpuVector3 operator * (const sgpuMatrix &i_TM)const;
	//========================================================================
	//	operator: addition (+)
	//========================================================================
	sgpuVector3 operator + (const sgpuVector3 & i_A) const;

	//========================================================================
	//	operator: addition (+=)
	//========================================================================
	void operator += (const sgpuVector3& i_A);
	//========================================================================
	//	operator: substraction (-)
	//========================================================================
	sgpuVector3 operator - (const sgpuVector3 & i_A) const;

	//========================================================================
	//	operator: substraction (-=)
	//========================================================================
	void operator -= (const sgpuVector3& i_A);
	//========================================================================
	//	operator: scaling (*)
	//========================================================================
	sgpuVector3 operator * (const float i_F ) const;

	//========================================================================
	//	operator: scaling (*=)
	//========================================================================
	void operator *= (const float i_F );
	//========================================================================
	//	operator: equality (==)
	//========================================================================
	bool operator == (const sgpuVector3& i_A)const;
	//========================================================================
	//	operator: Approximate Equal, 
	//  precision is specified by SGPU_EPSILON_EQUAL_PRECISION (sgpuExportLib.hpp)
	//========================================================================
	bool EpsilonEqual( const sgpuVector3& i_A)const;
public:
	friend class sgpuMatrix;
public:
	//only the first three floats in the array are used.
	//the last element is a dont care value
	//(ie: code will work whatever the value of the 4th element is)
	float	m_Data[ 4 ];
};


//========================================================================
//	operator: ()()												
//																
//		Returns the specified vector element (Elem).		
//========================================================================
inline float& sgpuVector3::operator ()( const int i_Elem )
{
	assert( i_Elem < 3 );
	return m_Data [ i_Elem ];
}

//========================================================================
//	operator: []()												
//																
//		Returns the specified vector element (Elem).		
//========================================================================
inline float& sgpuVector3::operator []( const int i_Elem )
{
	assert( i_Elem < 3 );
	return m_Data [ i_Elem ];
}
//========================================================================
//	operator: ()()												
//																
//		Returns the specified vector element (Elem).		
//========================================================================
inline float sgpuVector3::operator ()( const int i_Elem ) const
{
	assert( i_Elem < 3 );
	return m_Data [ i_Elem ];
}

//========================================================================
//	operator: []()												
//																
//		Returns the specified vector element (Elem).		
//========================================================================
inline float sgpuVector3::operator []( const int i_Elem ) const
{
	assert( i_Elem < 3 );
	return m_Data [ i_Elem ];
}



#endif // #ifndef SGPU_VECTOR_HPP
