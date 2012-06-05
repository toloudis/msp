/*****************************************************************************
**	rlyrRenderLayer.cpp
**
**	Keeps track of the render layers
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Support/rlyr/rlyrRenderLayer.hpp"

#include "Support/capt/captRenderOutputObject.hpp"
#include "Support/pfx/pfxPostEffectObject.hpp"
#include "Support/pfx/pfxPostEffectMgr.hpp"
#include "Support/rlyr/private/rlyrRenderLayerNode.hpp"
#include "Support/rlyr/private/rlyrRenderLayerObject.hpp"
#include "Support/rlyr/rlyrPassesObject.hpp"
#include "Support/rlyr/rlyrRenderCam.hpp"
#include "Support/rprf/rprfPrefsObject.hpp"

#include "Core/env/envSTLHelpers.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
rlyrRenderLayer::rlyrRenderLayer(const nameString& i_Name, bool i_IsMasterLayer)
:	m_Name(i_Name),
	m_ParentCam(NULL),
	m_bIsActive(true)
{
	m_CaptureOptions = new captRenderOutputObject(true, i_IsMasterLayer);
	m_CaptureOptions->ConvertQuickTime();
	m_RenderPrefs = new rprfPrefsObject();
	m_RenderPasses = new rlyrPassesObject();
	m_RenderPfx = new pfxPostEffectObject(pfxPostEffectMgr::GetDefaultPfxName(), 
												pfxPostEffectMgr::GetDefaultPfxPath(),
												pfxData::e_RenderLayer);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rlyrRenderLayer::rlyrRenderLayer(const nameString& i_Name, const rlyrRenderLayer& i_CopyLayer)
	: m_Name(i_Name)
{
	m_RenderPrefs = i_CopyLayer.m_RenderPrefs;
	m_ParentCam = i_CopyLayer.m_ParentCam;
	m_bIsActive = i_CopyLayer.m_bIsActive;
	m_ObjectStatus = i_CopyLayer.m_ObjectStatus;
	m_CaptureOptions = new captRenderOutputObject(i_CopyLayer.m_CaptureOptions->m_Data, true);
	m_CaptureOptions->ConvertQuickTime();
	m_RenderPrefs = new rprfPrefsObject(i_CopyLayer.m_RenderPrefs->m_Data, i_CopyLayer.m_RenderPrefs->m_ActualPrefs);
	m_RenderPasses = new rlyrPassesObject(i_CopyLayer.m_RenderPasses->m_Data);
	m_RenderPfx = new pfxPostEffectObject(i_CopyLayer.m_RenderPfx->GetData(), pfxData::e_RenderLayer);

	ObjectState* next_object;
	std::map<rlyrObject*, ObjectState*>::iterator it, end = m_ObjectStatus.end();
	for( it = m_ObjectStatus.begin(); it != end; ++it )
	{
		next_object = new ObjectState();
		next_object->m_bObjectActive = (*it).second->m_bObjectActive;
		next_object->m_FragmentState = (*it).second->m_FragmentState;

		(*it).second = next_object;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rlyrRenderLayer::~rlyrRenderLayer()
{
	delete m_CaptureOptions;
	delete m_RenderPrefs;
	delete m_RenderPasses;
	delete m_RenderPfx;
	
	std::map<rlyrObject*, ObjectState*>::iterator it, end = m_ObjectStatus.end();
	for( it = m_ObjectStatus.begin(); it != end; ++it )
	{
		delete (*it).second;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rlyrRenderLayer::SetName(const nameString& i_Name)
{
	m_Name = i_Name;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
nameString rlyrRenderLayer::GetName()
{
	return m_Name;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rlyrRenderLayer::SetParentCam(rlyrRenderCam* i_ParentCam)
{
	m_ParentCam = i_ParentCam;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rlyrRenderCam* rlyrRenderLayer::GetParentCam()
{
	return m_ParentCam;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rlyrRenderLayer::GetObjectVisiblity(std::map<nameString, bool>& o_ObjectStatus)
{
	o_ObjectStatus.clear();
	std::map<rlyrObject*, ObjectState*>::iterator it, end = m_ObjectStatus.end();
	for( it = m_ObjectStatus.begin(); it != end; ++it )
		o_ObjectStatus[ (*it).first->GetName() ] = (*it).second->m_bObjectActive;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
std::map<rlyrObject*, ObjectState*> rlyrRenderLayer::GetObjectMap()
{
	return m_ObjectStatus;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rlyrRenderLayer::AddObject(rlyrObject* i_Object)
{
	//initialize new objects as active
	ObjectState* object_state = new ObjectState();
	object_state->m_bObjectActive = true;

	for( int i = 0; i < i_Object->GetNumNodes(); ++i )
	{
		object_state->m_FragmentState[i] = true;
	}

	m_ObjectStatus[i_Object] = object_state;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rlyrRenderLayer::RemoveObject(rlyrObject* i_Object)
{
	//remove object from map
	delete m_ObjectStatus[i_Object];
	m_ObjectStatus.erase(i_Object);
	
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rlyrRenderLayer::SetObjectState(rlyrObject* i_Object, bool i_Active)
{
	std::map<rlyrObject*, ObjectState*>::iterator it, end = m_ObjectStatus.end();
	it = m_ObjectStatus.find(i_Object);
	if( it != end )
	{
		(*it).second->m_bObjectActive = i_Active;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool rlyrRenderLayer::GetObjectState(rlyrObject* i_Object)
{
	std::map<rlyrObject*, ObjectState*>::iterator it, end = m_ObjectStatus.end();
	it = m_ObjectStatus.find(i_Object);
	if( it != end )
	{
		return (*it).second->m_bObjectActive;
	}

	DBG_ASSERT(false, "The object: " << i_Object->GetName().GetString() << " does not exist in this layer.");
	return false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rlyrRenderLayer::SetFragmentState(rlyrObject* i_Object, int i_FragmentIndex, bool i_Active)
{
	std::map<rlyrObject*, ObjectState*>::iterator it, end = m_ObjectStatus.end();
	it = m_ObjectStatus.find(i_Object);
	if( it != end )
	{
		std::map<int, bool>::iterator it2, end2 = (*it).second->m_FragmentState.end();
		it2 = (*it).second->m_FragmentState.find(i_FragmentIndex);
		if(it2 != end2)
		{
			(*it).second->m_FragmentState[i_FragmentIndex] = i_Active;
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool rlyrRenderLayer::GetFragmentState(rlyrObject* i_Object, int i_FragmentIndex)
{
	std::map<rlyrObject*, ObjectState*>::iterator it, end = m_ObjectStatus.end();
	it = m_ObjectStatus.find(i_Object);
	if( it != end )
	{
		std::map<int, bool>::iterator it2, end2 = (*it).second->m_FragmentState.end();
		it2 = (*it).second->m_FragmentState.find(i_FragmentIndex);
		if(it2 != end2)
		{
			return (*it).second->m_FragmentState[i_FragmentIndex];
		}
	}
	return false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rlyrRenderLayer::SetFragmentActive( rlyrObject* i_Object, int i_FragmentIndex, bool i_bActive )
{
	std::map<rlyrObject*, ObjectState*>::iterator it, end = m_ObjectStatus.end();
	it = m_ObjectStatus.find(i_Object);
	if( it != end )
	{
		rlyrRenderLayerNode* cur_node = (*it).first->Node( i_FragmentIndex );
		cur_node->SetNodeActive(i_bActive);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool rlyrRenderLayer::GetFragmentActive( rlyrObject* i_Object, int i_FragmentIndex )
{
	std::map<rlyrObject*, ObjectState*>::iterator it, end = m_ObjectStatus.end();
	it = m_ObjectStatus.find(i_Object);
	if( it != end )
	{
		rlyrRenderLayerNode* cur_node = (*it).first->Node( i_FragmentIndex );
		return cur_node->GetNodeActive();
	}
	return false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool rlyrRenderLayer::GetActiveState()
{
	return m_bIsActive;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rlyrRenderLayer::SetActiveState(bool i_IsActive)
{
	m_bIsActive = i_IsActive;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rlyrRenderLayer::ApplyPrefs()
{
	m_RenderPrefs->Apply();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
const g3dPrefs::g3dRenderPrefs& rlyrRenderLayer::GetActualData()
{
	return m_RenderPrefs->m_ActualPrefs;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rprfPrefsObject* rlyrRenderLayer::GetRenderPrefs()
{
	return m_RenderPrefs;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rlyrRenderLayer::SetRenderPrefs(rprfPrefsData& i_NewPrefs)
{
	m_RenderPrefs->m_Data = i_NewPrefs;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
captRenderOutputObject* rlyrRenderLayer::GetCaptureOptions()
{
	return m_CaptureOptions;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rlyrRenderLayer::SetCaptureOptions(captRenderOutputData& i_NewOptions)
{
	m_CaptureOptions->m_Data = i_NewOptions;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rlyrPassesObject* rlyrRenderLayer::GetRenderPasses()
{
	return m_RenderPasses;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rlyrRenderLayer::SetRenderPasses(rlyrPassesData& i_NewPasses)
{
	m_RenderPasses->m_Data = i_NewPasses;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
pfxPostEffectObject* rlyrRenderLayer::GetPostEffect()
{
	return m_RenderPfx;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rlyrRenderLayer::SetPostEffect(pfxData& i_NewPfx)
{
	m_RenderPfx->SetData(i_NewPfx);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rlyrRenderLayer::AddToRenderDataList(RenderData& i_RenderData)
{
	m_RenderDataList.push_back(i_RenderData);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rlyrRenderLayer::AddToRenderDataList(fsLocator& i_RenderLocation, int i_RenderTime)
{
	RenderData data;
	data.m_RenderLocation = i_RenderLocation;
	data.m_RenderTime = i_RenderTime;

	AddToRenderDataList(data);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rlyrRenderLayer::ClearRenderDataList()
{
	m_RenderDataList.clear();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
std::vector<RenderData>& rlyrRenderLayer::GetRenderDataList()
{
	return m_RenderDataList;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rlyrRenderLayer::UpdateRenderPassesVisbility()
{
	m_RenderPasses->UpdateVisibility( m_RenderPrefs->m_Data.m_RendererEngine.GetValue());
}