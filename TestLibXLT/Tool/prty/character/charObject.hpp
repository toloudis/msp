//
//		char system object
//
#ifdef CHAR_OBJECT_HPP
#error charObject.hpp multiply included
#endif
#define CHAR_OBJECT_HPP

#ifndef CHAR_BASEDATA_HPP
#include "charBaseData.hpp"
#endif

#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif


//============================================================================
//============================================================================
class charObject : public prtyObject
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		charObject();

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void RegisterProperties();

	public:
		charBaseData m_basedata;
};

