/****************************************************************************\
**	prjltReportMemInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/PrjLt/Object/prjltReportMemInterest.hpp"

#include "Systems/PrjLt/Object/prjltObjectMgr.hpp"

#include "Core/gf/gfFileTxt.hpp"

//--------------------------------------------------------------------
//	Export only the items in the given list.
//	The strings will be some subset of what was returned from the
//	call to GatherItemNames.
//--------------------------------------------------------------------
void prjltReportMemInterest::Report( gfFileTxt& i_File )
{
	const int num_objects = prjltObjectMgr::GetNumObjects();
	for (int i=0; i<num_objects; ++i)
	{
		i_File.WriteLine("PROJECTED LIGHT: ");
		prjltScriptObject* pScriptObject = prjltObjectMgr::GetObject(i);
		i_File.WriteLine(pScriptObject->GetName().GetString());
		i_File.WriteLine("\r\n");

		// dump mem usage info here:
		pScriptObject->ReportMemory(i_File);
	}
}
