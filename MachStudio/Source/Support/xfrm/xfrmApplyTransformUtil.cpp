/*****************************************************************************
**  xfrmApplyTransformUtil.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support/xfrm/xfrmApplyTransformUtil.hpp"

#include "Core/Ma/maMatrix4x4.hpp"
#include "Core/prty/prtyFloat.hpp"
#include "Core/prty/prtyPoint3d.hpp"
#include "Core/prty/prtyRotation.hpp"
#include "Core/prty/prtyVector3d.hpp"
#include "Graphics/Sc/scObject.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace
{
	//--------------------------------------------------------------------
	// Decompose affine transformation into components assuming
	// uniform scaling.
	//--------------------------------------------------------------------
	void decompose_matrix(const maMatrix4x4& i_Matrix,
						  maPoint3d &o_Position,
						  maRotation &o_Rotation,
						  float &o_Scale)
	{
		o_Position.Set(i_Matrix.m_Mat[12], i_Matrix.m_Mat[13], i_Matrix.m_Mat[14]);

		//maMatrix3x3 minor = i_Matrix.GetSubMatrix(3,3);
		//o_Scale = minor.GetDeterminant();
		const float c_SqrtThree = 1.0f / ::sqrtf(3);
		maVector3d scale_vec(c_SqrtThree, c_SqrtThree, c_SqrtThree); // unit length
		i_Matrix.TransformDir(scale_vec);
		o_Scale = scale_vec.Length();

		// the rotation from the unscaled matrix: maRotation::SetValue
		// expects a pure rotation
		maMatrix4x4 rotation_matx(i_Matrix);
		if (o_Scale > 0)
		{
			for (int r = 0; r < 3; r++)
				for (int c = 0; c < 3; c++)
					rotation_matx(r, c) /= o_Scale;
		}
		o_Rotation.SetValue(rotation_matx);
		o_Rotation.Normalize();
	}
}

//--------------------------------------------------------------------
// Applies transformation from i_Matrix to the properties given.
// Changes are made to the properties in place without undo.
//--------------------------------------------------------------------
void xfrmApplyTransformUtil::ApplyTransformation(const maMatrix4x4& i_Matrix,
												 prtyPoint3d&		io_Position,
												 prtyRotation&		io_Orientation,
												 prtyFloat&			io_Scale,
												 prtyPoint3d&		io_PivotPoint,
												 prtyVector3d&		io_PivotCompensation)
{
	// Combine the current local matrix and the new matrix 
	// and then decompose that matrix into components
	maMatrix4x4 local_matx;
	maPoint3d scale_vec(io_Scale.GetValue(),io_Scale.GetValue(),io_Scale.GetValue());
	scObject::ComputeFullTransformation(io_PivotPoint.GetValue(),
										io_PivotCompensation.GetValue(),
										io_Position.GetValue(),
										scale_vec,
										io_Orientation.GetValue(),
										local_matx);
	// Row vectors: world = local * parent total, so the offset (old total *
	// inverse new total) goes on the right
	local_matx *= i_Matrix;

	maPoint3d position;
	maRotation rotation;
	float scale = 1;

	decompose_matrix(local_matx, position, rotation, scale);

	// TODO: need to manipulate pivot so that its stays still in world space
	io_PivotCompensation.SetValue(maPoint3d(0,0,0));

	io_Position.SetValue(position);
	io_Orientation.SetValue(rotation);
	io_Scale.SetValue(scale);
}
