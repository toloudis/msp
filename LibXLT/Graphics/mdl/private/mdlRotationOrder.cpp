/****************************************************************************\
**  mdlRotationOrder.cpp
**
**      mdlRotationOrder supplies data and functions for handling
**	rotation orders and converting to maRotation.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/private/mdlRotationOrder.hpp"

#include "Core/Ma/maRotation.hpp"


//============================================================================
//============================================================================
namespace mdlRotationOrder
{
	//------------------------------------------------------------------------
	// Set the rotation value based on 3 euler angles and a rotation order.
	//------------------------------------------------------------------------
	void SetOrderedEuler(maRotation &o_Rot, 
						 RotationOrder i_Order, 
						 float i_X, float i_Y, float i_Z)
	{
		// safe, but slow way
		maRotation rotx(maVector3d(1,0,0), i_X);
		maRotation roty(maVector3d(0,1,0), i_Y);
		maRotation rotz(maVector3d(0,0,1), i_Z);

		// although the order is XYZ, we have to multiply
		// in the opposite direction because of the organization
		// of our matrices
		switch (i_Order)
		{
			default:
			case e_XYZ:
				o_Rot = (rotz * roty * rotx);
				break;
			case e_YZX:
				o_Rot = (rotx * rotz * roty);
				break;
			case e_ZXY:
				o_Rot = (roty * rotx * rotz);
				break;
			case e_XZY:
				o_Rot = (roty * rotz * rotx);
				break;
			case e_YXZ:
				o_Rot = (rotz * rotx * roty);
				break;
			case e_ZYX:
				o_Rot = (rotx * roty * rotz);
				break;
		}
	}

}	// end of namespace

