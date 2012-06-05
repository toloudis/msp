//
//		prop system object
//
#ifdef PROP_OBJECT_HPP
#error propObject.hpp multiply included
#endif
#define PROP_OBJECT_HPP

#ifndef PROP_BASEDATA_HPP
#include "propBaseData.hpp"
#endif

#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif


//============================================================================
//============================================================================
class propObject : public prtyObject
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		propObject();

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void RegisterProperties();

	public:
		propBaseData m_basedata;
};

