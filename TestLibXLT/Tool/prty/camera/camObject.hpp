//
//		cam system object
//
#ifdef CAM_OBJECT_HPP
#error camObject.hpp multiply included
#endif
#define CAM_OBJECT_HPP

#ifndef CAM_BASEDATA_HPP
#include "camBaseData.hpp"
#endif

#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif


//============================================================================
//============================================================================
class camObject : public prtyObject
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		camObject();

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void RegisterProperties();

	public:
		camBaseData m_basedata;
};

