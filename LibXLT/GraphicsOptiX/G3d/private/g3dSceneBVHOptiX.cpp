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
#ifndef GOX_G3DGASCACHEOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dGasCacheOptiX.hpp"
#endif
#ifndef GOX_OPTXCHECK_HPP
#include "GraphicsOptiX/optx/optxCheck.hpp"
#endif
#ifndef GOX_OPTXCONTEXT_HPP
#include "GraphicsOptiX/optx/optxContext.hpp"
#endif
#ifndef GVK_TMESH_FRAG_HPP
#include "GraphicsVulkan/tmesh/tmeshFrag.hpp"
#endif
#ifndef GVK_HAIR_MODELFRAGVK_HPP
#include "GraphicsVulkan/hair/hairModelFragVk.hpp"
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
#ifndef G3D_RENDERSTATE_HPP
#include "Graphics/G3d/g3dRenderState.hpp"
#endif
#ifndef ENV_STRING_HPP
#include "Core/Env/envString.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <utility>

#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

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

		//	The index of a light set (sorted, without repeats) in io_Sets,
		//	added if new
		unsigned intern(const g3dSceneBVHOptiX::LightSet& i_Lights, std::vector<g3dSceneBVHOptiX::LightSet>& io_Sets)
		{
			g3dSceneBVHOptiX::LightSet set = i_Lights;
			std::sort(set.begin(), set.end());
			set.erase(std::unique(set.begin(), set.end()), set.end());
			for (size_t i = 0; i < io_Sets.size(); ++i)
			{
				if (io_Sets[i] == set)
				{
					return static_cast<unsigned>(i);
				}
			}
			io_Sets.push_back(set);
			return static_cast<unsigned>(io_Sets.size() - 1);
		}

		//	Camera rays see all but shadow hulls; occlusion rays what casts shadows
		unsigned mask_of(const gvk::tmeshFrag& i_Fragment)
		{
			return (i_Fragment.IsShadowHull() ? 0u : unsigned(e_optxMaskCamera)) |
				   (i_Fragment.GetCastsShadow() ? unsigned(e_optxMaskOcclusion) : 0u);
		}

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

	g3dSceneBVHOptiX::Pooling g3dSceneBVHOptiX::ReadPoolingEnvironment()
	{
		const char* value = std::getenv("MSP_GFX_OPTIX_POOLING");
		if (!value)
		{
			return e_PoolingGroup;
		}
		std::string mode = value;
		envString::ToLower(mode);
		if (mode == "none")
		{
			return e_PoolingNone;
		}
		if (mode != "group")
		{
			DBG_LOG("GraphicsOptiX: MSP_GFX_OPTIX_POOLING=\"" << value << "\" is unknown (use none or group); using group");
		}
		return e_PoolingGroup;
	}

	bool g3dSceneBVHOptiX::IsTraced(const g3dSceneNode& i_Node)
	{
		const gvk::tmeshFrag* fragment = dynamic_cast<const gvk::tmeshFrag*>(i_Node.GetFragment());
		return fragment && g3dGasCacheOptiX::IsTraceable(*fragment) && mask_of(*fragment) != 0;
	}

	bool g3dSceneBVHOptiX::IsRasterOverlay(const g3dSceneNode& i_Node)
	{
		const g3dFragment* fragment = i_Node.GetFragment();
		return !IsTraced(i_Node) && !(fragment && fragment->IsShadowHull()) &&
			   !dynamic_cast<const gvk::hairModelFragVk*>(fragment);
	}

	bool g3dSceneBVHOptiX::Entry::operator==(const Entry& i_Other) const
	{
		return m_pFragment == i_Other.m_pFragment && m_pMaterial == i_Other.m_pMaterial && m_bAnyHit == i_Other.m_bAnyHit &&
			   m_pVertices == i_Other.m_pVertices &&
			   m_pIndices == i_Other.m_pIndices && m_VertexGeneration == i_Other.m_VertexGeneration &&
			   m_IndexGeneration == i_Other.m_IndexGeneration && m_Mask == i_Other.m_Mask &&
			   m_LightSet == i_Other.m_LightSet && m_Flags == i_Other.m_Flags &&
			   std::memcmp(m_Transform, i_Other.m_Transform, sizeof(m_Transform)) == 0;
	}

	g3dSceneBVHOptiX::g3dSceneBVHOptiX(g3dGasCacheOptiX& i_Cache)
		: m_Cache(i_Cache)
		, m_Pooling(ReadPoolingEnvironment())
		, m_bBuilt(false)
		, m_nEnvironments(0)
		, m_Handle(0)
		, m_nInstances(0)
		, m_nSbtRecords(1)
		, m_nGas(0)
		, m_GasBytes(0)
		, m_nTriangles(0)
		, m_SceneVersion(0)
		, m_nIasBuilds(0)
	{
	}

	g3dSceneBVHOptiX::~g3dSceneBVHOptiX()
	{
	}

	void g3dSceneBVHOptiX::SetPooling(Pooling i_Pooling)
	{
		if (i_Pooling != m_Pooling)
		{
			m_Pooling = i_Pooling;
			Release();
		}
	}

	void g3dSceneBVHOptiX::gather(g3dSceneNode* i_pNode, bool i_bLowResolution, LightState& io_State,
								  std::vector<Entry>& io_Entries, std::vector<LightSet>& io_Sets,
								  EnvironmentCounts& io_Environments) const
	{
		if (!i_pNode || !i_pNode->GetRenderable() || !i_pNode->GetActiveInRenderLayer() ||
			!check_resolution(*i_pNode, i_bLowResolution))
		{
			return;
		}

		//	The node's render state applies to it and its subtree
		const g3dRenderState* state = i_pNode->GetRenderState();
		if (state)
		{
			++io_State.m_nStates;
			io_State.m_Lights.insert(io_State.m_Lights.end(), state->m_Lights.begin(), state->m_Lights.end());
		}
		//	and so does its environment
		const g3dAmbientEnvState* outer_environment = io_State.m_pEnvironment;
		if (i_pNode->GetEnvironment())
		{
			io_State.m_pEnvironment = i_pNode->GetEnvironment();
		}

		gvk::tmeshFrag* fragment = dynamic_cast<gvk::tmeshFrag*>(i_pNode->GetFragment());
		if (fragment && g3dGasCacheOptiX::IsTraceable(*fragment))
		{
			Entry entry;
			entry.m_pFragment = fragment;
			entry.m_pMaterial = fragment->GetMaterial();
			entry.m_bAnyHit = g3dGasCacheOptiX::NeedsAnyHit(*fragment);
			entry.m_pVertices = fragment->GetVertexBuffer().get();
			entry.m_pIndices = fragment->GetIndexBuffer().get();
			entry.m_VertexGeneration = fragment->GetVertexBuffer()->GetGeneration();
			entry.m_IndexGeneration = fragment->GetIndexBuffer()->GetGeneration();
			entry.m_Mask = mask_of(*fragment);
			entry.m_LightSet = io_State.m_nStates ? intern(io_State.m_Lights, io_Sets) : c_optxAllLights;
			entry.m_Flags = fragment->GetReceivesShadow() ? unsigned(e_optxInstanceReceivesShadow) : 0u;
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
				//	The traverser's default (no environment) outside every render state
				static const g3dAmbientEnvState s_None;
				const g3dAmbientEnvState& environment =
					(io_State.m_nStates && io_State.m_pEnvironment) ? *io_State.m_pEnvironment : s_None;
				auto counted = std::find_if(io_Environments.begin(), io_Environments.end(),
											[&](const std::pair<g3dAmbientEnvState, unsigned>& i_Count)
											{ return i_Count.first == environment; });
				if (counted != io_Environments.end())
				{
					++counted->second;
				}
				else
				{
					io_Environments.emplace_back(environment, 1u);
				}
			}
		}

		for (g3dSceneNode* child : i_pNode->GetChildren())
		{
			gather(child, i_bLowResolution, io_State, io_Entries, io_Sets, io_Environments);
		}
		io_State.m_pEnvironment = outer_environment;

		if (state)
		{
			io_State.m_Lights.resize(io_State.m_Lights.size() - state->m_Lights.size());
			--io_State.m_nStates;
		}
	}

	bool g3dSceneBVHOptiX::Update(const g3dScene& i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers, bool i_bLowResolution)
	{
		m_Gathered.clear();
		m_GatheredSets.clear();
		m_GatheredEnvironments.clear();
		for (const std::vector<g3dLayer*>* layers : { &i_Scene.GetLayers(), &i_ViewerLayers })
		{
			for (const g3dLayer* layer : *layers)
			{
				if (layer && layer->GetModelSpace() == g3dLayer::e_World)
				{
					LightState state;
					gather(layer->GetRootNode(), i_bLowResolution, state, m_Gathered, m_GatheredSets, m_GatheredEnvironments);
				}
			}
		}

		//	The environment most fragments have (the first seen, on a tie)
		m_Environment = g3dAmbientEnvState();
		m_nEnvironments = unsigned(m_GatheredEnvironments.size());
		unsigned most = 0;
		for (const std::pair<g3dAmbientEnvState, unsigned>& count : m_GatheredEnvironments)
		{
			if (count.second > most)
			{
				most = count.second;
				m_Environment = count.first;
			}
		}

		if (m_bBuilt && m_Gathered == m_Entries && m_GatheredSets == m_LightSets)
		{
			return false;
		}
		std::swap(m_Entries, m_Gathered);
		std::swap(m_LightSets, m_GatheredSets);
		optxContext::MakeCurrent();
		m_bBuilt = rebuild();
		++m_SceneVersion;
		return true;
	}

	//	Whether a group's entries (sharing a vertex buffer) can share one GAS:
	//	every fragment that uses the buffer is listed, each once, all with the
	//	same transform, mask, light set and flags
	bool g3dSceneBVHOptiX::poolable(const std::vector<size_t>& i_Group) const
	{
		if (i_Group.size() < 2)
		{
			return false;
		}
		const Entry& first = m_Entries[i_Group[0]];
		std::set<const gvk::tmeshFrag*> fragments;
		for (size_t index : i_Group)
		{
			const Entry& entry = m_Entries[index];
			if (entry.m_Mask != first.m_Mask || entry.m_LightSet != first.m_LightSet || entry.m_Flags != first.m_Flags ||
				std::memcmp(entry.m_Transform, first.m_Transform, sizeof(first.m_Transform)) != 0 ||
				!fragments.insert(entry.m_pFragment).second)
			{
				return false;
			}
		}
		return int(fragments.size()) == first.m_pFragment->GetVertexBuffer()->GetFragmentCount();
	}

	bool g3dSceneBVHOptiX::rebuild()
	{
		G3D_PROFILE_SCOPE("IasBuild");
		m_Handle = 0;
		m_nInstances = 0;
		m_nSbtRecords = 1;
		m_nGas = 0;
		m_GasBytes = 0;
		m_nTriangles = 0;
		m_Bounds = maAxisBox();
		m_Materials.clear();
		m_Cache.Collect();
		std::map<const matMaterial*, unsigned> material_of;
		auto material_index = [&](const gvk::tmeshFrag* i_pFragment) -> unsigned
		{
			const matMaterial* material = i_pFragment->GetMaterial();
			if (!material)
			{
				return c_optxNoMaterial;
			}
			auto found = material_of.find(material);
			if (found == material_of.end())
			{
				found = material_of.emplace(material, unsigned(m_Materials.size())).first;
				m_Materials.push_back(material);
			}
			return found->second;
		};

		//	The entries by vertex buffer, in the order first seen
		std::vector<std::vector<size_t>> groups;
		std::map<const void*, size_t> group_of;
		for (size_t i = 0; i < m_Entries.size(); ++i)
		{
			auto found = group_of.find(m_Entries[i].m_pVertices);
			if (found == group_of.end())
			{
				found = group_of.emplace(m_Entries[i].m_pVertices, groups.size()).first;
				groups.emplace_back();
			}
			groups[found->second].push_back(i);
		}

		//	A GAS for each group or fragment (the cache builds only what
		//	changed), then an instance for each that has one
		std::vector<OptixInstance> instances;
		std::vector<optxInstanceData> table;
		std::vector<optxGeometryInput> inputs;
		std::vector<size_t> first_input;	// per instance, its inputs' start in `inputs`
		std::set<OptixTraversableHandle> distinct;
		auto add = [&](const g3dGasCacheOptiX::Gas* i_pGas, const std::vector<const gvk::tmeshFrag*>& i_Fragments,
					   const Entry& i_Entry)
		{
			if (!i_pGas)
			{
				return;
			}
			OptixInstance instance = {};
			std::memcpy(instance.transform, i_Entry.m_Transform, sizeof(instance.transform));
			instance.instanceId = static_cast<unsigned>(instances.size());
			instance.sbtOffset = 0;
			instance.visibilityMask = i_Entry.m_Mask;
			instance.flags = OPTIX_INSTANCE_FLAG_DISABLE_TRIANGLE_FACE_CULLING;
			instance.traversableHandle = i_pGas->m_Handle;
			instances.push_back(instance);

			optxInstanceData data = {};
			data.m_Vertices = i_pGas->m_Vertices;
			data.m_VertexStride = i_pGas->m_VertexStride;
			data.m_NormalOffset = i_pGas->m_NormalOffset;
			data.m_TexCoordOffset = i_pGas->m_TexCoordOffset;
			data.m_TangentOffset = i_pGas->m_TangentOffset;
			data.m_BitangentOffset = i_pGas->m_BitangentOffset;
			data.m_NumInputs = static_cast<unsigned>(i_pGas->m_Inputs.size());
			data.m_NumTriangles = i_pGas->m_NumTriangles;
			data.m_LightSet = i_Entry.m_LightSet;
			data.m_Flags = i_Entry.m_Flags;
			table.push_back(data);
			first_input.push_back(inputs.size());
			for (size_t i = 0; i < i_pGas->m_Inputs.size(); ++i)
			{
				const g3dGasCacheOptiX::Input& input = i_pGas->m_Inputs[i];
				optxGeometryInput geometry = {};
				geometry.m_Indices = input.m_Indices;
				geometry.m_IndexSize = input.m_IndexSize;
				geometry.m_NumTriangles = input.m_NumTriangles;
				geometry.m_Material = i < i_Fragments.size() ? material_index(i_Fragments[i]) : c_optxNoMaterial;
				inputs.push_back(geometry);
			}

			m_nSbtRecords = std::max(m_nSbtRecords, data.m_NumInputs);
			m_nTriangles += i_pGas->m_NumTriangles;
			if (distinct.insert(i_pGas->m_Handle).second)
			{
				m_GasBytes += i_pGas->m_Bytes;
			}
			for (const gvk::tmeshFrag* fragment : i_Fragments)
			{
				add_bounds(fragment->GetBoundingBox(), i_Entry.m_Transform, m_Bounds);
			}
		};

		std::vector<const gvk::tmeshFrag*> fragments;
		for (const std::vector<size_t>& group : groups)
		{
			if (m_Pooling == e_PoolingGroup && poolable(group))
			{
				fragments.clear();
				for (size_t index : group)
				{
					fragments.push_back(m_Entries[index].m_pFragment);
				}
				const g3dGasCacheOptiX::Gas* gas = m_Cache.Ensure(fragments.data(), fragments.size());
				if (gas)
				{
					add(gas, fragments, m_Entries[group[0]]);
					continue;
				}
			}
			for (size_t index : group)
			{
				const Entry& entry = m_Entries[index];
				fragments.assign(1, entry.m_pFragment);
				add(m_Cache.Ensure(*entry.m_pFragment), fragments, entry);
			}
		}
		m_nGas = static_cast<unsigned>(distinct.size());

		//	The input arrays, then the table pointing into them
		if (!m_InputTable.Upload(inputs.data(), inputs.size() * sizeof(optxGeometryInput)))
		{
			return false;
		}
		for (size_t i = 0; i < table.size(); ++i)
		{
			table[i].m_Inputs = m_InputTable.Get() + first_input[i] * sizeof(optxGeometryInput);
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
		m_InputTable.Free();
		m_Temp.Free();
		m_Ias.Free();
		m_Handle = 0;
		m_nInstances = 0;
		m_nSbtRecords = 1;
		m_nGas = 0;
		m_GasBytes = 0;
		m_nTriangles = 0;
		m_Entries.clear();
		m_LightSets.clear();
		m_Materials.clear();
		m_bBuilt = false;
	}
}
