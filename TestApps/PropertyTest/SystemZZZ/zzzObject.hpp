//
//		zzz system object
//
#ifdef ZZZ_OBJECT_HPP
#error zzzObject.hpp multiply included
#endif
#define ZZZ_OBJECT_HPP

#ifndef ZZZ_BASEDATA_HPP
#include "zzzBaseData.hpp"
#endif


//============================================================================
//============================================================================
class zzzObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	zzzObject()
	{
	}

public:
	zzzBaseData m_basedata;
};

