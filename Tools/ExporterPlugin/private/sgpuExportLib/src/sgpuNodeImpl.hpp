/****************************************************************************\
**  sgpuNodeImpl.hpp
**
**      sgpuNodeImpl.hpp defines private implementation for sgpuNode.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_NODEIMPL_HPP
#define SGPU_NODEIMPL_HPP

#ifndef MDL_NODEINFO_HPP
#include "Graphics/mdl/mdlNodeInfo.hpp"
#endif 

//============================================================================
//============================================================================
struct sgpuNodeImpl
{
	shared_ptr<mdlNodeInfo> m_Node;
	bool operator ==( const sgpuNodeImpl &other )const
	{
		return m_Node == other.m_Node;
	}
	static bool GetPathToChildRec( const sgpuNodeImpl *i_pImpl, const shared_ptr< mdlNodeInfo > &i_CurNode,  std::vector< shared_ptr< mdlNodeInfo > > &o_Path );
};

#endif // #ifndef SGPU_NODEIMPL_HPP
