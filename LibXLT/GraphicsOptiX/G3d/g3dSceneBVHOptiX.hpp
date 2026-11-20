/*****************************************************************************
**  g3dSceneBVHOptiX.hpp
**
**      g3dSceneBVHOptiX is a scene's geometry as OptiX sees it: an
**	instance acceleration structure (IAS) over GAS from g3dGasCacheOptiX,
**	and the instance table (optxSceneTypes.h) device programs read hit
**	vertices through. A renderer keeps one (each renderer can see a
**	different scene) and calls Update before each launch; the GAS are
**	shared through the cache.
**
**	Update walks the root node of every world-space layer, the scene's and
**	the viewer's (other layers are skipped for now), with the raster
**	renderers' filters (GetRenderable, GetActiveInRenderLayer, the content
**	resolution), and lists each node whose fragment is a GraphicsVulkan
**	tmeshFrag triangle list (bump fragments are): the fragment, its vertex
**	and index buffers and their generations, the node's world transform,
**	a visibility mask (camera rays see all but shadow hulls, occlusion
**	rays what casts shadows), whether it receives shadows, and its light
**	set, its material, and whether that material can let rays through
**	(g3dGasCacheOptiX::NeedsAnyHit). The walk costs no GPU work.
**
**	Materials: the distinct materials of the listed fragments, in the
**	order first seen, are GetMaterials(); each build input's
**	optxGeometryInput::m_Material is its fragment's index in that list (a
**	renderer's material table converts the list, g3dMaterialTableOptiX).
**	A fragment given another material, or a material that starts or stops
**	letting rays through, changes the list: the BVH is rebuilt.
**
**	Light sets (light linking) follow the raster's render state traverser
**	(GraphicsVulkan's g3dRenderStateTraverser): a node under no render
**	state is reached by every light (c_optxAllLights); under one or more,
**	by the lights those render states name, together. Each distinct set is
**	listed once (GetLightSets), and instances name it by index; the light
**	table (g3dLightTableOptiX) turns the sets into light indices. If the list
**	equals the last one, nothing changed: Update returns false and touches
**	nothing. Otherwise it asks the cache for each GAS (which does nothing
**	for unchanged ones), rebuilds the IAS from scratch, uploads the
**	instance table, and returns true, raising the scene version.
**
**	Pooling (MSP_GFX_OPTIX_POOLING, or SetPooling):
**	- group (the default): the fragments of a group (one per material,
**	  sharing one vertex buffer) get one GAS with a build input each, and
**	  one instance, when every fragment of the group is listed, each once,
**	  with the same transform and mask (the usual case: they hang off one
**	  model node). Otherwise (a material hidden, say) they get a GAS each.
**	- none: every fragment gets its own GAS and instance.
**	Both give the same hits; only the GAS and instance counts differ.
**
**	Fragments outlive nodes, and a node may be deleted while the last list
**	still names its fragment: the list is only compared, never followed,
**	and generations are unique process-wide, so a deleted fragment (or a
**	new one at its address) only reads as a change.
**
**	Render thread; Update makes the OptiX context current.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G3DSCENEBVHOPTIX_HPP
#error g3dSceneBVHOptiX.hpp multiply included
#endif
#define GOX_G3DSCENEBVHOPTIX_HPP

#ifndef GOX_CUBUFFER_HPP
#include "GraphicsOptiX/cu/cuBuffer.hpp"
#endif
#ifndef GOX_OPTXSCENETYPES_H
#include "GraphicsOptiX/optx/private/Programs/optxSceneTypes.h"
#endif
#ifndef GOX_OPTXMATERIALTYPES_H
#include "GraphicsOptiX/optx/private/Programs/optxMaterialTypes.h"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/Ma/maAxisBox.hpp"
#endif

#include <cstdint>
#include <vector>

class g3dLayer;
class g3dLight;
class matMaterial;
class g3dScene;
class g3dSceneNode;

namespace gvk
{
	class tmeshFrag;
}

namespace gox
{
	class g3dGasCacheOptiX;

	class g3dSceneBVHOptiX
	{
		public:
			typedef std::vector<const g3dLight*> LightSet;

			enum Pooling
			{
				e_PoolingNone,		// a GAS and an instance per fragment
				e_PoolingGroup		// a GAS and an instance per fragment group
			};

			//--------------------------------------------------------------------
			//	The pooling MSP_GFX_OPTIX_POOLING asks for ("none" or "group",
			//	any case); group if it is unset or unknown (logged)
			//--------------------------------------------------------------------
			static Pooling ReadPoolingEnvironment();

			//--------------------------------------------------------------------
			//	IsTraced - whether Update lists a node's fragment when the walk
			//	reaches it (a traceable tmeshFrag seen by camera or occlusion
			//	rays): what a raster pass over the traced image skips
			//--------------------------------------------------------------------
			static bool IsTraced(const g3dSceneNode& i_Node);

			//--------------------------------------------------------------------
			//	IsRasterOverlay - whether a raster pass over the traced image
			//	draws a node: not if it is traced, nor a shadow hull (never
			//	seen by the camera, traced or not), nor strand hair (a
			//	hairModelFragVk), which isn't drawn at all until hair is
			//	traced
			//--------------------------------------------------------------------
			static bool IsRasterOverlay(const g3dSceneNode& i_Node);

			explicit g3dSceneBVHOptiX(g3dGasCacheOptiX& i_Cache);
			~g3dSceneBVHOptiX();
			g3dSceneBVHOptiX(const g3dSceneBVHOptiX&) = delete;
			g3dSceneBVHOptiX& operator=(const g3dSceneBVHOptiX&) = delete;

			//--------------------------------------------------------------------
			//	SetPooling - a change releases the BVH; the next Update rebuilds
			//--------------------------------------------------------------------
			void SetPooling(Pooling i_Pooling);
			Pooling GetPooling() const { return m_Pooling; }

			//--------------------------------------------------------------------
			//	Update - see above. i_bLowResolution is the renderers' low
			//	resolution preference (g3dPrefs::m_bLowResolution). Returns
			//	whether the BVH changed.
			//--------------------------------------------------------------------
			bool Update(const g3dScene& i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers, bool i_bLowResolution);

			//--------------------------------------------------------------------
			//	Release - frees everything on the device; the next Update
			//	rebuilds it
			//--------------------------------------------------------------------
			void Release();

			//--------------------------------------------------------------------
			//	GetHandle - the IAS (0 if the scene has no triangles: rays miss)
			//--------------------------------------------------------------------
			OptixTraversableHandle GetHandle() const { return m_Handle; }

			//--------------------------------------------------------------------
			//	The instance table: optxInstanceData per instance, indexed by
			//	instance ID
			//--------------------------------------------------------------------
			CUdeviceptr GetInstanceTable() const { return m_InstanceTable.Get(); }
			unsigned GetInstanceCount() const { return m_nInstances; }

			//--------------------------------------------------------------------
			//	GetSbtRecordCount - the most build inputs any instance's GAS has
			//	(at least 1): a pipeline tracing this BVH needs that many hit
			//	group records (instances have SBT offset 0, so a hit's record is
			//	its build input's, optixGetSbtGASIndex())
			//--------------------------------------------------------------------
			unsigned GetSbtRecordCount() const { return m_nSbtRecords; }

			//--------------------------------------------------------------------
			//	GetSceneVersion - raised each time Update returns true;
			//	accumulation restarts when it changes
			//--------------------------------------------------------------------
			uint64_t GetSceneVersion() const { return m_SceneVersion; }

			//--------------------------------------------------------------------
			//	GetBounds - the instances' world bounds (empty without any)
			//--------------------------------------------------------------------
			const maAxisBox& GetBounds() const { return m_Bounds; }

			//--------------------------------------------------------------------
			//	For tests and the profiler: IAS builds so far; distinct GAS
			//	among the instances, their bytes, the IAS's bytes and the
			//	triangles in the scene, as of the last rebuild
			//--------------------------------------------------------------------
			int GetIasBuildCount() const { return m_nIasBuilds; }
			unsigned GetGasCount() const { return m_nGas; }
			size_t GetGasBytes() const { return m_GasBytes; }
			size_t GetIasBytes() const { return m_Ias.GetSize(); }
			unsigned GetTriangleCount() const { return m_nTriangles; }

			//--------------------------------------------------------------------
			//	GetLightSets - the light sets the instances name (sorted lights),
			//	as of the last Update
			//--------------------------------------------------------------------
			const std::vector<LightSet>& GetLightSets() const { return m_LightSets; }

			//--------------------------------------------------------------------
			//	GetMaterials - the materials build inputs name by index, as of the
			//	last Update
			//--------------------------------------------------------------------
			const std::vector<const matMaterial*>& GetMaterials() const { return m_Materials; }

		private:
			struct Entry
			{
				gvk::tmeshFrag*		m_pFragment;
				const matMaterial*	m_pMaterial;
				bool				m_bAnyHit;
				const void*			m_pVertices;
				const void*			m_pIndices;
				uint64_t			m_VertexGeneration;
				uint64_t			m_IndexGeneration;
				float				m_Transform[12];	// OptiX's row-major 3x4, for column vectors
				unsigned			m_Mask;
				unsigned			m_LightSet;		// index into the light sets, or c_optxAllLights
				unsigned			m_Flags;		// optxInstanceFlags

				bool operator==(const Entry& i_Other) const;
			};

			//	The walk's render state: the lights the render states above
			//	name, and how many render states there are
			struct LightState
			{
				LightSet	m_Lights;
				int			m_nStates = 0;
			};

			void gather(g3dSceneNode* i_pNode, bool i_bLowResolution, LightState& io_State, std::vector<Entry>& io_Entries,
						std::vector<LightSet>& io_Sets) const;
			bool poolable(const std::vector<size_t>& i_Group) const;
			bool rebuild();

			g3dGasCacheOptiX&	m_Cache;
			Pooling				m_Pooling;
			std::vector<Entry>	m_Entries;			// the last Update's list
			std::vector<Entry>	m_Gathered;			// this Update's (kept for its capacity)
			std::vector<LightSet>	m_LightSets;		// the last Update's
			std::vector<LightSet>	m_GatheredSets;		// this Update's
			std::vector<const matMaterial*>	m_Materials;	// as of the last rebuild
			bool				m_bBuilt;			// m_Entries describes what is on the device
			cuBuffer			m_Instances;		// OptixInstance array
			cuBuffer			m_InstanceTable;	// optxInstanceData array
			cuBuffer			m_InputTable;		// optxGeometryInput arrays the instances point into
			cuBuffer			m_Temp;
			cuBuffer			m_Ias;
			OptixTraversableHandle	m_Handle;
			unsigned			m_nInstances;
			unsigned			m_nSbtRecords;
			unsigned			m_nGas;
			size_t				m_GasBytes;
			unsigned			m_nTriangles;
			uint64_t			m_SceneVersion;
			maAxisBox			m_Bounds;
			int					m_nIasBuilds;
	};
}
