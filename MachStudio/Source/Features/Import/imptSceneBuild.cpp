/*****************************************************************************
**  imptSceneBuild.cpp
**
**		see .hpp; which object each kind becomes is described in
**	imptSceneAppend.cpp
**
**	Extra Large Technology
\****************************************************************************/
#include "Features/Import/imptSceneBuild.hpp"

#ifndef CAPT_RENDEROUTPUTDATAUTIL_HPP
#include "Support/capt/captRenderOutputDataUtil.hpp"
#endif
#ifndef MTRL_SCRIPTOBJECT_HPP
#include "Support/mtrl/mtrlScriptObject.hpp"
#endif
#ifndef TMLN_CHANNELSET_HPP
#include "Support/tmln/tmlnChannelSet.hpp"
#endif
#ifndef TMLN_SCRIPTOBJECT_HPP
#include "Support/tmln/tmlnScriptObject.hpp"
#endif
#ifndef CMRA_OPERATIONS_HPP
#include "Systems/Cameras/Undo/cmraOperations.hpp"
#endif
#ifndef CMRA_SCRIPTDATA_HPP
#include "Systems/Cameras/Data/cmraScriptData.hpp"
#endif
#ifndef CMRA_OBJECTMGR_HPP
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#endif
#ifndef CMRA_SCRIPTOBJECT_HPP
#include "Systems/Cameras/Object/cmraScriptObject.hpp"
#endif
#ifndef CHTR_OBJECTMGR_HPP
#include "Systems/Character/Object/chtrObjectMgr.hpp"
#endif
#ifndef CHTR_OPERATIONS_HPP
#include "Systems/Character/Undo/chtrOperations.hpp"
#endif
#ifndef CMM_KEYUTIL_HPP
#include "Systems/Common/DriverCreator/cmmKeyUtil.hpp"
#endif
#ifndef PRJLT_SCRIPTDATA_HPP
#include "Systems/PrjLt/Data/prjltScriptData.hpp"
#endif
#ifndef PRJLT_OBJECTMGR_HPP
#include "Systems/PrjLt/Object/prjltObjectMgr.hpp"
#endif
#ifndef PRJLT_OPERATIONS_HPP
#include "Systems/PrjLt/Undo/prjltOperations.hpp"
#endif
#ifndef PTLT_SCRIPTDATA_HPP
#include "Systems/PtLt/Data/ptltScriptData.hpp"
#endif
#ifndef PTLT_OBJECTMGR_HPP
#include "Systems/PtLt/Object/ptltObjectMgr.hpp"
#endif
#ifndef PTLT_OPERATIONS_HPP
#include "Systems/PtLt/Undo/ptltOperations.hpp"
#endif
#ifndef TRFN_SCRIPTDATA_HPP
#include "Systems/Transforms/Data/trfnScriptData.hpp"
#endif
#ifndef TRFN_OBJECTMGR_HPP
#include "Systems/Transforms/Object/trfnObjectMgr.hpp"
#endif
#ifndef TRFN_OPERATIONS_HPP
#include "Systems/Transforms/Undo/trfnOperations.hpp"
#endif

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef SC_OBJECT_HPP
#include "Graphics/Sc/scObject.hpp"
#endif

#include <functional>
#include <string>
#include <vector>


//============================================================================
//============================================================================
namespace imptSceneBuild
{
	namespace
	{
		//--------------------------------------------------------------------
		// The script object of an object, and the names of its keyed
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
		// i_Data moved from the file's world space into i_pParent's
		//--------------------------------------------------------------------
		template <class xxxBaseData>
		void to_parent_space(xxxBaseData& io_Data, const iscnObjectDesc* i_pParent)
		{
			if (!i_pParent)
				return;
			maMatrix4x4 to_parent = WorldMatrix(*i_pParent);
			to_parent.Invert();
			io_Data.ApplyTransformation(to_parent);
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	System SystemFor(iscnObjectDesc::Kind i_Kind)
	{
		switch (i_Kind)
		{
			case iscnObjectDesc::e_Group:				return e_Transform;
			case iscnObjectDesc::e_Mesh:				return e_Character;
			case iscnObjectDesc::e_PointLight:			return e_PointLight;
			case iscnObjectDesc::e_SpotLight:
			case iscnObjectDesc::e_DirectionalLight:	return e_ProjectedLight;
			case iscnObjectDesc::e_Camera:				return e_Camera;
		}
		return e_Transform;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	trfnScriptData GroupData(const iscnObjectDesc& i_Object, const nameString& i_Name, const fsLocator& i_File)
	{
		trfnScriptData data;
		data.m_BaseData.m_Name = i_Name;
		data.m_BaseData.m_Position = i_Object.m_Position;
		data.m_BaseData.m_Orientation = i_Object.m_Orientation;
		data.m_BaseData.m_Scale = i_Object.m_Scale;
		data.m_BaseData.m_SourceFile = i_File;
		data.m_BaseData.m_SourceIdentity = i_Object.m_Identity;
		return data;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	chtrScriptData MeshData(const iscnObjectDesc& i_Object, const nameString& i_Name, const fsLocator& i_File)
	{
		chtrScriptData data((chtrData(i_File)));
		data.m_BaseData.m_Name = i_Name;
		data.m_BaseData.m_SourceIdentity = i_Object.m_Identity;
		data.m_BaseData.m_Position = i_Object.m_Position;
		data.m_BaseData.m_Orientation = i_Object.m_Orientation;
		data.m_BaseData.m_Scale = i_Object.m_Scale;
		return data;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	ptltScriptData PointLightData(const iscnObjectDesc& i_Object, const nameString& i_Name, const fsLocator& i_File)
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
		return data;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prjltScriptData ProjectedLightData(const iscnObjectDesc& i_Object, const nameString& i_Name, const fsLocator& i_File)
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
		return data;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cmraScriptData CameraData(const iscnObjectDesc& i_Object, const nameString& i_Name, const fsLocator& i_File)
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
		return data;
	}

	//------------------------------------------------------------------------
	// the render output's width / height, 1 if it has no height
	//------------------------------------------------------------------------
	iscnSceneSettings SceneSettings()
	{
		iscnSceneSettings settings;
		const captRenderOutputData& output = captRenderOutputDataUtil::Data();
		const int height = output.m_nHeight.GetValue();
		settings.m_RenderAspect = height > 0 ? (float)output.m_nWidth.GetValue() / height : 1.0f;
		return settings;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	maMatrix4x4 WorldMatrix(const iscnObjectDesc& i_Object)
	{
		maMatrix4x4 matrix;
		scObject::ComputeFullTransformation(maPoint3d(0, 0, 0), maVector3d(0, 0, 0), i_Object.m_Position,
											maPoint3d(i_Object.m_Scale, i_Object.m_Scale, i_Object.m_Scale),
											i_Object.m_Orientation, matrix);
		return matrix;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void AddKeysAndClips(const iscnObjectDesc& i_Object, const nameString& i_Name, const fsLocator& i_File)
	{
		// a mesh's clips, where the importer put them
		if (i_Object.m_Kind == iscnObjectDesc::e_Mesh)
		{
			for (const iscnClipDesc& clip : i_Object.m_Clips)
				chtrOperations::AddAnimationClip(i_Name, i_File, clip.m_Identity, clip.m_Name, maTime::FromSeconds(clip.m_Begin));
		}
		add_keys(i_Object, i_Name);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool Rebuild(const nameString& i_Name, const iscnObjectDesc& i_Object,
				 const iscnObjectDesc* i_pParent, const fsLocator& i_File)
	{
		switch (SystemFor(i_Object.m_Kind))
		{
			case e_Transform:
			{
				const int index = trfnObjectMgr::GetIndexForObject(i_Name);
				if (index < 0)
					return false;
				const trfnScriptData current = trfnObjectMgr::GetScriptData(index);
				trfnScriptData data = GroupData(i_Object, i_Name, i_File);
				to_parent_space(data.m_BaseData, i_pParent);
				data.m_BaseData.m_Name = current.m_BaseData.m_Name;
				data.m_BaseData.m_Objects = current.m_BaseData.m_Objects;
				data.m_ConnectionData = current.m_ConnectionData;
				trfnOperations::SetObjectData(i_Name, data);
				break;
			}
			case e_Character:
			{
				const int index = chtrObjectMgr::GetIndexForObject(i_Name);
				if (index < 0)
					return false;
				const chtrScriptData current = chtrObjectMgr::GetScriptData(index);
				chtrScriptData data = MeshData(i_Object, i_Name, i_File);
				to_parent_space(data.m_BaseData, i_pParent);
				data.m_BaseData.m_Name = current.m_BaseData.m_Name;
				data.m_BaseData.m_bEditorVisible = current.m_BaseData.m_bEditorVisible;
				data.m_ConnectionData = current.m_ConnectionData;
				chtrOperations::ReplaceObjectData(i_Name, data);
				break;
			}
			case e_PointLight:
			{
				const int index = ptltObjectMgr::GetIndexForObject(i_Name);
				if (index < 0)
					return false;
				const ptltScriptData current = ptltObjectMgr::GetScriptData(index);
				ptltScriptData data = PointLightData(i_Object, i_Name, i_File);
				to_parent_space(data.m_BaseData, i_pParent);
				data.m_BaseData.m_Name = current.m_BaseData.m_Name;
				data.m_BaseData.m_bEditorVisible = current.m_BaseData.m_bEditorVisible;
				data.m_ConnectionData = current.m_ConnectionData;
				ptltOperations::SetObjectData(i_Name, data);
				break;
			}
			case e_ProjectedLight:
			{
				const int index = prjltObjectMgr::GetIndexForObject(i_Name);
				if (index < 0)
					return false;
				const prjltScriptData current = prjltObjectMgr::GetScriptData(index);
				prjltScriptData data = ProjectedLightData(i_Object, i_Name, i_File);
				to_parent_space(data.m_BaseData, i_pParent);
				data.m_BaseData.m_Name = current.m_BaseData.m_Name;
				data.m_BaseData.m_bEditorVisible = current.m_BaseData.m_bEditorVisible;
				data.m_ConnectionData = current.m_ConnectionData;
				prjltOperations::SetObjectData(i_Name, data);
				break;
			}
			case e_Camera:
			{
				const int index = cmraObjectMgr::GetIndexForObject(i_Name);
				if (index < 0)
					return false;
				const cmraScriptData current = cmraObjectMgr::GetScriptData(index);
				cmraScriptData data = CameraData(i_Object, i_Name, i_File);
				to_parent_space(data.m_BaseData, i_pParent);
				data.m_BaseData.m_Name = current.m_BaseData.m_Name;
				data.m_BaseData.m_bEditorVisible = current.m_BaseData.m_bEditorVisible;
				data.m_ConnectionData = current.m_ConnectionData;
				cmraOperations::SetObjectData(i_Name, data);
				break;
			}
		}

		AddKeysAndClips(i_Object, i_Name, i_File);
		return true;
	}
}
