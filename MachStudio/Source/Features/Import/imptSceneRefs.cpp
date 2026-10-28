/*****************************************************************************
**  imptSceneRefs.cpp
**
**		see .hpp
**
**	Rulings (glTF Plan 7)
**
**	- Reference = equal bytes. A sourced object is a reference when the
**	  ledger has a snapshot under its name and its bytes now equal it;
**	  otherwise it is a copy. The bytes are its script data written by
**	  its system's parser into a chMemWriter (no document locator, so no
**	  relative paths), with editor visibility, a group's children list
**	  and name UIDs left out. Edits reach objects through many paths
**	  (prty's own undo, drivers, the trax editor, keyframing, the
**	  systems' set operations); a comparison sees all of them, and undoing
**	  the only edit makes the bytes equal again. Nothing marks objects.
**	- What a .mab stores: every object in full, as before, plus (SRCR
**	  chunk, imptSceneRefsChunk) each sourced object's state and each
**	  file's stamp when its references were last built. On open,
**	  references are rebuilt only when their file's stamp changed (or a
**	  missing file is back); otherwise the stored data is what the file
**	  gives. A node gone from the file leaves its object with its last
**	  values (a warning, a resource tracker entry); it becomes a copy. A
**	  missing file leaves its objects as they are, still references.
**	- Materials belong to the character (MachStudio has no shared
**	  material object), so a material edit makes the character a copy and
**	  Revert brings back the file's materials.
**	- Names and the hierarchy are the user's: rebuilds keep an object's
**	  name and place in its group (a rename or a move to another group is
**	  still an edit: the name and the values are in the bytes).
**	- No snapshot = copy: duplicates, objects merged in from another .mab,
**	  and objects of scenes saved before this tracking existed. Revert
**	  makes them references.
**	- Rebuilding in place: an object is rebuilt in its file parent's
**	  space (its world pose from the file times the inverse of its file
**	  parent's world matrix, imptSceneBuild::Rebuild), so a reference
**	  child follows its group when the user has moved the group.
**
**	Extra Large Technology
\****************************************************************************/
#include "Features/Import/imptSceneRefs.hpp"

#ifndef MNM_SOURCESTATE_HPP
#include "Support/mnm/mnmSourceState.hpp"
#endif
#ifndef MNM_THINKMGR_HPP
#include "Support/mnm/mnmThinkMgr.hpp"
#endif
#ifndef CMRA_DATAPARSER_HPP
#include "Systems/Cameras/Data/cmraDataParser.hpp"
#endif
#ifndef CMRA_OBJECTMGR_HPP
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#endif
#ifndef CMRA_CAMERAOBJECT_HPP
#include "Systems/Cameras/Object/cmraCameraObject.hpp"
#endif
#ifndef CHTR_DATAPARSER_HPP
#include "Systems/Character/Data/chtrDataParser.hpp"
#endif
#ifndef CHTR_OBJECTMGR_HPP
#include "Systems/Character/Object/chtrObjectMgr.hpp"
#endif
#ifndef CHTR_OBJECT_HPP
#include "Systems/Character/Object/chtrObject.hpp"
#endif
#ifndef PRJLT_DATAPARSER_HPP
#include "Systems/PrjLt/Data/prjltDataParser.hpp"
#endif
#ifndef PRJLT_OBJECTMGR_HPP
#include "Systems/PrjLt/Object/prjltObjectMgr.hpp"
#endif
#ifndef PRJLT_PROJECTEDLIGHTOBJECT_HPP
#include "Systems/PrjLt/Object/prjltProjectedLightObject.hpp"
#endif
#ifndef PTLT_DATAPARSER_HPP
#include "Systems/PtLt/Data/ptltDataParser.hpp"
#endif
#ifndef PTLT_OBJECTMGR_HPP
#include "Systems/PtLt/Object/ptltObjectMgr.hpp"
#endif
#ifndef PTLT_POINTLIGHTOBJECT_HPP
#include "Systems/PtLt/Object/ptltPointLightObject.hpp"
#endif
#ifndef TRFN_TRANSFORMSDATAPARSER_HPP
#include "Systems/Transforms/Data/trfnTransformsDataParser.hpp"
#endif
#ifndef TRFN_OBJECTMGR_HPP
#include "Systems/Transforms/Object/trfnObjectMgr.hpp"
#endif
#ifndef TRFN_TRANSFORMOBJECT_HPP
#include "Systems/Transforms/Object/trfnTransformObject.hpp"
#endif

#ifndef IMPT_SCENEREFSCHUNK_HPP
#include "Features/Import/imptSceneRefsChunk.hpp"
#endif
#ifndef DOC_DOCUMENTINTEREST_HPP
#include "Tool/doc/docDocumentInterest.hpp"
#endif
#ifndef DOC_SINGLETYPEMGR_HPP
#include "Tool/doc/docSingleTypeMgr.hpp"
#endif
#ifndef ISCN_SCENEIMPORT_HPP
#include "ImportExport/iscn/iscnSceneImport.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif
#ifndef CH_MEMWRITER_HPP
#include "Core/ch/chMemWriter.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef IT_STRINGUTIL_HPP
#include "Core/it/itStringUtil.hpp"
#endif
#ifndef UNDO_UNDOINTEREST_HPP
#include "Core/undo/undoUndoInterest.hpp"
#endif
#ifndef UNDO_UNDOMGR_HPP
#include "Core/undo/undoUndoMgr.hpp"
#endif
#ifndef CTXM_CONTEXTMENU_HPP
#include "Tool/ctxm/ctxmContextMenu.hpp"
#endif
#ifndef CTXM_CONTEXTMENUINTEREST_HPP
#include "Tool/ctxm/ctxmContextMenuInterest.hpp"
#endif
#ifndef GUI_CONTEXTMENU_HPP
#include "Tool/gui/guiContextMenu.hpp"
#endif
#ifndef GUI_MESSAGEBOX_HPP
#include "Tool/gui/guiMessageBox.hpp"
#endif
#ifndef SEL3D_MGR_HPP
#include "Tool/sel3d/sel3dMgr.hpp"
#endif
#ifndef SEL3D_SELECTINTEREST_HPP
#include "Tool/sel3d/sel3dSelectInterest.hpp"
#endif


#include <set>


//============================================================================
//============================================================================
namespace imptSceneRefs
{
	namespace
	{
		iscnRefLedger l_Ledger;
		bool l_bRefreshQueued = false;
		std::map<std::string, MissingNodes> l_Missing;

		//--------------------------------------------------------------------
		// the file's object with the object's identity, of its system
		//--------------------------------------------------------------------
		const iscnObjectDesc* FindDesc(const iscnSceneDesc& i_Scene, const Sourced& i_Object)
		{
			for (const iscnObjectDesc& desc : i_Scene.m_Objects)
			{
				if (desc.m_Identity == i_Object.m_Identity && imptSceneBuild::SystemFor(desc.m_Kind) == i_Object.m_System)
					return &desc;
			}
			return NULL;
		}

		//--------------------------------------------------------------------
		// the Source line of each selected sourced object
		//--------------------------------------------------------------------
		mnmSourceState* source_state(const Sourced& i_Object, sel3dObject*& o_pPick)
		{
			o_pPick = NULL;
			mnmSourceState* pState = NULL;
			switch (i_Object.m_System)
			{
				case imptSceneBuild::e_Character:
				{
					const int index = chtrObjectMgr::GetIndexForObject(i_Object.m_Name);
					if (index >= 0) { chtrObject* p = chtrObjectMgr::GetPickObject(index); o_pPick = p; pState = p; }
					break;
				}
				case imptSceneBuild::e_Transform:
				{
					const int index = trfnObjectMgr::GetIndexForObject(i_Object.m_Name);
					if (index >= 0) { trfnTransformObject* p = trfnObjectMgr::GetPickObject(index); o_pPick = p; pState = p; }
					break;
				}
				case imptSceneBuild::e_PointLight:
				{
					const int index = ptltObjectMgr::GetIndexForObject(i_Object.m_Name);
					if (index >= 0) { ptltPointLightObject* p = ptltObjectMgr::GetPickObject(index); o_pPick = p; pState = p; }
					break;
				}
				case imptSceneBuild::e_ProjectedLight:
				{
					const int index = prjltObjectMgr::GetIndexForObject(i_Object.m_Name);
					if (index >= 0) { prjltProjectedLightObject* p = prjltObjectMgr::GetPickObject(index); o_pPick = p; pState = p; }
					break;
				}
				case imptSceneBuild::e_Camera:
				{
					const int index = cmraObjectMgr::GetIndexForObject(i_Object.m_Name);
					if (index >= 0) { cmraCameraObject* p = cmraObjectMgr::GetPickObject(index); o_pPick = p; pState = p; }
					break;
				}
			}
			return pState;
		}

		void refresh_source_states()
		{
			l_bRefreshQueued = false;
			for (const Sourced& object : GetSourcedObjects())
			{
				sel3dObject* pPick = NULL;
				mnmSourceState* pState = source_state(object, pPick);
				if (!pState || !pPick || !sel3dMgr::IsSelected(pPick))
					continue;
				const std::string file_name = itStringUtil::GetStdString(object.m_File.GetLastName());
				pState->SetSourceState(GetState(object) == iscnRefLedger::e_Reference
									   ? "Reference: " + file_name
									   : "Copy (edited): " + file_name);
			}
		}

		//--------------------------------------------------------------------
		// once per think: an undo step is added before its change is applied
		//--------------------------------------------------------------------
		void queue_refresh()
		{
			if (l_bRefreshQueued)
				return;
			l_bRefreshQueued = true;
			mnmThinkMgr::CallFunctionDelayed(&refresh_source_states);
		}

		class UndoInterest : public undoUndoInterest
		{
		public:
			virtual void UndoAdded() { queue_refresh(); }
			virtual void StackChanged() { queue_refresh(); }
		};
		UndoInterest l_UndoInterest;

		class SelectInterest : public sel3dSelectInterest
		{
		public:
			virtual void SelectionChanged() { queue_refresh(); }
			virtual void AddedToSelection(sel3dObject*) { queue_refresh(); }
			virtual void RemovedFromSelection(sel3dObject*) { queue_refresh(); }
		};
		SelectInterest l_SelectInterest;

		//--------------------------------------------------------------------
		// "Revert to <file>" on a selection holding copies
		//--------------------------------------------------------------------
		class ContextMenuInterest : public ctxmContextMenuInterest
		{
		public:
			virtual void AddToContextMenu(guiContextMenu& io_ContextMenu,
										  const sel3dObject* /*i_pChosenObject*/,
										  const g3dPickInfo* /*i_pPickInfo*/) const
			{
				std::vector<nameString> names;
				std::set<std::string> files;
				std::string file_name;
				for (const Sourced& object : GetSourcedObjects())
				{
					sel3dObject* pPick = NULL;
					if (!source_state(object, pPick) || !pPick || !sel3dMgr::IsSelected(pPick)
					 || GetState(object) != iscnRefLedger::e_Copy)
						continue;
					names.push_back(object.m_Name);
					files.insert(FileKey(object.m_File));
					file_name = itStringUtil::GetStdString(object.m_File.GetLastName());
				}
				if (names.empty())
					return;
				const std::string label = files.size() == 1 ? "Revert to " + file_name : std::string("Revert to Source Files");
				io_ContextMenu.AddMenuItem("", label.c_str(), [names]() { Revert(names); });
			}
		};
		ContextMenuInterest l_ContextMenuInterest;

		//--------------------------------------------------------------------
		// one undo block, ended even when a rebuild throws
		//--------------------------------------------------------------------
		struct UndoBlock
		{
			UndoBlock(const std::string& i_Name) { undoUndoMgr::BeginMultipleOperationBlock(i_Name.c_str()); }
			~UndoBlock() { undoUndoMgr::EndMultipleOperationBlock(); }
		};

		//--------------------------------------------------------------------
		// a one-item list of the data with name UIDs cleared
		//--------------------------------------------------------------------
		template <class xxxListData, class xxxScriptData, class xxxObjectMgr>
		xxxListData normalized_list(const xxxScriptData& i_Data)
		{
			xxxListData list;
			list.m_Items.push_back(i_Data);
			xxxObjectMgr::ClearItemNameUIDs(list);
			return list;
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	std::vector<Sourced> GetSourcedObjects()
	{
		std::vector<Sourced> objects;
		auto add = [&objects](imptSceneBuild::System i_System, const nameString& i_Name, const fsLocator& i_File, const std::string& i_Identity)
		{
			Sourced object;
			object.m_System = i_System;
			object.m_Name = i_Name;
			object.m_File = i_File;
			object.m_Identity = i_Identity;
			objects.push_back(object);
		};

		for (int i = 0; i < chtrObjectMgr::GetNumObjects(); ++i)
		{
			const chtrData data = chtrObjectMgr::GetBaseData(i);
			if (!data.m_SourceIdentity.empty())
				add(imptSceneBuild::e_Character, data.m_Name.GetValue(), data.m_Filename.GetValue(), data.m_SourceIdentity);
		}
		for (int i = 0; i < trfnObjectMgr::GetNumObjects(); ++i)
		{
			const trfnData data = trfnObjectMgr::GetBaseData(i);
			if (data.m_SourceFile.GetValue().GetNumNames() > 0)
				add(imptSceneBuild::e_Transform, data.m_Name.GetValue(), data.m_SourceFile.GetValue(), data.m_SourceIdentity);
		}
		for (int i = 0; i < ptltObjectMgr::GetNumObjects(); ++i)
		{
			const ptltData data = ptltObjectMgr::GetBaseData(i);
			if (data.m_SourceFile.GetValue().GetNumNames() > 0)
				add(imptSceneBuild::e_PointLight, data.m_Name.GetValue(), data.m_SourceFile.GetValue(), data.m_SourceIdentity);
		}
		for (int i = 0; i < prjltObjectMgr::GetNumObjects(); ++i)
		{
			const prjltData data = prjltObjectMgr::GetBaseData(i);
			if (data.m_SourceFile.GetValue().GetNumNames() > 0)
				add(imptSceneBuild::e_ProjectedLight, data.m_Name.GetValue(), data.m_SourceFile.GetValue(), data.m_SourceIdentity);
		}
		for (int i = 0; i < cmraObjectMgr::GetNumObjects(); ++i)
		{
			const cmraCameraData data = cmraObjectMgr::GetBaseData(i);
			if (data.m_SourceFile.GetValue().GetNumNames() > 0)
				add(imptSceneBuild::e_Camera, data.m_Name.GetValue(), data.m_SourceFile.GetValue(), data.m_SourceIdentity);
		}
		return objects;
	}

	bool FindSourced(const nameString& i_Name, Sourced& o_Object)
	{
		for (const Sourced& object : GetSourcedObjects())
		{
			if (object.m_Name == i_Name)
			{
				o_Object = object;
				return true;
			}
		}
		return false;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	std::vector<uint8_t> GetBytes(const Sourced& i_Object)
	{
		chMemWriter writer;
		switch (i_Object.m_System)
		{
			case imptSceneBuild::e_Character:
			{
				const int index = chtrObjectMgr::GetIndexForObject(i_Object.m_Name);
				if (index < 0)
					break;
				chtrScriptData data = chtrObjectMgr::GetScriptData(index);
				data.m_BaseData.m_bEditorVisible.SetValue(true);
				const chtrCharactersData list = normalized_list<chtrCharactersData, chtrScriptData, chtrObjectMgr>(data);
				chtrDataParser::WriteCharacterChunk(writer, list.m_Items[0]);
				break;
			}
			case imptSceneBuild::e_Transform:
			{
				const int index = trfnObjectMgr::GetIndexForObject(i_Object.m_Name);
				if (index < 0)
					break;
				trfnScriptData data = trfnObjectMgr::GetScriptData(index);
				data.m_BaseData.m_Objects.clear();
				const trfnTransformsData list = normalized_list<trfnTransformsData, trfnScriptData, trfnObjectMgr>(data);
				trfnTransformsDataParser::WriteTransformData(writer, list.m_Items[0]);
				break;
			}
			case imptSceneBuild::e_PointLight:
			{
				const int index = ptltObjectMgr::GetIndexForObject(i_Object.m_Name);
				if (index < 0)
					break;
				ptltScriptData data = ptltObjectMgr::GetScriptData(index);
				data.m_BaseData.m_bEditorVisible.SetValue(true);
				const ptltPointLightsData list = normalized_list<ptltPointLightsData, ptltScriptData, ptltObjectMgr>(data);
				ptltDataParser::WritePointLightData(writer, list.m_Items[0]);
				break;
			}
			case imptSceneBuild::e_ProjectedLight:
			{
				const int index = prjltObjectMgr::GetIndexForObject(i_Object.m_Name);
				if (index < 0)
					break;
				prjltScriptData data = prjltObjectMgr::GetScriptData(index);
				data.m_BaseData.m_bEditorVisible.SetValue(true);
				const prjltProjectLightsData list = normalized_list<prjltProjectLightsData, prjltScriptData, prjltObjectMgr>(data);
				prjltDataParser::WriteProjectedLightData(writer, list.m_Items[0]);
				break;
			}
			case imptSceneBuild::e_Camera:
			{
				const int index = cmraObjectMgr::GetIndexForObject(i_Object.m_Name);
				if (index < 0)
					break;
				cmraScriptData data = cmraObjectMgr::GetScriptData(index);
				data.m_BaseData.m_bEditorVisible.SetValue(true);
				const cmraCamerasData list = normalized_list<cmraCamerasData, cmraScriptData, cmraObjectMgr>(data);
				cmraCamerasDataParser::WriteData(writer, list);
				break;
			}
		}
		return writer.GetBytes();
	}

	iscnRefLedger::State GetState(const Sourced& i_Object)
	{
		return l_Ledger.GetState(i_Object.m_Name.GetString(), GetBytes(i_Object));
	}

	void Remember(const Sourced& i_Object)
	{
		l_Ledger.SetReference(i_Object.m_Name.GetString(), GetBytes(i_Object));
		queue_refresh();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	std::string FileKey(const fsLocator& i_File)
	{
		std::string key;
		fsFileUtil::LocatorToANSIFilename(i_File, key);
		return key;
	}

	void StampFile(const fsLocator& i_File)
	{
		l_Ledger.SetFileStamp(FileKey(i_File), iscnStampFile(i_File));
	}


	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	iscnRefLedger::Saved GetSaved()
	{
		iscnRefLedger::Saved saved;
		for (const Sourced& object : GetSourcedObjects())
		{
			const iscnRefLedger::State state = GetState(object);
			saved.m_Objects[object.m_Name.GetString()] = state;
			if (state != iscnRefLedger::e_Reference)
				continue;
			const std::string key = FileKey(object.m_File);
			const iscnFileStamp* pStamp = l_Ledger.GetFileStamp(key);
			saved.m_Files[key] = pStamp ? *pStamp : iscnStampFile(object.m_File);
		}
		return saved;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Reload(const iscnRefLedger::Saved& i_Saved)
	{
		// the references, per file
		std::map<std::string, std::vector<Sourced> > files;
		for (const Sourced& object : GetSourcedObjects())
		{
			auto saved = i_Saved.m_Objects.find(object.m_Name.GetString());
			if (saved != i_Saved.m_Objects.end() && saved->second == iscnRefLedger::e_Reference)
				files[FileKey(object.m_File)].push_back(object);
		}

		bool rebuilt = false;
		for (const auto& file : files)
		{
			const std::string& key = file.first;
			const std::vector<Sourced>& objects = file.second;
			const fsLocator& locator = objects.front().m_File;
			const std::string file_name = itStringUtil::GetStdString(locator.GetLastName());
			auto saved_stamp = i_Saved.m_Files.find(key);
			const iscnFileStamp stamp = iscnStampFile(locator);

			// unchanged: what the scene stored is what the file gives
			if (stamp.m_bRead && saved_stamp != i_Saved.m_Files.end() && saved_stamp->second == stamp)
			{
				for (const Sourced& object : objects)
					Remember(object);
				l_Ledger.SetFileStamp(key, stamp);
				continue;
			}

			// missing or unreadable: they keep their values and stay references
			iscnSceneDesc scene;
			if (!stamp.m_bRead || !iscnSceneImport::Read(locator, imptSceneBuild::SceneSettings(), scene))
			{
				DBG_WARNING("Scene file " << key << " could not be read: its " << objects.size()
							<< " objects keep the values the scene saved");
				for (const Sourced& object : objects)
					Remember(object);
				l_Ledger.SetFileStamp(key, saved_stamp != i_Saved.m_Files.end() ? saved_stamp->second : stamp);
				continue;
			}

			// changed: rebuild each from its node
			int updated = 0;
			for (const Sourced& object : objects)
			{
				const iscnObjectDesc* pDesc = FindDesc(scene, object);
				if (!pDesc)
				{
					DBG_WARNING(object.m_Name.GetString() << ": " << file_name << " no longer has its node "
								<< object.m_Identity << "; it keeps its values and is now a copy");
					l_Missing[key].m_File = locator;
					l_Missing[key].m_Objects.push_back(object.m_Name.GetString());
					continue;
				}
				const iscnObjectDesc* pParent = pDesc->m_Parent >= 0 ? &scene.m_Objects[pDesc->m_Parent] : NULL;
				if (imptSceneBuild::Rebuild(object.m_Name, *pDesc, pParent, locator))
				{
					Remember(object);
					++updated;
				}
			}
			l_Ledger.SetFileStamp(key, stamp);
			DBG_LOG(updated << " objects updated from " << file_name);
			rebuilt = rebuilt || updated > 0;
		}

		// rebuilds when a scene opens aren't undoable, as the load isn't
		if (rebuilt)
			undoUndoMgr::Commit();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Revert(const std::vector<nameString>& i_Names)
	{
		// the objects, per file
		std::map<std::string, std::vector<Sourced> > files;
		for (const nameString& name : i_Names)
		{
			Sourced object;
			if (FindSourced(name, object))
				files[FileKey(object.m_File)].push_back(object);
		}

		// read each file first; one that can't be read changes nothing
		std::map<std::string, iscnSceneDesc> scenes;
		for (const auto& file : files)
		{
			const fsLocator& locator = file.second.front().m_File;
			iscnSceneDesc scene;
			if (!iscnSceneImport::Read(locator, imptSceneBuild::SceneSettings(), scene))
			{
				const std::string msg = "Could not read the scene file " + file.first
										+ "; its objects were not reverted.";
				DBG_ERROR(msg);
				guiMessageBox::Show(msg.c_str(), "Revert");
				continue;
			}
			scenes[file.first] = std::move(scene);
		}
		if (scenes.empty())
			return;

		const std::string block_name = scenes.size() == 1
			? "Revert to " + itStringUtil::GetStdString(files[scenes.begin()->first].front().m_File.GetLastName())
			: std::string("Revert to Source Files");
		UndoBlock block(block_name);
		for (const auto& file : scenes)
		{
			const iscnSceneDesc& scene = file.second;
			for (const Sourced& object : files[file.first])
			{
				const iscnObjectDesc* pDesc = FindDesc(scene, object);
				if (!pDesc)
				{
					DBG_WARNING(object.m_Name.GetString() << " not reverted: its file no longer has its node " << object.m_Identity);
					continue;
				}
				const iscnObjectDesc* pParent = pDesc->m_Parent >= 0 ? &scene.m_Objects[pDesc->m_Parent] : NULL;
				if (imptSceneBuild::Rebuild(object.m_Name, *pDesc, pParent, object.m_File))
					Remember(object);
			}
			StampFile(files[file.first].front().m_File);
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const std::map<std::string, MissingNodes>& GetMissingNodes()
	{
		return l_Missing;
	}

	void Clear()
	{
		l_Ledger.Clear();
		l_Missing.clear();
	}

	iscnRefLedger& Ledger()
	{
		return l_Ledger;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Init()
	{
		// the SRCR chunk (the document type owns its interests)
		docSingleTypeMgr::AddDocumentInterest(new docSimpleDocumentInterest<imptSceneRefsChunk>());
		undoUndoMgr::AddInterest(&l_UndoInterest);
		sel3dMgr::RegisterSelectInterest(&l_SelectInterest);
		ctxmContextMenu::RegisterInterest(&l_ContextMenuInterest);
	}

	void CleanUp() throw()
	{
		ctxmContextMenu::UnRegisterInterest(&l_ContextMenuInterest);
		sel3dMgr::UnRegisterSelectInterest(&l_SelectInterest);
		undoUndoMgr::RemoveInterest(&l_UndoInterest);
		l_Ledger.Clear();
	}
}
