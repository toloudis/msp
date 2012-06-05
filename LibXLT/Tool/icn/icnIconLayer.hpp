/*****************************************************************************\
**	icnIconLayer.hpp
**
**		Manages multiple instances of the same icon in order to 
**	have one icon per render panel and let it set its size and visibility
**	separately.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef ICN_ICONLAYER_HPP
#error icnIconLayer.hpp multiply included
#endif
#define ICN_ICONLAYER_HPP


//============================================================================
//============================================================================
class api3dObjectSingle;
class g3dSceneNode;
class icnIconSet;
class camCamera;


//============================================================================
//============================================================================
namespace icnIconLayer
{
	//--------------------------------------------------------------------
	// Add a new icon layer (represents a render panel that needs its
	// own copy of the icons).
	// Note: all icon layers need to be created before the first
	// icon set is created.
	// Returns a layer index that can be used with icnIconSet objects
	// to alter the transformation of just that´s view's copy.
	//--------------------------------------------------------------------
	int AddIconLayers(g3dSceneNode* i_pIconsRootNode,
					  g3dSceneNode* i_pManipulatorsRootNode);

	//--------------------------------------------------------------------
	// Put the given scene subgraph into each registered icon layer,
	// returning an icon set class that allows you to control all 
	// icons at once.
	//--------------------------------------------------------------------
	icnIconSet* CreateIconSet(api3dObjectSingle* i_pBaseObject);
	icnIconSet* CreateManipulatorSet(api3dObjectSingle* i_pBaseObject);

	//--------------------------------------------------------------------
	// Get number of icon layers available
	//--------------------------------------------------------------------
	int GetNumIconLayers();

	//--------------------------------------------------------------------
	// Sets the current icon layer to use for ray picking.
	//--------------------------------------------------------------------
	void SetActiveIconLayer(int i_IconLayerIndex);
	int GetActiveIconLayer();

	//--------------------------------------------------------------------
	// Sets the camera being used for a given icon layer index
	//--------------------------------------------------------------------
	void SetCameraForIconLayer(int i_IconLayerIndex, const camCamera* i_pCamera);
	const camCamera* GetCameraForIconLayer(int i_IconLayerIndex);
};
