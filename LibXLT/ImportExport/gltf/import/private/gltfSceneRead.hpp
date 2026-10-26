/****************************************************************************\
**  gltfSceneRead.hpp
**
**      gltfSceneRead reads a glTF scene's node tree into the list of
**	MachStudio objects it becomes: groups, meshes, lights and cameras,
**	each with its identity (its glTF name path), its transform in
**	centimetres, and its light or camera values in MachStudio's units.
**	It needs no app or GPU, so it is tested on its own; the importer
**	creates the objects from it.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GLTF_SCENEREAD_HPP
#error gltfSceneRead.hpp multiply included
#endif
#define GLTF_SCENEREAD_HPP

#ifndef MA_VECTOR3D_HPP
#include "Core/ma/maVector3d.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif

#include "tiny_gltf.h"

#include <string>
#include <vector>

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

	// The default scene (else scene 0; none: empty), parents before children
	std::vector<gltfSceneObject> ReadScene(const tinygltf::Model& i_Model);
}
