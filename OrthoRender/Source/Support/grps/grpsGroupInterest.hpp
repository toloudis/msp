/*****************************************************************************
**  grpsGroupInterest.hpp
**
**	Callback for when Group data changes
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef GRPS_GROUPINTEREST_HPP
#error grpsGroupInterest.hpp multiply included
#endif
#define GRPS_GROUPINTEREST_HPP


//============================================================================
//============================================================================
class grpsGroupInterest
{
	public:
		//--------------------------------------------------------------------
		//	DataChanged - objects have been added/removed from groups or a 
		//	new group has been created/deleted
		//--------------------------------------------------------------------
		virtual void DataChanged() = 0;
};
