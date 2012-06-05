#include "stdafx.h"
#include "InheritanceManager.h"
#include "SGPUExporter.h"

CInheritanceManager::CInheritanceManager(SGPUExporter& exporter): m_exporter(exporter), underRoot(m_exporter.Scene().GetRootNode().AddChildNode())
{
    m_bMaterialsByLayer = false;
}

CInheritanceManager::CInheritanceManager(SGPUExporter& exporter, bool bMaterialsByLayer): m_exporter(exporter), underRoot(m_exporter.Scene().GetRootNode().AddChildNode())
{
    m_bMaterialsByLayer = bMaterialsByLayer;
}

CInheritanceManager::~CInheritanceManager()
{
  std::vector<ISkpMaterial*>::iterator material_it = m_BackMaterials.begin();
  while (material_it != m_BackMaterials.end()) 
  {
      ISkpMaterial* const pMaterial = *material_it;
    if (NULL != pMaterial) pMaterial->Release();
  }
  material_it = m_FrontMaterials.begin();
  while (material_it != m_FrontMaterials.end()) 
  {
    ISkpMaterial* const pMaterial = *material_it;
    if (NULL != pMaterial) pMaterial->Release();
  }
  std::vector<ISkpLayer*>::iterator layer_it = m_Layers.begin();
  while (layer_it != m_Layers.end()) 
  {
    ISkpLayer* const pLayer = *layer_it;
    if (NULL != pLayer) pLayer->Release();
  }  
}

double CInheritanceManager::
Det( CTransform& t )
{
	const double *m = t.Matrix();

	return
	m[0]*m[5]*m[10]*m[15] - m[0]*m[5]*m[11]*m[14] - m[0]*m[9]*m[6]*m[15] + m[0]*m[9]*m[7]*m[14] + m[0]*m[13]*m[6]*m[11] - m[0]*m[13]*m[7]*m[10] - 
	m[4]*m[1]*m[10]*m[15] + m[4]*m[1]*m[11]*m[14] + m[4]*m[9]*m[2]*m[15] - m[4]*m[9]*m[3]*m[14] - m[4]*m[13]*m[2]*m[11] + m[4]*m[13]*m[3]*m[10] + 
	m[8]*m[1]*m[6]*m[15] - m[8]*m[1]*m[7]*m[14] - m[8]*m[5]*m[2]*m[15] + m[8]*m[5]*m[3]*m[14] + m[8]*m[13]*m[2]*m[7] - m[8]*m[13]*m[3]*m[6] - 
	m[12]*m[1]*m[6]*m[11] + m[12]*m[1]*m[7]*m[10] + m[12]*m[5]*m[2]*m[11] - m[12]*m[5]*m[3]*m[10] - m[12]*m[9]*m[2]*m[7] + m[12]*m[9]*m[3]*m[6];
}

void CInheritanceManager::PushTransform(CTransform t, bool createNode)
{
    CTransform tCurrent = GetCurrentTransform();
    CTransform tNew = tCurrent * t;
    m_Transforms.push_back(tNew);

	m_isDetPositive = Det( tNew ) >= 0.0f;

	if (createNode)
	{
		std::wstring name;// = m_exporter.MakeUniqueName(L"Node0", false);
		if (m_sgpuNodes.size())
			m_sgpuNodes.push_back(m_sgpuNodes.back().AddChildNode());
		else
		{
			sgpuNode root_node = m_exporter.Scene().GetRootNode();
			m_sgpuNodes.push_back(root_node.AddChildNode());
		}

		const double* tM = t.Matrix();

		sgpuMatrix matrix(
			(float)(tM[ 0]), (float)(tM[ 1]), (float)(tM[ 2]), (float)(tM[ 3]),
			(float)(tM[ 4]), (float)(tM[ 5]), (float)(tM[ 6]), (float)(tM[ 7]),
			(float)(tM[ 8]), (float)(tM[ 9]), (float)(tM[10]), (float)(tM[11]),
			(float)(tM[12]), (float)(tM[13]), (float)(tM[14]), (float)(tM[15])
			);

		m_sgpuNodes.back().SetNodeName( name.c_str() );
		m_sgpuNodes.back().SetTransformationMatrix( matrix );

		m_sgpuNodesPresent.push_back( true );
	}
	else
		m_sgpuNodesPresent.push_back( false );
}

void CInheritanceManager::PushElement()
{
	CTransform tIdentity;
    PushTransform(tIdentity, true);

	m_FrontMaterials.push_back(0);
    m_BackMaterials.push_back(0);
    m_Layers.push_back(0);
}

void CInheritanceManager::PushElement(CComPtr<ISkpComponentInstance> pInstance)
{
    HRESULT hr;

    //Transform
    CComPtr<ISkpTransform> pTransform;
    hr = pInstance->get_Transform(&pTransform);
    double tMatrix[16];
    hr = pTransform->GetData(tMatrix);
    CTransform t(tMatrix);
    PushTransform(t, true);

    CComPtr<ISkpDrawingElement> pDrawElement = NULL;
    hr = pInstance->QueryInterface(IID_ISkpDrawingElement, (void**)&pDrawElement);

    //Material
    BOOL hasMaterial;
    hr = pDrawElement->get_HasMaterial(&hasMaterial);
    if(hasMaterial)
    {
        ISkpMaterial* pMaterial;
        hr = pDrawElement->get_Material(&pMaterial);

        pMaterial->AddRef();
        m_FrontMaterials.push_back(pMaterial);
        pMaterial->AddRef();        
        m_BackMaterials.push_back(pMaterial);
    }
    else
    {
        m_FrontMaterials.push_back(NULL);
        m_BackMaterials.push_back(NULL);
    }

    //Layer
    ISkpLayer* pLayer;
    hr = pDrawElement->get_Layer(&pLayer);
    pLayer->AddRef();
    m_Layers.push_back(pLayer);
}

void CInheritanceManager::PushElement(CComPtr<ISkpGroup> pGroup)
{
    HRESULT hr;

    //Transform
    CComPtr<ISkpTransform> pTransform;
    hr = pGroup->get_Transform(&pTransform);
    double tMatrix[16];
    hr = pTransform->GetData(tMatrix);
    CTransform t(tMatrix);
    PushTransform(t, true);

    CComPtr<ISkpDrawingElement> pDrawElement = NULL;
    hr = pGroup->QueryInterface(IID_ISkpDrawingElement, (void**)&pDrawElement);

    //Material
    BOOL hasMaterial;
    hr = pDrawElement->get_HasMaterial(&hasMaterial);
    if(hasMaterial)
    {
        ISkpMaterial* pMaterial;
        hr = pDrawElement->get_Material(&pMaterial);

        pMaterial->AddRef();
        m_FrontMaterials.push_back(pMaterial);
        pMaterial->AddRef();
        m_BackMaterials.push_back(pMaterial);
    }
    else
    {
        m_FrontMaterials.push_back(NULL);
        m_BackMaterials.push_back(NULL);
    }

    //Layer
    ISkpLayer* pLayer;
    hr = pDrawElement->get_Layer(&pLayer);
    pLayer->AddRef();
    m_Layers.push_back(pLayer);
}

void CInheritanceManager::PushElement(CComPtr<ISkpImage> pImage)
{
    HRESULT hr;

    //Transform
    CComPtr<ISkpTransform> pTransform;
    hr = pImage->get_Transform(&pTransform);
    double tMatrix[16];
    hr = pTransform->GetData(tMatrix);
    CTransform t(tMatrix);
    PushTransform(t, true);

    CComPtr<ISkpDrawingElement> pDrawElement = NULL;
    hr = pImage->QueryInterface(IID_ISkpDrawingElement, (void**)&pDrawElement);

    //Material
    BOOL hasMaterial;
    hr = pDrawElement->get_HasMaterial(&hasMaterial);
    if(hasMaterial)
    {
        ISkpMaterial* pMaterial;
        hr = pDrawElement->get_Material(&pMaterial);

        pMaterial->AddRef();
        m_FrontMaterials.push_back(pMaterial);
        pMaterial->AddRef();
        m_BackMaterials.push_back(pMaterial);
    }
    else
    {
        m_FrontMaterials.push_back(NULL);
        m_BackMaterials.push_back(NULL);
    }

    //Layer
    ISkpLayer* pLayer;
    hr = pDrawElement->get_Layer(&pLayer);
    pLayer->AddRef();
    m_Layers.push_back(pLayer);
}

void CInheritanceManager::PushElement(CComPtr<ISkpFace> pFace)
{
    HRESULT hr;

    //Transform - Faces don't have a transforms so just push an identity on the stack
    CTransform tIdentity;
    PushTransform(tIdentity, false);

    //Front Material
    ISkpMaterial* pFrontMaterial = NULL;

    if (FAILED(pFace->get_FrontMaterial(&pFrontMaterial)) ||
        pFrontMaterial == NULL)
    {
        m_FrontMaterials.push_back(NULL);
    }
    else
    {
        pFrontMaterial->AddRef();
        m_FrontMaterials.push_back(pFrontMaterial);
    }

    //Back Material
    ISkpMaterial* pBackMaterial = NULL;

    if (FAILED(pFace->get_BackMaterial(&pBackMaterial)) ||
        pBackMaterial == NULL)
    {
        m_BackMaterials.push_back(NULL);
    }
    else
    {
        pBackMaterial->AddRef();
        m_BackMaterials.push_back(pBackMaterial);
    }

    CComPtr<ISkpDrawingElement> pDrawElement = NULL;
    hr = pFace->QueryInterface(IID_ISkpDrawingElement, (void**)&pDrawElement);

    //Layer
    ISkpLayer* pLayer;
    hr = pDrawElement->get_Layer(&pLayer);
    pLayer->AddRef();
    m_Layers.push_back(pLayer);
}

void CInheritanceManager::PushElement(CComPtr<ISkpEdge> pEdge)
{
    HRESULT hr;

    //Transform - Edges don't have a transforms so just push an identity on the stack
    CTransform tIdentity;
    PushTransform(tIdentity, false);

    //Materials - Edges don't have materials so just push a null on the stack
    m_FrontMaterials.push_back(NULL);
    m_BackMaterials.push_back(NULL);

    CComPtr<ISkpDrawingElement> pDrawElement = NULL;
    hr = pEdge->QueryInterface(IID_ISkpDrawingElement, (void**)&pDrawElement);

    //Layer
    ISkpLayer* pLayer;
    hr = pDrawElement->get_Layer(&pLayer);
    pLayer->AddRef();
    m_Layers.push_back(pLayer);
}

void CInheritanceManager::PopElement()
{
    //Transforms
    assert (m_Transforms.size() > 0);
    m_Transforms.pop_back();
	m_isDetPositive = m_Transforms.size()? Det( m_Transforms.back() ) >= 0.0f : true;

    //Materials
    assert (m_FrontMaterials.size() > 0);
    ISkpMaterial* pMaterial = m_FrontMaterials.back();
    if (NULL != pMaterial) pMaterial->Release();
    m_FrontMaterials.pop_back();

    assert (m_BackMaterials.size() > 0);
    pMaterial = m_BackMaterials.back();
    if (NULL != pMaterial) pMaterial->Release();      
    m_BackMaterials.pop_back();

    //Layers
    assert (m_Layers.size() > 0);
    ISkpLayer* pLayer = m_Layers.back();
    if (NULL != pLayer) pLayer->Release();
    m_Layers.pop_back();

	//Nodes
	assert (m_sgpuNodes.size() > 0);
	assert (m_sgpuNodesPresent.size() > 0);

	if (m_sgpuNodesPresent.back())
		m_sgpuNodes.pop_back();
	m_sgpuNodesPresent.pop_back();
}

CComPtr<ISkpLayer> CInheritanceManager::GetCurrentLayer()
{
    //Search layer stack for first non-null layer
    int n = static_cast<int>(m_Layers.size());
    for (int i = n; --i >= 0;)
    {
        if (m_Layers[i]) return m_Layers[i];
    }
    return NULL;
}

CTransform CInheritanceManager::GetCurrentTransform()
{
    int size = static_cast<int>(m_Transforms.size());
    if (size == 0)
    {
        return CTransform(); // identity
    }
    else
    {
        return m_Transforms[size - 1];
    }
}
CComPtr<ISkpMaterial> CInheritanceManager::GetCurrentFrontMaterial()
{
    HRESULT hr;
    if (m_bMaterialsByLayer)
    {
        CComPtr<ISkpLayer> pLayer = GetCurrentLayer();
        ISkpMaterial* pMaterial;
        hr = pLayer->get_Material(&pMaterial);
        return pMaterial;
    }
    else
    {
        //Search material stack for first non-null layer
        int n = static_cast<int>(m_FrontMaterials.size());
        for (int i = n; --i >= 0;)
        {
            if (m_FrontMaterials[i]) return m_FrontMaterials[i];
        }
        return NULL;
   }
}

CComPtr<ISkpMaterial> CInheritanceManager::GetCurrentBackMaterial()
{
    HRESULT hr;
    if (m_bMaterialsByLayer)
    {
        CComPtr<ISkpLayer> pLayer = GetCurrentLayer();
        ISkpMaterial* pMaterial;
        hr = pLayer->get_Material(&pMaterial);
        return pMaterial;
    }
    else
    {
        //Search material stack for first non-null layer
        int n = static_cast<int>(m_BackMaterials.size());
        for (int i = n; --i >= 0;)
        {
            if (m_BackMaterials[i]) return m_BackMaterials[i];
        }
        return NULL;
    }
}