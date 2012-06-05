/****************************************************************************\
**  sgpuNodeImpl.cpp
**
**      sgpuNodeImpl.hpp defines private implementation
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "sgpuNodeImpl.hpp"
#include "sgpuException.hpp"


bool sgpuNodeImpl::GetPathToChildRec( const sgpuNodeImpl *i_pDesiredChild, const shared_ptr< mdlNodeInfo > &i_CurNode,  std::vector< shared_ptr< mdlNodeInfo> > &o_Path )
{
	bool bRet = false;
	if( i_pDesiredChild->m_Node == i_CurNode )
	{
		o_Path.push_back( i_CurNode );
		bRet = true;
		return bRet;
	}
	std::vector< shared_ptr< mdlNodeInfo > >::const_iterator cit;
	for( cit = i_CurNode->m_Children.begin(); cit != i_CurNode->m_Children.end(); ++cit )
	{
		bRet = GetPathToChildRec( i_pDesiredChild, *cit, o_Path );			
		if( bRet )
		{
			o_Path.push_back( i_CurNode );
			return bRet;
		}
	}
	return bRet;
}
