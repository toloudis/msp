/*****************************************************************************
**	tmlnTimeInOutMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnTimeInOutMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"

namespace tmlnTimeInOutMgr
{
	//============================================================================
	//	Time In / Out Data
	//============================================================================
	namespace TIOD 
	{
		tmlnTimeInOutDataLists	l_Lists;

		// Callbacks
		std::vector<TimeInOutInterest*> l_Callbacks;
	}


	//------------------------------------------------------------------------
	//	clear all the data in all the lists
	//------------------------------------------------------------------------
	void ClearLists()
	{
		TimeInOutLists::iterator it, end;
		end = TIOD::l_Lists.m_InOutLists.end();
		for (it = TIOD::l_Lists.m_InOutLists.begin(); it != end; ++it)
		{
			it->second.Clear();
		}
		TIOD::l_Lists.m_InOutLists.clear();
	}

	//------------------------------------------------------------------------
	//	find the data list and return true if name matches
	//------------------------------------------------------------------------
	bool GetDataList(const std::string& i_Name,tmlnTimeInOutDataList& o_DataList)
	{
		TimeInOutLists::const_iterator it = TIOD::l_Lists.m_InOutLists.find(i_Name);
		if (it != TIOD::l_Lists.m_InOutLists.end())
		{
			o_DataList = (it->second);
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------
	//	find the DataList
	//------------------------------------------------------------------------
	tmlnTimeInOutDataList& GetDataList(const std::string& i_Name)
	{
		TimeInOutLists::iterator it = TIOD::l_Lists.m_InOutLists.find(i_Name);
		if (it != TIOD::l_Lists.m_InOutLists.end())
		{
			return (it->second);
		}
		else
		{
			return TIOD::l_Lists.m_InOutLists[i_Name];
		}
	}

	//------------------------------------------------------------------------
	//	add in and out time to a list
	//------------------------------------------------------------------------
	void AddTimeInOut(const std::string& i_Name, const maTime& i_fIn, const maTime& i_fOut)
	{
		//TimeInOutLists::iterator it = TIOD::l_Lists.m_InOutLists.find(i_Name);
		//if (it != TIOD::l_Lists.m_InOutLists.end())
		//{
		//	(it->second).AddTimeInOut(i_fIn, i_fOut);
		//}
		//else
		{
			TIOD::l_Lists.m_InOutLists[i_Name].AddTimeInOut(i_fIn, i_fOut);
		}
	}

	//------------------------------------------------------------------------
	//	for the given time calculate the percentage complete for all 
	//	data in the lists.
	//------------------------------------------------------------------------
	float GetPercentageComplete(const maTime& i_CurrentTime)
	{
		float total_time = 0.0f;
		float finished_time = 0.0f;

		TimeInOutLists::iterator it, end;
		end = TIOD::l_Lists.m_InOutLists.end();
		for (it = TIOD::l_Lists.m_InOutLists.begin(); it != end; ++it)
		{
			float time_secs = it->second.GetTotalTime().AsSeconds();
			total_time += time_secs;
			finished_time += time_secs * it->second.GetPercentage(i_CurrentTime);
		}

		float percentage = (finished_time / total_time);
		if (percentage > 1.0f)
		{
			percentage = 1.0f;	// debug ONLY
		}
		return percentage;
	}

	//--------------------------------------------------------------------
	// Call interests and gather in out lists per camera
	//--------------------------------------------------------------------
	void BuildTimeInOutLists()
	{
		std::for_each(TIOD::l_Callbacks.begin(), TIOD::l_Callbacks.end(), 
			std::mem_fun(&TimeInOutInterest::BuildTimeInOutLists));
	}

	//--------------------------------------------------------------------
	//  Add/Remove interests - interest objects are not owned
	//--------------------------------------------------------------------
	void AddInterest(TimeInOutInterest* i_pInterest)
	{
		TIOD::l_Callbacks.push_back(i_pInterest);
	}
	void RemoveInterest(TimeInOutInterest* i_pInterest)
	{
		envSTLHelpers::RemoveOneValue(TIOD::l_Callbacks, i_pInterest);
	}

}	// end of namespace
