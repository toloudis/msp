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
**	- e_SkinnedMesh: until skins are imported, an ordinary mesh in the bind
**	  pose (one warning per file).
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
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GLTF_SCENEIMPORT_HPP
#include "ImportExport/gltf/import/private/gltfSceneImport.hpp"
#endif
#ifndef GLTF_PLACEMENT_HPP
#include "ImportExport/gltf/import/private/gltfPlacement.hpp"
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
	bool warned_skin = false;
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
				if (!warned_skin)
				{
					DBG_WARNING("glTF file " << i_FileName << ": skins come in a later version; skinned meshes are in their bind pose");
					warned_skin = true;
				}
				desc.m_Kind = iscnObjectDesc::e_Mesh;
				break;
			case gltfSceneObject::e_Mesh:
				desc.m_Kind = iscnObjectDesc::e_Mesh;
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
		o_Scene.m_Objects.push_back(desc);
	}
}
