/*****************************************************************************
**	rlyrRenderLayerMgr.cpp
**
**	Keeps track of objects to be grouped with common environment settings
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"

#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/capt/captRenderOutputObject.hpp"
#include "Support/pfx/pfxPostEffectObject.hpp"
#include "Support/rlyr/private/rlyrRenderLayerNode.hpp"
#include "Support/rlyr/private/rlyrRenderLayerObject.hpp"
#include "Support/rlyr/rlyrRenderCam.hpp"
#include "Support/rlyr/rlyrRenderLayer.hpp"
#include "Support/rlyr/rlyrRenderLayerInterest.hpp"
#include "Support/rman/rmanMgr.hpp"
#include "Support/rprf/rprfPrefsObject.hpp"
#include "Support/vis/visMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/name/nameObject.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Tool/api3d/api3dObjectSingle.hpp"

#include <sstream>
#include <iomanip>
#include <map>


//============================================================================
//============================================================================
namespace
{
	std::vector<rlyrRenderLayerInterest*>	l_InterestList;

	bool l_bDisableNotify = false;
	void notify_interests_changed()
	{
		if (l_bDisableNotify) return;
		std::vector<rlyrRenderLayerInterest*>::iterator it, end = l_InterestList.end();
		for (it  = l_InterestList.begin(); it != end; ++it)
		{
			(*it)->DataChanged();
		}
	}
	void notify_interests_added()
	{
		if (l_bDisableNotify) return;
		std::vector<rlyrRenderLayerInterest*>::iterator it, end = l_InterestList.end();
		for (it  = l_InterestList.begin(); it != end; ++it)
		{
			(*it)->ObjectAdded();
		}
	}
	void notify_interests_removed(const nameString& i_Name)
	{
		if (l_bDisableNotify) return;
		std::vector<rlyrRenderLayerInterest*>::iterator it, end = l_InterestList.end();
		for (it  = l_InterestList.begin(); it != end; ++it)
		{
			(*it)->ObjectRemoved(i_Name);
		}
	}
	void notify_interests_renamed()
	{
		if (l_bDisableNotify) return;
		std::vector<rlyrRenderLayerInterest*>::iterator it, end = l_InterestList.end();
		for (it  = l_InterestList.begin(); it != end; ++it)
		{
			(*it)->ObjectRenamed();
		}
	}

	//============================================================================
	//	get_if gets the pointer in the collection for which the predicate
	//	is true
	//============================================================================
	template<class S, class Pred>
	S* get_if(std::vector<S*>& i_Container, Pred p)
	{
		std::vector<S*>::iterator it = std::find_if(i_Container.begin(), i_Container.end(), p);
		if  (it != i_Container.end())
		{
			return (*it);
		}
		return NULL;
	}

	template<class S> 
	class name_search
	{
	public:
		name_search(const nameString& i_Name) : m_Name(i_Name) {};
		bool operator () ( S* i_Set )
		{
			return (m_Name == i_Set->m_Name);
		}
		nameString m_Name;
	};

	template<class S> 
	class nameobj_search
	{
	public:
		nameobj_search(const nameString& i_Name) : m_Name(i_Name) {};
		bool operator () ( S* i_Set )
		{
			return (m_Name == i_Set->m_pNameObj->GetName());
		}
		nameString m_Name;
	};

	template<class S> 
	class nameobjptr_search
	{
	public:
		nameobjptr_search(nameObject* i_pNameObj) : m_pNameObj(i_pNameObj) {};
		bool operator () ( S* i_Set )
		{
			return (m_pNameObj == i_Set->m_pNameObj);
		}
		nameObject* m_pNameObj;
	};

// Note: It may have been easier to use std::map here, but I wasn't sure if
// we could guarantee that the names were unique.
	std::vector<rlyrObject*> l_Objects;
	std::map<nameString, bool> l_EditorObjects;  //will hold name/bool pairing of each objects editor visibility
	std::map<nameString, bool> object_status;
	std::vector<rlyrRenderCam*> l_RenderCams;
	std::vector<rlyrRenderLayer*> l_RenderLayers;
	std::vector<rlyrRenderLayer*> l_HiddenRenderLayers;
	bool l_isChunkAdd = false;
	rlyrLayersData l_LayerData;
	bool l_bHaveHidden = false;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	rlyrRenderLayer* getRenderLayerByName(const nameString& i_LayerName)
	{
		std::vector<rlyrRenderLayer*>::iterator it, end = l_RenderLayers.end();
		for (it = l_RenderLayers.begin(); it != end; ++it)
		{
			if((*it)->GetName() == i_LayerName)
				return (*it);
		}

		for (it = l_HiddenRenderLayers.begin(); it != l_HiddenRenderLayers.end(); ++it)
		{
			if ((*it)->GetName() == i_LayerName)
				return (*it);
		}
		return NULL;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	rlyrRenderCam* getRenderCamByName(const nameString& i_CamName)
	{
		std::vector<rlyrRenderCam*>::iterator it, end = l_RenderCams.end();
		for (it = l_RenderCams.begin(); it != end; ++it)
		{
			if((*it)->GetName() == i_CamName)
				return (*it);
		}
		return NULL;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	rlyrObject* getObjectByName(const nameString& i_ObjectName)
	{
		std::vector<rlyrObject*>::iterator it, end = l_Objects.end();
		for (it = l_Objects.begin(); it != end; ++it)
		{
			if((*it)->GetName() == i_ObjectName)
				return (*it);
		}
		return NULL;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool isValidLayerName(std::vector<nameString>& i_LayerNames, const nameString& i_UniqueName)
	{
		std::vector<nameString>::iterator it, end = i_LayerNames.end();
		for ( it = i_LayerNames.begin(); it != end; ++it)
		{
			if(i_UniqueName == (*it).GetString())
			{
				return false;
			}
		}
		return true;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void get_layer_names(std::vector<nameString>& o_LayerNames)
	{
		o_LayerNames.clear();
		// Go through all of the render layers in this manager
		std::vector<rlyrRenderLayer*>::iterator it, end = l_RenderLayers.end();
		for (it = l_RenderLayers.begin(); it != end; ++it)
		{
			o_LayerNames.push_back((*it)->GetName());
		}
	}
	
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	nameString generate_layer_name(const nameString& i_LayerName, int i_LayerID)
	{
		std::ostringstream name_stream(std::ostringstream::out);
		std::string cur_name;
		name_stream << i_LayerName.GetString() << std::setw(2) << std::setfill('0') << i_LayerID;
		cur_name = name_stream.str();
		return nameString(cur_name);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	nameString get_unique_layer_name(const nameString& i_LayerName)
	{
		std::vector<nameString> layer_names;
		get_layer_names(layer_names);

		int layerID = 0;
		nameString cur_name = generate_layer_name(i_LayerName, layerID);
		while(!isValidLayerName(layer_names, cur_name))
		{
			layerID++;
			cur_name = generate_layer_name(i_LayerName, layerID);
		}

		return cur_name;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	/*nameString get_unique_layer_name(rlyrRenderCam* i_Cam)
	{
		std::vector<nameString> layer_names;
		i_Cam->GetLayerNames(layer_names);

		int layerID = 0;
		nameString cur_name = generate_layer_name(layerID);
		while(!isValidLayerName(layer_names, cur_name))
		{
			layerID++;
			cur_name = generate_layer_name(layerID);
		}

		return cur_name;
	}*/

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void add_object_to_layers(rlyrObject* i_Object)
	{
		std::vector<rlyrRenderLayer*>::iterator it, end = l_RenderLayers.end();
		for (it = l_RenderLayers.begin(); it != end; ++it)
		{
			(*it)->AddObject(i_Object);
		}

		for (it = l_HiddenRenderLayers.begin(); it != l_HiddenRenderLayers.end(); ++it)
		{
			(*it)->AddObject(i_Object);
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void remove_object_from_layers(rlyrObject* i_Object)
	{
		std::vector<rlyrRenderLayer*>::iterator it, end = l_RenderLayers.end();
		for (it = l_RenderLayers.begin(); it != end; ++it)
		{
			(*it)->RemoveObject(i_Object);
		}

		for (it = l_HiddenRenderLayers.begin(); it != l_HiddenRenderLayers.end(); ++it)
		{
			(*it)->RemoveObject(i_Object);
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void init_objects_in_layer(rlyrRenderLayer* io_RenderLayer)
	{
		std::vector<rlyrObject*>::iterator it, end = l_Objects.end();
		for (it = l_Objects.begin(); it != end; ++it)
		{
			io_RenderLayer->AddObject((*it));
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void get_all_layers(std::vector<rlyrRenderLayer*> &o_AllLayers)
	{
		std::vector<rlyrRenderCam*>::iterator it, end = l_RenderCams.end();
		std::vector<rlyrRenderLayer*>::iterator it2, end2;
		for (it = l_RenderCams.begin(); it != end; ++it)
		{
			end2 = (*it)->m_RenderLayers.end();
			for (it2 = (*it)->m_RenderLayers.begin(); it2 != end2; ++it2)
				o_AllLayers.push_back((*it2));
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void clear_camera_lists()
	{
		//clear all render layers from cameras, should be done only when resetting 
		//the entire system. for example, when reading in a scene data chunk
		std::vector<rlyrRenderCam*>::iterator it, end = l_RenderCams.end();
		for (it = l_RenderCams.begin(); it != end; ++it)
		{
			(*it)->RemoveAllLayers();
			(*it)->m_RenderLayers.clear();
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void clear_layer_lists()
	{
		envSTLHelpers::DeleteContainer(l_RenderLayers);
		l_RenderLayers.clear();
	}

}	// end of namespace

namespace rlyrRenderLayerMgr
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Initialize()
	{
		CreateMasterLayer();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void DeInitialize()
	{
		// Clean up
		envSTLHelpers::DeleteContainer(l_Objects);
		envSTLHelpers::DeleteContainer(l_RenderCams);
		envSTLHelpers::DeleteContainer(l_RenderLayers);
		envSTLHelpers::DeleteContainer(l_HiddenRenderLayers);
	}

	
	//------------------------------------------------------------------------
	//	Create the master layer
	//------------------------------------------------------------------------
	void CreateMasterLayer()
	{
		rlyrRenderLayer *masterLayer = new rlyrRenderLayer(nameString("Master"), true);
		init_objects_in_layer(masterLayer);
		l_RenderLayers.push_back(masterLayer);
	}

	//------------------------------------------------------------------------
	//	Create the master layer
	//------------------------------------------------------------------------
	void CreateMasterLayer(rlyrRenderCam* i_Cam)
	{
		rlyrRenderLayer *masterLayer = new rlyrRenderLayer(nameString("Master"), true);
		init_objects_in_layer(masterLayer);
		i_Cam->AddLayer(masterLayer);
	}

	//------------------------------------------------------------------------
	//	Update the layers with the information from the document chunk
	//------------------------------------------------------------------------
	void Update()
	{
		if( l_LayerData.m_Layers.size() == 0 )
			return;

		//if the data is not up to date, run through the layer data and update them
		clear_layer_lists();
		nameString layer_name, init_name, object_name;
		bool object_visible;
		bool master_layer;
		int node_index;
		bool node_visible;
		std::vector<rlyrLayerDataItem>::iterator it, end = l_LayerData.m_Layers.end();
		for (it = l_LayerData.m_Layers.begin(); it != end; ++it)
		{
			//parent_name = nameString((*it).m_ParentName.GetValue());
			l_isChunkAdd = true;
		
			master_layer = false;
			if( (*it).m_Name.GetValue() == "Master" )
				master_layer = true;

			init_name = AddRenderLayer( master_layer );

			layer_name = nameString((*it).m_Name.GetValue());
			ChangeLayerName(init_name, layer_name);

			SetLayerActive(layer_name, (*it).m_IsActive.GetValue());

			//update capture options
			SetLayerCaptureOptions(layer_name, (*it).m_OutputFormat);
			
			//update render prefs
			SetLayerRenderPrefs(layer_name, (*it).m_RenderPrefs);

			//update render passes
			SetLayerRenderPasses(layer_name, (*it).m_RenderPasses);

			//update render prefs
			SetLayerPfxData(layer_name, (*it).m_PostEffect);

			std::vector<rlyrObjectDataItem>::iterator it2, end2= (*it).m_Objects.end();
			for (it2 = (*it).m_Objects.begin(); it2 != end2; ++it2)
			{
				object_name = nameString((*it2).m_Name.GetValue());
				object_visible = (*it2).m_IsVisible.GetValue();
				SetLayerObjectState(layer_name, object_name, object_visible);

				//set the node values
				int node_size = (*it2).m_Nodes.size();
				for ( int i = 0; i < node_size; ++i )
				{
					node_index = (*it2).m_Nodes[i].m_Index.GetValue();
					node_visible = (*it2).m_Nodes[i].m_IsVisible.GetValue();
					SetLayerFragmentState(layer_name, object_name, node_index, node_visible);
				}
			}
		}
		l_LayerData.m_Layers.clear();
	}

	//------------------------------------------------------------------------
	//set the layer data gathered from the document chunk
	//------------------------------------------------------------------------
	void SetData(rlyrLayersData &i_LayerData)
	{
		l_LayerData = i_LayerData;
	}

	//------------------------------------------------------------------------
	//return the layer data for the document chunk
	//------------------------------------------------------------------------
	void GetData(rlyrLayersData &o_LayerData)
	{
		o_LayerData.m_Layers.clear();

		rlyrLayerDataItem cur_layer;
		rlyrObjectDataItem cur_object;
		rlyrNodeDataItem cur_node;

		std::vector<rlyrRenderLayer*>::iterator it, end = l_RenderLayers.end();
		for (it = l_RenderLayers.begin(); it != end; ++it)
		{
			//assign current layers properties to the data object
			cur_layer.m_Name.SetValue((*it)->GetName().GetString());
			//cur_layer.m_ParentName.SetValue((*it)->GetParentCam()->GetName().GetString());
			cur_layer.m_IsActive.SetValue((*it)->GetActiveState());
			cur_layer.m_OutputFormat = (*it)->GetCaptureOptions()->m_Data;
			cur_layer.m_RenderPrefs = (*it)->GetRenderPrefs()->m_Data;
			cur_layer.m_RenderPasses = (*it)->GetRenderPasses()->m_Data;
			cur_layer.m_PostEffect = (*it)->GetPostEffect()->GetData();
			
			cur_layer.m_Objects.clear();
			//gather object data for each layer
			//std::map<rlyrObject*, bool>::iterator it2, end2 = (*it)->GetObjectMap().end();
			std::vector<rlyrObject*>::iterator it2, end2 = l_Objects.end();
			for (it2 = l_Objects.begin(); it2 != end2; ++it2)
			{
				cur_object.m_Name.SetValue( (*it2)->GetName().GetString() );
				cur_object.m_IsVisible.SetValue( (*it)->GetObjectMap()[*(it2)]->m_bObjectActive );
				
				cur_object.m_Nodes.clear();
				//gather the node data for the current object
				int node_size = (*it2)->GetNumNodes();
				for ( int i = 0; i < node_size; ++i )
				{
					cur_node.m_Index.SetValue(i);
					cur_node.m_IsVisible.SetValue((*it)->GetFragmentState((*it2), i));
					cur_object.m_Nodes.push_back(cur_node);
				}
				cur_layer.m_Objects.push_back(cur_object);
			}

			o_LayerData.m_Layers.push_back(cur_layer);
		}
	}

	//------------------------------------------------------------------------
	//	merge a set of layer data from the chunk with our current layer data
	//
	//	If i_bRenameDupes is true, the duplicate entry will be renamed
	//	and merged.  If it is false, the duplicate will NOT be merged.
	//--------------------------------------------------------------------
	void MergeData(rlyrLayersData &i_LayerData, bool i_bRenameDupes)
	{
		nameString layer_name, init_name, object_name;
		std::vector<rlyrLayerDataItem>::iterator it, end = i_LayerData.m_Layers.end();
		for (it = i_LayerData.m_Layers.begin(); it != end; ++it)
		{
			//parent_name = nameString((*it).m_ParentName.GetValue());
			l_isChunkAdd = true;
			init_name = AddRenderLayer();

			layer_name = nameString((*it).m_Name.GetValue());
			if(!ChangeLayerName(init_name, layer_name))
			{
				layer_name = get_unique_layer_name(layer_name);
				ChangeLayerName(init_name, layer_name);
			}

			SetLayerActive(layer_name, (*it).m_IsActive.GetValue());

			//update capture options
			SetLayerCaptureOptions(layer_name, (*it).m_OutputFormat);

			//update render prefs
			SetLayerRenderPrefs(layer_name, (*it).m_RenderPrefs);

			//update render prefs
			SetLayerRenderPasses(layer_name, (*it).m_RenderPasses);

			//update render prefs
			SetLayerPfxData(layer_name, (*it).m_PostEffect);

			//don't import object visibility flags, scene objects may not match
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void AddToLayerRenderDataList(const nameString& i_LayerName, 
								  fsLocator& i_RenderLocation, int i_RenderTime)
	{
		rlyrRenderLayer* pLayer = getRenderLayerByName(i_LayerName);
		if( pLayer != NULL )
		{
			pLayer->AddToRenderDataList(i_RenderLocation, i_RenderTime);
		}
	}
	
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void AddToLayerRenderDataList(const nameString& i_LayerName, RenderData& i_RenderData)
	{
		rlyrRenderLayer* pLayer = getRenderLayerByName(i_LayerName);
		if( pLayer != NULL )
		{
			pLayer->AddToRenderDataList(i_RenderData);
		}

	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ClearLayerRenderDataList(const nameString& i_LayerName)
	{
		rlyrRenderLayer* pLayer = getRenderLayerByName(i_LayerName);
		if( pLayer != NULL )
		{
			pLayer->ClearRenderDataList();
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ClearAllRenderDataList()
	{
		for (int i = 0; i < l_RenderLayers.size(); i++)
			l_RenderLayers[i]->ClearRenderDataList();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void GetLayerRenderDataList(const nameString& i_LayerName, std::vector<RenderData>& o_DataList)
	{
		rlyrRenderLayer* pLayer = getRenderLayerByName(i_LayerName);
		if( pLayer != NULL )
		{
			o_DataList = pLayer->GetRenderDataList();
		}
	}

	//------------------------------------------------------------------------
	//  Add named camera as a render cam for the render layer tree
	//------------------------------------------------------------------------
	void  AddRenderCam(nameObject* i_pNameObj)
	{
		rlyrRenderCam* cam = new rlyrRenderCam(i_pNameObj);
		CreateMasterLayer(cam);
		l_RenderCams.push_back(cam);
	}

	//------------------------------------------------------------------------
	//	Remove render camera from manager (removing all render layers belonging
	//		to this camera)
	//------------------------------------------------------------------------
	void RemoveRenderCam(nameObject* i_pNameObj)
	{
		for (int i = 0; i < l_RenderCams.size(); i++)
		{
			if( l_RenderCams[i]->m_NameObject == i_pNameObj )
			{
				envSTLHelpers::DeleteOneValue(l_RenderCams, l_RenderCams[i]);
			}
		}
	}

	////------------------------------------------------------------------------
	//// return the index of the camera with the given name
	////------------------------------------------------------------------------
	//int GetCameraIndexByName( const nameString& i_CameraName )
	//{
	//	for (int i = 0; i < l_RenderCams.size(); i++)
	//		if( l_RenderCams[i]->GetName() == i_CameraName )
	//			return i;
	//	return -1;
	//}

	//------------------------------------------------------------------------
	// return the index of the layer at the camera with the given index
	//------------------------------------------------------------------------
	int GetLayerIndexByName( const nameString& i_LayerName )
	{
		for (int i = 0; i < l_RenderLayers.size(); i++)
			if( l_RenderLayers[i]->GetName() == i_LayerName )
				return i;
		return -1;
	}

	//------------------------------------------------------------------------
	// return the number of cameras in the manager
	//------------------------------------------------------------------------
	/*int GetNumCameras( )
	{
		return l_RenderCams.size();
	}*/

	//------------------------------------------------------------------------
	// return the number of cameras in the manager
	//------------------------------------------------------------------------
	int GetNumRenderLayers()
	{
		return l_RenderLayers.size();
	}

	//------------------------------------------------------------------------
	// return the number of active cameras in the manager
	//------------------------------------------------------------------------
	//int GetNumActiveCameras()
	//{
	//	int active_count = 0;
	//	for (int i = 0; i < l_RenderCams.size(); i++)
	//	{
	//		for ( int j = 0; j < l_RenderCams[i]->m_RenderLayers.size(); j++)
	//		{
	//			//if the camera has one active layer, it is an active camera
	//			if(l_RenderCams[i]->m_RenderLayers[j]->GetActiveState())
	//			{
	//				active_count++;
	//				break;
	//			}
	//		}
	//	}
	//	return active_count;
	//}

	////------------------------------------------------------------------------
	//// return the next active camera index
	////------------------------------------------------------------------------
	//int GetNextCameraIndex(int CurrentCamIndex)
	//{
	//	//if the camera has one active layer, it is an active camera
	//	for ( int i = CurrentCamIndex + 1; i < l_RenderCams.size(); i++ )
	//		for ( int j = 0; j < l_RenderCams[i]->m_RenderLayers.size(); j++)
	//			if(l_RenderCams[i]->m_RenderLayers[j]->GetActiveState())
	//				return i;
	//	
	//	return -1;
	//}

	//------------------------------------------------------------------------
	// return the next active layer index, for a specified camera
	//------------------------------------------------------------------------
	int GetNextLayerIndex(int i_CurrentLayerIndex)
	{
		for (int i = i_CurrentLayerIndex + 1; i < l_RenderLayers.size();	i++)
		{
			if(l_RenderLayers[i]->GetActiveState())
				return i;
		}

		return -1;
	}

	//------------------------------------------------------------------------
	// return whether or not there is at least 1 active render layer
	//------------------------------------------------------------------------
	bool CanRender()
	{
		for (int i = 0; i < l_RenderLayers.size();	i++)
		{
			if(l_RenderLayers[i]->GetActiveState())
				return true;
		}

		return false;
	}

	//------------------------------------------------------------------------
	// Add a new layer to the named camera
	//------------------------------------------------------------------------
	const nameString AddRenderLayer(bool i_IsMasterLayer)
	{
		nameString layer_name;
		if(!l_isChunkAdd)
			layer_name = get_unique_layer_name(nameString("Layer"));
		else
			layer_name = nameString("temp");

		rlyrRenderLayer *newLayer = new rlyrRenderLayer(layer_name, i_IsMasterLayer);
		init_objects_in_layer(newLayer);
		l_RenderLayers.push_back(newLayer);

		newLayer->UpdateRenderPassesVisbility();
		
		l_isChunkAdd = false;
		
		return layer_name;
	}

	//------------------------------------------------------------------------
	//  Add a new hidden layer
	//------------------------------------------------------------------------
	rlyrRenderLayer* AddHiddenRenderLayer(const nameString& i_HiddenLayerName)
	{
		// check if a layer with the same name is already in the list
		if (getRenderLayerByName(i_HiddenLayerName) != NULL)
			return NULL;	

		rlyrRenderLayer *newLayer = new rlyrRenderLayer(i_HiddenLayerName, false);
		init_objects_in_layer(newLayer);
		l_HiddenRenderLayers.push_back(newLayer);

		return newLayer;
	}

	//------------------------------------------------------------------------
	// make a duplicate of a current render layer of a camera
	//------------------------------------------------------------------------
	const nameString CopyRenderLayer( const nameString& i_CopyLayer)
	{
		rlyrRenderLayer* copyLayer = getRenderLayerByName(i_CopyLayer);
		nameString layer_name = get_unique_layer_name(nameString("Layer"));
		if(copyLayer != NULL)
		{
			rlyrRenderLayer *newLayer = new rlyrRenderLayer(layer_name, *copyLayer);
			l_RenderLayers.push_back(newLayer);
		}

		return layer_name;
	}

	//------------------------------------------------------------------------
	// return the name of the layer at the index
	//------------------------------------------------------------------------
	const nameString GetLayerName( int i_LayerIndex )
	{
		nameString layer_name = l_RenderLayers[i_LayerIndex]->GetName();
		return layer_name;
	}

	//------------------------------------------------------------------------
	//  Add named object to list
	//------------------------------------------------------------------------
	void  AddObject(nameObject* i_pNameObj, 
				   api3dObjectSingle* i_pObject)
	{
		rlyrObject* obj = new rlyrObject(i_pNameObj,i_pObject);
		l_Objects.push_back(obj);
		add_object_to_layers(obj);
		notify_interests_added();
	}

	//------------------------------------------------------------------------
	//	Remove object from manager (removing from all layers)
	//------------------------------------------------------------------------
	void RemoveObject(nameObject* i_pNameObj, 
				      api3dObjectSingle* i_pObject)
	{
		for (int i = 0; i < l_Objects.size(); i++)
		{
			if( l_Objects[i]->m_pNameObj == i_pNameObj )
			{
				remove_object_from_layers(l_Objects[i]);
				envSTLHelpers::DeleteOneValue(l_Objects, l_Objects[i]);
			}
		}
	
	}

	//------------------------------------------------------------------------
	//	Before rendering, get the original editor visibilty levels for each
	//	object in the scene
	//------------------------------------------------------------------------
	void GetObjectsEditorVisibility()
	{
		bool bVisible;
		l_EditorObjects.clear();
		std::vector<rlyrObject*>::iterator it, end = l_Objects.end();
		for (it = l_Objects.begin(); it != end; ++it)
		{
			bVisible = visMgr::GetVisibleInEditor((*it)->GetName().GetString());
			l_EditorObjects[(*it)->GetName()] = bVisible;
		}
	}

	//------------------------------------------------------------------------
	//	Get visibility for a single object
	//------------------------------------------------------------------------
	bool GetSingleObjectVisibility(nameString i_Name)
	{
		for (std::map<nameString, bool>::const_iterator it = object_status.begin(); it != object_status.end(); ++it)
		{
			if ( it->first == i_Name )
			{
				return it->second;
			}
		}
		return true;
	}
	
	//------------------------------------------------------------------------
	//  Set the editor visibility to match the visible prefs for
	//  the given layer.
	//------------------------------------------------------------------------
	void SetupLayerObjectsVisible(int i_RenderLayerIndex)
	{
		//get the layer's personal map and then apply it
		object_status.clear();
		rlyrRenderLayer* pLayer = l_RenderLayers[i_RenderLayerIndex];
		if(pLayer != NULL)
		{
			pLayer->GetObjectVisiblity( object_status );
			SetObjectsEditorVisibility( object_status );
			SetFragmentVisibility( pLayer );
		}
	}

	//------------------------------------------------------------------------
	//  Set the editor visibility to match the visible prefs for
	//  the given layer name.
	//------------------------------------------------------------------------
	void SetupLayerObjectsVisible(const nameString& i_RenderLayerName)
	{
		//get the layer's personal map and then apply it
		object_status.clear();
		rlyrRenderLayer* pLayer = getRenderLayerByName(i_RenderLayerName);
		if(pLayer != NULL)
		{
			pLayer->GetObjectVisiblity( object_status );
			SetObjectsEditorVisibility( object_status );
			SetFragmentVisibility( pLayer );
		}
	}


	//------------------------------------------------------------------------
	// Return true if we have hidden some objects from a call to 
	//	SetObjectsEditorVisibility. 
	//------------------------------------------------------------------------
	bool HaveHiddenObjects()
	{
		return l_bHaveHidden;
	}

	//------------------------------------------------------------------------
	//	after and during rendering, set the editor visibility for each object
	//	based on the given std::map pairing
	//------------------------------------------------------------------------
	void SetObjectsEditorVisibility(const std::map<nameString, bool>& i_ObjectStatus)
	{
		l_bHaveHidden = false; // keep track of if we have any hidden objects

		bool visible;
		std::vector<rlyrObject*>::iterator it, end = l_Objects.end();
		for (it = l_Objects.begin(); it != end; ++it)
		{
			//visMgr::SetVisibleInEditor((*it)->GetName().GetString(), 
			//							i_ObjectStatus[(*it)->GetName()]);

			//bga - changed to const lookup
			//visible = i_ObjectStatus[(*it)->GetName()];
			std::map<nameString, bool>::const_iterator vit = i_ObjectStatus.find((*it)->GetName());
			if (vit == i_ObjectStatus.end())
				visible = false;
			else
				visible = vit->second;

			visMgr::SetActiveInRenderLayer((*it)->GetName().GetString(), visible);

			if (!visible)
				l_bHaveHidden = true;
		}
	}

	//------------------------------------------------------------------------
	//	after and during rendering, set the rendering flags for the nodes of each object
	//------------------------------------------------------------------------
	void SetFragmentVisibility(rlyrRenderLayer* i_pLayer)
	{	
		std::vector<rlyrObject*>::iterator it, end = l_Objects.end();
		for (it = l_Objects.begin(); it != end; ++it)
		{
			if( i_pLayer->GetObjectState((*it)) )
			{
				int num_nodes = (*it)->GetNumNodes();
				for ( int i = 0; i < num_nodes; ++i)
				{
					if( i_pLayer->GetFragmentState( (*it), i ) )
					{
						//turn on fragment visibility
						i_pLayer->SetFragmentActive( (*it), i, true );
					}
					else
					{
						//turn off fragment visibility
						i_pLayer->SetFragmentActive( (*it), i, false );
					}
				}
			}
		}
	}	
	
	//------------------------------------------------------------------------
	//	Set fragment visibility for an object without referencing
	//	a named render layer. Used in lighting isolation.
	//------------------------------------------------------------------------
	void SetFragmentVisibility(const nameString &i_ObjectName,
							   const std::vector<int>& i_LitFragmentIndices)
	{	
		std::vector<rlyrObject*>::iterator it, end = l_Objects.end();
		for (it = l_Objects.begin(); it != end; ++it)
		{
			if ((*it)->GetName() == i_ObjectName)
			{
				// Turn off all nodes first
				int num_nodes = (*it)->GetNumNodes();
				for ( int n = 0; n < num_nodes; ++n )
				{
					(*it)->Node(n)->SetNodeActive(false);
				}

				// Then turn on the ones in the list
				int num_lit = i_LitFragmentIndices.size();
				for ( int i = 0; i < num_lit; ++i)
				{
					(*it)->Node(i_LitFragmentIndices[i])->SetNodeActive(true);
				}
			}
		}
	}

	//------------------------------------------------------------------------
	//	after capture restore the editor to the default state
	//------------------------------------------------------------------------
	void RestoreEditorVisibility()
	{
		SetObjectsEditorVisibility(l_EditorObjects);
	}

	//------------------------------------------------------------------------
	//	after capture restore the fragments to their visible state
	//------------------------------------------------------------------------
	void RestoreFragmentVisibilty()
	{
		rlyrRenderLayerNode* cur_node;
		int num_nodes;
		std::vector<rlyrObject*>::iterator it, end = l_Objects.end();
		for (it = l_Objects.begin(); it != end; ++it)
		{
			num_nodes = (*it)->GetNumNodes();
			for ( int n = 0; n < num_nodes; ++n )
			{
				cur_node = (*it)->Node(n);
				cur_node->SetNodeActive(true);
			}
		}
	}

	//------------------------------------------------------------------------
	//	after capture restore the visibility to ture
	//------------------------------------------------------------------------
	void RestoreActiveInRenderLayer()
	{
		std::map<nameString, bool> objectStatus(l_EditorObjects);
		std::vector<rlyrObject*>::iterator it, end = l_Objects.end();
		for (it = l_Objects.begin(); it != end; ++it)
		{
			objectStatus[(*it)->GetName()] = true;
		}
		SetObjectsEditorVisibility(objectStatus);
		objectStatus.clear();
		RestoreFragmentVisibilty();
	}

	//------------------------------------------------------------------------
	// Get the visibility flags of the object from each layer
	//------------------------------------------------------------------------
	void GetVisibilityForObject(const nameString& i_ObjectName, std::vector<nameString>& o_LayerNames)
	{
		//make sure manager is up to date
		Update();
		o_LayerNames.clear();
		rlyrObject* pObj = getObjectByName(i_ObjectName);
		if( pObj != NULL )
		{
			std::vector<rlyrRenderLayer*>::iterator it, end = l_RenderLayers.end();
			for (it = l_RenderLayers.begin(); it != end; ++it)
			{
				//when an object is added, they are by default enabled for each layer, 
				//so when an undo/redo is done, we only keep track of the layers that have
				//the object disabled
				if(!(*it)->GetObjectState(pObj))
					o_LayerNames.push_back((*it)->GetName());
			}
		}
	}

	//------------------------------------------------------------------------
	// Set the visibility flags of the object for each layer
	//------------------------------------------------------------------------
	void SetVisibilityForObject(const nameString& i_ObjectName, std::vector<nameString> i_LayerNames)
	{
		rlyrObject* pObj = getObjectByName(i_ObjectName);
		if( pObj != NULL )
		{
			rlyrRenderLayer* curLayer;
			std::vector<nameString>::iterator it, end = i_LayerNames.end();
			for (it = i_LayerNames.begin(); it != end; ++it)
			{
				//when an object is added, they are by default enabled for each layer, 
				//so when an undo/redo is done, we only keep track of the layers that have
				//the object disabled
				//if the layer is in this list, they need to disable the object
				curLayer = getRenderLayerByName((*it));
				if( curLayer != NULL )
					curLayer->SetObjectState( pObj, false);
			}
		}
	}

	//------------------------------------------------------------------------
	// Get the number of nodes for the given object
	//------------------------------------------------------------------------
	int GetObjectNumNodes(const nameString& i_ObjectName)
	{
		rlyrObject* pObj = getObjectByName(i_ObjectName);
		if( pObj != NULL )
		{
			return pObj->GetNumNodes();
		}
		return 0;
	}

	//------------------------------------------------------------------------
	//	Change assignments from OldNameObj to NewNameObj (both of which
	//	should be registered with the manager at the point of calling 
	//	this function). This is used when reloading or replacing 
	//	geometry in a system.
	//------------------------------------------------------------------------
	void ReplaceObject(nameObject* i_pOldNameObj, 
					   nameObject* i_pNewNameObj)
	{
		
	}

	//------------------------------------------------------------------------
	//	Notify the environment manager that the name of this object has changed
	//------------------------------------------------------------------------
	void ObjectRenamed(nameObject* i_pNameObj)
	{
		notify_interests_renamed();
	}

	//------------------------------------------------------------------------
	// set the active state of an object for a given layer
	//------------------------------------------------------------------------
	void SetLayerObjectState( const nameString& i_LayerName, const nameString& i_ObjectName, bool i_Active)
	{
		rlyrRenderLayer* pLayer = getRenderLayerByName(i_LayerName);
		rlyrObject* pObj = getObjectByName(i_ObjectName);
		if((pLayer != NULL) && (pObj != NULL))
		{
			pLayer->SetObjectState(pObj, i_Active);
		}
	}

	//------------------------------------------------------------------------
	// get the active state of an object for a given layer
	//------------------------------------------------------------------------
	bool GetLayerObjectState( const nameString& i_LayerName, const nameString& i_ObjectName)
	{
		rlyrRenderLayer* pLayer = getRenderLayerByName(i_LayerName);
		rlyrObject* pObj = getObjectByName(i_ObjectName);
		if((pLayer != NULL) && (pObj != NULL))
		{
			return pLayer->GetObjectState(pObj);
		}
		//assertion will be hit if object not found
		return false;
	}

	//------------------------------------------------------------------------
	// Get the names of the fragments belonging to the object
	//------------------------------------------------------------------------
	void GetFragmentNodeNames( const nameString& i_ObjectName, std::vector<std::string>& o_NodeNames )
	{
		rlyrObject* pObject = getObjectByName(i_ObjectName);
		if (pObject)
		{
			const int num_nodes = pObject->GetNumNodes();
			o_NodeNames.resize(num_nodes);
			for (int i=0; i<num_nodes; ++i)
			{
				o_NodeNames[i] = pObject->GetNodeName(i);
			}
		}
	}

	//------------------------------------------------------------------------
	// Get the names of the fragments belonging to the object
	//------------------------------------------------------------------------
	void GetRenderedFragmentIndices( const nameString& i_ObjectName, std::vector<int>& o_FragmentIndices )
	{

	}

	//------------------------------------------------------------------------
	// Set the render state of the indexed fragment for the given object
	//------------------------------------------------------------------------
	void SetLayerFragmentState(const nameString& i_LayerName, const nameString& i_ObjectName, int i_FragmentIndex, bool i_Active)
	{
		rlyrRenderLayer* pLayer = getRenderLayerByName(i_LayerName);
		rlyrObject* pObj = getObjectByName(i_ObjectName);
		if((pLayer != NULL) && (pObj != NULL))
		{
			pLayer->SetFragmentState(pObj, i_FragmentIndex, i_Active);
		}
	}

	//------------------------------------------------------------------------
	// Get the render state of the indexed fragment for the given object
	//------------------------------------------------------------------------
	bool GetLayerFragmentState(const nameString& i_LayerName, const nameString& i_ObjectName, int i_FragmentIndex)
	{
		rlyrRenderLayer* pLayer = getRenderLayerByName(i_LayerName);
		rlyrObject* pObj = getObjectByName(i_ObjectName);
		if((pLayer != NULL) && (pObj != NULL))
		{
			return pLayer->GetFragmentState(pObj, i_FragmentIndex);
		}
		return false;
	}

	//------------------------------------------------------------------------
	//	Allow the manager to set the active state of a particular layer
	//------------------------------------------------------------------------
	void SetLayerActive( const nameString& i_LayerName, bool i_IsActive)
	{
		rlyrRenderLayer* pLayer = getRenderLayerByName(i_LayerName);
		if(pLayer != NULL)
		{
			pLayer->SetActiveState(i_IsActive);
		}
	}

	//------------------------------------------------------------------------
	//	Allow the manager to set the active state of all layers belonging to the camera
	//------------------------------------------------------------------------
	void SetAllLayersActive( bool i_IsActive)
	{
		std::vector<rlyrRenderLayer*>::iterator it, end = l_RenderLayers.end();
		for (it = l_RenderLayers.begin(); it != end; ++it)
		{
			(*it)->SetActiveState(i_IsActive);
		}
	}

	//------------------------------------------------------------------------
	//	Allow the manager to get the active state of a particular layer
	//------------------------------------------------------------------------
	bool GetLayerActive( const nameString& i_LayerName)
	{
		rlyrRenderLayer* pLayer = getRenderLayerByName(i_LayerName);
		if(pLayer != NULL)
		{
			return pLayer->GetActiveState();
		}
		return false;
	}

	//------------------------------------------------------------------------
	//	Change the name of a layer - returns true on success
	//------------------------------------------------------------------------
	bool ChangeLayerName(const nameString& i_OldName, const nameString& i_NewName)
	{
		if( i_NewName.GetString() == "" )
			return false;

		rlyrRenderLayer* pLayer = getRenderLayerByName(i_OldName);
		if( pLayer == NULL )
			return false;

		std::vector<nameString> layerNames;
		get_layer_names(layerNames);
		if( isValidLayerName( layerNames, i_NewName ) )
		{
			pLayer->SetName(i_NewName);
			return true;
		}
		
		return false;
	}

	//------------------------------------------------------------------------
	// Returns true if non-empty and unique name for a render layer.
	//------------------------------------------------------------------------
	bool IsValidRenderLayerName(const std::string &i_Name)
	{
		//if (i_Name.empty()) return false;

		//std::vector<rlyrRenderLayer*>::iterator it, end = l_RenderLayers.end();
		//for (it = l_RenderLayers.begin(); it != end; ++it)
		//{
		//	if ((*it)->GetName().GetString() == i_Name)
		//		return false;
		//}
		return true;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void  DeleteRenderLayer( const nameString& i_RemovedLayer)
	{
		for (int i = 0; i < l_RenderLayers.size(); i++)
		{
			if( l_RenderLayers[i]->GetName() == i_RemovedLayer )
			{
				envSTLHelpers::DeleteOneValue(l_RenderLayers, l_RenderLayers[i]);
			}
		}

		for (int i = 0; i < l_HiddenRenderLayers.size(); i++)
		{
			if (l_HiddenRenderLayers[i]->GetName() == i_RemovedLayer)
			{
				envSTLHelpers::DeleteOneValue(l_HiddenRenderLayers, l_HiddenRenderLayers[i]);
			}
		}
	}

	//------------------------------------------------------------------------
	//  Get names of render layers
	//------------------------------------------------------------------------
	void GetRenderLayerNames(std::vector<nameString> &o_Names)
	{
		std::vector<rlyrRenderLayer*>::iterator it, end = l_RenderLayers.end();
		for (it = l_RenderLayers.begin(); it != end; ++it)
		{
			o_Names.push_back((*it)->GetName());		
		}
	}

	//------------------------------------------------------------------------
	//  Get names of active objects in the render layer. Use empty
	//	i_SetName ("") to ask about "unassigned" objects.
	//------------------------------------------------------------------------
	void GetActiveObjects(const nameString& i_SetName, 
						 std::vector<nameString> &o_ObjectNames)
	{

	}

	//------------------------------------------------------------------------
	//  Get list of all names of objects registered in the manager
	//------------------------------------------------------------------------
	void GetAllObjects(std::vector<nameString> &o_ObjectNames)
	{
		std::vector<rlyrObject*>::iterator it, end = l_Objects.end();
		for (it = l_Objects.begin(); it != end; ++it)
		{
			o_ObjectNames.push_back((*it)->m_pNameObj->GetName());
		}
	}
	
	//------------------------------------------------------------------------
	//  Get list of all names of render cams registered in the manager
	//------------------------------------------------------------------------
	void GetRenderCamNames(std::vector<nameString> &o_CamNames)
	{
		std::vector<rlyrRenderCam*>::iterator it, end = l_RenderCams.end();
		for (it = l_RenderCams.begin(); it != end; ++it)
		{
			o_CamNames.push_back((*it)->GetName());
		}
	}

	//------------------------------------------------------------------------
	//  Get the render prefs of a layer
	//------------------------------------------------------------------------
	rprfPrefsObject* GetLayerRenderPrefs(const nameString& i_RenderLayer)
	{
		rlyrRenderLayer* pLayer = getRenderLayerByName(i_RenderLayer);
		if(pLayer != NULL)
		{
			return pLayer->GetRenderPrefs();
		}
		return NULL;
	}

	//------------------------------------------------------------------------
	//  Get the render passes of a layer
	//------------------------------------------------------------------------
	rlyrPassesObject* GetLayerRenderPasses(const nameString& i_RenderLayer)
	{
		rlyrRenderLayer* pLayer = getRenderLayerByName(i_RenderLayer);
		if(pLayer != NULL)
		{
			return pLayer->GetRenderPasses();
		}
		return NULL;
	}

	//------------------------------------------------------------------------
	//  Get the render passes of a layer
	//------------------------------------------------------------------------
	pfxPostEffectObject* GetLayerRenderPfx(const nameString& i_RenderLayer)
	{
		rlyrRenderLayer* pLayer = getRenderLayerByName(i_RenderLayer);
		if(pLayer != NULL)
		{
			return pLayer->GetPostEffect();
		}
		return NULL;
	}

	//------------------------------------------------------------------------
	//  Apply the prefs of the layer to the scene
	//------------------------------------------------------------------------
	void ApplyLayerPrefs(int i_RenderLayerIndex)
	{
		rlyrRenderLayer* pLayer = l_RenderLayers[i_RenderLayerIndex];
		if(pLayer != NULL)
			pLayer->ApplyPrefs();
	}

	void ApplyLayerPrefs(nameString i_RenderLayerName)
	{
		rlyrRenderLayer* pLayer = getRenderLayerByName(i_RenderLayerName);
		if(pLayer != NULL)
			pLayer->ApplyPrefs();
	}

	//------------------------------------------------------------------------
	//  return the g3dPrefs of the layer to the scene
	//------------------------------------------------------------------------
	const g3dPrefs::g3dRenderPrefs& GetLayerActualData(int i_RenderLayerIndex)
	{
		rlyrRenderLayer* pLayer = l_RenderLayers[i_RenderLayerIndex];
		if(pLayer != NULL)
			return pLayer->GetActualData();

		return g3dPrefs::CurrentPrefs();
	}

	//------------------------------------------------------------------------
	//  Set the render prefs of a layer
	//------------------------------------------------------------------------
	void SetLayerRenderPrefs( const nameString& i_RenderLayer, rprfPrefsData& i_NewRenderPrefs)
	{
		rlyrRenderLayer* pLayer = getRenderLayerByName(i_RenderLayer);
		if(pLayer != NULL)
			pLayer->SetRenderPrefs(i_NewRenderPrefs);
	}

	//------------------------------------------------------------------------
	//  Set the render passes of a layer
	//------------------------------------------------------------------------
	void SetLayerRenderPasses( const nameString& i_RenderLayer, rlyrPassesData& i_NewRenderPasses)
	{
		rlyrRenderLayer* pLayer = getRenderLayerByName(i_RenderLayer);
		if(pLayer != NULL)
			pLayer->SetRenderPasses(i_NewRenderPasses);
	}

	//------------------------------------------------------------------------
	//  Set the pfx data of a layer
	//------------------------------------------------------------------------
	void SetLayerPfxData( const nameString& i_RenderLayer, pfxData& i_pfxData)
	{
		rlyrRenderLayer* pLayer = getRenderLayerByName(i_RenderLayer);
		if(pLayer != NULL)
			pLayer->SetPostEffect(i_pfxData);
	}

	//------------------------------------------------------------------------
	//  Get the capture options of a layer
	//------------------------------------------------------------------------
	captRenderOutputObject* GetLayerCaptureOptions( const nameString& i_RenderLayer)
	{
		rlyrRenderLayer* pLayer = getRenderLayerByName(i_RenderLayer);
		if(pLayer != NULL)
		{
			return pLayer->GetCaptureOptions();
		}
		return NULL;
	}

	//------------------------------------------------------------------------
	//  set the capture options of a layer
	//------------------------------------------------------------------------
	void SetLayerCaptureOptions( const nameString& i_RenderLayer, captRenderOutputData& i_NewCaptureData)
	{
		rlyrRenderLayer* pLayer = getRenderLayerByName(i_RenderLayer);
		if(pLayer != NULL)
		{
			pLayer->SetCaptureOptions(i_NewCaptureData);
			pLayer->GetCaptureOptions()->ConvertQuickTime();
		}
	}

	//------------------------------------------------------------------------
	//  apply the individual capture options of the layer to the 
	//	global options
	//------------------------------------------------------------------------
	void ApplyLayerCaptureOptions(int i_RenderLayerIndex)
	{
		captRenderOutputObject* layer_options;
		rlyrRenderLayer* pLayer = l_RenderLayers[i_RenderLayerIndex];
		if(pLayer != NULL)
		{
			layer_options = pLayer->GetCaptureOptions();
			captRenderOutputDataUtil::SetLayerData(layer_options->m_Data);
		}
	}

	//------------------------------------------------------------------------
	//  setup controls for the layers capture dialog
	//------------------------------------------------------------------------
	void SetupLayerCaptureControls( const nameString& i_RenderLayer)
	{
		captRenderOutputObject* layer_options = GetLayerCaptureOptions(i_RenderLayer);
		//if( layer_options != NULL )
		//	layer_options->SetupControls(false);
	}

	//------------------------------------------------------------------------
	//  Get list of all names of render cams registered in the manager
	//------------------------------------------------------------------------
	std::vector<rlyrRenderCam*> GetRenderCams()
	{
		return l_RenderCams;
	}

	//------------------------------------------------------------------------
	//  Get list of all names of render layers registered in the manager
	//------------------------------------------------------------------------
	std::vector<rlyrRenderLayer*> GetRenderLayers()
	{
		return l_RenderLayers;
	}

	//------------------------------------------------------------------------
	//	RegisterRenderLayerInterest() - add a render layer interest 
	//------------------------------------------------------------------------
	void RegisterRenderLayerInterest( rlyrRenderLayerInterest* i_pInterest )
	{
		DBG_ASSERT( i_pInterest != 0, "Cannot register a NULL render layer Interest" );
		l_InterestList.push_back( i_pInterest );
	}

	//------------------------------------------------------------------------
	//	UnRegisterRenderLayerInterest() - remove a render layer interest 
	//
	//	Note: this will NOT delete the render layer interest.  It is up to the
	//	registerer.
	//------------------------------------------------------------------------
	void UnRegisterRenderLayerInterest( rlyrRenderLayerInterest* i_pInterest )
	{
		envSTLHelpers::RemoveOneValue( l_InterestList, i_pInterest );
	}

}	// end of namespace
