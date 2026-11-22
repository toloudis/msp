/****************************************************************************\
**  gltfSceneObjects.hpp
**
**      The objects a glTF scene's node tree becomes (groups, meshes,
**	lights, cameras), each with its identity (its glTF name path), its
**	transform in centimetres, and its light or camera values in
**	MachStudio's units; and gltfSceneFile::ReadObjects, which reads them
**	from a file. gltfSceneRead.cpp has the rules. No tinygltf here, so
**	MachStudio code can use it.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GLTF_SCENEOBJECTS_HPP
#error gltfSceneObjects.hpp multiply included
#endif
#define GLTF_SCENEOBJECTS_HPP

#ifndef MA_VECTOR3D_HPP
#include "Core/ma/maVector3d.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif

#include <string>
#include <vector>

class fsLocator;

//----------------------------------------------------------------------------
//	A KHR_lights_punctual light in MachStudio's units
//----------------------------------------------------------------------------
struct gltfLightDesc
{
	enum Type { e_Point, e_Spot, e_Directional };
	Type m_Type = e_Point;
	float m_Color[3] = { 1, 1, 1 };
	float m_Intensity = 1;			// MachStudio units (gltfSceneRead.cpp has the conversion)
	maPoint3d m_Falloff;			// MachStudio's (constant, linear, quadratic): (0, 0, c_QuadraticFalloff) for
									// point and spot, (1, 0, 0) for directional
	float m_Range = 0;				// centimetres; 0 = no limit
	float m_Angle = 0;				// spot: full inner cone, degrees (prjlt m_Angle)
	float m_Penumbra = 0;			// spot: outer minus inner, full cone, degrees (prjlt m_Penumbra)
};

//----------------------------------------------------------------------------
//	A glTF camera in MachStudio's units
//----------------------------------------------------------------------------
struct gltfCameraDesc
{
	bool m_bOrthographic = false;
	float m_YFov = 0;				// perspective: vertical field of view, degrees
	float m_Aspect = 0;				// 0 when the file doesn't say (the importer uses the render aspect)
	float m_Near = 0, m_Far = 0;	// centimetres; far 0 = infinite (the importer picks)
	float m_XMag = 0, m_YMag = 0;	// orthographic half-extents, centimetres
};

//----------------------------------------------------------------------------
//	A glTF animation that plays on an object (gltfAnimRead.cpp has the rule)
//----------------------------------------------------------------------------
struct gltfClipRef
{
	std::string m_Identity;		// the animation's identity in the file
	std::string m_Name;			// display name
	float m_Begin = 0;			// seconds: where it plays on the object's timeline
};

//----------------------------------------------------------------------------
//	Keys of one property of one object, from its animations, on its
//	timeline (gltfAnimTracks.cpp has the rule)
//----------------------------------------------------------------------------
struct gltfTrack
{
	enum Property
	{
		e_Translation, e_Rotation, e_Scale,			// the node's local TRS: centimetres; x y z w; three scales
		e_LightColor, e_LightIntensity, e_LightRange,	// RGB; as the file; centimetres
		e_SpotInnerCone, e_SpotOuterCone,			// half-angles, radians
		e_CameraYFov, e_CameraXMag, e_CameraYMag, e_CameraNear, e_CameraFar,	// radians; centimetres
		e_MaterialParam,							// m_Param of material m_Material
		e_MaterialNormalScale						// material m_Material's normal map scale
	};
	Property m_Property = e_Translation;
	std::string m_Material;			// gltfMaterialImport::GetMaterialKey
	std::string m_Param;			// an effPbrData parameter name
	int m_Components = 1;
	bool m_bStep = false;			// each key holds until the next; else linear (slerp for rotations)
	std::vector<float> m_Times;		// seconds from time 0, ascending
	std::vector<float> m_Values;	// m_Components per time
};

//----------------------------------------------------------------------------
//	One object a glTF node becomes
//----------------------------------------------------------------------------
struct gltfSceneObject
{
	enum Kind { e_Group, e_Mesh, e_SkinnedMesh, e_Light, e_Camera };
	// e_SkinnedMesh: placed at its skeleton root's parent with an identity
	// transform (its node's own transform is ignored, as glTF does), when
	// its skeleton can be read (gltfSceneRead.cpp has the rule)
	Kind m_Kind = e_Group;
	std::string m_Identity;		// the glTF name path (gltfSceneRead.cpp has the rule)
	std::string m_Name;			// node name; if empty, the mesh/light/camera's name; else "Node_<index>"
	int m_Node = -1;			// glTF node index
	int m_Parent = -1;			// index into the returned vector (a group), -1 at the top
	maVector3d m_Translation;	// local to the parent, centimetres
	maRotation m_Rotation;
	maVector3d m_Scale = maVector3d(1, 1, 1);
	bool m_bUniformScale = true;	// all three within 1e-4 of each other
	int m_Mesh = -1, m_Skin = -1, m_Light = -1, m_Camera = -1;	// glTF indices, -1 if not this kind
	gltfLightDesc m_LightDesc;	// e_Light only
	gltfCameraDesc m_CameraDesc;	// e_Camera only
	std::vector<gltfClipRef> m_Clips;	// e_Mesh, e_SkinnedMesh: the animations that play on it, in file order
	std::vector<int> m_Materials;		// e_Mesh, e_SkinnedMesh: the glTF materials its mesh's primitives use,
										// ascending, no repeats
	std::vector<gltfTrack> m_Tracks;	// its keys (gltfAnimTracks::Decide)
	bool m_bNodeTransform = true;		// it carries its node's transform: false for a skinned mesh and for
										// content under its own node's group (gltfAnimTracks::Decide)
	bool m_bTransformClip = false;		// e_Mesh: its transform animation plays in its clips; it sits at its
										// parent with an identity transform (gltfAnimTracks.cpp)
};

namespace gltfSceneRead
{
	// MachStudio's quadratic falloff for glTF's inverse-square lights
	const float c_QuadraticFalloff = 1e-4f;
}

namespace gltfSceneFile
{
	// The objects of the file's default scene, parents before children
	// (the parsed file and its derived objects are cached); false when the
	// file doesn't parse. The caller receives its own editable copy.
	bool ReadObjects(const fsLocator& i_File, std::vector<gltfSceneObject>& o_Objects);
}
