/*****************************************************************************
**  imptSceneBuild.hpp
**
**      imptSceneBuild makes the data of the object an iscnObjectDesc becomes
**	(imptSceneAppend adds it; imptSceneRefs rebuilds an existing object
**	from it), and puts the desc's keys and clips on the object.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef IMPT_SCENEBUILD_HPP
#error imptSceneBuild.hpp multiply included
#endif
#define IMPT_SCENEBUILD_HPP

#ifndef ISCN_SCENEDESC_HPP
#include "ImportExport/iscn/iscnSceneDesc.hpp"
#endif
#ifndef ISCN_SCENEIMPORT_HPP
#include "ImportExport/iscn/iscnSceneImport.hpp"
#endif
#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif

class chtrScriptData;
class cmraScriptData;
class fsLocator;
class nameString;
class prjltScriptData;
class ptltScriptData;
class trfnScriptData;

//============================================================================
//============================================================================
namespace imptSceneBuild
{
	//------------------------------------------------------------------------
	// The system each kind of object belongs to
	//------------------------------------------------------------------------
	enum System { e_Character, e_Transform, e_PointLight, e_ProjectedLight, e_Camera };
	System SystemFor(iscnObjectDesc::Kind i_Kind);

	//------------------------------------------------------------------------
	// The object's data as AppendFile adds it: its world pose, no drivers
	//------------------------------------------------------------------------
	trfnScriptData GroupData(const iscnObjectDesc& i_Object, const nameString& i_Name, const fsLocator& i_File);
	chtrScriptData MeshData(const iscnObjectDesc& i_Object, const nameString& i_Name, const fsLocator& i_File);
	ptltScriptData PointLightData(const iscnObjectDesc& i_Object, const nameString& i_Name, const fsLocator& i_File);
	prjltScriptData ProjectedLightData(const iscnObjectDesc& i_Object, const nameString& i_Name, const fsLocator& i_File);
	cmraScriptData CameraData(const iscnObjectDesc& i_Object, const nameString& i_Name, const fsLocator& i_File);

	//------------------------------------------------------------------------
	// The settings of the open scene that reading a scene file needs
	//------------------------------------------------------------------------
	iscnSceneSettings SceneSettings();

	//------------------------------------------------------------------------
	// The object's world matrix as the file places it (no pivot)
	//------------------------------------------------------------------------
	maMatrix4x4 WorldMatrix(const iscnObjectDesc& i_Object);

	//------------------------------------------------------------------------
	// The desc's keys (on the object's channels, warning for any it lacks)
	// and, for a mesh, its clips (Anim-Full drivers)
	//------------------------------------------------------------------------
	void AddKeysAndClips(const iscnObjectDesc& i_Object, const nameString& i_Name, const fsLocator& i_File);

	//------------------------------------------------------------------------
	// Rebuild the existing object i_Name from the desc, in i_pParent's space
	// (the desc of its parent in the file; NULL: the top level), as
	// undoable steps: it keeps its name, editor visibility, connections
	// (light sets, layers, groups) and, for a group, its children; then
	// its keys and clips. False if i_Name isn't an object of the desc's
	// system.
	//------------------------------------------------------------------------
	bool Rebuild(const nameString& i_Name, const iscnObjectDesc& i_Object,
				 const iscnObjectDesc* i_pParent, const fsLocator& i_File);
}
