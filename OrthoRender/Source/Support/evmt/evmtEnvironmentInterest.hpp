/*****************************************************************************
**  evmtEnvironmentInterest.hpp
**
**	Callback for when Environment data changes
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef EVMT_ENVIRONMENTINTEREST_HPP
#error evmtEnvironmentInterest.hpp multiply included
#endif
#define EVMT_ENVIRONMENTINTEREST_HPP

class nameString;

//============================================================================
//============================================================================
class evmtEnvironmentInterest
{
	public:
		//--------------------------------------------------------------------
		//	ObjectAdded - an object has been 
		//		added to the system
		//--------------------------------------------------------------------
		virtual void ObjectAdded() = 0;

		//--------------------------------------------------------------------
		//	ObjectRemoved - an object has been 
		//		removed from the system
		//--------------------------------------------------------------------
		virtual void ObjectRemoved(const nameString& i_Name) = 0;

		//--------------------------------------------------------------------
		//	ObjectRenamed - an object has been renamed
		//--------------------------------------------------------------------
		virtual void ObjectRenamed() = 0;

		//--------------------------------------------------------------------
		//	DataChanged - objects have been added/removed from 
		//		environments
		//--------------------------------------------------------------------
		virtual void DataChanged() = 0;
};
