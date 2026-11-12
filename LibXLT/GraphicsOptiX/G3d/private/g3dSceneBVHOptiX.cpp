/*****************************************************************************
**  g3dSceneBVHOptiX.cpp
**
**      See g3dSceneBVHOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G3DSCENEBVHOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dSceneBVHOptiX.hpp"
#endif
#ifndef GOX_G3DFRAGMENTOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dFragmentOptiX.hpp"
#endif
#ifndef GOX_OPTXCHECK_HPP
#include "GraphicsOptiX/optx/optxCheck.hpp"
#endif
#ifndef GOX_OPTXCONTEXT_HPP
#include "GraphicsOptiX/optx/optxContext.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/G3d/g3dScene.hpp"
#endif
#ifndef G3D_LAYER_HPP
#include "Graphics/G3d/g3dLayer.hpp"
#endif
#ifndef G3D_SCENENODE_HPP
#include "Graphics/G3d/g3dSceneNode.hpp"
#endif

#include <cstring>
#include <utility>

namespace gox
{
	namespace
	{
		//	The raster renderers' resolution filter (GraphicsVulkan's
		//	g3dSceneRenderUtil::check_resolution): false if the node's content
		//	is for the other resolution. A node may force low resolution on
		//	its subtree.
		bool check_resolution(const g3dSceneNode& i_Node, bool& io_bLowResolution)
		{
			if (i_Node.GetForceLowResolution())
			{
				io_bLowResolution = true;
			}
			switch (i_Node.GetContentResolution())
			{
				case g3dSceneNode::e_LowRes:
					return io_bLowResolution;
				case g3dSceneNode::e_HighRes:
					return !io_bLowResolution;
				default:
					return true;
			}
		}

		//	maMatrix4x4 multiplies row vectors (p * M, translation in row 3);
		//	OptiX takes a row-major 3x4 matrix for column vectors
		void to_optix(const maMatrix4x4& i_Matrix, float o_Transform[12])
		{
			for (int r = 0; r < 3; ++r)
			{
				for (int c = 0; c < 4; ++c)
				{
					o_Transform[r * 4 + c] = i_Matrix(c, r);
				}
			}
		}

		const float c_Identity[12] = { 1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0 };

		//	Grows io_Bounds by i_Box under the transform
		void add_bounds(const maAxisBox& i_Box, const float i_Transform[12], maAxisBox& io_Bounds)
		{
			if (i_Box.IsEmpty())
			{
				return;
			}
			for (int corner = 0; corner < 8; ++corner)
			{
				const float p[3] = { (corner & 1) ? i_Box.GetMaxX() : i_Box.GetMinX(),
									 (corner & 2) ? i_Box.GetMaxY() : i_Box.GetMinY(),
									 (corner & 4) ? i_Box.GetMaxZ() : i_Box.GetMinZ() };
				float w[3];
				for (int r = 0; r < 3; ++r)
				{
					w[r] = i_Transform[r * 4] * p[0] + i_Transform[r * 4 + 1] * p[1] + i_Transform[r * 4 + 2] * p[2] +
						   i_Transform[r * 4 + 3];
				}
				io_Bounds.Union(maPoint3d(w[0], w[1], w[2]));
			}
		}
	}

	bool g3dSceneBVHOptiX::Entry::operator==(const Entry& i_Other) const
	{
		return m_pFragment == i_Other.m_pFragment && m_VertexGeneration == i_Other.m_VertexGeneration &&
			   m_IndexGeneration == i_Other.m_IndexGeneration && m_Mask == i_Other.m_Mask &&
			   std::memcmp(m_Transform, i_Other.m_Transform, sizeof(m_Transform)) == 0;
	}

	g3dSceneBVHOptiX::g3dSceneBVHOptiX()
		: m_bBuilt(false)
		, m_Handle(0)
		, m_nInstances(0)
		, m_SceneVersion(0)
		, m_nIasBuilds(0)
	{
	}

	g3dSceneBVHOptiX::~g3dSceneBVHOptiX()
	{
	}

	void g3dSceneBVHOptiX::gather(g3dSceneNode* i_pNode, bool i_bLowResolution, std::vector<Entry>& io_Entries) const
	{
		if (!i_pNode || !i_pNode->GetRenderable() || !i_pNode->GetActiveInRenderLayer() ||
			!check_resolution(*i_pNode, i_bLowResolution))
		{
			return;
		}

		g3dFragmentOptiX* fragment = dynamic_cast<g3dFragmentOptiX*>(i_pNode->GetFragment());
		if (fragment && fragment->GetPrimitiveType() == g3dFragmentOptiX::e_TriangleList && fragment->GetNumIndices() >= 3)
		{
			Entry entry;
			entry.m_pFragment = fragment;
			entry.m_VertexGeneration = fragment->GetVertexGeneration();
			entry.m_IndexGeneration = fragment->GetIndexGeneration();
			entry.m_Mask = (fragment->IsShadowHull() ? 0u : unsigned(e_optxMaskCamera)) |
						   (fragment->GetCastsShadow() ? unsigned(e_optxMaskOcclusion) : 0u);
			if (fragment->IsModelSpaceVertices())
			{
				to_optix(i_pNode->GetTotalTransform(), entry.m_Transform);
			}
			else
			{
				std::memcpy(entry.m_Transform, c_Identity, sizeof(c_Identity));
			}
			if (entry.m_Mask)
			{
				io_Entries.push_back(entry);
			}
		}

		for (g3dSceneNode* child : i_pNode->GetChildren())
		{
			gather(child, i_bLowResolution, io_Entries);
		}
	}

	bool g3dSceneBVHOptiX::Update(const g3dScene& i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers, bool i_bLowResolution)
	{
		m_Gathered.clear();
		for (const std::vector<g3dLayer*>* layers : { &i_Scene.GetLayers(), &i_ViewerLayers })
		{
			for (const g3dLayer* layer : *layers)
			{
				if (layer && layer->GetModelSpace() == g3dLayer::e_World)
				{
					gather(layer->GetRootNode(), i_bLowResolution, m_Gathered);
				}
			}
		}

		if (m_bBuilt && m_Gathered == m_Entries)
		{
			return false;
		}
		std::swap(m_Entries, m_Gathered);
		optxContext::MakeCurrent();
		m_bBuilt = rebuild();
		++m_SceneVersion;
		return true;
	}

	bool g3dSceneBVHOptiX::rebuild()
	{
		m_Handle = 0;
		m_nInstances = 0;
		m_Bounds = maAxisBox();

		//	The fragments' GAS (each only once: EnsureGAS does nothing for a
		//	current one), then an instance for each node whose fragment has one
		std::vector<OptixInstance> instances;
		std::vector<optxInstanceData> table;
		instances.reserve(m_Entries.size());
		table.reserve(m_Entries.size());
		for (const Entry& entry : m_Entries)
		{
			const OptixTraversableHandle gas = entry.m_pFragment->EnsureGAS();
			if (!gas)
			{
				continue;
			}
			OptixInstance instance = {};
			std::memcpy(instance.transform, entry.m_Transform, sizeof(instance.transform));
			instance.instanceId = static_cast<unsigned>(instances.size());
			instance.sbtOffset = 0;
			instance.visibilityMask = entry.m_Mask;
			instance.flags = OPTIX_INSTANCE_FLAG_DISABLE_TRIANGLE_FACE_CULLING;
			instance.traversableHandle = gas;
			instances.push_back(instance);

			optxInstanceData data = {};
			data.m_Vertices = entry.m_pFragment->GetDeviceVertices();
			data.m_Indices = entry.m_pFragment->GetDeviceIndices();
			data.m_VertexStride = sizeof(g3dType::BumpTex1Vertex);
			data.m_NumTriangles = entry.m_pFragment->GetGasTriangleCount();
			table.push_back(data);

			add_bounds(entry.m_pFragment->GetBoundingBox(), entry.m_Transform, m_Bounds);
		}

		if (!m_InstanceTable.Upload(table.data(), table.size() * sizeof(optxInstanceData)))
		{
			return false;
		}
		if (instances.empty())
		{
			return true;
		}

		if (!m_Instances.Upload(instances.data(), instances.size() * sizeof(OptixInstance)))
		{
			return false;
		}
		OptixBuildInput input = {};
		input.type = OPTIX_BUILD_INPUT_TYPE_INSTANCES;
		input.instanceArray.instances = m_Instances.Get();
		input.instanceArray.numInstances = static_cast<unsigned>(instances.size());

		OptixAccelBuildOptions options = {};
		options.buildFlags = OPTIX_BUILD_FLAG_PREFER_FAST_TRACE;
		options.operation = OPTIX_BUILD_OPERATION_BUILD;

		const OptixDeviceContext context = optxContext::GetOptix();
		OptixAccelBufferSizes sizes = {};
		if (OPTIX_CHECK(optixAccelComputeMemoryUsage(context, &options, &input, 1, &sizes)) ||
			!m_Temp.Reserve(sizes.tempSizeInBytes) || !m_Ias.Reserve(sizes.outputSizeInBytes))
		{
			return false;
		}
		OptixTraversableHandle handle = 0;
		if (OPTIX_CHECK(optixAccelBuild(context, optxContext::GetStream(), &options, &input, 1,
										m_Temp.Get(), m_Temp.GetSize(), m_Ias.Get(), m_Ias.GetSize(),
										&handle, NULL, 0)))
		{
			return false;
		}
		m_Handle = handle;
		m_nInstances = static_cast<unsigned>(instances.size());
		++m_nIasBuilds;
		return true;
	}

	void g3dSceneBVHOptiX::Release()
	{
		m_Instances.Free();
		m_InstanceTable.Free();
		m_Temp.Free();
		m_Ias.Free();
		m_Handle = 0;
		m_nInstances = 0;
		m_Entries.clear();
		m_bBuilt = false;
	}
}
