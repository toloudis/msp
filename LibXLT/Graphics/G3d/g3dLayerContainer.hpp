/****************************************************************************\
**	g3dLayerContainer.hpp
**
**	The g3dLayerContainer is a base class that holds g3dLayers
**	for a scene or a viewer.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_LAYERCONTAINER_HPP
#error g3dLayerContainer.hpp multiply included
#endif
#define G3D_LAYERCONTAINER_HPP

#include <vector>


//============================================================================
//	Forward References
//============================================================================
class g3dLayer;


//============================================================================
//============================================================================
class g3dLayerContainer
{
public:
	//--------------------------------------------------------------------
	// default constructor has no layers and no world root
	//--------------------------------------------------------------------
	g3dLayerContainer();

	//--------------------------------------------------------------------
	// creates scene with single layer and sets it to be the world root
	//--------------------------------------------------------------------
	g3dLayerContainer(g3dLayer* i_Layer);

	//--------------------------------------------------------------------
	// construct scene with given layer scheme.
	//	the scene takes ownership of these layers.
	//--------------------------------------------------------------------
	g3dLayerContainer(const std::vector<g3dLayer*> &i_Layers);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~g3dLayerContainer();

	//--------------------------------------------------------------------
	// Set whole set of layers at once, deleting any previous layers
	//--------------------------------------------------------------------
	void SetLayers(const std::vector<g3dLayer*> &i_Layers);

	//--------------------------------------------------------------------
	// get vector of layers in this scene
	//--------------------------------------------------------------------
	void GetLayers(std::vector<const g3dLayer*> &o_Layers) const;
	void GetLayers(std::vector<g3dLayer*> &o_Layers);
	const std::vector<g3dLayer*>& GetLayers() const;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	int GetNumLayers() const;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	const g3dLayer* GetLayer(int i_Index) const;
	g3dLayer* GetLayer(int i_Index);

	//--------------------------------------------------------------------
	// append layer to end of list
	//--------------------------------------------------------------------
	void AppendLayer(g3dLayer* i_Layer);

	//--------------------------------------------------------------------
	// inserts layer in given position
	//--------------------------------------------------------------------
	void InsertLayer(int i_Index, g3dLayer* i_Layer);

	//--------------------------------------------------------------------
	// removes layer without deleting it (assuming caller is
	//	going to take care of it)
	//--------------------------------------------------------------------
	void RemoveLayer(int i_Index);
	void RemoveLayer(g3dLayer* i_Layer);

	//--------------------------------------------------------------------
	// removes and deletes layer
	//--------------------------------------------------------------------
	void DeleteLayer(int i_Index);
	void DeleteLayer(g3dLayer* i_Layer);

	//------------------------------------------------------------------------
	// Update the transforms within the scene graphs
	//------------------------------------------------------------------------
	void UpdateWorldData();

	//------------------------------------------------------------------------
	// Update the transforms within a single layer in the scene
	//------------------------------------------------------------------------
	void UpdateLayerData(int i_LayerIndex);

private:
	// layers, owned by layer container
	std::vector<g3dLayer*> m_Layers;

};

