/*****************************************************************************
**  g3dVertexBufferOptiX.cpp
**
**      See g3dVertexBufferOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G3DVERTEXBUFFEROPTIX_HPP
#include "GraphicsOptiX/G3d/g3dVertexBufferOptiX.hpp"
#endif

#include <utility>

namespace gox
{
	g3dVertexBufferOptiX::g3dVertexBufferOptiX(const std::shared_ptr<g3dVertexStoreOptiX>& i_pStore)
		: m_pStore(i_pStore)
	{
	}

	g3dVertexBufferOptiX::~g3dVertexBufferOptiX()
	{
	}

	void g3dVertexBufferOptiX::SetVertices(std::vector<g3dType::BumpTex1Vertex>&& i_Vertices)
	{
		m_pStore->GetVertices() = std::move(i_Vertices);
		m_pStore->Publish();
	}

	void g3dVertexBufferOptiX::UpdateVertices(int i_NumVertices, const maPoint3d* i_pVertices, const maVector3d* i_pNormals)
	{
		m_pStore->UpdateVertices(i_NumVertices, i_pVertices, i_pNormals);
	}

	int g3dVertexBufferOptiX::GetNumVertices() const
	{
		return m_pStore->GetNumVertices();
	}

	g3dType::VertexFormat g3dVertexBufferOptiX::GetVertexFormat() const
	{
		return g3dType::e_BumpTex1Vertex;
	}

	unsigned char* g3dVertexBufferOptiX::Lock()
	{
		std::vector<g3dType::BumpTex1Vertex>& vertices = m_pStore->GetVertices();
		return vertices.empty() ? NULL : reinterpret_cast<unsigned char*>(vertices.data());
	}

	void g3dVertexBufferOptiX::Unlock()
	{
		m_pStore->Publish();
	}

	void g3dVertexBufferOptiX::SetBoundingBox(const maAxisBox& i_BoundingBox)
	{
		m_BoundingBox = i_BoundingBox;
	}
}
