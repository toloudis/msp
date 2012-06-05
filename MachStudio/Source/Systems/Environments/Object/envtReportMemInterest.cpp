/****************************************************************************\
**	envtReportMemInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/Object/envtReportMemInterest.hpp"

#include "Systems/Environments/Object/envtObjectMgr.hpp"

#include "Core/gf/gfFileTxt.hpp"

//--------------------------------------------------------------------
//	Export only the items in the given list.
//	The strings will be some subset of what was returned from the
//	call to GatherItemNames.
//--------------------------------------------------------------------
void envtReportMemInterest::Report( gfFileTxt& i_File )
{
	const int num_objects = envtObjectMgr::GetNumObjects();
	for (int i=0; i<num_objects; ++i)
	{
		i_File.WriteLine("ENVIRONMENT: ");
		envtScriptObject* pScriptObject = envtObjectMgr::GetObject(i);
		i_File.WriteLine(pScriptObject->GetName().GetString());
		i_File.WriteLine("\r\n");

		// dump mem usage info here:
		pScriptObject->ReportMemory(i_File);
	}
}
