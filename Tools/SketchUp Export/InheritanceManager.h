#pragma once

#include "stdafx.h"
#include "GeomUtilities.h"
#include "sgpuNode.hpp"

class SGPUExporter;

//CInheritanceManager - A cross-platform class that manages the properties
//of geometric elements (faces and edges) that can be inherited from component
//instances, groups and images.  These properties are transformations to world
//space, layers and materials.
class CInheritanceManager
{
public:

    CInheritanceManager(SGPUExporter& exporter);
    CInheritanceManager(SGPUExporter& exporter, bool bMaterialsByLayer);
    virtual ~CInheritanceManager();

    void PushElement(CComPtr<ISkpComponentInstance> pElement);
    void PushElement(CComPtr<ISkpGroup> pElement);
    void PushElement(CComPtr<ISkpImage> pElement);
    void PushElement(CComPtr<ISkpFace> pElement);
    void PushElement(CComPtr<ISkpEdge> pElement);
	void PushElement();
    void PopElement();

    CComPtr<ISkpLayer> GetCurrentLayer();
    CTransform GetCurrentTransform();
    CComPtr<ISkpMaterial> GetCurrentFrontMaterial();
    CComPtr<ISkpMaterial> GetCurrentBackMaterial();
	
	sgpuNode& Node() { return m_sgpuNodes.size()?m_sgpuNodes.back():underRoot; }

	void Clear()
	{
		m_Transforms.clear();
		m_Layers.clear();
		m_FrontMaterials.clear();
		m_BackMaterials.clear();
		m_sgpuNodes.clear();
		m_sgpuNodesPresent.clear();
	}

	bool IsDetPositive() { return m_isDetPositive; }

protected: //Methods

    void PushTransform(CTransform t, bool createNode);
	static double Det( CTransform& t );

	SGPUExporter& m_exporter;

protected: //Data

    bool m_bMaterialsByLayer;
    std::vector<CTransform> m_Transforms;
    std::vector<ISkpLayer*> m_Layers;
    std::vector<ISkpMaterial*> m_FrontMaterials;
    std::vector<ISkpMaterial*> m_BackMaterials;
	std::vector<sgpuNode> m_sgpuNodes;
	std::vector<bool> m_sgpuNodesPresent;
	sgpuNode underRoot;
	bool m_isDetPositive;
};
