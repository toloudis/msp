/****************************************************************************\
**	cmmSplineSelectInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/Spline/cmmSplineSelectInterest.hpp"

#include "Support/spln/splnCurvePointObject.hpp"
#include "Support/spln/splnCurveSelect.hpp"
#include "Systems/Common/Spline/cmmSplineCommands.hpp"
#include "Systems/Common/Spline/cmmSplineDialogUtil.hpp"

#include "Tool/sel3d/sel3dCastUtil.hpp"


//--------------------------------------------------------------------
//	AddedToSelection
//--------------------------------------------------------------------
//virtual 
void cmmSplineSelectInterest::AddedToSelection( sel3dObject* i_pSelObj )
{
	if ( splnCurvePointObject *pPoint = dynamic_cast<splnCurvePointObject*>(i_pSelObj) )
	{
		//	change colors
		//
		pPoint->SetColor(maFloatRGBA(1.0f, 1.0f, 0.0f, 1.0f));

		splnSpline* pCurve = pPoint->GetCurve();
		if (pCurve != 0)
		{
			splnCurveSelect::SetColor(pCurve,maFloatRGBA(0.0f, 1.0f, 0.0f, 1.0f));
		}
	}
}

//--------------------------------------------------------------------
//	RemovedFromSelection
//--------------------------------------------------------------------
//virtual 
void cmmSplineSelectInterest::RemovedFromSelection( sel3dObject* i_pSelObj )
{
	//	change the colors
	//
	if ( splnCurvePointObject *pPoint = dynamic_cast<splnCurvePointObject*>(i_pSelObj) )
	{
		pPoint->SetColor(maFloatRGBA(0.5f, 0.5f, 0.0f, 1.0f));

		splnSpline* pCurve = pPoint->GetCurve();
		if (pCurve != 0)
		{
			splnCurveSelect::SetColor(pCurve,maFloatRGBA(0.0f, 0.3f, 0.0f, 1.0f));
		}
	}
}

//--------------------------------------------------------------------
//	SelectionChanged
//--------------------------------------------------------------------
//virtual 
void cmmSplineSelectInterest::SelectionChanged()
{
	if ( splnCurvePointObject *pPoint = sel3dCastUtil::CastSelectedObject<splnCurvePointObject>() )
	{
		//DBG_LOG( "selected an splnCurvePointObject" );

		//nameString name = pNamed->GetName();
		cmmSplineDialogUtil::UpdateDialog(pPoint);
		cmmSplineDialogUtil::AddDataPage();
		cmmSplineCommands::ShowSplineToolbar(true);
	}
	else
	{
		// safe to call even if not added
		cmmSplineDialogUtil::RemoveDataPage();
		cmmSplineCommands::ShowSplineToolbar(false);
	}
}
