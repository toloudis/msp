/****************************************************************************\
**  fbxLayerMapper.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/fbx/import/private/fbxLayerMapper.hpp"


#ifdef USE_FBX_IMPORTEXPORT

//------------------------------------------------------------------------
// Constructor begins unattached.
//------------------------------------------------------------------------
fbxLayerMapperBase::fbxLayerMapperBase()
: m_pLayer(NULL)
{

}

//------------------------------------------------------------------------
// Returns true if the mapper is attached to a layer which has mappings
// that it understands.
//------------------------------------------------------------------------
bool fbxLayerMapperBase::IsAttached() const
{
	return (m_pLayer != NULL);
}

//------------------------------------------------------------------------
// Attempts to get access to direct and indirect arrays in layer.
// Returns false if the mappings cannot be handled.
//------------------------------------------------------------------------
bool fbxLayerMapperBase::attempt_attach(KFbxLayerElement *i_pLayer)
{
	if (!i_pLayer) return false;

    switch (i_pLayer->GetMappingMode())
    {
    case KFbxLayerElement::eBY_CONTROL_POINT:
		{
			switch (i_pLayer->GetReferenceMode())
			{
			case KFbxLayerElement::eDIRECT:
			case KFbxLayerElement::eINDEX_TO_DIRECT:
				m_pLayer = i_pLayer;
				return true;
			default:
				break; // other reference modes not shown here!
			}
			break;
		}

    case KFbxLayerElement::eBY_POLYGON_VERTEX:
        {
            switch (i_pLayer->GetReferenceMode())
            {
            case KFbxLayerElement::eDIRECT:
            case KFbxLayerElement::eINDEX_TO_DIRECT:
				m_pLayer = i_pLayer;
				return true;
            default:
                break; // other reference modes not shown here!
            }
        }
        break;

    case KFbxLayerElement::eBY_POLYGON: // not handled
    case KFbxLayerElement::eALL_SAME:   // not handled
    case KFbxLayerElement::eNONE:       // not handled
        break;
    }
	return false;
}

#endif // USE_FBX_IMPORTEXPORT
