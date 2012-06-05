/*****************************************************************************
**  xfrmApplyTransformUtil.hpp
**
**      xfrmApplyTransformUtil supplies functions for combining and
**	and decomposing matrices in order to update transformation
**	components when changing the parenting of nodes.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef XFRM_APPLYTRANSFORMUTIL_HPP
#error xfrmApplyTransformUtil.hpp multiply included
#endif
#define XFRM_APPLYTRANSFORMUTIL_HPP

//============================================================================
//============================================================================
class maMatrix4x4;
class prtyPoint3d;
class prtyRotation;
class prtyFloat;
class prtyVector3d;

//============================================================================
//============================================================================
namespace xfrmApplyTransformUtil 
{
	//--------------------------------------------------------------------
	// Applies transformation from i_Matrix to the properties given.
	// Changes are made to the properties in place without undo.
	//--------------------------------------------------------------------
	void ApplyTransformation(const maMatrix4x4& i_Matrix,
							 prtyPoint3d&		io_Position,
							 prtyRotation&		io_Orientation,
							 prtyFloat&			io_Scale,
							 prtyPoint3d&		io_PivotPoint,
							 prtyVector3d&		io_PivotCompensation);

};

