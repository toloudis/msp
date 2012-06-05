/****************************************************************************\
**	icnIconLayer.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/icn/icnIconLayer.hpp"

#include "Graphics/G3d/g3dSceneNode.hpp"
#include "Graphics/Sc/scObject.hpp"
#include "Tool/api3d/api3dObjectSingle.hpp"
#include "Tool/icn/icnIconSet.hpp"


//============================================================================
//============================================================================
namespace icnIconLayer
{

	//============================================================================
	//============================================================================
	namespace
	{
		std::vector<g3dSceneNode*> l_IconLayers;
		std::vector<g3dSceneNode*> l_ManipulatorLayers;
		std::vector<const camCamera*> l_LayerCameras;
		int l_ActiveIconLayer = 0;

		//--------------------------------------------------------------------
		// Attach base object to the icon layers, making a clone
		// for each extra layer.
		//--------------------------------------------------------------------
		void attach_icon_set(api3dObjectSingle* i_pBaseObject,
							 icnIconSet *io_pIconSet,
							 std::vector<g3dSceneNode*> &io_IconLayers)
		{
			const int num_layers = io_IconLayers.size();
			if (num_layers > 0)
			{
				// Add the base object to the first layer
				io_IconLayers[0]->AddChild(i_pBaseObject->Object()->GetBase());

				// Create clones for each additional layer
				for (int i=1; i<num_layers; ++i)
					io_pIconSet->AddClone( io_IconLayers[i] );
			}
		}
	}

	//--------------------------------------------------------------------
	// Add a new icon layer (represents a render panel that needs its
	// own copy of the icons.
	// Note: all icon layers need to be created before the first
	// icon set is created.
	// Returns a layer index that can be used with icnIconSet objects
	// to alter the transformation of just that´s view's copy.
	//--------------------------------------------------------------------
	int AddIconLayers(g3dSceneNode* i_pIconsRootNode,
					  g3dSceneNode* i_pManipulatorsRootNode)
	{
		int layer_index = l_IconLayers.size();
		l_IconLayers.push_back(i_pIconsRootNode);
		l_ManipulatorLayers.push_back(i_pManipulatorsRootNode);
		l_LayerCameras.push_back(NULL);
		return layer_index;
	}

	//--------------------------------------------------------------------
	// Put the given scene subgraph into each registered icon layer,
	// returning an icon set class that allows you to control all 
	// icons at once.
	//--------------------------------------------------------------------
	icnIconSet* CreateIconSet(api3dObjectSingle* i_pBaseObject)
	{
		icnIconSet *pIconSet = new icnIconSet(i_pBaseObject);
		attach_icon_set(i_pBaseObject, pIconSet, l_IconLayers);
		return pIconSet;
	}
	icnIconSet* CreateManipulatorSet(api3dObjectSingle* i_pBaseObject)
	{
		icnIconSet *pIconSet = new icnIconSet(i_pBaseObject);
		attach_icon_set(i_pBaseObject, pIconSet, l_ManipulatorLayers);
		return pIconSet;
	}

	//--------------------------------------------------------------------
	// Get number of icon layers available
	//--------------------------------------------------------------------
	int GetNumIconLayers()
	{
		return l_IconLayers.size();
	}

	//--------------------------------------------------------------------
	// Sets the current icon layer to use for ray picking.
	//--------------------------------------------------------------------
	void SetActiveIconLayer(int i_IconLayerIndex)
	{
		DBG_ASSERT(i_IconLayerIndex<l_IconLayers.size(), "SetActiveIconLayer, layer index out of range, " << i_IconLayerIndex << " out of " << l_IconLayers.size());
		l_ActiveIconLayer = i_IconLayerIndex;
	}
	int GetActiveIconLayer()
	{
		return l_ActiveIconLayer;
	}

	//--------------------------------------------------------------------
	// Sets the camera being used for a given icon layer index
	//--------------------------------------------------------------------
	void SetCameraForIconLayer(int i_IconLayerIndex, const camCamera* i_pCamera)
	{
		DBG_ASSERT(i_IconLayerIndex<l_IconLayers.size(), "SetCameraForIconLayer, layer index out of range, " << i_IconLayerIndex << " out of " << l_IconLayers.size());
		l_LayerCameras[i_IconLayerIndex] = i_pCamera;
	}	
	const camCamera* GetCameraForIconLayer(int i_IconLayerIndex)
	{
		DBG_ASSERT(i_IconLayerIndex<l_IconLayers.size(), "GetCameraForIconLayer, layer index out of range, " << i_IconLayerIndex << " out of " << l_IconLayers.size());
		return l_LayerCameras[i_IconLayerIndex];
	}

}	// end of namespace
