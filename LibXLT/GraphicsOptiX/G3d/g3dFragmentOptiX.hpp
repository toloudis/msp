/*****************************************************************************
**  g3dFragmentOptiX.hpp
**
**      g3dFragmentOptiX is the OptiX backend's mesh fragment: its vertices
**	(in a g3dVertexStoreOptiX, which fragments may share) and its 32-bit
**	indices, kept in CPU memory, and the geometry acceleration structure
**	(GAS) built from them. Vertices are g3dType::BumpTex1Vertex (position,
**	normal, UV, tangent S and T), the format GraphicsDX11's and
**	GraphicsVulkan's fragments report, so code that locks a fragment and
**	writes its vertices works unchanged. Lock and LockIndices give the
**	arrays themselves; Unlock, UpdateVertices and UnlockIndices mark them
**	changed (the store's generation, or the fragment's index generation).
**
**	EnsureGAS, called by the scene BVH on the render thread, uploads and
**	builds only what changed since the GAS was built:
**	- a static fragment (not morphable) is built for fast tracing and
**	  compacted;
**	- a morphable one is built to allow updates, and refit when only its
**	  vertices changed; it is rebuilt when its vertex or index count or its
**	  indices change, and after c_MaxRefits refits in a row (refitting
**	  slowly makes the BVH worse).
**	Line lists, and fragments without vertices or triangles, have no GAS.
**
**	Deallocate frees everything on the device and marks the vertices and
**	indices changed, so the next EnsureGAS makes it all again. A copy
**	(CloneFragment) copies the CPU data into its own store and shares
**	nothing on the device.
**
**	Tangents are left zero.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G3DFRAGMENTOPTIX_HPP
#error g3dFragmentOptiX.hpp multiply included
#endif
#define GOX_G3DFRAGMENTOPTIX_HPP

#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_TYPE_HPP
#include "Graphics/G3d/g3dType.hpp"
#endif
#ifndef GOX_G3DVERTEXSTOREOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dVertexStoreOptiX.hpp"
#endif
#ifndef GOX_CUBUFFER_HPP
#include "GraphicsOptiX/cu/cuBuffer.hpp"
#endif

#include <atomic>
#include <cstdint>
#include <memory>
#include <vector>

class g3dIndexPtr;

namespace gox
{
	class g3dFragmentOptiX : public g3dFragment
	{
		public:
			enum PrimitiveType { e_TriangleList, e_LineList };

			//	Refits of a morphable fragment's GAS before it is rebuilt
			static const int c_MaxRefits = 64;

			//--------------------------------------------------------------------
			//	i_Normals, i_UVs, i_Ss and i_Ts may be NULL (zero).
			//--------------------------------------------------------------------
			g3dFragmentOptiX(const maPoint3d* i_Vertices,
							 const maPoint3d* i_Normals,
							 const maPoint2d* i_UVs,
							 const maPoint3d* i_Ss,
							 const maPoint3d* i_Ts,
							 int i_nVertices,
							 const g3dIndexPtr& i_Indices,
							 int i_nIndices,
							 matMaterial* i_pMaterial,
							 bool i_bMorphable,
							 PrimitiveType i_PrimitiveType);
			g3dFragmentOptiX(const g3dFragmentOptiX& i_Frag);
			virtual ~g3dFragmentOptiX();

			PrimitiveType GetPrimitiveType() const { return m_PrimitiveType; }

			//--------------------------------------------------------------------
			//	GetStore - the vertices (shared with the other fragments of a
			//	group)
			//--------------------------------------------------------------------
			const std::shared_ptr<g3dVertexStoreOptiX>& GetStore() const { return m_pStore; }

			//--------------------------------------------------------------------
			//	GetVertexGeneration, GetIndexGeneration - change whenever the
			//	vertices or the indices do (see g3dVertexStoreOptiX)
			//--------------------------------------------------------------------
			uint64_t GetVertexGeneration() const { return m_pStore->GetGeneration(); }
			uint64_t GetIndexGeneration() const { return m_IndexGeneration.load(); }

			//--------------------------------------------------------------------
			//	EnsureGAS - makes the GAS current (see above) and returns its
			//	handle, 0 if the fragment has none. Render thread, with the
			//	OptiX context current; the work goes on optxContext's stream.
			//--------------------------------------------------------------------
			OptixTraversableHandle EnsureGAS();

			//--------------------------------------------------------------------
			//	For tests: GAS builds and refits so far, and the device copy of
			//	the indices
			//--------------------------------------------------------------------
			int GetGasBuildCount() const { return m_nGasBuilds; }
			int GetGasRefitCount() const { return m_nGasRefits; }
			CUdeviceptr GetDeviceIndices() const { return m_DeviceIndices.Get(); }

			virtual void Deallocate();
			virtual void Reallocate();
			virtual void UpdateVertices(int i_NumVertices, const maPoint3d* i_pVertices, const maVector3d* i_pNormals = NULL);
			virtual int GetNumVertices() const;
			virtual int GetNumIndices() const;
			virtual g3dType::VertexFormat GetVertexFormat() const;
			virtual unsigned char* Lock();
			virtual void Unlock();
			virtual unsigned char* LockIndices();
			virtual void UnlockIndices();
			virtual unsigned char* ReadOnlyLock();
			virtual void ReadOnlyUnlock();
			virtual unsigned char* ReadOnlyLockIndices();
			virtual void ReadOnlyUnlockIndices();
			virtual void ComponentSort(const maMatrix4x4& i_Transform, const maPoint3d& i_CameraPos);

		private:
			void update_bounding_box();
			void release_gas();
			bool upload_indices(unsigned i_nTriangles);
			bool build_gas(const g3dVertexStoreOptiX::DeviceVertices& i_Vertices, unsigned i_nTriangles, bool i_bRefit);

			std::shared_ptr<g3dVertexStoreOptiX>	m_pStore;
			std::vector<uint32_t>					m_Indices;
			std::atomic<uint64_t>					m_IndexGeneration;
			PrimitiveType							m_PrimitiveType;

			//	Render thread: the device indices and the GAS
			cuBuffer								m_DeviceIndices;
			uint64_t								m_DeviceIndexGeneration;	// 0: not uploaded
			uint32_t								m_MaxIndex;					// of the uploaded triangles
			cuBuffer								m_Gas;
			cuBuffer								m_GasTemp;					// morphable: build and refit scratch
			OptixTraversableHandle					m_GasHandle;
			uint64_t								m_GasVertexGeneration;		// what the GAS (or its absence) is from
			uint64_t								m_GasIndexGeneration;
			unsigned								m_GasVertexCount;
			unsigned								m_GasTriangleCount;
			int										m_nRefitsSinceBuild;
			int										m_nGasBuilds;
			int										m_nGasRefits;
	};
}
