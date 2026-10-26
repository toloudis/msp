/****************************************************************************\
**  gltfPlacement.cpp
**
**      see gltfPlacement.hpp
**
**	Which MachStudio code each value feeds
**
**	- Forward: glTF cameras and lights look down their local -Z. maRotation
**	  rotates a vector as GetMatrix() does row vectors (RotateVector), the
**	  convention gltfSceneRead's matrix decomposition is tested against.
**	- Spot lights and cameras are aimed with a target point
**	  (prjltData::m_Target, cmraData::m_Target): position + Forward *
**	  c_TargetDistance.
**	- DirectionalOrientation: prjltProjectedLightObject::transformation_changed
**	  turns +Z by prjltData::m_Orientation for a directional light, so the
**	  orientation is the glTF rotation after a half turn about Y (+Z to -Z).
**	- CameraTilt: cmraCameraObject rotates (0,1,0) about the view by
**	  m_Tilt and hands it to LookAt as up, which keeps only its part across
**	  the view. That part, at tilt 0, is world up made perpendicular to the
**	  view; the tilt is the signed angle about the view from it to glTF's
**	  up (local +Y rotated). Looking straight up or down there is no zero-
**	  tilt up to measure from, so the tilt is 0.
**	- CameraHorizontalFov: camCamera::SetFOV takes the horizontal field of
**	  view; glTF gives the vertical one and (optionally) the aspect.
**	- LightRange: MachStudio lights end at their range and have no
**	  "unlimited"; a range of 0 would light nothing. glTF lights without a
**	  range use the inverse-square falloff gltfSceneRead sets up
**	  (intensity / (c_QuadraticFalloff * d^2)), so the range is where that
**	  falls to c_MinVisible of the intensity.
**	- UniformScale: a character's and a transform group's scale is one
**	  float.
**	- WorldTransforms: objects are added at their world pose and then
**	  parented with trfnOperations::AddNodeToTransform, which keeps the
**	  world pose (as its undo and redo do), so the parenting gives back
**	  the glTF's local transforms.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GLTF_PLACEMENT_HPP
#include "ImportExport/gltf/import/private/gltfPlacement.hpp"
#endif

#ifndef MA_CONSTANTS_HPP
#include "Core/ma/maConstants.hpp"
#endif

#include <algorithm>
#include <cmath>

namespace gltfPlacement
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	maVector3d Forward(const maRotation& i_Rotation)
	{
		maVector3d forward(0, 0, -1);
		i_Rotation.RotateVector(forward);
		return forward;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	maRotation DirectionalOrientation(const maRotation& i_Rotation)
	{
		return i_Rotation * maRotation(maVector3d(0, 1, 0), maConstants::c_fPI);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	float CameraTilt(const maRotation& i_Rotation)
	{
		maVector3d view = Forward(i_Rotation);
		view.Normalize();
		const maVector3d world_up(0, 1, 0);
		if (fabsf(view * world_up) > 1 - 1e-4f)
			return 0;

		maVector3d zero_tilt_up = world_up - view * (world_up * view);
		zero_tilt_up.Normalize();
		maVector3d up(0, 1, 0);
		i_Rotation.RotateVector(up);

		const float angle = atan2f(zero_tilt_up.Cross(up) * view, zero_tilt_up * up);
		return angle * maConstants::c_fRadToAngle;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	float CameraHorizontalFov(const gltfCameraDesc& i_Camera, float i_RenderAspect)
	{
		const float aspect = i_Camera.m_Aspect > 0 ? i_Camera.m_Aspect : i_RenderAspect;
		const float half_y = 0.5f * i_Camera.m_YFov * maConstants::c_fAngleToRad;
		return 2 * atanf(tanf(half_y) * aspect) * maConstants::c_fRadToAngle;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	float LightRange(const gltfLightDesc& i_Light)
	{
		if (i_Light.m_Type == gltfLightDesc::e_Directional)
			return 0;
		if (i_Light.m_Range > 0)
			return i_Light.m_Range;
		const float range = sqrtf(i_Light.m_Intensity / (gltfSceneRead::c_QuadraticFalloff * c_MinVisible));
		return std::max(range, c_TargetDistance);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	float UniformScale(const maVector3d& i_Scale)
	{
		return cbrtf(fabsf(i_Scale.m_X) * fabsf(i_Scale.m_Y) * fabsf(i_Scale.m_Z));
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	float ObjectScale(const gltfSceneObject& i_Object)
	{
		const bool is_mesh = i_Object.m_Kind == gltfSceneObject::e_Mesh || i_Object.m_Kind == gltfSceneObject::e_SkinnedMesh;
		if (is_mesh && !i_Object.m_bUniformScale)
			return 1;	// baked into the template
		return UniformScale(i_Object.m_Scale);
	}

	//------------------------------------------------------------------------
	// Row vectors, as MachStudio's: a point goes through the object's scale,
	// rotation and translation, then its parent's.
	//------------------------------------------------------------------------
	std::vector<WorldTransform> WorldTransforms(const std::vector<gltfSceneObject>& i_Objects)
	{
		std::vector<WorldTransform> world(i_Objects.size());
		for (size_t i = 0; i < i_Objects.size(); ++i)
		{
			const gltfSceneObject& object = i_Objects[i];
			WorldTransform local;
			local.m_Translation = object.m_Translation;
			local.m_Rotation = object.m_Rotation;
			local.m_Scale = ObjectScale(object);

			const int parent = object.m_Parent;
			if (parent < 0 || parent >= (int)i)
			{
				world[i] = local;
				continue;
			}
			const WorldTransform& p = world[parent];
			maVector3d offset = local.m_Translation * p.m_Scale;
			p.m_Rotation.RotateVector(offset);
			world[i].m_Translation = p.m_Translation + offset;
			world[i].m_Rotation = p.m_Rotation * local.m_Rotation;
			world[i].m_Scale = p.m_Scale * local.m_Scale;
		}
		return world;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const gltfSceneObject* FindMeshObject(const std::vector<gltfSceneObject>& i_Objects, const std::string& i_Identity)
	{
		for (const gltfSceneObject& object : i_Objects)
		{
			if ((object.m_Kind == gltfSceneObject::e_Mesh || object.m_Kind == gltfSceneObject::e_SkinnedMesh) &&
				object.m_Identity == i_Identity)
				return &object;
		}
		return NULL;
	}
}
