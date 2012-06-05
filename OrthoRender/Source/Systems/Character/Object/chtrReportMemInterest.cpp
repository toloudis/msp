/****************************************************************************\
**	chtrReportMemInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/Object/chtrReportMemInterest.hpp"

#include "Systems/Character/Object/chtrObjectMgr.hpp"

#include "Core/gf/gfFileTxt.hpp"

//--------------------------------------------------------------------
//	Export only the items in the given list.
//	The strings will be some subset of what was returned from the
//	call to GatherItemNames.
//--------------------------------------------------------------------
void chtrReportMemInterest::Report( gfFileTxt& i_File )
{
	const int num_objects = chtrObjectMgr::GetNumObjects();
	for (int i=0; i<num_objects; ++i)
	{
		i_File.WriteLine("CHARACTER: ");
		chtrScriptObject* pScriptObject = chtrObjectMgr::GetObject(i);
		i_File.WriteLine(pScriptObject->GetName().GetString());
		i_File.WriteLine("\r\n");

		fgmtScriptObject* pFgmtObject = dynamic_cast<fgmtScriptObject*>(pScriptObject);
		DBG_ASSERT0(pFgmtObject != NULL, "character not a fragment object");
		pFgmtObject->ReportMemory(i_File);
		
		mtrlScriptObject* pMtrlObject = dynamic_cast<mtrlScriptObject*>(pScriptObject);
		DBG_ASSERT0(pMtrlObject != NULL, "character not a material object");
		pMtrlObject->ReportMemory(i_File);
	}
}
