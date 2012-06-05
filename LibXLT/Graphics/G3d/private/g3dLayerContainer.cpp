/****************************************************************************\
**	g3dLayerContainer.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "Graphics/g3d/g3dLayerContainer.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"


//============================================================================
//============================================================================
namespace
{

	//------------------------------------------------------------------------
	//	update_world_data - update the total transforms and bounding boxes
	//	Optionally will fill a list with the scene nodes with fragments
	//------------------------------------------------------------------------
	void update_world_data( g3dSceneNode* i_pNode )
	{
{//PROFILE("compute");
		//save current transform before update
		// Compute current matrix
		if (!i_pNode->GetIgnoreParentTransform() && i_pNode->GetParent())
		{
			i_pNode->SetTotalTransform( i_pNode->GetTransform() * i_pNode->GetParent()->GetTotalTransform() );
		}
		else
		{
			i_pNode->SetTotalTransform( i_pNode->GetTransform() );
		}
}

		maAxisBox world_box;

//{//PROFILE("children");
		// Update the children
		std::vector<g3dSceneNode*>& children = i_pNode->GetChildren();
		std::vector<g3dSceneNode*>::iterator it = children.begin(), end = children.end();
		for ( ; it != end; ++it )
		{
			g3dSceneNode* child = (*it);
			//all children of a non-renderable node should also not render
			//by passing NULL instead of i_pFragNodes, we can prevent all children from rendering
			update_world_data( child );
			world_box.Union( child->GetWorldBox() );
		}
//}
{//PROFILE("geometry");
		// Check if it has geometry
		const g3dFragment* pFrag = i_pNode->GetFragment();
		if ( pFrag )
		{
			// Convert box to world space
			if ( pFrag->IsModelSpaceBox() )
			{
				const maMatrix4x4& total_transform = i_pNode->GetTotalTransform();

				// temp storage to be reused on subsequent calls
				static maPoint3d	l_box_points[8];

				pFrag->GetBoundingBox().GetBoxPoints( l_box_points );

				// Trasform the points to world space
				for ( int i = 0; i < 8; ++i )
				{
					total_transform.Transform( l_box_points[i] );
					world_box.Union( l_box_points[i] );
				}
			}
			// Already in world space
			else
			{
				world_box.Union( pFrag->GetBoundingBox() );
			}
		}
}
{//PROFILE("wbox");
		i_pNode->SetWorldBox( world_box );
}
	}

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dLayerContainer::g3dLayerContainer()
{

}

//--------------------------------------------------------------------
// creates scene with single layer and sets it to be the world root
//--------------------------------------------------------------------
g3dLayerContainer::g3dLayerContainer(g3dLayer* i_Layer)
{
	m_Layers.push_back(i_Layer);
}

//--------------------------------------------------------------------
// construct scene with given layer scheme.
//	the scene takes ownership of these layers.
//--------------------------------------------------------------------
g3dLayerContainer::g3dLayerContainer(const std::vector<g3dLayer*> &i_Layers)
{
	m_Layers = i_Layers;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dLayerContainer::~g3dLayerContainer()
{
	envSTLHelpers::DeleteContainer(m_Layers);
}

//--------------------------------------------------------------------
// Set whole set of layers at once, deleting any previous layers
//--------------------------------------------------------------------
void g3dLayerContainer::SetLayers(const std::vector<g3dLayer*> &i_Layers)
{
	envSTLHelpers::DeleteContainer(m_Layers);
	m_Layers = i_Layers;
}

//--------------------------------------------------------------------
// get vector of layers in this scene
//--------------------------------------------------------------------
void g3dLayerContainer::GetLayers(std::vector<const g3dLayer*> &o_Layers) const
{
	int nlayers = m_Layers.size();
	o_Layers.resize(nlayers);
	for (int i=0; i<nlayers; i++)
		o_Layers[i] = m_Layers[i];
}
void g3dLayerContainer::GetLayers(std::vector<g3dLayer*> &o_Layers)
{
	o_Layers = m_Layers;
}
const std::vector<g3dLayer*>& g3dLayerContainer::GetLayers() const
{
	return m_Layers;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int g3dLayerContainer::GetNumLayers() const
{
	return (int) m_Layers.size();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const g3dLayer* g3dLayerContainer::GetLayer(int i_Index) const
{
	return m_Layers[i_Index];
}
g3dLayer* g3dLayerContainer::GetLayer(int i_Index)
{
	return m_Layers[i_Index];

}

//--------------------------------------------------------------------
// append layer to end of list
//--------------------------------------------------------------------
void g3dLayerContainer::AppendLayer(g3dLayer* i_Layer)
{
	m_Layers.push_back(i_Layer);
}

//--------------------------------------------------------------------
// inserts layer in given position
//--------------------------------------------------------------------
void g3dLayerContainer::InsertLayer(int i_Index, g3dLayer* i_Layer)
{
	m_Layers.insert(m_Layers.begin()+i_Index, i_Layer);
}

//--------------------------------------------------------------------
// removes layer without deleting it (assuming caller is
//	going to take care of it)
//--------------------------------------------------------------------
void g3dLayerContainer::RemoveLayer(int i_Index)
{
	envSTLHelpers::RemoveOneValue(m_Layers, m_Layers[i_Index]);
}
void g3dLayerContainer::RemoveLayer(g3dLayer* i_Layer)
{
	envSTLHelpers::RemoveOneValue(m_Layers, i_Layer);
}

//--------------------------------------------------------------------
// removes and deletes layer
//--------------------------------------------------------------------
void g3dLayerContainer::DeleteLayer(int i_Index)
{
	envSTLHelpers::DeleteOneValue(m_Layers, m_Layers[i_Index]);
}
void g3dLayerContainer::DeleteLayer(g3dLayer* i_Layer)
{
	envSTLHelpers::DeleteOneValue(m_Layers, i_Layer);

}

//------------------------------------------------------------------------
// Update the transforms within the scene graphs
//------------------------------------------------------------------------
void g3dLayerContainer::UpdateWorldData()
{
	for (int i=0; i<m_Layers.size(); i++)
	{
		update_world_data(m_Layers[i]->GetRootNode());
	}
}

//------------------------------------------------------------------------
// Update the transforms within a single layer in the scene
//------------------------------------------------------------------------
void g3dLayerContainer::UpdateLayerData(int i_LayerIndex)
{
	if (i_LayerIndex > 0 && i_LayerIndex < m_Layers.size())
	{
		update_world_data(m_Layers[i_LayerIndex]->GetRootNode());
	}
}


