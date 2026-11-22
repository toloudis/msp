/****************************************************************************\
**  gltfPlacement.hpp
**
**      gltfPlacement computes the values MachStudio's objects need for a
**	glTF object that glTF doesn't give directly: look directions and
**	targets, a directional light's orientation, a camera's tilt and
**	horizontal field of view, a light range for "no limit", and one
**	scale for a non-uniform one. gltfPlacement.cpp says which MachStudio
**	code each value feeds. No app or GPU needed.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GLTF_PLACEMENT_HPP
#error gltfPlacement.hpp multiply included
#endif
#define GLTF_PLACEMENT_HPP

#ifndef GLTF_SCENEOBJECTS_HPP
#include "ImportExport/gltf/import/private/gltfSceneObjects.hpp"
#endif

#include <string>
#include <vector>

namespace gltfPlacement
{
	// How far in front of a spot light or camera its look-at target goes,
	// centimetres (only the direction matters)
	const float c_TargetDistance = 100;

	// The fraction of a light's intensity at which LightRange ends it
	const float c_MinVisible = 1.0f / 256;

	// glTF's view and light direction: local -Z rotated by i_Rotation
	maVector3d Forward(const maRotation& i_Rotation);

	// A directional light's prjltData::m_Orientation: it turns +Z to Forward(i_Rotation)
	maRotation DirectionalOrientation(const maRotation& i_Rotation);

	// cmraData::m_Tilt, degrees: the camera's roll about its view, so its up
	// is glTF's (local +Y rotated); 0 looking straight up or down
	float CameraTilt(const maRotation& i_Rotation);

	// cmraData::m_FOV, degrees (horizontal): from the vertical field of view
	// and the file's aspect, else i_RenderAspect
	float CameraHorizontalFov(const gltfCameraDesc& i_Camera, float i_RenderAspect);

	// A light's range, centimetres: the file's; else where its inverse-square
	// falloff drops to c_MinVisible of its intensity (at least
	// c_TargetDistance); directional: 0 (unused)
	float LightRange(const gltfLightDesc& i_Light);

	// One scale for three: their absolute values' geometric mean
	float UniformScale(const maVector3d& i_Scale);

	// An object's pose in the world, with the one scale MachStudio's object
	// carries: a group's UniformScale; a mesh's uniform scale, or 1 when its
	// non-uniform scale is baked into its template
	struct WorldTransform
	{
		maVector3d m_Translation;
		maRotation m_Rotation;
		float m_Scale = 1;
	};

	// Each object's WorldTransform (same order): its local transform under
	// its parents' (objects come parents first)
	std::vector<WorldTransform> WorldTransforms(const std::vector<gltfSceneObject>& i_Objects);

	// The scale MachStudio's object carries for this object, local (as above)
	float ObjectScale(const gltfSceneObject& i_Object);

	// The e_Mesh or e_SkinnedMesh object with that identity, else NULL
	const gltfSceneObject* FindMeshObject(const std::vector<gltfSceneObject>& i_Objects, const std::string& i_Identity);
}
