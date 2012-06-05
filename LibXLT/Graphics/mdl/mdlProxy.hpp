/****************************************************************************\
**	mdlProxy.hpp
**
**		Contains structures for passing around proxy for referencing nodes
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_PROXY_HPP
#error mdlProxy.hpp multiply included
#endif
#define MDL_PROXY_HPP


//============================================================================
//============================================================================
class mdlProxy
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	mdlProxy(const std::string & instanceRef)
	:	m_InstanceRef( instanceRef ){}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	virtual ~mdlProxy(){}
	
public:
	std::string m_InstanceRef;
};

