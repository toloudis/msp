/*****************************************************************************
**	rlyrRenderCam.cpp
**
**	Keeps track of cameras and the render layers associated with each one
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Support/rlyr/rlyrRenderCam.hpp"
#include "Support/rlyr/private/rlyrRenderLayerObject.hpp"
#include "Support/rlyr/rlyrRenderLayer.hpp"
#include "Support/rprf/rprfPrefsObject.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/name/nameObject.hpp"


namespace
{
}	// end of namespace

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rlyrRenderCam::rlyrRenderCam(nameObject* i_NameObject)
	: m_NameObject(i_NameObject)
{
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rlyrRenderCam::~rlyrRenderCam()
{
	envSTLHelpers::DeleteContainer(m_RenderLayers);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rlyrRenderCam::SetName(const nameString& i_Name)
{
	m_NameObject->SetName(i_Name);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
nameString rlyrRenderCam::GetName()
{
	return m_NameObject->GetName();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rlyrRenderCam::GetLayerNames(std::vector<nameString>& o_RenderLayers)
{
	o_RenderLayers.clear();
	// Go through all of the render layers in this Cam
	std::vector<rlyrRenderLayer*>::iterator layer_it, end = m_RenderLayers.end();
	for (layer_it = m_RenderLayers.begin(); layer_it != end; ++layer_it)
	{
		o_RenderLayers.push_back((*layer_it)->GetName());
	}
}

//--------------------------------------------------------------------
// GetLayerByName - returns the render layer that has the given name
//--------------------------------------------------------------------
rlyrRenderLayer* rlyrRenderCam::GetLayerByName(const nameString& i_LayerName)
{
	std::vector<rlyrRenderLayer*>::iterator it, end = m_RenderLayers.end();
	for (it = m_RenderLayers.begin(); it != end; ++it)
	{
		if((*it)->GetName() == i_LayerName)
			return (*it);
	}
	return NULL;
}

//--------------------------------------------------------------------
// Remove a render layer from the camera
//--------------------------------------------------------------------
void rlyrRenderCam::RemoveLayer(const nameString& i_LayerName)
{
	std::vector<rlyrRenderLayer*>::iterator it, end = m_RenderLayers.end();
	for(int i = 0; i < m_RenderLayers.size(); i++)
	{
		if( m_RenderLayers[i]->GetName() == i_LayerName )
		{
			//m_RenderLayers[i]->SetParentCam(NULL);
			envSTLHelpers::DeleteOneValue(m_RenderLayers, m_RenderLayers[i]);
		}
	}
}

//--------------------------------------------------------------------
// Remove all render layers from the camera
//--------------------------------------------------------------------
void rlyrRenderCam::RemoveAllLayers()
{
	envSTLHelpers::DeleteContainer(m_RenderLayers);
}

//--------------------------------------------------------------------
// Add a render layer to this camera
//--------------------------------------------------------------------
void rlyrRenderCam::AddLayer(rlyrRenderLayer* i_RenderLayer)
{
	m_RenderLayers.push_back(i_RenderLayer);
	i_RenderLayer->SetParentCam(this);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rlyrRenderCam::SetLayerActiveState(const nameString &i_LayerName, bool i_ActiveState)
{
	rlyrRenderLayer* cur_layer = GetLayerByName(i_LayerName);
	cur_layer->SetActiveState(i_ActiveState);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rlyrRenderCam::SetAllLayersActiveState(bool i_ActiveState)
{
	std::vector<rlyrRenderLayer*>::iterator it, end = m_RenderLayers.end();
	for(it = m_RenderLayers.begin(); it != end; ++it)
	{
		(*it)->SetActiveState(i_ActiveState);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool rlyrRenderCam::GetLayerActiveState(const nameString &i_LayerName)
{
	rlyrRenderLayer* cur_layer = GetLayerByName(i_LayerName);
	return cur_layer->GetActiveState();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rlyrRenderCam::UpdateLayerName(const nameString& i_OldName, const nameString& i_NewName)
{
	rlyrRenderLayer* cur_layer = GetLayerByName(i_OldName);
	cur_layer->SetName(i_NewName);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rlyrRenderCam::ApplyLayerPrefs(int i_LayerIndex)
{
	rlyrRenderLayer* cur_layer = m_RenderLayers[i_LayerIndex];
	if( cur_layer != NULL)
	{
		cur_layer->ApplyPrefs();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
const g3dPrefs::g3dRenderPrefs& rlyrRenderCam::GetLayerActualData(int i_LayerIndex)
{
	rlyrRenderLayer* cur_layer = m_RenderLayers[i_LayerIndex];
	if( cur_layer != NULL)
	{
		return cur_layer->GetActualData();
	}

	return g3dPrefs::CurrentPrefs();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
rprfPrefsObject* rlyrRenderCam::GetLayerPrefs(const nameString& i_LayerName)
{
	rlyrRenderLayer* cur_layer = GetLayerByName(i_LayerName);
	if( cur_layer != NULL)
	{
		return cur_layer->GetRenderPrefs();
	}
	return NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rlyrRenderCam::SetLayerPrefs(const nameString& i_LayerName, rprfPrefsData& i_NewPrefs)
{
	rlyrRenderLayer* cur_layer = GetLayerByName(i_LayerName);
	if( cur_layer != NULL)
	{
		cur_layer->SetRenderPrefs(i_NewPrefs);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
captRenderOutputObject* rlyrRenderCam::GetLayerCaptureOptions(const nameString& i_LayerName)
{
	rlyrRenderLayer* cur_layer = GetLayerByName(i_LayerName);
	if( cur_layer != NULL)
	{
		return cur_layer->GetCaptureOptions();
	}
	return NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rlyrRenderCam::SetLayerCaptureOptions(const nameString& i_LayerName, captRenderOutputData& i_NewOptions)
{
	rlyrRenderLayer* cur_layer = GetLayerByName(i_LayerName);
	if( cur_layer != NULL)
	{
		cur_layer->SetCaptureOptions(i_NewOptions);
	}
}

//--------------------------------------------------------------------
// when adding an object, add to all layers
//--------------------------------------------------------------------
void rlyrRenderCam::AddObjectToLayers(rlyrObject* i_Object)
{
	std::vector<rlyrRenderLayer*>::iterator it, end = m_RenderLayers.end();
	for(it = m_RenderLayers.begin(); it != end; ++it)
	{
		(*it)->AddObject(i_Object);
	}
}

//--------------------------------------------------------------------
// remove an object from all layers
//--------------------------------------------------------------------
void rlyrRenderCam::RemoveObjectFromLayers(rlyrObject* i_Object)
{
	std::vector<rlyrRenderLayer*>::iterator it, end = m_RenderLayers.end();
	for(it = m_RenderLayers.begin(); it != end; ++it)
	{
		(*it)->RemoveObject(i_Object);
	}
}

//--------------------------------------------------------------------
// get the object name, visible flag pairing for the specified layer
//--------------------------------------------------------------------
void rlyrRenderCam::GetLayerObjectVisibility(int i_LayerIndex, std::map<nameString, bool>& o_ObjectStatus)
{
	rlyrRenderLayer* cur_layer = m_RenderLayers[i_LayerIndex];
	if( cur_layer != NULL)
	{
		cur_layer->GetObjectVisiblity( o_ObjectStatus );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rlyrRenderCam::SetObjectState(const nameString& i_LayerName, rlyrObject* i_Object, bool i_Active)
{
	rlyrRenderLayer* cur_layer = GetLayerByName(i_LayerName);
	if( cur_layer != NULL)
	{
		cur_layer->SetObjectState(i_Object, i_Active);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool rlyrRenderCam::GetObjectState(const nameString& i_LayerName, rlyrObject* i_Object)
{
	rlyrRenderLayer* cur_layer = GetLayerByName(i_LayerName);
	if( cur_layer != NULL)
	{
		return cur_layer->GetObjectState(i_Object);
	}
	//assertion will be hit in layer if object isn't found
	return false;
}