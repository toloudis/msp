/****************************************************************************\
**  gltfSceneImport.cpp
**
**      see gltfSceneImport.hpp
**
**	Which glTF object becomes which MachStudio object
**	(gltfSceneRead.cpp says which objects a node tree becomes)
**
**	- e_Group: a group, with one scale: the geometric mean of a
**	  non-uniform one (gltfPlacement::UniformScale), with a warning naming
**	  the node.
**	- e_Mesh: a mesh whose model is only that node's mesh: its identity is
**	  the part of the file the model template loads
**	  (gltfImport::LoadGeometryPart). A non-uniform scale is baked into the
**	  template; the object's scale is then its parents'.
**	- e_SkinnedMesh: a mesh too; its template holds the mesh and its
**	  skeleton (gltfImport::LoadGeometryPart), and the object sits at the
**	  skeleton root's parent (gltfSceneRead.cpp).
**	- Meshes carry their clips (gltfSceneObject::m_Clips: the animations
**	  that play on them, gltfAnimRead.cpp), each loaded by its identity
**	  for the mesh's part (gltfImport::LoadAnimKeysPart).
**	- Lights: point, spot or directional by type. Colour, intensity,
**	  falloff and cone come from gltfLightDesc; the range is
**	  gltfPlacement::LightRange (MachStudio has no unlimited range); spot
**	  lights aim at a target along glTF's -Z, directional lights take
**	  gltfPlacement::DirectionalOrientation.
**	- e_Camera: aimed along glTF's -Z, its roll as the tilt
**	  (gltfPlacement::CameraTilt), its horizontal field of view from the
**	  vertical one and the file's aspect, else the render output's
**	  (gltfPlacement::CameraHorizontalFov); orthographic width 2 * xmag;
**	  far 0 (MachStudio's default) when the file has none.
**	- Poses are world poses (gltfPlacement::WorldTransforms): MachStudio
**	  adds each object there and parents it keeping its world pose, which
**	  gives back the glTF's local transform.
**	- Clips keep their begins (gltfClipRef::m_Begin).
**
**	Keys (gltfSceneObject::m_Tracks, gltfAnimTracks.cpp) in MachStudio's terms
**
**	- Keys are written after parenting, so positions, targets and
**	  orientations are in the parent's space, as the glTF's local values
**	  are; except a projected light's orientation and a camera's tilt,
**	  which MachStudio applies in world space: they come from the node's
**	  world rotation (its ancestors' rotation keys, else rest rotations,
**	  times its own), keyed where any of those rotations has a key.
**	- Groups and meshes: translation -> Position, rotation ->
**	  Orientation, scale -> Scale (gltfPlacement::UniformScale of each).
**	- Point and spot lights: translation -> Position. Spot lights and
**	  cameras: Target = position + Forward(rotation) *
**	  gltfPlacement::c_TargetDistance where the translation or rotation
**	  has a key (the other sampled, or its rest value). Directional:
**	  Orientation = DirectionalOrientation(world rotation). Cameras: Tilt =
**	  CameraTilt(world rotation).
**	- Light colour (alpha 1), intensity (as the static value), range
**	  (gltfPlacement::LightRange for 0); spot cones as the static values
**	  are made (Angle = 2 inner, Penumbra = 2 (outer - inner), degrees),
**	  keyed where either cone has a key.
**	- Cameras: yfov -> FOV (CameraHorizontalFov); xmag -> OrthoWidth (2
**	  xmag); ymag is skipped (MachStudio's orthographic camera has a width
**	  only; one warning); near and far as they are.
**	- Material parameters and normal map scale as they are, colours with
**	  alpha 1 when the file gives three components.
**	- Derived keys (targets, world orientations, tilt, cones) step only
**	  when all their sources step, and are reduced (gltfAnimTracks::Reduce).
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GLTF_SCENEIMPORT_HPP
#include "ImportExport/gltf/import/private/gltfSceneImport.hpp"
#endif
#ifndef GLTF_PLACEMENT_HPP
#include "ImportExport/gltf/import/private/gltfPlacement.hpp"
#endif
#ifndef GLTF_ANIMTRACKS_HPP
#include "ImportExport/gltf/import/private/gltfAnimTracks.hpp"
#endif
#ifndef GLTF_SDKMANAGER_HPP
#include "ImportExport/gltf/import/private/gltfSdkManager.hpp"
#endif
#ifndef ISCN_SCENEDESC_HPP
#include "ImportExport/iscn/iscnSceneDesc.hpp"
#endif

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/Fs/fsFileUtil.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef MA_CONSTANTS_HPP
#include "Core/ma/maConstants.hpp"
#endif

#include <algorithm>
#include <cmath>
#include <functional>

namespace
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	maPoint3d target_of(const gltfPlacement::WorldTransform& i_Pose)
	{
		return i_Pose.m_Translation + gltfPlacement::Forward(i_Pose.m_Rotation) * gltfPlacement::c_TargetDistance;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	iscnLightDesc convert_light(const gltfLightDesc& i_Light, const gltfPlacement::WorldTransform& i_Pose)
	{
		iscnLightDesc light;
		light.m_Color = maFloatRGBA(i_Light.m_Color[0], i_Light.m_Color[1], i_Light.m_Color[2], 1.0f);
		light.m_Intensity = i_Light.m_Intensity;
		light.m_Falloff = i_Light.m_Falloff;
		light.m_Range = gltfPlacement::LightRange(i_Light);
		if (i_Light.m_Type == gltfLightDesc::e_Spot)
		{
			light.m_Angle = i_Light.m_Angle;
			light.m_Penumbra = i_Light.m_Penumbra;
			light.m_Target = target_of(i_Pose);
		}
		return light;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	iscnCameraDesc convert_camera(const gltfCameraDesc& i_Camera, const gltfPlacement::WorldTransform& i_Pose,
								  float i_RenderAspect)
	{
		iscnCameraDesc camera;
		camera.m_Target = target_of(i_Pose);
		camera.m_Tilt = gltfPlacement::CameraTilt(i_Pose.m_Rotation);
		camera.m_bOrthographic = i_Camera.m_bOrthographic;
		if (i_Camera.m_bOrthographic)
			camera.m_OrthoWidth = 2 * i_Camera.m_XMag;
		else
			camera.m_FOV = gltfPlacement::CameraHorizontalFov(i_Camera, i_RenderAspect);
		camera.m_Near = i_Camera.m_Near;
		camera.m_Far = i_Camera.m_Far;
		return camera;
	}

	//--------------------------------------------------------------------
	// Keys
	//--------------------------------------------------------------------
	const float c_SameTime = 1e-6f;		// seconds

	const gltfTrack* find_track(const gltfSceneObject& i_Object, gltfTrack::Property i_Property)
	{
		for (const gltfTrack& track : i_Object.m_Tracks)
		{
			if (track.m_Property == i_Property)
				return &track;
		}
		return NULL;
	}

	iscnKeyTrack keys_of(const gltfTrack& i_Track, iscnKeyTrack::Channel i_Channel)
	{
		iscnKeyTrack keys;
		keys.m_Channel = i_Channel;
		keys.m_Material = i_Track.m_Material;
		keys.m_Param = i_Track.m_Param;
		keys.m_bStep = i_Track.m_bStep;
		keys.m_Times = i_Track.m_Times;
		keys.m_Values = i_Track.m_Values;
		return keys;
	}

	// each key's value through i_Convert, i_Components in, i_Out out
	iscnKeyTrack converted(const gltfTrack& i_Track, iscnKeyTrack::Channel i_Channel, int i_Out,
						   const std::function<void (const float*, float*)>& i_Convert)
	{
		iscnKeyTrack keys = keys_of(i_Track, i_Channel);
		keys.m_Values.clear();
		std::vector<float> out(i_Out);
		for (size_t k = 0; k < i_Track.m_Times.size(); k++)
		{
			i_Convert(&i_Track.m_Values[k * i_Track.m_Components], &out[0]);
			keys.m_Values.insert(keys.m_Values.end(), out.begin(), out.end());
		}
		return keys;
	}

	// RGB, or RGBA, as RGBA
	iscnKeyTrack colours(const gltfTrack& i_Track, iscnKeyTrack::Channel i_Channel)
	{
		const int n = i_Track.m_Components;
		return converted(i_Track, i_Channel, 4, [n](const float* i_pIn, float* o_pOut)
		{
			for (int c = 0; c < 4; c++)
				o_pOut[c] = c < n ? i_pIn[c] : 1.0f;
		});
	}

	// the times of all the tracks, ascending, once each; step when all are
	std::vector<float> union_times(const std::vector<const gltfTrack*>& i_Tracks, bool& o_bStep)
	{
		std::vector<float> times;
		o_bStep = true;
		for (const gltfTrack* pTrack : i_Tracks)
		{
			times.insert(times.end(), pTrack->m_Times.begin(), pTrack->m_Times.end());
			o_bStep &= pTrack->m_bStep;
		}
		std::sort(times.begin(), times.end());
		times.erase(std::unique(times.begin(), times.end(), [](float a, float b) { return b - a <= c_SameTime; }),
					times.end());
		return times;
	}

	// a derived track: i_Value at every time of i_Sources, reduced as i_Property
	iscnKeyTrack derived(const std::vector<const gltfTrack*>& i_Sources, gltfTrack::Property i_Property, int i_Components,
						 iscnKeyTrack::Channel i_Channel, const std::function<void (float, float*)>& i_Value)
	{
		gltfTrack track;
		track.m_Property = i_Property;
		track.m_Components = i_Components;
		track.m_Times = union_times(i_Sources, track.m_bStep);
		std::vector<float> value(i_Components);
		for (float t : track.m_Times)
		{
			i_Value(t, &value[0]);
			track.m_Values.insert(track.m_Values.end(), value.begin(), value.end());
		}
		gltfAnimTracks::Reduce(track);
		return keys_of(track, i_Channel);
	}

	// a track's value at i_Time, else i_Rest
	void sample(const gltfTrack* i_pTrack, float i_Time, const float* i_pRest, float* o_pValue, int i_Components)
	{
		if (i_pTrack)
			gltfAnimTracks::Sample(*i_pTrack, i_Time, o_pValue);
		else
			std::copy(i_pRest, i_pRest + i_Components, o_pValue);
	}

	maVector3d translation_at(const gltfSceneObject& i_Object, float i_Time)
	{
		const float rest[3] = { i_Object.m_Translation.m_X, i_Object.m_Translation.m_Y, i_Object.m_Translation.m_Z };
		float v[3];
		sample(find_track(i_Object, gltfTrack::e_Translation), i_Time, rest, v, 3);
		return maVector3d(v[0], v[1], v[2]);
	}

	maRotation rotation_at(const gltfSceneObject& i_Object, float i_Time)
	{
		const maRotation& r = i_Object.m_Rotation;
		const float rest[4] = { r.GetX(), r.GetY(), r.GetZ(), r.GetW() };
		float q[4];
		sample(find_track(i_Object, gltfTrack::e_Rotation), i_Time, rest, q, 4);
		maRotation rotation(q[0], q[1], q[2], q[3]);
		rotation.Normalize();
		return rotation;
	}

	// the object's world rotation at i_Time, and the rotation tracks it depends on
	maRotation world_rotation_at(const std::vector<gltfSceneObject>& i_Objects, int i_Index, float i_Time)
	{
		const gltfSceneObject& object = i_Objects[i_Index];
		const maRotation local = rotation_at(object, i_Time);
		if (object.m_Parent < 0 || object.m_Parent >= i_Index)
			return local;
		return world_rotation_at(i_Objects, object.m_Parent, i_Time) * local;
	}

	std::vector<const gltfTrack*> chain_rotation_tracks(const std::vector<gltfSceneObject>& i_Objects, int i_Index)
	{
		std::vector<const gltfTrack*> tracks;
		for (int i = i_Index; i >= 0 && i < (int)i_Objects.size(); )
		{
			if (const gltfTrack* pTrack = find_track(i_Objects[i], gltfTrack::e_Rotation))
				tracks.push_back(pTrack);
			const int parent = i_Objects[i].m_Parent;
			i = (parent < i) ? parent : -1;
		}
		return tracks;
	}

	// keys for a spot light's or camera's target, where its translation or rotation has keys
	void add_target_keys(const gltfSceneObject& i_Object, std::vector<iscnKeyTrack>& io_Keys)
	{
		std::vector<const gltfTrack*> sources;
		if (const gltfTrack* pTrack = find_track(i_Object, gltfTrack::e_Translation))
			sources.push_back(pTrack);
		if (const gltfTrack* pTrack = find_track(i_Object, gltfTrack::e_Rotation))
			sources.push_back(pTrack);
		if (sources.empty())
			return;
		io_Keys.push_back(derived(sources, gltfTrack::e_Translation, 3, iscnKeyTrack::e_Target,
								  [&](float i_Time, float* o_pValue)
		{
			const maVector3d target = translation_at(i_Object, i_Time)
									+ gltfPlacement::Forward(rotation_at(i_Object, i_Time)) * gltfPlacement::c_TargetDistance;
			o_pValue[0] = target.m_X;
			o_pValue[1] = target.m_Y;
			o_pValue[2] = target.m_Z;
		}));
	}

	//--------------------------------------------------------------------
	// the object's keys in MachStudio's terms
	//--------------------------------------------------------------------
	std::vector<iscnKeyTrack> convert_keys(const std::vector<gltfSceneObject>& i_Objects, int i_Index,
										   const iscnSceneSettings& i_Settings)
	{
		const gltfSceneObject& object = i_Objects[i_Index];
		std::vector<iscnKeyTrack> keys;
		const bool bLight = object.m_Kind == gltfSceneObject::e_Light;
		const bool bCamera = object.m_Kind == gltfSceneObject::e_Camera;
		const gltfLightDesc::Type light_type = object.m_LightDesc.m_Type;

		// the transform
		if (const gltfTrack* pTrack = find_track(object, gltfTrack::e_Translation))
		{
			if (!(bLight && light_type == gltfLightDesc::e_Directional))
				keys.push_back(keys_of(*pTrack, iscnKeyTrack::e_Position));
		}
		if (!bLight && !bCamera)
		{
			if (const gltfTrack* pTrack = find_track(object, gltfTrack::e_Rotation))
				keys.push_back(keys_of(*pTrack, iscnKeyTrack::e_Orientation));
			if (const gltfTrack* pTrack = find_track(object, gltfTrack::e_Scale))
			{
				keys.push_back(converted(*pTrack, iscnKeyTrack::e_Scale, 1, [](const float* i_pIn, float* o_pOut)
				{
					o_pOut[0] = gltfPlacement::UniformScale(maVector3d(i_pIn[0], i_pIn[1], i_pIn[2]));
				}));
			}
		}
		if ((bLight && light_type == gltfLightDesc::e_Spot) || bCamera)
			add_target_keys(object, keys);

		// world rotations: a directional light's orientation, a camera's tilt
		const bool bDirectional = bLight && light_type == gltfLightDesc::e_Directional;
		if (bDirectional || bCamera)
		{
			const std::vector<const gltfTrack*> rotations = chain_rotation_tracks(i_Objects, i_Index);
			if (!rotations.empty() && bDirectional)
			{
				keys.push_back(derived(rotations, gltfTrack::e_Rotation, 4, iscnKeyTrack::e_Orientation,
									   [&](float i_Time, float* o_pValue)
				{
					const maRotation r = gltfPlacement::DirectionalOrientation(world_rotation_at(i_Objects, i_Index, i_Time));
					o_pValue[0] = r.GetX();
					o_pValue[1] = r.GetY();
					o_pValue[2] = r.GetZ();
					o_pValue[3] = r.GetW();
				}));
			}
			if (!rotations.empty() && bCamera)
			{
				// (reduced as a plain value: any property but translation, rotation and scale)
				keys.push_back(derived(rotations, gltfTrack::e_CameraYFov, 1, iscnKeyTrack::e_Tilt,
									   [&](float i_Time, float* o_pValue)
				{
					o_pValue[0] = gltfPlacement::CameraTilt(world_rotation_at(i_Objects, i_Index, i_Time));
				}));
			}
		}

		// light values
		if (bLight)
		{
			if (const gltfTrack* pTrack = find_track(object, gltfTrack::e_LightColor))
				keys.push_back(colours(*pTrack, iscnKeyTrack::e_Color));
			if (const gltfTrack* pTrack = find_track(object, gltfTrack::e_LightIntensity))
				keys.push_back(keys_of(*pTrack, iscnKeyTrack::e_Intensity));
			const gltfTrack* pRange = find_track(object, gltfTrack::e_LightRange);
			if (pRange && light_type != gltfLightDesc::e_Directional)
			{
				const gltfLightDesc light = object.m_LightDesc;
				keys.push_back(converted(*pRange, iscnKeyTrack::e_Range, 1, [light](const float* i_pIn, float* o_pOut)
				{
					gltfLightDesc keyed = light;
					keyed.m_Range = i_pIn[0];
					o_pOut[0] = gltfPlacement::LightRange(keyed);
				}));
			}
			std::vector<const gltfTrack*> cones;
			const gltfTrack* pInner = find_track(object, gltfTrack::e_SpotInnerCone);
			const gltfTrack* pOuter = find_track(object, gltfTrack::e_SpotOuterCone);
			if (pInner)
				cones.push_back(pInner);
			if (pOuter)
				cones.push_back(pOuter);
			if (!cones.empty() && light_type == gltfLightDesc::e_Spot)
			{
				// the static half-angles, radians
				const float rest_inner = object.m_LightDesc.m_Angle / 2 * maConstants::c_fAngleToRad;
				const float rest_outer = (object.m_LightDesc.m_Angle + object.m_LightDesc.m_Penumbra) / 2 * maConstants::c_fAngleToRad;
				const auto half_angles = [=](float i_Time, float& o_Inner, float& o_Outer)
				{
					sample(pInner, i_Time, &rest_inner, &o_Inner, 1);
					sample(pOuter, i_Time, &rest_outer, &o_Outer, 1);
				};
				keys.push_back(derived(cones, gltfTrack::e_SpotInnerCone, 1, iscnKeyTrack::e_Angle, [&](float i_Time, float* o_pValue)
				{
					float inner, outer;
					half_angles(i_Time, inner, outer);
					o_pValue[0] = 2 * inner * maConstants::c_fRadToAngle;
				}));
				keys.push_back(derived(cones, gltfTrack::e_SpotOuterCone, 1, iscnKeyTrack::e_Penumbra, [&](float i_Time, float* o_pValue)
				{
					float inner, outer;
					half_angles(i_Time, inner, outer);
					o_pValue[0] = 2 * (outer - inner) * maConstants::c_fRadToAngle;
				}));
			}
		}

		// camera values
		if (bCamera)
		{
			if (const gltfTrack* pTrack = find_track(object, gltfTrack::e_CameraYFov))
			{
				const gltfCameraDesc camera = object.m_CameraDesc;
				const float aspect = i_Settings.m_RenderAspect;
				keys.push_back(converted(*pTrack, iscnKeyTrack::e_FOV, 1, [camera, aspect](const float* i_pIn, float* o_pOut)
				{
					gltfCameraDesc keyed = camera;
					keyed.m_YFov = i_pIn[0] * maConstants::c_fRadToAngle;
					o_pOut[0] = gltfPlacement::CameraHorizontalFov(keyed, aspect);
				}));
			}
			if (const gltfTrack* pTrack = find_track(object, gltfTrack::e_CameraXMag))
			{
				keys.push_back(converted(*pTrack, iscnKeyTrack::e_OrthoWidth, 1, [](const float* i_pIn, float* o_pOut)
				{
					o_pOut[0] = 2 * i_pIn[0];
				}));
			}
			if (find_track(object, gltfTrack::e_CameraYMag))
				DBG_WARNING("glTF camera " << object.m_Identity << ": an animated ymag is skipped (MachStudio's orthographic camera has a width only)");
			if (const gltfTrack* pTrack = find_track(object, gltfTrack::e_CameraNear))
				keys.push_back(keys_of(*pTrack, iscnKeyTrack::e_Near));
			if (const gltfTrack* pTrack = find_track(object, gltfTrack::e_CameraFar))
				keys.push_back(keys_of(*pTrack, iscnKeyTrack::e_Far));
		}

		// materials
		for (const gltfTrack& track : object.m_Tracks)
		{
			if (track.m_Property == gltfTrack::e_MaterialParam)
				keys.push_back(track.m_Components == 3 ? colours(track, iscnKeyTrack::e_MaterialParam)
													   : keys_of(track, iscnKeyTrack::e_MaterialParam));
			else if (track.m_Property == gltfTrack::e_MaterialNormalScale)
				keys.push_back(keys_of(track, iscnKeyTrack::e_MaterialNormalScale));
		}
		return keys;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool gltfSceneImport::IsRecognized(const fsLocator& i_File) const
{
	std::string filename;
	fsFileUtil::LocatorToANSIFilename(i_File, filename);
	return gltfSdkManager::IsRecognizedFileFormat(filename.c_str());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
std::string gltfSceneImport::GetFilePatterns() const
{
	return "*.gltf;*.glb";
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool gltfSceneImport::Read(const fsLocator& i_File, const iscnSceneSettings& i_Settings, iscnSceneDesc& o_Scene)
{
	std::vector<gltfSceneObject> objects;
	if (!gltfSceneFile::ReadObjects(i_File, objects))
		return false;
	std::string filename;
	fsFileUtil::LocatorToANSIFilename(i_File, filename);
	Convert(objects, filename, i_Settings, o_Scene);
	return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void gltfSceneImport::Convert(const std::vector<gltfSceneObject>& i_Objects, const std::string& i_FileName,
							  const iscnSceneSettings& i_Settings, iscnSceneDesc& o_Scene)
{
	const std::vector<gltfPlacement::WorldTransform> poses = gltfPlacement::WorldTransforms(i_Objects);

	o_Scene.m_Objects.clear();
	o_Scene.m_Objects.reserve(i_Objects.size());
	for (size_t i = 0; i < i_Objects.size(); ++i)
	{
		const gltfSceneObject& object = i_Objects[i];
		const gltfPlacement::WorldTransform& pose = poses[i];

		iscnObjectDesc desc;
		desc.m_Name = object.m_Name;
		desc.m_Identity = object.m_Identity;
		desc.m_Parent = object.m_Parent;
		desc.m_Position = pose.m_Translation;
		desc.m_Orientation = pose.m_Rotation;
		desc.m_Scale = pose.m_Scale;
		switch (object.m_Kind)
		{
			case gltfSceneObject::e_Group:
				desc.m_Kind = iscnObjectDesc::e_Group;
				if (!object.m_bUniformScale)
					DBG_WARNING("glTF node " << object.m_Identity << " has a non-uniform scale; its group uses their mean");
				break;
			case gltfSceneObject::e_SkinnedMesh:
			case gltfSceneObject::e_Mesh:
				desc.m_Kind = iscnObjectDesc::e_Mesh;
				for (const gltfClipRef& clip : object.m_Clips)
				{
					iscnClipDesc clip_desc;
					clip_desc.m_Identity = clip.m_Identity;
					clip_desc.m_Name = clip.m_Name;
					clip_desc.m_Begin = clip.m_Begin;
					desc.m_Clips.push_back(clip_desc);
				}
				break;
			case gltfSceneObject::e_Light:
				desc.m_Light = convert_light(object.m_LightDesc, pose);
				if (object.m_LightDesc.m_Type == gltfLightDesc::e_Point)
					desc.m_Kind = iscnObjectDesc::e_PointLight;
				else if (object.m_LightDesc.m_Type == gltfLightDesc::e_Spot)
					desc.m_Kind = iscnObjectDesc::e_SpotLight;
				else
				{
					desc.m_Kind = iscnObjectDesc::e_DirectionalLight;
					desc.m_Orientation = gltfPlacement::DirectionalOrientation(pose.m_Rotation);
				}
				break;
			case gltfSceneObject::e_Camera:
				desc.m_Kind = iscnObjectDesc::e_Camera;
				desc.m_Camera = convert_camera(object.m_CameraDesc, pose, i_Settings.m_RenderAspect);
				break;
		}
		desc.m_Keys = convert_keys(i_Objects, (int)i, i_Settings);
		o_Scene.m_Objects.push_back(desc);
	}
}
