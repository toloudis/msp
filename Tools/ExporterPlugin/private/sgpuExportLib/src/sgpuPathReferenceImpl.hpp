/****************************************************************************\
**  sgpuPathReferenceImpl.hpp
**
**      sgpuPathReferenceImpl.hpp defines private implementation for sgpuPathReference.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_PATHREFERENCEIMPL_HPP
#define SGPU_PATHREFERENCEIMPL_HPP


#ifndef MDL_NODEINFO_HPP
#include "Graphics/mdl/mdlNodeInfo.hpp"
#endif 
//============================================================================
//============================================================================
struct sgpuPathReferenceImpl
{
	shared_ptr< mdlPathReference > m_InstanceInfo;
	bool operator ==( const sgpuPathReferenceImpl &other ) const
	{
		return m_InstanceInfo == other.m_InstanceInfo;
	}
};

#endif // #ifndef SGPU_MESHIMPL_HPP
