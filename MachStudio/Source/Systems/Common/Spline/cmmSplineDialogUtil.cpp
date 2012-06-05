/*****************************************************************************
**	cmmSplineDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/Spline/cmmSplineDialogUtil.hpp"

#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "Support/spln/splnCurvePointObject.hpp"


namespace cmmSplineDialogUtil
{
	namespace
	{
		class cmmPointCallbackObject : public splnCurvePointObject::PointChangedCallback
		{
		public:
			void PointChanged(splnCurvePointObject* i_pCPO)
			{

			};
		};

		cmmPointCallbackObject* l_pPointCallbackObject = 0;

	}	// end of namespace

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init()
	{

		l_pPointCallbackObject = new cmmPointCallbackObject();
	}

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp()
	{
		RemoveDataPage();

		if (l_pPointCallbackObject != 0)
			delete l_pPointCallbackObject;
	}

	//--------------------------------------------------------------------
	// Update common data tab page
	//--------------------------------------------------------------------
	void UpdateDialog(splnCurvePointObject *i_pObject)
	{

	}

	//--------------------------------------------------------------------
	//  Add/Remove tab page from selected object dialog
	//--------------------------------------------------------------------
	void  AddDataPage()
	{

	}
	void  RemoveDataPage()
	{

	}



}	// end of namespace
