/*****************************************************************************
**	cmmSplineOperations.hpp
**
**	Operations on spline control spoints
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_SPLINEOPERATIONS_HPP
#error cmmSplineOperations.hpp multiply included
#endif
#define CMM_SPLINEOPERATIONS_HPP

//============================================================================
//============================================================================
namespace cmmSplineOperations
{
	//--------------------------------------------------------------------
	//  Select previous and next control points in selected spline
	//--------------------------------------------------------------------
	void  SelectPreviousControlPoint();
	void  SelectNextControlPoint();

	//--------------------------------------------------------------------
	//  Select all control points in selected spline
	//--------------------------------------------------------------------
	void  SelectAllControlPoints();

	//--------------------------------------------------------------------
	//  Insert control point after selection
	//--------------------------------------------------------------------
	void  InsertControlPoint();

	//--------------------------------------------------------------------
	// Delete selected control points
	//--------------------------------------------------------------------
	void  DeleteControlPoint();

	//--------------------------------------------------------------------
	// Association between editor camera and selected control point
	//--------------------------------------------------------------------
	void  SetEditCamToPoint();
	void  SetPointToEditCam();

}	// end of namespace
