/*****************************************************************************
**  imptSceneAppend.cpp
**
**		see .hpp
**
**	Which object each iscnObjectDesc becomes (the format's importer has
**	already put its values in MachStudio's terms, iscnSceneDesc.hpp)
**
**	- e_Group: a transform group (Systems/Transforms).
**	- e_Mesh: a character (Systems/Character) whose model is the part of
**	  the file named by the object's identity (chtrData::m_SourceIdentity;
**	  templates are loaded and shared per file and part). Each of its
**	  clips becomes an Anim-Full driver on it that plays that clip of the
**	  file (chtrOperations::AddAnimationClip), beginning where the clip
**	  says (iscnClipDesc::m_Begin; the format's importer lays them out).
**	- e_PointLight: a point light (Systems/PtLt). e_SpotLight,
**	  e_DirectionalLight: a projected light (Systems/PrjLt) of that type,
**	  with the defaults prjltOperations::AddObject(type) gives.
**	- e_Camera: a camera (Systems/Cameras).
**	- Every object is added at its world pose and then put into its group
**	  with trfnOperations::AddNodeToTransform, which keeps the world pose
**	  (so do its undo and redo), giving back the file's local transforms.
**	- The data each object starts from, its keys and its clips are made by
**	  imptSceneBuild (which imptSceneRefs also rebuilds objects with).
**	  Once added, each object is remembered as a reference to the file
**	  (imptSceneRefs::Remember) and the file is stamped.
**	- Clips and keys (iscnObjectDesc::m_Keys) are added after the parenting (which
**	  moves property values, not drivers), so their values are in the
**	  parent's space as they come: each track on the channel of that
**	  name on the object (the property's name in its system's data:
**	  Position, Orientation, Scale; a light's Color, Intensity, Range,
**	  Target Point, Angle, Penumbra; a camera's Target, Tilt, Field of
**	  View, Ortho Width, Near Clip, Far Clip), material tracks on the
**	  character's material channels (mtrlScriptObject), through
**	  cmmKeyUtil::AddKeys. A track whose channel isn't there is skipped
**	  with a warning.
**	- Names are the objects' names, made unique with "_<n>" when an object
**	  of that name exists. Each object remembers the file and its identity
**	  in it (m_SourceFile / m_SourceIdentity).
**	- The whole file (or the chosen part of it) is one undo step.
**	- A partial append (the Import dialog) adds the chosen objects and
**	  their ancestor groups; the rest of the file is skipped.
**
**	Extra Large Technology
\****************************************************************************/
#include "Features/Import/imptSceneAppend.hpp"

#include "Features/Import/imptSceneBuild.hpp"
#include "Features/Import/imptSceneRefs.hpp"
#include "Systems/Cameras/Undo/cmraOperations.hpp"
#include "Systems/Cameras/Data/cmraScriptData.hpp"
#include "Systems/Character/Undo/chtrOperations.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Systems/PrjLt/Data/prjltScriptData.hpp"
#include "Systems/PrjLt/Undo/prjltOperations.hpp"
#include "Systems/PtLt/Data/ptltScriptData.hpp"
#include "Systems/PtLt/Undo/ptltOperations.hpp"
#include "Systems/Transforms/Data/trfnScriptData.hpp"
#include "Systems/Transforms/Undo/trfnOperations.hpp"

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef IT_STRINGUTIL_HPP
#include "Core/it/itStringUtil.hpp"
#endif
#ifndef NAME_MGR_HPP
#include "Core/name/nameMgr.hpp"
#endif
#ifndef UNDO_UNDOMGR_HPP
#include "Core/undo/undoUndoMgr.hpp"
#endif
#ifndef ISCN_SCENECHOICE_HPP
#include "ImportExport/iscn/iscnSceneChoice.hpp"
#endif
#ifndef ISCN_SCENEDESC_HPP
#include "ImportExport/iscn/iscnSceneDesc.hpp"
#endif
#ifndef ISCN_SCENEIMPORT_HPP
#include "ImportExport/iscn/iscnSceneImport.hpp"
#endif
#ifndef GUI_MESSAGEBOX_HPP
#include "Tool/gui/guiMessageBox.hpp"
#endif
#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/Mat/matTextureMgr.hpp"
#endif

#include <algorithm>
#include <functional>
#include <string>
#include <vector>


//============================================================================
//============================================================================
namespace imptSceneAppend
{
	namespace
	{
		//--------------------------------------------------------------------
		// one undo block, ended even when an add throws (the caller,
		// chtrOperations::BrowseLoadObjects, catches out of memory)
		//--------------------------------------------------------------------
		struct UndoBlock
		{
			UndoBlock(const std::string& i_Name) { undoUndoMgr::BeginMultipleOperationBlock(i_Name.c_str()); }
			~UndoBlock() { undoUndoMgr::EndMultipleOperationBlock(); }
		};

		//--------------------------------------------------------------------
		// the name, or with "_<n>" if an object has it already
		//--------------------------------------------------------------------
		nameString unique_name(const std::string& i_Name)
		{
			if (!nameMgr::GetObjectByName(nameString(i_Name)))
				return nameString(i_Name);
			for (int n = 1; ; ++n)
			{
				const std::string name = i_Name + "_" + std::to_string(n);
				if (!nameMgr::GetObjectByName(nameString(name)))
					return nameString(name);
			}
		}

		//--------------------------------------------------------------------
		// the Objects dialog hook: scene files are added here, others aren't
		//--------------------------------------------------------------------
		bool append_if_scene(const fsLocator& i_File)
		{
			if (!IsSceneFile(i_File))
				return false;
			AppendFile(i_File);
			return true;
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Init()
	{
		chtrOperations::SetFileAppender(&append_if_scene, iscnSceneImport::GetFilePatterns());
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void CleanUp() throw()
	{
		chtrOperations::SetFileAppender(std::function<bool (const fsLocator&)>(), std::string());
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool IsSceneFile(const fsLocator& i_File)
	{
		return iscnSceneImport::IsRecognized(i_File);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool ReadFile(const fsLocator& i_File, iscnSceneDesc& o_Scene)
	{
		if (iscnSceneImport::Read(i_File, imptSceneBuild::SceneSettings(), o_Scene))
			return true;
		const std::string msg = "Could not load the scene file " + itStringUtil::GetStdString(i_File.GetLastName());
		DBG_ERROR(msg);
		guiMessageBox::Show(msg.c_str(), "Geometry Load Error");
		return false;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool AppendFile(const fsLocator& i_File)
	{
		iscnSceneDesc scene;
		if (!ReadFile(i_File, scene))
			return false;
		AppendScene(i_File, scene, std::vector<bool>(scene.m_Objects.size(), true));
		return true;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool AppendScene(const fsLocator& i_File, const iscnSceneDesc& i_Scene, const std::vector<bool>& i_Chosen)
	{
		const std::vector<bool> chosen = iscnSceneChoice::WithAncestors(i_Scene, i_Chosen);
		if (std::find(chosen.begin(), chosen.end(), true) == chosen.end())
			return false;

		DBG_TRACE("Texture load diagnostics: importing " << i_File);
		matTextureMgr::LoadDiagnostics texture_diagnostics;
		const std::string file_name = itStringUtil::GetStdString(i_File.GetLastName());

		UndoBlock block("Add " + file_name);
		// Rebuild trees and property controls once, using the final selection.
		cmmObjectDialogUtil::DeferUpdates defer_object_updates;
		cmmSystemDialogUtil::DeferUpdates defer_updates;

		// the name each object got, empty if it wasn't chosen or couldn't be added
		const std::vector<iscnObjectDesc>& objects = i_Scene.m_Objects;
		std::vector<nameString> names(objects.size());
		for (size_t i = 0; i < objects.size(); ++i)
		{
			if (!chosen[i])
				continue;
			const iscnObjectDesc& object = objects[i];
			const nameString name = unique_name(object.m_Name);
			switch (imptSceneBuild::SystemFor(object.m_Kind))
			{
				case imptSceneBuild::e_Transform:
					names[i] = trfnOperations::AddObject(imptSceneBuild::GroupData(object, name, i_File));
					break;
				case imptSceneBuild::e_Character:
					names[i] = chtrOperations::AddObject(imptSceneBuild::MeshData(object, name, i_File));
					break;
				case imptSceneBuild::e_PointLight:
					names[i] = ptltOperations::AddObject(imptSceneBuild::PointLightData(object, name, i_File));
					break;
				case imptSceneBuild::e_ProjectedLight:
					names[i] = prjltOperations::AddObject(imptSceneBuild::ProjectedLightData(object, name, i_File));
					break;
				case imptSceneBuild::e_Camera:
					names[i] = cmraOperations::AddObject(imptSceneBuild::CameraData(object, name, i_File));
					break;
			}
		}

		// the hierarchy: parents come before children
		for (size_t i = 0; i < objects.size(); ++i)
		{
			const int parent = objects[i].m_Parent;
			if (parent >= 0 && parent < (int)i && !names[i].IsEmpty() && !names[parent].IsEmpty())
				trfnOperations::AddNodeToTransform(names[parent], names[i]);
		}

		// clips and keys, once every object is in its group
		for (size_t i = 0; i < objects.size(); ++i)
		{
			if (!names[i].IsEmpty())
				imptSceneBuild::AddKeysAndClips(objects[i], names[i], i_File);
		}

		// each is a reference to the file as it is now
		for (size_t i = 0; i < objects.size(); ++i)
		{
			imptSceneRefs::Sourced sourced;
			if (!names[i].IsEmpty() && imptSceneRefs::FindSourced(names[i], sourced))
				imptSceneRefs::Remember(sourced);
		}
		imptSceneRefs::StampFile(i_File);
		return true;
	}
}
