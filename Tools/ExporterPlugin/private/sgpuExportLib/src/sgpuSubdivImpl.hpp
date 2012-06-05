/****************************************************************************\
**  sgpuSubdivImpl.hpp
**
**      sgpuSubdivImpl.hpp defines private implementation for sgpuSubdiv.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_SUBDIVIMPL_HPP
#define SGPU_SUBDIVIMPL_HPP

#ifndef MDL_SUBDIVINFO_HPP
#include "Graphics/mdl/mdlSubdivInfo.hpp"
#endif 
#include <map>

//============================================================================
//============================================================================
struct sgpuSubdivImpl
{
	shared_ptr<mdlSubdivInfo> m_Subdiv;
	std::map< int, int> m_FaceIndexCache;
	bool			m_bVertexAnim;
	sgpuSubdivImpl():
		m_bVertexAnim(false)
		{};
	sgpuSubdivImpl( const sgpuSubdivImpl & i_Other );
	sgpuSubdivImpl &operator=( const sgpuSubdivImpl &i_Other );
	bool operator ==( const sgpuSubdivImpl &other ) const
	{
		return m_Subdiv == other.m_Subdiv && m_bVertexAnim == other.m_bVertexAnim;
	}
};

#endif // #ifndef SGPU_SUBDIVIMPL_HPP
