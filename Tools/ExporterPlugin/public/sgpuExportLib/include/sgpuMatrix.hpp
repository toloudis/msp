/****************************************************************************\
**  sgpuMatrix.hpp
**
**      sgpuMatrix.hpp defines class for 4x4 transformation matrices.
**
**	To help explain the orientation of rows and columns, translations 
**	are stored in m_Mat[12], m_Mat[13],m_Mat[14] and
**	can be accessed in row index "3".
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_MATRIX_HPP
#define SGPU_MATRIX_HPP

#include "sgpuExportLib.hpp"
#include "sgpuVector.hpp"

//============================================================================
//============================================================================
class SGPUEXPORTLIB_API sgpuMatrix
{
public:

	//========================================================================
	//	default constructor makes identity matrix
	//========================================================================
	sgpuMatrix();

	//========================================================================
	//	constructor
	//========================================================================
	sgpuMatrix( float i_M0,  float i_M1,  float i_M2,  float i_M3,
		float i_M4,  float i_M5,  float i_M6,  float i_M7,
		float i_M8,  float i_M9,  float i_M10, float i_M11,
		float i_M12, float i_M13, float i_M14, float i_M15 );

	sgpuMatrix( const sgpuVector3 &i_V0, 
		const sgpuVector3 & i_v1, 
		const sgpuVector3 & i_V2, 
		const sgpuVector3 & i_V3);

	//========================================================================
	//	Identity()
	//========================================================================
	void Identity();

	//========================================================================
	//	MakeTranslate
	//========================================================================
	void MakeTranslate( float i_Tx, float i_Ty, float i_Tz );

	//========================================================================
	//	MakeRotate makes the matrix into a rotation about the given axis.
	//	The rotation will be CCW from a viewpoint in the direction the axis
	//	vector is pointing.
	//========================================================================
	void MakeRotate( float i_AngleRad, float i_AxisX, float i_AxisY, float i_AxisZ  );

	//========================================================================
	//	MakeScale
	//========================================================================
	void MakeScale( float i_Sx, float i_Sy, float i_Sz );

	//========================================================================
	//	operator: ()()												
	//																
	//		Returns the specified matrix element (Row, Column).
	//========================================================================
	inline float&	operator ()( const int i_Row, const int i_Col );

	//========================================================================
	//	operator: ()()	
	//		Returns the specified matrix element (Row, Column).	
	//																
	//========================================================================
	inline float	operator ()( const int i_Row, const int i_Col ) const;

	//========================================================================
	//	operator: multiplication (*)
	//========================================================================
	sgpuMatrix operator * (const sgpuMatrix& i_A) const;

	//========================================================================
	//	operator: multiplication (*=)
	//========================================================================
	void operator *= (const sgpuMatrix& i_A);
	//========================================================================
	//	operator: equality (==)
	//========================================================================
	bool operator == (const sgpuMatrix& i_A)const;
	//========================================================================
	//	operator: Approximate equal,
	//  precision is specified by SGPU_EPSILON_EQUAL_PRECISION (sgpuExportLib.hpp)
	//========================================================================
	bool EpsilonEqual( const sgpuMatrix& i_A)const;

public:
	sgpuVector3 m_Vec[4];
};
//========================================================================
//	operator: ()()												
//																
//		Returns the specified matrix element (Row, Column).		
//========================================================================
inline float& sgpuMatrix::operator ()( const int i_Row, const int i_Col )
{
	return m_Vec[i_Row].m_Data[i_Col];
}

//========================================================================
//	operator: ()()												
//																
//		Returns the specified matrix element (Row, Column).		
//========================================================================
inline float sgpuMatrix::operator ()( const int i_Row, const int i_Col ) const
{
	return m_Vec[i_Row].m_Data[i_Col];
}


#endif // #ifndef SGPU_MATRIX_HPP
