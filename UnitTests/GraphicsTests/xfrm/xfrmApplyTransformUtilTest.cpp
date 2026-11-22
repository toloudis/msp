/*****************************************************************************
**  xfrmApplyTransformUtilTest.cpp
**
**      xfrmApplyTransformUtil::ApplyTransformation (MachStudio's Support
**      library, compiled into this exe as MainAppTests does with the
**      MainApp sources it tests): moving an object into a transform group
**      keeps its world pose. A child's world is local * parent total (row
**      vectors), so the group offset old total * inverse(new total) goes
**      on the right of the object's local matrix.
**
**	Extra Large Technology
\****************************************************************************/
#include "Support/xfrm/xfrmApplyTransformUtil.hpp"

#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_POINT3D_HPP
#include "Core/prty/prtyPoint3d.hpp"
#endif
#ifndef PRTY_ROTATION_HPP
#include "Core/prty/prtyRotation.hpp"
#endif
#ifndef PRTY_VECTOR3D_HPP
#include "Core/prty/prtyVector3d.hpp"
#endif
#ifndef SC_OBJECT_HPP
#include "Graphics/Sc/scObject.hpp"
#endif

#include <gtest/gtest.h>

namespace
{
	maMatrix4x4 Pose(const maPoint3d& i_Position, const maRotation& i_Rotation, float i_Scale)
	{
		maMatrix4x4 m;
		scObject::ComputeFullTransformation(maPoint3d(0, 0, 0), maVector3d(0, 0, 0), i_Position,
											maPoint3d(i_Scale, i_Scale, i_Scale), i_Rotation, m);
		return m;
	}

	void ExpectSame(const maMatrix4x4& i_Expected, const maMatrix4x4& i_Actual)
	{
		for (int i = 0; i < 16; i++)
			EXPECT_NEAR(i_Expected.GetPtr()[i], i_Actual.GetPtr()[i], 1e-3f) << "element " << i;
	}
}

TEST(xfrmApplyTransformUtil, KeepsWorldPoseInRotatedGroup)
{
	// glTF's Z-up node (-90 degrees about X) as the group, at (10, 0, 0), scale 2;
	// the object turned -90 degrees about Z inside it, at (0, 5, 0) in the group
	const maRotation group_rotation(maVector3d(1, 0, 0), -1.5707963f), local_rotation(maVector3d(0, 0, 1), -1.5707963f);
	const maMatrix4x4 group = Pose(maPoint3d(10, 0, 0), group_rotation, 2);
	const maMatrix4x4 local = Pose(maPoint3d(0, 5, 0), local_rotation, 1);
	const maMatrix4x4 world = local * group;

	// the object, unparented, holds its world pose (the local turn, then the group's)
	const maRotation world_rotation = group_rotation * local_rotation;
	prtyPoint3d position("Position", world.GetTranslation());
	prtyRotation orientation("Orientation", world_rotation);
	prtyFloat scale("Scale", 2);
	prtyPoint3d pivot("Pivot", maPoint3d(0, 0, 0));
	prtyVector3d compensation("Pivot Compensation", maVector3d(0, 0, 0));

	// moved into the group: offset = old total (none) * inverse(new total)
	maMatrix4x4 offset = group;
	offset.Invert();
	xfrmApplyTransformUtil::ApplyTransformation(offset, position, orientation, scale, pivot, compensation);

	ExpectSame(local, Pose(position.GetValue(), orientation.GetValue(), scale.GetValue()));
	ExpectSame(world, Pose(position.GetValue(), orientation.GetValue(), scale.GetValue()) * group);
}

TEST(xfrmApplyTransformUtil, KeepsWorldPoseWithLocalScale)
{
	// a Blender armature: the group scales by 0.01, so the object's local scale is 100
	const maRotation group_rotation(maVector3d(1, 0, 0), -1.5707963f), local_rotation(maVector3d(0, 1, 0), 0.7f);
	const maMatrix4x4 group = Pose(maPoint3d(0, 0, 0), group_rotation, 0.01f);
	const maMatrix4x4 local = Pose(maPoint3d(0, 50, 0), local_rotation, 100);
	const maMatrix4x4 world = local * group;

	prtyPoint3d position("Position", world.GetTranslation());
	prtyRotation orientation("Orientation", group_rotation * local_rotation);
	prtyFloat scale("Scale", 1);
	prtyPoint3d pivot("Pivot", maPoint3d(0, 0, 0));
	prtyVector3d compensation("Pivot Compensation", maVector3d(0, 0, 0));

	maMatrix4x4 offset = group;
	offset.Invert();
	xfrmApplyTransformUtil::ApplyTransformation(offset, position, orientation, scale, pivot, compensation);

	EXPECT_NEAR(100, scale.GetValue(), 1e-2f);
	ExpectSame(world, Pose(position.GetValue(), orientation.GetValue(), scale.GetValue()) * group);
}
