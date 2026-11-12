/*****************************************************************************
**  g3dSceneBVHOptiX.hpp
**
**      g3dSceneBVHOptiX is a scene's geometry as OptiX sees it: an
**	instance acceleration structure (IAS) over the fragments' GAS, one
**	instance per scene node that has triangles, and the instance table
**	(optxSceneTypes.h) device programs read hit vertices through. A
**	renderer keeps one (each renderer can see a different scene) and calls
**	Update before each launch.
**
**	Update walks the root node of every world-space layer, the scene's and
**	the viewer's (other layers are skipped for now), with the raster
**	renderers' filters (GetRenderable, GetActiveInRenderLayer, the content
**	resolution), and lists each node whose fragment is a g3dFragmentOptiX
**	triangle list: the fragment, its vertex and index generations, the
**	node's world transform, and a visibility mask (camera rays see all
**	but shadow hulls, occlusion rays what casts shadows). The walk costs no
**	GPU work. If the list equals the last one, nothing changed: Update
**	returns false and touches nothing. Otherwise it brings each listed
**	fragment's GAS up to date (EnsureGAS, which does nothing for unchanged
**	ones), rebuilds the IAS from scratch, uploads the instance table, and
**	returns true, raising the scene version.
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
#ifndef MA_AXISBOX_HPP
#include "Core/Ma/maAxisBox.hpp"
#endif

#include <cstdint>
#include <vector>

class g3dLayer;
class g3dScene;
class g3dSceneNode;

namespace gox
{
	class g3dFragmentOptiX;

	class g3dSceneBVHOptiX
	{
		public:
			g3dSceneBVHOptiX();
			~g3dSceneBVHOptiX();
			g3dSceneBVHOptiX(const g3dSceneBVHOptiX&) = delete;
			g3dSceneBVHOptiX& operator=(const g3dSceneBVHOptiX&) = delete;

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
			//	GetSceneVersion - raised each time Update returns true;
			//	accumulation restarts when it changes
			//--------------------------------------------------------------------
			uint64_t GetSceneVersion() const { return m_SceneVersion; }

			//--------------------------------------------------------------------
			//	GetBounds - the instances' world bounds (empty without any)
			//--------------------------------------------------------------------
			const maAxisBox& GetBounds() const { return m_Bounds; }

			//--------------------------------------------------------------------
			//	For tests: IAS builds so far
			//--------------------------------------------------------------------
			int GetIasBuildCount() const { return m_nIasBuilds; }

		private:
			struct Entry
			{
				g3dFragmentOptiX*	m_pFragment;
				uint64_t			m_VertexGeneration;
				uint64_t			m_IndexGeneration;
				float				m_Transform[12];	// OptiX's row-major 3x4, for column vectors
				unsigned			m_Mask;

				bool operator==(const Entry& i_Other) const;
			};

			void gather(g3dSceneNode* i_pNode, bool i_bLowResolution, std::vector<Entry>& io_Entries) const;
			bool rebuild();

			std::vector<Entry>	m_Entries;			// the last Update's list
			std::vector<Entry>	m_Gathered;			// this Update's (kept for its capacity)
			bool				m_bBuilt;			// m_Entries describes what is on the device
			cuBuffer			m_Instances;		// OptixInstance array
			cuBuffer			m_InstanceTable;	// optxInstanceData array
			cuBuffer			m_Temp;
			cuBuffer			m_Ias;
			OptixTraversableHandle	m_Handle;
			unsigned			m_nInstances;
			uint64_t			m_SceneVersion;
			maAxisBox			m_Bounds;
			int					m_nIasBuilds;
	};
}
