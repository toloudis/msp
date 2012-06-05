/*****************************************************************************
**	tmlnTimeInOutMgr.hpp
**
**	A TimeInOutMgr is a list of lists of in and out times for a name
**	(usually a camera name)
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_TIMEINOUTMGR_HPP
#error tmlnTimeInOutMgr.hpp multiply included
#endif
#define TMLN_TIMEINOUTMGR_HPP

#ifndef TMLN_TIMEINOUTDATA_HPP
#include "Support/tmln/tmlnTimeInOutData.hpp"
#endif

#include <string>


//============================================================================
//	forward references
//============================================================================


//============================================================================
//============================================================================
namespace tmlnTimeInOutMgr
{
	//------------------------------------------------------------------------
	//	clear all the data in all the lists
	//------------------------------------------------------------------------
	void ClearLists();

	//------------------------------------------------------------------------
	//	find the DataList and return true if name matches
	//------------------------------------------------------------------------
	bool GetDataList(const std::string& i_Name, tmlnTimeInOutDataList& o_DataList);

	//------------------------------------------------------------------------
	//	find the DataList
	//------------------------------------------------------------------------
	tmlnTimeInOutDataList& GetDataList(const std::string& i_Name);

	//------------------------------------------------------------------------
	//	add in and out time to a list
	//------------------------------------------------------------------------
	void AddTimeInOut(const std::string& i_Name, float i_fIn, float i_fOut);

	//------------------------------------------------------------------------
	//	for the given time calculate the percentage complete for all 
	//	data in the lists.
	//------------------------------------------------------------------------
	float GetPercentageComplete(float i_CurrentTime);

	//--------------------------------------------------------------------
	// Call interests and gather in out lists per camera
	//--------------------------------------------------------------------
	void BuildTimeInOutLists();

	//--------------------------------------------------------------------
	// Callback for when to gather in/out lists
	//--------------------------------------------------------------------
	class TimeInOutInterest
	{
	public:
		virtual void BuildTimeInOutLists() = 0;
	};

	//--------------------------------------------------------------------
	//  Add/Remove interests - interest objects are not owned
	//--------------------------------------------------------------------
	void AddInterest(TimeInOutInterest* i_pInterest);
	void RemoveInterest(TimeInOutInterest* i_pInterest);
};
