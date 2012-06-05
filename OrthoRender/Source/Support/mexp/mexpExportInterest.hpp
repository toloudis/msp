/****************************************************************************\
**	mexpExportInterest.hpp
**
**		A Export Interest is registered by a system that has data
**	to be exported to Maya ascii format.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MEXP_EXPORTINTEREST_HPP
#error mexpExportInterest.hpp multiply included
#endif
#define MEXP_EXPORTINTEREST_HPP

#include <string>
#include <vector>

//============================================================================
//	Forward References
//============================================================================
class mexpExporter;

//============================================================================
//============================================================================
class mexpExportInterest
{
	public:
		//--------------------------------------------------------------------
		//  returns chunk description for display purposes
		//--------------------------------------------------------------------
		virtual const char* GetChunkDesc() const = 0;

		//--------------------------------------------------------------------
		// Gather names of items that could potentially be exported.
		//--------------------------------------------------------------------
		virtual void GatherItemNames(std::vector<std::string> &o_Items) const = 0;

		//--------------------------------------------------------------------
		//	Export only the items in the given list.
		//	The strings will be some subset of what was returned from the
		//	call to GatherItemNames.
		//--------------------------------------------------------------------
		virtual void Export( mexpExporter& i_Exporter,
							 const std::vector<std::string> &i_Items) = 0;
};
