/****************************************************************************\
**  sgpuUtilsImpl.cpp
**
**      sgpuUtilsImpl.hpp defines private implementation
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "sgpuUtilsImpl.hpp"
#include "sgpuConstructor.hpp"
#include "sgpuException.hpp"

#include <cmath>
#include <vector>

const char* c_ExportLib_Version = "1.1.0";

PolyFace::PolyFace(int nSides, const sgpuMaterial &i_Mtl):
m_Mtl( i_Mtl ),
m_FaceVertices( new std::vector< sgpuConstructor::FaceVertex > () )
{
	m_FaceVertices->resize( nSides );
}

PolyFace::PolyFace( const PolyFace &i_Other )
:m_Mtl(i_Other.m_Mtl),
m_FaceVertices(i_Other.m_FaceVertices)
{

}

PolyFace & PolyFace::operator=( const PolyFace& i_Other )
{
	m_Mtl = i_Other.m_Mtl;
	m_FaceVertices = i_Other.m_FaceVertices;
	return *this;
}


bool fEpsilonEqual( float a, float b )
{
	return fabs(b - a) < SGPU_EPSILON_EQUAL_PRECISION ;
}
