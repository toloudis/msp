//
//		xxx system object
//
#ifdef XXX_OBJECT_HPP
#error xxxObject.hpp multiply included
#endif
#define XXX_OBJECT_HPP

#ifndef XXX_BASEDATA_HPP
#include "xxxBaseData.hpp"
#endif


//============================================================================
//============================================================================
class xxxObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	xxxObject()
	{
	}

public:
	xxxBaseData m_basedata;
};

