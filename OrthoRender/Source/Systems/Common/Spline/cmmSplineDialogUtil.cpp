/*****************************************************************************
**	cmmSplineDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/Spline/cmmSplineDialogUtil.hpp"

#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "Support/spln/splnCurvePointObject.hpp"
#include "Systems/Common/Spline/mGUI/cmmSplinePointForm.h"


#ifdef _MANAGED
using namespace StudioFramework;
#endif

namespace cmmSplineDialogUtil
{
	namespace
	{
		class cmmPointCallbackObject : public splnCurvePointObject::PointChangedCallback
		{
		public:
			void PointChanged(splnCurvePointObject* i_pCPO)
			{
#ifdef _MANAGED
				cmmSplineDialogUtil::UpdateDialog(i_pCPO);
#endif
			};
		};

		cmmPointCallbackObject* l_pPointCallbackObject = 0;

	}	// end of namespace

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init()
	{
#ifdef _MANAGED
		// Create tab page dialog
		if (!cmmSplinePointForm::FormInstance)
		{
			cmmSplinePointForm::FormInstance = gcnew cmmSplinePointForm();
		}
#endif

		l_pPointCallbackObject = new cmmPointCallbackObject();
	}

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp()
	{
		RemoveDataPage();

#ifdef _MANAGED
		cmmSplinePointForm::FormInstance = nullptr;
#endif

		if (l_pPointCallbackObject != 0)
			delete l_pPointCallbackObject;
	}

	//--------------------------------------------------------------------
	// Update common data tab page
	//--------------------------------------------------------------------
	void UpdateDialog(splnCurvePointObject *i_pObject)
	{
#ifdef _MANAGED
		if (cmmSplinePointForm::FormInstance )
		{
			cmmSplinePointForm::FormInstance->Update(i_pObject);

			i_pObject->AddCallback(l_pPointCallbackObject);
		}
#endif
	}

	//--------------------------------------------------------------------
	//  Add/Remove tab page from selected object dialog
	//--------------------------------------------------------------------
	void  AddDataPage()
	{
#ifdef _MANAGED
		if (!cmmObjectDialogUtil::HasTabPage(cmmSplinePointForm::FormInstance->GetTabPage(0)))
			cmmObjectDialogUtil::AddTabPage( cmmSplinePointForm::FormInstance->GetTabPage(0), true );
#endif
	}
	void  RemoveDataPage()
	{
#ifdef _MANAGED
		if (cmmObjectDialogUtil::HasTabPage(cmmSplinePointForm::FormInstance->GetTabPage(0)))
			cmmObjectDialogUtil::RemoveTabPage(cmmSplinePointForm::FormInstance->GetTabPage(0));
#endif
	}



}	// end of namespace
