/****************************************************************************\
**	propReportMemInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Props/Object/propReportMemInterest.hpp"

#include "Systems/Props/Object/propObjectMgr.hpp"

#include "Core/gf/gfFileTxt.hpp"

//--------------------------------------------------------------------
//	Export only the items in the given list.
//	The strings will be some subset of what was returned from the
//	call to GatherItemNames.
//--------------------------------------------------------------------
void propReportMemInterest::Report( gfFileTxt& i_File )
{
	const int num_objects = propObjectMgr::GetNumObjects();
	for (int i=0; i<num_objects; ++i)
	{
		i_File.WriteLine("PROP: ");
		propScriptObject* pScriptObject = propObjectMgr::GetObject(i);
		i_File.WriteLine(pScriptObject->GetName().GetString());
		i_File.WriteLine("\r\n");

		fgmtScriptObject* pFgmtObject = dynamic_cast<fgmtScriptObject*>(pScriptObject);
		DBG_ASSERT0(pFgmtObject != NULL, "prop is not a fragment object");
		pFgmtObject->ReportMemory(i_File);
		
		mtrlScriptObject* pMtrlObject = dynamic_cast<mtrlScriptObject*>(pScriptObject);
		DBG_ASSERT0(pMtrlObject != NULL, "prop is not a material object");
		pMtrlObject->ReportMemory(i_File);
	}
}
