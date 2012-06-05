/****************************************************************************\
**	mdlRotationOrder.hpp
**
**		mdlRotationOrder supplies data and functions for handling
**	rotation orders and converting to maRotation.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_ROTATIONORDER_HPP
#error mdlRotationOrder.hpp multiply included
#endif
#define MDL_ROTATIONORDER_HPP


//============================================================================
//============================================================================
class maRotation;


//============================================================================
//============================================================================
namespace mdlRotationOrder
{
	// Designed to match Maya's enumeration order
	enum RotationOrder
	{
		e_XYZ = 0,
		e_YZX = 1,
		e_ZXY = 2,
		e_XZY = 3,
		e_YXZ = 4,
		e_ZYX = 5
	};

	//------------------------------------------------------------------------
	// Set the rotation value based on 3 euler angles and a rotation order.
	//------------------------------------------------------------------------
	void SetOrderedEuler(maRotation &o_Rot, 
						 RotationOrder i_Order, 
						 float i_X, float i_Y, float i_Z);
}
