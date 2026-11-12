/*****************************************************************************
**  g3dVertexBufferOptiX.hpp
**
**      g3dVertexBufferOptiX is the vertex buffer a group of fragments
**	shares (mdlFragCreate::CreateFragmentGroup: an imported mesh with one
**	fragment per material): a g3dVertexBuffer over the g3dVertexStoreOptiX
**	the group's fragments hold. Lock gives the store's vertices; Unlock and
**	UpdateVertices publish them (a new generation, so every fragment of the
**	group sees the change and the store is uploaded once). SetVertices
**	replaces them all, for UpdateFragmentGroup.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G3DVERTEXBUFFEROPTIX_HPP
#error g3dVertexBufferOptiX.hpp multiply included
#endif
#define GOX_G3DVERTEXBUFFEROPTIX_HPP

#ifndef G3D_VERTEXBUFFER_HPP
#include "Graphics/G3d/g3dVertexBuffer.hpp"
#endif
#ifndef GOX_G3DVERTEXSTOREOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dVertexStoreOptiX.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/Ma/maAxisBox.hpp"
#endif

#include <memory>
#include <vector>

namespace gox
{
	class g3dVertexBufferOptiX : public g3dVertexBuffer
	{
		public:
			explicit g3dVertexBufferOptiX(const std::shared_ptr<g3dVertexStoreOptiX>& i_pStore);
			virtual ~g3dVertexBufferOptiX();

			const std::shared_ptr<g3dVertexStoreOptiX>& GetStore() const { return m_pStore; }

			//--------------------------------------------------------------------
			//	SetVertices - replaces the vertices (and their count), and
			//	publishes them
			//--------------------------------------------------------------------
			void SetVertices(std::vector<g3dType::BumpTex1Vertex>&& i_Vertices);

			const maAxisBox& GetBoundingBox() const { return m_BoundingBox; }

			virtual void UpdateVertices(int i_NumVertices, const maPoint3d* i_pVertices, const maVector3d* i_pNormals = NULL);
			virtual int GetNumVertices() const;
			virtual g3dType::VertexFormat GetVertexFormat() const;
			virtual unsigned char* Lock();
			virtual void Unlock();
			virtual void SetBoundingBox(const maAxisBox& i_BoundingBox);

		private:
			std::shared_ptr<g3dVertexStoreOptiX>	m_pStore;
			maAxisBox								m_BoundingBox;
	};
}
