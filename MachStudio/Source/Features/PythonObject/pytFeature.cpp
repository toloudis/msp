/*****************************************************************************
**  pytFeature.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Features/PythonObject/pytFeature.hpp"

#include "Features/PythonObject/pytDialogInterest.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
namespace pytFeature
{
	pytDialogInterest*		l_pPythonObjectDI = 0;

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		// register the dialog interest
		if( l_pPythonObjectDI == 0 )
		{
			l_pPythonObjectDI = new pytDialogInterest();
			cmmDialogInterestMgr::RegisterInterest( l_pPythonObjectDI );
		}
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//	Unregister the Dialog Interest
		if ( l_pPythonObjectDI != 0 )
		{
			cmmDialogInterestMgr::UnRegisterInterest( l_pPythonObjectDI );
			delete l_pPythonObjectDI;
			l_pPythonObjectDI = 0;
		}
	}

}