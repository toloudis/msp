/********************************************************************************************\
**  xfrmTransformNodeCallback.hpp
**
**		xfrmTransformNodeCallback provides API functions that will be called
**	from the xfrmTransformMgr. The system objects should derive from this
**	class and implement the virtual functions.
**
**  StudioGPU
**  Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/

#ifdef XFRM_TRANSFORMNODECALLBACK_HPP
#error xfrmTransformNodeCallback.hpp multiply included
#endif
#define XFRM_TRANSFORMNODECALLBACK_HPP

#ifndef MA_MATRIX4X4_HPP
#include "Core/Ma/maMatrix4x4.hpp"
#endif 

//--------------------------------------------------------------------
//--------------------------------------------------------------------


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class xfrmTransformNodeCallback
{
public:
	//--------------------------------------------------------------------
	// Called from transformation manager once per frame, update 
	//	position based on parent transformation matrix.
	//--------------------------------------------------------------------
	virtual void UpdateParentTransform() = 0;

	//--------------------------------------------------------------------
	// Called from transformation manager when the parenting of this 
	// object changes in a way that we need to alter our values to
	// stay in the same world position.
	//--------------------------------------------------------------------
	virtual void ApplyTransformation(const maMatrix4x4& i_Matrix) = 0;
	
	//--------------------------------------------------------------------
	// Return true if this object has a pivot point based on its icon.
	// If so, return pivot point in the o_Pivot argument.
	//--------------------------------------------------------------------
	virtual bool HasIconPivotPoint(maPoint3d& o_Pivot) { return false; }
};
