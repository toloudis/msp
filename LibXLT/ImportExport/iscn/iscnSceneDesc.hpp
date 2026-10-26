/****************************************************************************\
**  iscnSceneDesc.hpp
**
**      The objects a scene file brings into MachStudio, whatever its
**	format: groups, meshes, lights and cameras, each with its world pose
**	and its values already in MachStudio's units and terms (centimetres,
**	degrees, MachStudio's light falloff, a look-at target). A format's
**	iscnSceneImportImpl fills it; the application makes the objects.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef ISCN_SCENEDESC_HPP
#error iscnSceneDesc.hpp multiply included
#endif
#define ISCN_SCENEDESC_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif

#include <string>
#include <vector>

//----------------------------------------------------------------------------
//	A light: a point light, or a projected light of spot or directional type
//----------------------------------------------------------------------------
struct iscnLightDesc
{
	maFloatRGBA m_Color = maFloatRGBA(1, 1, 1, 1);
	float m_Intensity = 1;
	maPoint3d m_Falloff = maPoint3d(1, 0, 0);	// (constant, linear, quadratic)
	float m_Range = 0;				// centimetres; point and spot (directional: unused)
	float m_Angle = 0;				// spot: full inner cone, degrees
	float m_Penumbra = 0;			// spot: outer minus inner, full cone, degrees
	maPoint3d m_Target;				// spot: the point it aims at
};

//----------------------------------------------------------------------------
//	A camera
//----------------------------------------------------------------------------
struct iscnCameraDesc
{
	maPoint3d m_Target;				// the point it looks at
	float m_Tilt = 0;				// roll about the view, degrees
	bool m_bOrthographic = false;
	float m_FOV = 0;				// perspective: horizontal field of view, degrees
	float m_OrthoWidth = 0;			// orthographic: width, centimetres
	float m_Near = 0;				// centimetres
	float m_Far = 0;				// centimetres; 0: MachStudio's default
};

//----------------------------------------------------------------------------
//	An animation of the file that plays on an object
//----------------------------------------------------------------------------
struct iscnClipDesc
{
	std::string m_Identity;		// its identity in the file (entImport::LoadAnimKeys' clip)
	std::string m_Name;			// display name
};

//----------------------------------------------------------------------------
//	One object
//----------------------------------------------------------------------------
struct iscnObjectDesc
{
	enum Kind { e_Group, e_Mesh, e_PointLight, e_SpotLight, e_DirectionalLight, e_Camera };
	Kind m_Kind = e_Group;
	std::string m_Name;			// the name it wants (the application makes it unique)
	std::string m_Identity;		// its identity in the file; for e_Mesh also the part of the file
								// its model is (entImport::LoadGeometry)
	int m_Parent = -1;			// index of its e_Group, -1 at the top; parents come first
	maPoint3d m_Position;		// world, centimetres
	maRotation m_Orientation;	// world; a directional light's orientation turns +Z to its direction
	float m_Scale = 1;			// world, one scale (groups and meshes)
	iscnLightDesc m_Light;		// lights only
	iscnCameraDesc m_Camera;	// e_Camera only
	std::vector<iscnClipDesc> m_Clips;	// e_Mesh only: the animations of the file that play on it,
										// in file order, each an Anim-Full clip
};

//----------------------------------------------------------------------------
//	What a scene file contains
//----------------------------------------------------------------------------
struct iscnSceneDesc
{
	std::vector<iscnObjectDesc> m_Objects;
};
