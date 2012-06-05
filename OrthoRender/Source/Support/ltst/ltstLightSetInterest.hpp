/*****************************************************************************
**  ltstLightSetInterest.hpp
**
**	Callback for when LightSet data changes
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef LTST_LIGHTSETINTEREST_HPP
#error ltstLightSetInterest.hpp multiply included
#endif
#define LTST_LIGHTSETINTEREST_HPP


//============================================================================
//============================================================================
class ltstLightSetInterest
{
	public:
		//--------------------------------------------------------------------
		//	LightObjectAdded - a light or object has been 
		//		added or removed to the system
		//--------------------------------------------------------------------
		virtual void LightObjectAdded() = 0;

		//--------------------------------------------------------------------
		//	LightRenamed - a light has been renamed
		//--------------------------------------------------------------------
		virtual void LightRenamed() = 0;

		//--------------------------------------------------------------------
		//	DataChanged - lights or objects have been added/removed from 
		//		light sets
		//--------------------------------------------------------------------
		virtual void DataChanged() = 0;
};
