/*****************************************************************************
**  g3dFragmentOptiX.hpp
**
**      g3dFragmentOptiX is the OptiX backend's mesh fragment: its vertices
**	and indices kept in CPU memory, from which OptiX geometry will be
**	built. Vertices are g3dType::BumpTex1Vertex (position, normal, UV,
**	tangent S and T), the format GraphicsDX11's and GraphicsVulkan's
**	fragments report, so code that locks a fragment and writes its
**	vertices works unchanged; indices are 32-bit. Lock and LockIndices give
**	the arrays themselves.
**
**	Not drawn yet. Tangents are left zero.
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

#include <cstdint>
#include <vector>

class g3dIndexPtr;

namespace gox
{
	class g3dFragmentOptiX : public g3dFragment
	{
		public:
			enum PrimitiveType { e_TriangleList, e_LineList };

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

			std::vector<g3dType::BumpTex1Vertex>	m_Vertices;
			std::vector<uint32_t>					m_Indices;
			PrimitiveType							m_PrimitiveType;
	};
}
