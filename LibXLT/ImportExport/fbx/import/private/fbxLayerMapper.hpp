/****************************************************************************\
**  fbxLayerMapper.hpp
**
**      fbxLayerMapper supplies functions for handling 16 and 32 bit indices.
**	It is used to support the geometry-parsing namespaces.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef FBX_LAYERMAPPER_HPP
#error fbxLayerMapper.hpp multiply included
#endif
#define FBX_LAYERMAPPER_HPP

#ifndef FBX_SDK_HPP
#include "ImportExport/fbx/fbxSdk.hpp"
#endif 

#ifdef USE_FBX_IMPORTEXPORT

//----------------------------------------------------------------------------
//	fbxLayerMapperBase wraps a FBX SDK Layer in order to have easier
//	access to the indices and values within it.
//----------------------------------------------------------------------------
class fbxLayerMapperBase
{
public:
	//------------------------------------------------------------------------
	// Constructor begins unattached.
	//------------------------------------------------------------------------
	fbxLayerMapperBase();
	
	//------------------------------------------------------------------------
	// Returns true if the mapper is attached to a layer which has mappings
	// that it understands.
	//------------------------------------------------------------------------
	bool IsAttached() const;

protected:
	//------------------------------------------------------------------------
	// Attempts to get access to direct and indirect arrays in layer.
	// Returns false if the mappings cannot be handled.
	//------------------------------------------------------------------------
	bool attempt_attach(KFbxLayerElement *i_pLayer);

private:
	KFbxLayerElement *m_pLayer;
};


//----------------------------------------------------------------------------
//	fbxLayerMapperBase wraps a FBX SDK Layer with a specific type
// of layer in order to get values by index from the direct array.
//----------------------------------------------------------------------------
template <class T>
class fbxLayerMapper : public fbxLayerMapperBase
{
public:
	//------------------------------------------------------------------------
	// Attempts to get access to direct and indirect arrays in layer.
	// Returns false if the mappings cannot be handled.
	//------------------------------------------------------------------------
	bool Attach(KFbxLayerElementTemplate<T> *i_pLayer)
	{
		if (this->attempt_attach(i_pLayer))
		{
			m_pLayer = i_pLayer;
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------
	// Some mapping modes are inefficient and force us to compress the indices
	// and generate our own indicing.  Returns true if this compression of
	// indices should be done for this layer data.
	//------------------------------------------------------------------------
	bool ShouldCompressIndices() const
	{
		if (m_pLayer)
		{
			// Indices should be compressed if values are given directly per
			// polygon vertex. In this mapping, every single index would be different
			// so we should do our own check on the values and see which ones
			// are close enough to be shared.
			return ((m_pLayer->GetMappingMode() == KFbxLayerElement::eBY_POLYGON_VERTEX) &&
					(m_pLayer->GetReferenceMode() == KFbxLayerElement::eDIRECT));
		}
		return false;
	}

	//------------------------------------------------------------------------
	// Get index to use into the layer's direct array access.
	//	- ControlPointIndex is the index into the array of geometric 
	//		positions in a mesh.
	//	- VertexIndex is a counter for each vertex of each polygon
	//		in order.
	// Which index will be used depends on the mapping in the layer.
	//------------------------------------------------------------------------
	int GetIndex(int i_ControlPointIndex, int i_VertexIndex) const
	{
		if (!m_pLayer) return -1;

		switch (m_pLayer->GetMappingMode())
		{
			// eBY_CONTROL_POINT uses i_ControlPointIndex
		case KFbxLayerElement::eBY_CONTROL_POINT:
			{
				switch (m_pLayer->GetReferenceMode())
				{
				case KFbxLayerElement::eDIRECT:
					return i_ControlPointIndex;
				case KFbxLayerElement::eINDEX_TO_DIRECT:
					return m_pLayer->GetIndexArray().GetAt(i_ControlPointIndex);
				}
				break;
			}

			// eBY_POLYGON_VERTEX uses i_VertexIndex
		case KFbxLayerElement::eBY_POLYGON_VERTEX:
			{
				switch (m_pLayer->GetReferenceMode())
				{
				case KFbxLayerElement::eDIRECT:
					return i_VertexIndex;
				case KFbxLayerElement::eINDEX_TO_DIRECT:
					return m_pLayer->GetIndexArray().GetAt(i_VertexIndex);
				}
			}
		}
		return -1;
	}


	//------------------------------------------------------------------------
	// Get value from direct array by index. Use the index returned
	//	from a call to GetIndex()
	//------------------------------------------------------------------------
	T GetValue(int i_Index) const
	{
		return m_pLayer->GetDirectArray().GetAt(i_Index);
	}

	//------------------------------------------------------------------------
	// Returns number of elements in the direct array
	//------------------------------------------------------------------------
	int GetDirectArraySize() const
	{
		if (!m_pLayer) 
			return 0;
		else
			return (m_pLayer->GetDirectArray().GetCount());
	}

private:
	KFbxLayerElementTemplate<T> *m_pLayer;
};


#endif // USE_FBX_IMPORTEXPORT
