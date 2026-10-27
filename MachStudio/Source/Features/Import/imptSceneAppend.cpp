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
**	- Keys (iscnObjectDesc::m_Keys) are added after the parenting (which
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
**	- The whole file is one undo step.
**
**	Extra Large Technology
\****************************************************************************/
#include "Features/Import/imptSceneAppend.hpp"

#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/mtrl/mtrlScriptObject.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"
#include "Systems/Cameras/Undo/cmraOperations.hpp"
#include "Systems/Cameras/Data/cmraScriptData.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#include "Systems/Cameras/Object/cmraScriptObject.hpp"
#include "Systems/Character/Object/chtrObjectMgr.hpp"
#include "Systems/Character/Undo/chtrOperations.hpp"
#include "Systems/Common/DriverCreator/cmmKeyUtil.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Systems/PrjLt/Data/prjltScriptData.hpp"
#include "Systems/PrjLt/Object/prjltObjectMgr.hpp"
#include "Systems/PrjLt/Undo/prjltOperations.hpp"
#include "Systems/PtLt/Data/ptltScriptData.hpp"
#include "Systems/PtLt/Object/ptltObjectMgr.hpp"
#include "Systems/PtLt/Undo/ptltOperations.hpp"
#include "Systems/Transforms/Data/trfnScriptData.hpp"
#include "Systems/Transforms/Object/trfnObjectMgr.hpp"
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
		// the render output's width / height, 1 if it has no height
		//--------------------------------------------------------------------
		float render_aspect()
		{
			const captRenderOutputData& output = captRenderOutputDataUtil::Data();
			const int height = output.m_nHeight.GetValue();
			return height > 0 ? (float)output.m_nWidth.GetValue() / height : 1.0f;
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		nameString add_group(const iscnObjectDesc& i_Object, const nameString& i_Name, const fsLocator& i_File)
		{
			trfnScriptData data;
			data.m_BaseData.m_Name = i_Name;
			data.m_BaseData.m_Position = i_Object.m_Position;
			data.m_BaseData.m_Orientation = i_Object.m_Orientation;
			data.m_BaseData.m_Scale = i_Object.m_Scale;
			data.m_BaseData.m_SourceFile = i_File;
			data.m_BaseData.m_SourceIdentity = i_Object.m_Identity;
			return trfnOperations::AddObject(data);
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		nameString add_mesh(const iscnObjectDesc& i_Object, const nameString& i_Name, const fsLocator& i_File)
		{
			chtrScriptData data((chtrData(i_File)));
			data.m_BaseData.m_Name = i_Name;
			data.m_BaseData.m_SourceIdentity = i_Object.m_Identity;
			data.m_BaseData.m_Position = i_Object.m_Position;
			data.m_BaseData.m_Orientation = i_Object.m_Orientation;
			data.m_BaseData.m_Scale = i_Object.m_Scale;
			const nameString name = chtrOperations::AddObject(data);
			if (name.IsEmpty())
				return name;

			// its clips, where the importer put them
			for (const iscnClipDesc& clip : i_Object.m_Clips)
				chtrOperations::AddAnimationClip(name, i_File, clip.m_Identity, clip.m_Name, maTime::FromSeconds(clip.m_Begin));
			return name;
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		nameString add_point_light(const iscnObjectDesc& i_Object, const nameString& i_Name, const fsLocator& i_File)
		{
			const iscnLightDesc& light = i_Object.m_Light;
			ptltScriptData data;
			data.m_BaseData.m_Name = i_Name;
			data.m_BaseData.m_Color = light.m_Color;
			data.m_BaseData.m_Intensity = light.m_Intensity;
			data.m_BaseData.m_Falloff = light.m_Falloff;
			data.m_BaseData.m_Range = light.m_Range;
			data.m_BaseData.m_Position = i_Object.m_Position;
			data.m_BaseData.m_SourceFile = i_File;
			data.m_BaseData.m_SourceIdentity = i_Object.m_Identity;
			return ptltOperations::AddObject(data);
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		nameString add_projected_light(const iscnObjectDesc& i_Object, const nameString& i_Name, const fsLocator& i_File)
		{
			const iscnLightDesc& light = i_Object.m_Light;
			prjltScriptData data;
			data.m_BaseData.m_Name = i_Name;
			data.m_BaseData.m_Color = light.m_Color;
			data.m_BaseData.m_Intensity = light.m_Intensity;
			data.m_BaseData.m_Falloff = light.m_Falloff;
			data.m_BaseData.m_Position = i_Object.m_Position;
			data.m_BaseData.m_SourceFile = i_File;
			data.m_BaseData.m_SourceIdentity = i_Object.m_Identity;
			if (i_Object.m_Kind == iscnObjectDesc::e_SpotLight)
			{
				data.m_BaseData.m_LightType = prjltData::e_SpotLight;
				data.m_BaseData.m_Range = light.m_Range;
				data.m_BaseData.m_Angle = light.m_Angle;
				data.m_BaseData.m_Penumbra = light.m_Penumbra;
				data.m_BaseData.m_Target = light.m_Target;
			}
			else
			{
				data.m_BaseData.m_LightType = prjltData::e_DirectionalLight;
				data.m_BaseData.m_Orientation = i_Object.m_Orientation;
			}
			return prjltOperations::AddObject(data);
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		nameString add_camera(const iscnObjectDesc& i_Object, const nameString& i_Name, const fsLocator& i_File)
		{
			const iscnCameraDesc& camera = i_Object.m_Camera;
			cmraScriptData data;
			data.m_BaseData.m_Name = i_Name;
			data.m_BaseData.m_Position = i_Object.m_Position;
			data.m_BaseData.m_Target = camera.m_Target;
			data.m_BaseData.m_Tilt = camera.m_Tilt;
			data.m_BaseData.m_bOrthographic = camera.m_bOrthographic;
			if (camera.m_bOrthographic)
				data.m_BaseData.m_OrthoWidth = camera.m_OrthoWidth;
			else
				data.m_BaseData.m_FOV = camera.m_FOV;
			data.m_BaseData.m_Near = camera.m_Near;
			if (camera.m_Far > 0)
				data.m_BaseData.m_Far = camera.m_Far;
			data.m_BaseData.m_SourceFile = i_File;
			data.m_BaseData.m_SourceIdentity = i_Object.m_Identity;
			return cmraOperations::AddObject(data);
		}

		//--------------------------------------------------------------------
		// The script object of an added object, and the names of its keyed
		// channels (its system's data's property names; empty for a
		// channel it doesn't have)
		//--------------------------------------------------------------------
		struct KeyedObject
		{
			tmlnScriptObject* m_pObject = NULL;
			std::function<std::string (iscnKeyTrack::Channel)> m_ChannelName;
		};

		template <class xxxBaseData>
		std::string transform_channel(const xxxBaseData& i_Data, iscnKeyTrack::Channel i_Channel)
		{
			switch (i_Channel)
			{
				case iscnKeyTrack::e_Position:		return i_Data.m_Position.GetPropertyName();
				case iscnKeyTrack::e_Orientation:	return i_Data.m_Orientation.GetPropertyName();
				case iscnKeyTrack::e_Scale:			return i_Data.m_Scale.GetPropertyName();
				default:							return std::string();
			}
		}

		KeyedObject keyed_object(iscnObjectDesc::Kind i_Kind, const nameString& i_Name)
		{
			KeyedObject keyed;
			switch (i_Kind)
			{
				case iscnObjectDesc::e_Group:
				{
					const int index = trfnObjectMgr::GetIndexForObject(i_Name);
					if (index < 0)
						break;
					keyed.m_pObject = trfnObjectMgr::GetObject(index);
					const auto data = trfnObjectMgr::GetScriptData(index).m_BaseData;
					keyed.m_ChannelName = [data](iscnKeyTrack::Channel c) { return transform_channel(data, c); };
					break;
				}
				case iscnObjectDesc::e_Mesh:
				{
					const int index = chtrObjectMgr::GetIndexForObject(i_Name);
					if (index < 0)
						break;
					keyed.m_pObject = chtrObjectMgr::GetObject(index);
					const auto data = chtrObjectMgr::GetScriptData(index).m_BaseData;
					keyed.m_ChannelName = [data](iscnKeyTrack::Channel c) { return transform_channel(data, c); };
					break;
				}
				case iscnObjectDesc::e_PointLight:
				{
					const int index = ptltObjectMgr::GetIndexForObject(i_Name);
					if (index < 0)
						break;
					keyed.m_pObject = ptltObjectMgr::GetObject(index);
					const auto data = ptltObjectMgr::GetScriptData(index).m_BaseData;
					keyed.m_ChannelName = [data](iscnKeyTrack::Channel c)
					{
						switch (c)
						{
							case iscnKeyTrack::e_Position:	return data.m_Position.GetPropertyName();
							case iscnKeyTrack::e_Color:		return data.m_Color.GetPropertyName();
							case iscnKeyTrack::e_Intensity:	return data.m_Intensity.GetPropertyName();
							case iscnKeyTrack::e_Range:		return data.m_Range.GetPropertyName();
							default:						return std::string();
						}
					};
					break;
				}
				case iscnObjectDesc::e_SpotLight:
				case iscnObjectDesc::e_DirectionalLight:
				{
					const int index = prjltObjectMgr::GetIndexForObject(i_Name);
					if (index < 0)
						break;
					keyed.m_pObject = prjltObjectMgr::GetObject(index);
					const auto data = prjltObjectMgr::GetScriptData(index).m_BaseData;
					keyed.m_ChannelName = [data](iscnKeyTrack::Channel c)
					{
						switch (c)
						{
							case iscnKeyTrack::e_Position:		return data.m_Position.GetPropertyName();
							case iscnKeyTrack::e_Target:		return data.m_Target.GetPropertyName();
							case iscnKeyTrack::e_Orientation:	return data.m_Orientation.GetPropertyName();
							case iscnKeyTrack::e_Color:			return data.m_Color.GetPropertyName();
							case iscnKeyTrack::e_Intensity:		return data.m_Intensity.GetPropertyName();
							case iscnKeyTrack::e_Range:			return data.m_Range.GetPropertyName();
							case iscnKeyTrack::e_Angle:			return data.m_Angle.GetPropertyName();
							case iscnKeyTrack::e_Penumbra:		return data.m_Penumbra.GetPropertyName();
							default:							return std::string();
						}
					};
					break;
				}
				case iscnObjectDesc::e_Camera:
				{
					const int index = cmraObjectMgr::GetIndexForObject(i_Name);
					if (index < 0)
						break;
					keyed.m_pObject = cmraObjectMgr::GetObject(index);
					const auto data = cmraObjectMgr::GetScriptData(index).m_BaseData;
					keyed.m_ChannelName = [data](iscnKeyTrack::Channel c)
					{
						switch (c)
						{
							case iscnKeyTrack::e_Position:		return data.m_Position.GetPropertyName();
							case iscnKeyTrack::e_Target:		return data.m_Target.GetPropertyName();
							case iscnKeyTrack::e_Tilt:			return data.m_Tilt.GetPropertyName();
							case iscnKeyTrack::e_FOV:			return data.m_FOV.GetPropertyName();
							case iscnKeyTrack::e_OrthoWidth:	return data.m_OrthoWidth.GetPropertyName();
							case iscnKeyTrack::e_Near:			return data.m_Near.GetPropertyName();
							case iscnKeyTrack::e_Far:			return data.m_Far.GetPropertyName();
							default:							return std::string();
						}
					};
					break;
				}
			}
			return keyed;
		}

		//--------------------------------------------------------------------
		// the object's keys on its channels
		//--------------------------------------------------------------------
		void add_keys(const iscnObjectDesc& i_Object, const nameString& i_Name)
		{
			if (i_Object.m_Keys.empty())
				return;
			const KeyedObject keyed = keyed_object(i_Object.m_Kind, i_Name);
			if (!keyed.m_pObject)
				return;

			for (const iscnKeyTrack& track : i_Object.m_Keys)
			{
				// its channel
				tmlnChannel* pChannel = NULL;
				std::string channel_name;
				if (track.m_Channel == iscnKeyTrack::e_MaterialParam || track.m_Channel == iscnKeyTrack::e_MaterialNormalScale)
				{
					channel_name = track.m_Material + "." + track.m_Param;
					if (const mtrlScriptObject* pMaterials = dynamic_cast<const mtrlScriptObject*>(keyed.m_pObject))
					{
						pChannel = (track.m_Channel == iscnKeyTrack::e_MaterialParam)
								   ? pMaterials->FindMaterialChannel(track.m_Material, track.m_Param)
								   : pMaterials->FindNormalScaleChannel(track.m_Material);
					}
				}
				else
				{
					channel_name = keyed.m_ChannelName(track.m_Channel);
					const int index = channel_name.empty() ? -1 : keyed.m_pObject->GetChannelSet().GetChannel(channel_name.c_str());
					if (index >= 0)
						pChannel = &keyed.m_pObject->ChannelSet().Channel(index);
				}
				if (!pChannel)
				{
					DBG_WARNING("Keys for " << i_Name.GetString() << " skipped: it has no channel "
								<< (channel_name.empty() ? std::to_string((int)track.m_Channel) : channel_name));
					continue;
				}

				// its keys
				const size_t num = track.m_Times.size();
				const size_t components = num ? track.m_Values.size() / num : 0;
				std::vector<cmmKey> keys(num);
				for (size_t k = 0; k < num; ++k)
				{
					keys[k].m_Time = maTime::FromSeconds(track.m_Times[k]);
					for (size_t c = 0; c < components && c < 4; ++c)
						keys[k].m_Value[c] = track.m_Values[k * components + c];
				}
				if (!cmmKeyUtil::AddKeys(keyed.m_pObject, pChannel, keys, track.m_bStep))
					DBG_WARNING("Keys for " << i_Name.GetString() << " skipped: channel " << pChannel->GetName()
								<< " takes no float, position, colour or orientation keys");
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
	bool AppendFile(const fsLocator& i_File)
	{
		DBG_TRACE("Texture load diagnostics: importing " << i_File);
		matTextureMgr::LoadDiagnostics texture_diagnostics;
		const std::string file_name = itStringUtil::GetStdString(i_File.GetLastName());

		iscnSceneSettings settings;
		settings.m_RenderAspect = render_aspect();
		iscnSceneDesc scene;
		if (!iscnSceneImport::Read(i_File, settings, scene))
		{
			const std::string msg = "Could not load the scene file " + file_name;
			DBG_ERROR(msg);
			guiMessageBox::Show(msg.c_str(), "Geometry Load Error");
			return false;
		}

		UndoBlock block("Add " + file_name);
		// Rebuild trees and property controls once, using the final selection.
		cmmObjectDialogUtil::DeferUpdates defer_object_updates;
		cmmSystemDialogUtil::DeferUpdates defer_updates;

		// the name each object got, empty if it couldn't be added
		const std::vector<iscnObjectDesc>& objects = scene.m_Objects;
		std::vector<nameString> names(objects.size());
		for (size_t i = 0; i < objects.size(); ++i)
		{
			const iscnObjectDesc& object = objects[i];
			const nameString name = unique_name(object.m_Name);
			switch (object.m_Kind)
			{
				case iscnObjectDesc::e_Group:
					names[i] = add_group(object, name, i_File);
					break;
				case iscnObjectDesc::e_Mesh:
					names[i] = add_mesh(object, name, i_File);
					break;
				case iscnObjectDesc::e_PointLight:
					names[i] = add_point_light(object, name, i_File);
					break;
				case iscnObjectDesc::e_SpotLight:
				case iscnObjectDesc::e_DirectionalLight:
					names[i] = add_projected_light(object, name, i_File);
					break;
				case iscnObjectDesc::e_Camera:
					names[i] = add_camera(object, name, i_File);
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

		// keys, once every object is in its group
		for (size_t i = 0; i < objects.size(); ++i)
		{
			if (!names[i].IsEmpty())
				add_keys(objects[i], names[i]);
		}
		return true;
	}
}
