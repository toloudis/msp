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
//	One object a glTF node becomes
//----------------------------------------------------------------------------
struct gltfSceneObject
{
	enum Kind { e_Group, e_Mesh, e_SkinnedMesh, e_Light, e_Camera };
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
};

namespace gltfSceneRead
{
	// MachStudio's quadratic falloff for glTF's inverse-square lights
	const float c_QuadraticFalloff = 1e-4f;
}

namespace gltfSceneFile
{
	// The objects of the file's default scene, parents before children
	// (gltfSceneRead::ReadScene of the parsed file, which is cached); false
	// when the file doesn't parse.
	bool ReadObjects(const fsLocator& i_File, std::vector<gltfSceneObject>& o_Objects);
}
