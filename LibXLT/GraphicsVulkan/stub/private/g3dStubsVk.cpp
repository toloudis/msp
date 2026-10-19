/*****************************************************************************
**  g3dStubsVk.cpp
**
**      See g3dStubsVk.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_G3DSTUBSVK_HPP
#include "GraphicsVulkan/stub/g3dStubsVk.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif
#ifndef G3D_INDEXPTR_HPP
#include "Graphics/G3d/g3dIndexPtr.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif

namespace gvk
{
	//------------------------------------------------------------------------
	//	Renderers
	//------------------------------------------------------------------------
	int g3dSceneRendererVk::Render(g2dRenderTarget*, const camCamera&, const g3dScene&, const std::vector<g3dLayer*>&, float)
	{
		GVK_NOT_YET(m_pType);
		return 0;
	}

	void g3dSceneRendererVk::ReleaseResources()
	{
	}

	int g3dPickRendererVk::Render(g2dRenderTarget*, const camCamera&, const g3dScene&, const std::vector<g3dLayer*>&, float)
	{
		GVK_NOT_YET("g3dPickRendererVk::Render");
		return 0;
	}

	void g3dPickRendererVk::ReleaseResources()					{}
	void g3dPickRendererVk::SetPickViewport(g2dRenderTarget*)	{ GVK_NOT_YET("g3dPickRendererVk::SetPickViewport"); }
	void g3dPickRendererVk::RestoreViewport()					{}
	void g3dPickRendererVk::GetPickInfo(g2dRenderTarget*, g3dPickInfo&)	{ GVK_NOT_YET("g3dPickRendererVk::GetPickInfo"); }

	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateDefaultRenderer()		{ return new g3dSceneRendererVk("default renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateHDRRenderer()			{ return new g3dSceneRendererVk("HDR renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateSimpleRenderer()		{ return new g3dSceneRendererVk("simple renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateTextureRenderer()		{ return new g3dSceneRendererVk("texture renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateDepthMapRenderer()	{ return new g3dSceneRendererVk("depth map renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateRSMRenderer()			{ return new g3dSceneRendererVk("RSM renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateOpacityMapRenderer(g3dProjectedLight*)	{ return new g3dSceneRendererVk("opacity map renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateCubeMapRenderer(scObject*, matMaterial*, bool)	{ return new g3dSceneRendererVk("cube map renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreatePlanarReflectionRenderer(scObject*, matMaterial*)	{ return new g3dSceneRendererVk("planar reflection renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateAmbientOcclusionRenderer()	{ return new g3dSceneRendererVk("ambient occlusion renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateGlobalIlluminationRenderer()	{ return new g3dSceneRendererVk("global illumination renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateDepthRenderer()		{ return new g3dSceneRendererVk("depth renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateUVRenderer()			{ return new g3dSceneRendererVk("UV renderer"); }
	g3dPickRenderer* g3dSceneRendererCreateVk::CreatePickRenderer()			{ return new g3dPickRendererVk; }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateShadowMaskRenderer()	{ return new g3dSceneRendererVk("shadow mask renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateIlluminationRenderer()	{ return new g3dSceneRendererVk("illumination renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateNormalRenderer()		{ return new g3dSceneRendererVk("normal renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateVelocityMapRenderer()	{ return new g3dSceneRendererVk("velocity map renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateMaterialsRenderer()	{ return new g3dSceneRendererVk("materials renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateReflectionOnlyRenderer()	{ return new g3dSceneRendererVk("reflection-only renderer"); }
	g3dSceneRenderer* g3dSceneRendererCreateVk::CreateGlowRenderer()		{ return new g3dSceneRendererVk("glow renderer"); }

	//------------------------------------------------------------------------
	//	Baking
	//------------------------------------------------------------------------
	int g3dBakeVk::Init(const g2dPFD&, const g3dScene*, const fsLocator&, float, const camCamera&, bool,
						const std::string&, int, int)
	{
		GVK_NOT_YET("g3dBakeVk::Init");
		return 0;
	}

	int g3dBakeVk::CleanUp()													{ return 0; }
	bool g3dBakeVk::BakeNextNode(std::map<const g3dFragment*, std::string>&)	{ return false; }		// nothing (more) to bake
	void g3dBakeVk::SetOutputDirectory(const fsLocator&)						{}

	//------------------------------------------------------------------------
	//	hairModelFragVk
	//------------------------------------------------------------------------
	int hairModelFragVk::sm_RendererId = 0;

	void hairModelFragVk::SetRendererId(int i_RenderMode)
	{
		sm_RendererId = i_RenderMode;
	}

	hairModelFragVk::hairModelFragVk(const maPoint3d* i_Vertices, const maPoint3d* i_Normals, const maPoint2d* i_UVs,
								 int i_nVertices, const g3dIndexPtr* i_pIndices, int i_nIndices,
								 matMaterial* i_pMaterial, bool i_bMorphable)
		: g3dFragment(i_pMaterial, sm_RendererId, i_bMorphable)
	{
		for (int i = 0; i < i_nVertices; ++i)
		{
			g3dType::BumpTex1Vertex vertex;
			vertex.m_Vertex = i_Vertices ? i_Vertices[i] : maPoint3d(0, 0, 0);
			vertex.m_Normal = i_Normals ? i_Normals[i] : maPoint3d(0, 0, 1);
			vertex.m_TexCoord = i_UVs ? i_UVs[i] : maPoint2d(0, 0);
			vertex.m_S = maPoint3d(1, 0, 0);
			vertex.m_T = maPoint3d(0, 1, 0);
			m_Vertices.push_back(vertex);
		}
		const bool wide = g3dIndexPtr::Needs32Bit(i_nVertices);
		for (int i = 0; i_pIndices && i < i_nIndices; ++i)
		{
			const uint32_t index = i_pIndices->GetIndex(i);
			if (wide)
			{
				m_Indices32.push_back(index);
			}
			else
			{
				m_Indices16.push_back(static_cast<uint16_t>(index));
			}
		}
		update_bounds();
	}

	hairModelFragVk::hairModelFragVk(const hairModelFragVk& i_Other)
		: g3dFragment(i_Other)
		, m_Vertices(i_Other.m_Vertices)
		, m_Indices16(i_Other.m_Indices16)
		, m_Indices32(i_Other.m_Indices32)
	{
	}

	hairModelFragVk::~hairModelFragVk()
	{
	}

	void hairModelFragVk::update_bounds()
	{
		maAxisBox box;
		bool first = true;
		const int count = GetNumVertices();
		for (int i = 0; i < count; ++i)
		{
			const maPoint3d& p = m_Vertices[i].m_Vertex;
			if (first)
			{
				box = maAxisBox(p, p);
				first = false;
			}
			else
			{
				box.Union(p);
			}
		}
		SetBoundingBox(box);
	}

	void hairModelFragVk::Deallocate()	{}
	void hairModelFragVk::Reallocate()	{}

	void hairModelFragVk::UpdateVertices(int i_NumVertices, const maPoint3d* i_pVertices, const maVector3d* i_pNormals)
	{
		const int count = std::min(i_NumVertices, GetNumVertices());
		for (int i = 0; i < count; ++i)
		{
			maPoint3d& position = m_Vertices[i].m_Vertex;
			maPoint3d& normal = m_Vertices[i].m_Normal;
			if (i_pVertices)
			{
				position = i_pVertices[i];
			}
			if (i_pNormals)
			{
				normal = maPoint3d(i_pNormals[i].GetX(), i_pNormals[i].GetY(), i_pNormals[i].GetZ());
			}
		}
		update_bounds();
	}

	int hairModelFragVk::GetNumVertices() const
	{
		return static_cast<int>(m_Vertices.size());
	}

	int hairModelFragVk::GetNumIndices() const
	{
		return static_cast<int>(m_Indices16.size() + m_Indices32.size());
	}

	g3dType::VertexFormat hairModelFragVk::GetVertexFormat() const
	{
		return g3dType::e_BumpTex1Vertex;
	}

	unsigned char* hairModelFragVk::Lock()
	{
		return reinterpret_cast<unsigned char*>(m_Vertices.data());
	}

	void hairModelFragVk::Unlock()								{ update_bounds(); }
	unsigned char* hairModelFragVk::LockIndices()
	{
		return m_Indices32.empty() ? reinterpret_cast<unsigned char*>(m_Indices16.data())
								   : reinterpret_cast<unsigned char*>(m_Indices32.data());
	}

	void hairModelFragVk::UnlockIndices()							{}
	unsigned char* hairModelFragVk::ReadOnlyLock()				{ return Lock(); }
	void hairModelFragVk::ReadOnlyUnlock()						{}
	unsigned char* hairModelFragVk::ReadOnlyLockIndices()			{ return LockIndices(); }
	void hairModelFragVk::ReadOnlyUnlockIndices()					{}
	void hairModelFragVk::ComponentSort(const maMatrix4x4&, const maPoint3d&)	{ GVK_NOT_YET("hairModelFragVk::ComponentSort"); }

	//------------------------------------------------------------------------
	//	hairRendererVk
	//------------------------------------------------------------------------
	int hairRendererVk::Render(const g3dSceneNode*, const matMaterial*, matShaderEffect*)
	{
		GVK_NOT_YET("hairRendererVk::Render (hair is Plan 7)");
		return 0;
	}
}
