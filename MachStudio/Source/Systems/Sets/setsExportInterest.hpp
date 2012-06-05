/****************************************************************************\
**	setsExportInterest.hpp
**
**		A Export Interest is registered by a system that has data
**	to be exported to Maya ascii format.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef SETS_EXPORTINTEREST_HPP
#error setsExportInterest.hpp multiply included
#endif
#define SETS_EXPORTINTEREST_HPP

#ifndef MEXP_EXPORTINTEREST_HPP
#include "Support/mexp/mexpExportInterest.hpp"
#endif

//============================================================================
//	Forward References
//============================================================================


//============================================================================
//============================================================================
class setsExportInterest : public mexpExportInterest
{
	public:
		//--------------------------------------------------------------------
		//  returns chunk description for display purposes
		//--------------------------------------------------------------------
		virtual const char* GetChunkDesc() const;

		//--------------------------------------------------------------------
		// Gather names of items that could potentially be exported.
		//--------------------------------------------------------------------
		virtual void GatherItemNames(std::vector<std::string> &o_Items) const;

		//--------------------------------------------------------------------
		//	Export only the items in the given list.
		//	The strings will be some subset of what was returned from the
		//	call to GatherItemNames.
		//--------------------------------------------------------------------
		virtual void Export( mexpExporter& i_Exporter,
							 const std::vector<std::string> &i_Items);
};
