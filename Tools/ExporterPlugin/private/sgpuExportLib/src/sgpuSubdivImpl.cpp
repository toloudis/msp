/****************************************************************************\
**  sgpuSubdivImpl.cpp
**
**      sgpuSubdivImpl.hpp defines private implementation
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "sgpuSubdivImpl.hpp"
#include "sgpuException.hpp"



sgpuSubdivImpl::sgpuSubdivImpl( const sgpuSubdivImpl & i_Other ):
m_Subdiv( i_Other.m_Subdiv),
m_FaceIndexCache(i_Other.m_FaceIndexCache),
m_bVertexAnim( i_Other.m_bVertexAnim )
{

}

sgpuSubdivImpl & sgpuSubdivImpl::operator=( const sgpuSubdivImpl &i_Other )
{
	m_Subdiv = i_Other.m_Subdiv;
	m_FaceIndexCache = i_Other.m_FaceIndexCache;
	m_bVertexAnim = i_Other.m_bVertexAnim;
	return *this;
}